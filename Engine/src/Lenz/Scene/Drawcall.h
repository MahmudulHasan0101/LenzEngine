#pragma once
#include"Lenz/Core.h"
#include"entt/entt.hpp"
#include"Lenz/Renderer/Defines.h"
#include"Lenz/Utils/Vector.h"
#include"Lenz/Renderer/Shader.h"
#include"Lenz/Scene/Shape.h"
#include"Lenz/Renderer/Renderer.h"
#include"Entity.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <span>
#include <optional>

namespace lenz
{
    class Scene;

    template <typename... Components>
    constexpr std::size_t component_total_size_v = (sizeof(Components) + ...);

    template<typename T>
    struct ComponentMeta {
        static constexpr uint32_t id = []() constexpr {
            std::string_view name = __FUNCSIG__;

            uint32_t hash = 0;
            for (char c : name)
                hash = hash * 33 + static_cast<uint32_t>(c);

            return hash;
            }();

        static constexpr size_t size = sizeof(T);
        static constexpr auto type = T::GetFrameworkType();
        static constexpr size_t type_size = T::GetTypeSize();
        static constexpr auto subdivide = T::GetSubDivideNumber();
        static constexpr bool normalized = (type != GL_FLOAT);
    };

    class Storage
    {
    public:
        static std::unordered_map<uint32_t, Shape> SHAPES;
        static std::vector<Texture> TEXTURES;
        static std::vector<std::string> TEXTURE_PATHS;

        static std::shared_ptr<Texture> addTexture2D(const std::string& path, uint8_t slot)
        {
            if (auto it = std::find(TEXTURE_PATHS.begin(), TEXTURE_PATHS.end(), path);
                it != TEXTURE_PATHS.end()) {
                auto index = std::distance(TEXTURE_PATHS.begin(), it);
                return std::shared_ptr<Texture>(&TEXTURES[index]);
            }

            TEXTURES.emplace_back(Texture2D(path, slot));
            TEXTURE_PATHS.emplace_back(path);
            return std::shared_ptr<Texture>(&TEXTURES.back());
        }

        template<typename T>
        Shape& GetShape() {
            return SHAPES.at(ComponentMeta<T>::id);
        }
    };

    class Drawcall
    {
        friend class Scene;
        template<typename, typename...> friend class SceneDrawcall;

    public:
        Drawcall(uint32_t shapeID, entt::registry& reg)
            : m_instanceBuffer(BufferType::InstanceBuffer), m_registry(reg)
        {
            m_arraybuffer.Init();
            m_instanceBuffer.Init(100);
            m_instancebufferlayout.setAttribDivisor(1);
            m_shape = &Storage::SHAPES.at(shapeID);
        }

        ~Drawcall() {
            m_instanceBuffer.Destroy();
        }

        Drawcall(const Drawcall&) = delete;
        Drawcall& operator=(const Drawcall&) = delete;

        Drawcall(Drawcall&& other) noexcept
            : m_instanceBuffer(std::move(other.m_instanceBuffer)), m_arraybuffer(std::move(other.m_arraybuffer)), m_instancebufferlayout(std::move(other.m_instancebufferlayout)),
            m_registry(other.m_registry), m_shape(std::exchange(other.m_shape, nullptr)), m_columnSize(other.m_columnSize), m_attributeOffsets(std::move(other.m_attributeOffsets))
        {}

        Drawcall& operator=(Drawcall&& other) noexcept {
            if (this != &other) {
                m_instanceBuffer = std::move(other.m_instanceBuffer);
                m_arraybuffer = std::move(other.m_arraybuffer);
                m_instancebufferlayout = std::move(other.m_instancebufferlayout);
                m_shape = std::exchange(other.m_shape, nullptr);
                m_columnSize = other.m_columnSize;
                m_attributeOffsets = std::move(other.m_attributeOffsets);
            }
            return *this;
        }

        template<typename... Components>
        void SetupAttributes() {
            static_assert(sizeof...(Components) > 0, "At least one component required");

            (m_instancebufferlayout.push_back(
                ComponentMeta<Components>::type,
                ComponentMeta<Components>::size / ComponentMeta<Components>::type_size,
                ComponentMeta<Components>::normalized,
                ComponentMeta<Components>::subdivide
            ), ...);

            LZ_CORE_ASSERT(m_shape->vertexBuffer.GetStride() != 0,
                "Shape must be bound before setting up attributes");

            CompileBuffers();
            CalculateOffsets<Components...>();
        }

        template<typename ShapeType, typename... Components>
        void UpdateInstanceData() {
            auto view = m_registry.view<VOID_CLASS_BINDER<ShapeType>, Components...>();

            const size_t entityCount = view.size();
            const size_t requiredSize = entityCount * m_columnSize;

            if (requiredSize > m_instanceBuffer.GetSize()) {
                m_instanceBuffer.Init(requiredSize * 2);
                CompileBuffers();
            }

            UpdateInstanceDataImpl<Components...>(view);
        }

        void Draw(const Shader& shader, size_t instanceCount) {
            Renderer::Draw(instanceCount,
                m_shape->indexBuffer.GetStride() / sizeof(uint32_t),
                m_arraybuffer, shader);
        }

        void DrawTriangleStrips(size_t instanceCount) {
            m_arraybuffer.Bind();
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instanceCount);
        }

    private:
        Buffer m_instanceBuffer;
        ArrayBuffer m_arraybuffer;
        BufferLayout m_instancebufferlayout;
        entt::registry& m_registry;
        Shape* m_shape = nullptr;

        uint32_t m_columnSize = 0;
        std::unordered_map<uint32_t, uint32_t> m_attributeOffsets;

        void CompileBuffers() {
            m_arraybuffer.Compile(&m_shape->vertexBuffer, &m_instanceBuffer,
                &m_shape->indexBuffer, &m_shape->vertexLayout,
                &m_instancebufferlayout, nullptr);
        }

        template<typename... Components>
        void CalculateOffsets() {
            ((m_attributeOffsets[ComponentMeta<Components>::id] =
                (m_columnSize += ComponentMeta<Components>::size) - ComponentMeta<Components>::size), ...);
        }

        template<typename... Components, typename View>
        void UpdateInstanceDataImpl(View& view) {
            if constexpr (sizeof...(Components) == 1) {
                // Single component optimization - direct memory copy possible
                using Component = std::tuple_element_t<0, std::tuple<Components...>>;
                if constexpr (std::is_trivially_copyable_v<Component>) {
                    std::vector<Component> data;
                    data.reserve(view.size());
                    for (auto entity : view) {
                        data.emplace_back(view.template get<Component>(entity));
                    }
                    m_instanceBuffer.SetData(0, data.data(), data.size() * sizeof(Component));
                    return;
                }
            }

            // General case - component by component update
            size_t entityIndex = 0;
            std::array<uint32_t, sizeof...(Components)> offsets;
            size_t offsetIndex = 0;
            ((offsets[offsetIndex++] = m_attributeOffsets[ComponentMeta<Components>::id]), ...);

            for (auto entity : view) {
                offsetIndex = 0;
                (m_instanceBuffer.SetData(
                    m_columnSize * entityIndex + offsets[offsetIndex++],
                    &view.template get<Components>(entity),
                    ComponentMeta<Components>::size
                ), ...);
                ++entityIndex;
            }
        }
    };

    /*
    template<typename... Components>
    class AttributeCache {
    public:
        static constexpr size_t total_size = component_total_size_v<Components...>;

        struct alignas(std::max({ alignof(Components)... })) Entry {
            std::tuple<Components...> data;

            template<size_t I>
            auto& get() { return std::get<I>(data); }

            template<size_t I>
            const auto& get() const { return std::get<I>(data); }
        };

        void reserve(size_t count) {
            entries.reserve(count);
        }

        void clear() {
            entries.clear();
        }

        template<typename... Args>
        void emplace_back(Args&&... args) {
            entries.emplace_back(Entry{ {std::forward<Args>(args)...} });
        }

        const void* data() const { return entries.data(); }
        size_t size() const { return entries.size(); }
        size_t byte_size() const { return entries.size() * sizeof(Entry); }

    private:
        std::vector<Entry> entries;
    };
    */

    template<typename ShapeType, typename... Components>
    class SceneDrawcall
    {
    public:
        SceneDrawcall() = default;

        SceneDrawcall(Drawcall* drawcall, Scene* scene)
            : m_drawcall(drawcall), m_scene(scene) {
            if constexpr (sizeof...(Components) > 0) {
                m_drawcall->SetupAttributes<Components...>();
            }
        }

        SceneDrawcall(Scene* scene, Shape&& shape)
            : m_scene(scene) {
            m_drawcall = m_scene->CreateDrawcall<ShapeType, Components...>(std::move(shape)).drawcall();
        }

        // Drawing methods
        void Draw(const Shader& shader) {
            shader.Bind();
            m_drawcall->Draw(shader, m_entityCount);
        }

        void DrawTriangleStrips(const Shader& shader) {
            shader.Bind();
            m_drawcall->DrawTriangleStrips(m_entityCount);
        }

        // Entity management
        Entity CreateEntity() {
            Entity entity = Entity(&m_scene->m_registry);
            entity.template AddVoidComponent<VOID_CLASS_BINDER<ShapeType>>();
            if constexpr (sizeof...(Components) > 0) {
                (entity.template AddComponent<Components>(), ...);
            }
            return entity;
        }

        void RemoveEntity(const Entity& entity) {
            m_drawcall->m_registry.destroy(entity.GetHandle());
        }

        // Efficient batch update methods
        void UpdateAll() {
            if constexpr (sizeof...(Components) > 0) {
                m_drawcall->UpdateInstanceData<ShapeType, Components...>();
                m_entityCount = GetEntityCount();
            }
            else {
                m_entityCount = GetEntityCount();
            }
        }

        /*
        void UpdateWithCache() requires (sizeof...(Components) > 0) {
            auto view = m_drawcall->m_registry.view<VOID_CLASS_BINDER<ShapeType>, Components...>();

            m_cache.clear();
            m_cache.reserve(view.size());

            for (auto entity : view) {
                m_cache.emplace_back(view.template get<Components>(entity)...);
            }

            const size_t requiredSize = m_cache.byte_size();
            if (requiredSize > m_drawcall->m_instanceBuffer.GetSize()) {
                m_drawcall->m_instanceBuffer.Init(requiredSize * 2);
                m_drawcall->CompileBuffers();
            }

            m_drawcall->m_instanceBuffer.SetData(0, m_cache.data(), requiredSize);
            m_entityCount = m_cache.size();
        }
        */



        template<typename... Attributes>
        auto GetView() {
            return m_drawcall->m_registry.view<VOID_CLASS_BINDER<ShapeType>, Attributes...>();
        }

        size_t GetEntityCount() const {
            return m_drawcall->m_registry.view<VOID_CLASS_BINDER<ShapeType>>().size();
        }

        size_t entity_count() const { return m_entityCount; }
        Drawcall* drawcall() { return m_drawcall; }
        Scene* scene() { return m_scene; }

    private:
        Drawcall* m_drawcall = nullptr;
        Scene* m_scene = nullptr;
        size_t m_entityCount = 0;

        /*
        [[no_unique_address]] std::conditional_t <
            sizeof...(Components) > 0,
            AttributeCache<Components...>,
            std::monostate
        > m_cache;
        */
    };

    // Specialization for shape-only drawcalls
    template<typename ShapeType>
    class SceneDrawcall<ShapeType>
    {
    public:
        SceneDrawcall(Drawcall* drawcall, Scene* scene)
            : m_drawcall(drawcall), m_scene(scene) {}

        SceneDrawcall(Scene* scene, Shape&& shape)
            : m_scene(scene) {
            m_drawcall = m_scene->CreateDrawcall<ShapeType>(std::move(shape)).drawcall;
        }

        Entity CreateEntity() {
            Entity entity = Entity(&m_scene->m_registry);
            entity.template AddVoidComponent<VOID_CLASS_BINDER<ShapeType>>();
            return entity;
        }

        void RemoveEntity(const Entity& entity) {
            m_drawcall->m_registry.destroy(entity.GetHandle());
        }

        void UpdateEntityCount() {
            m_entityCount = m_drawcall->m_registry.view<VOID_CLASS_BINDER<ShapeType>>().size();
        }

        void Draw(const Shader& shader) {
            UpdateEntityCount();
            m_drawcall->Draw(shader, m_entityCount);
        }

        void DrawTriangleStrips(const Shader& shader) {
            UpdateEntityCount();
            shader.Bind();
            m_drawcall->DrawTriangleStrips(m_entityCount);
        }

        size_t entity_count() const { return m_entityCount; }
        Drawcall* drawcall() { return m_drawcall; }
        Scene* scene() { return m_scene; }

    private:
        Drawcall* m_drawcall;
        Scene* m_scene;
        size_t m_entityCount = 0;
    };
}