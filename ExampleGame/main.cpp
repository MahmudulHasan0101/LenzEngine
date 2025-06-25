#if 1

#include<lenz.h>
#include<lenz/Renderer/Cubemap.h>
#include<lenz/Renderer/Camera.h>
#include<lenz/Scene/Component.h>
#include"Model.h"
#include"BasicShapes.h"

#include<random>
#include<vector>
#include<sstream>

typedef std::string EntityName;


// ============================================================================
// FALLING CUBE CLASS
// ============================================================================
class FallingCube
{
public:
    lenz::Entity entity;
    float lifetime;
    float maxLifetime;

    FallingCube(lenz::Entity e, float maxLife)
        : entity(e), lifetime(0.0f), maxLifetime(maxLife) {}

    bool ShouldBeDestroyed() const {
        return lifetime >= maxLifetime;
    }

    void UpdateLifetime(float deltaTime) {
        lifetime += deltaTime;
    }

    glm::vec3 GetPosition() {
        return glm::vec3(entity.GetComponent<RendererComponent::Transformation>().matrix[3]);
    }
};

// ============================================================================
// CUBE MANAGER CLASS
// ============================================================================
class CubeManager
{
private:
    lenz::Scene* scene;
    lenz::PhysicsSystem* physics_system;
    lenz::SceneDrawcall<Cube, RendererComponent::Transformation, RendererComponent::Color> drawcall;
    lenz::Shader shader;

    std::vector<FallingCube> falling_cubes;
    std::mt19937 rng;
    std::uniform_real_distribution<float> pos_dist;
    std::uniform_real_distribution<float> color_dist;
    std::uniform_real_distribution<float> lifetime_dist;

    // Spawn timing
    float cube_spawn_timer = 0.0f;
    float cube_spawn_interval = 1.0f;
    bool key1_pressed = false;
    float key1_spawn_timer = 0.0f;
    float key1_spawn_interval = 0.1f;

public:
    CubeManager(lenz::Scene* scene_ptr, lenz::PhysicsSystem* physics_ptr)
        : scene(scene_ptr), physics_system(physics_ptr), drawcall(scene, Cube()), rng(std::random_device{}()), pos_dist(-15.0f, 15.0f), color_dist(0.2f, 1.0f), lifetime_dist(8.0f, 20.0f)
    {
        shader.Init("Shaders/coloredcube.shader");
    }

    void SpawnRandomCube() {
        auto& entity = drawcall.CreateEntity();

        // Random position (spawn above ground)
        float x = pos_dist(rng);
        float z = pos_dist(rng);
        float y = 15.0f + pos_dist(rng) * 0.5f;

        SetupCube(entity, glm::vec3(x, y, z), "FallingCube");
    }

    void SpawnCubeInFrontOfCamera(const lenz::Camera& camera) {
        auto& entity = drawcall.CreateEntity();

        // Calculate position in front of camera
        glm::vec3 forward = camera.Orientation;
        glm::vec3 spawn_pos = camera.Position + forward * 20.0f;

        SetupCube(entity, spawn_pos, "CameraSpawnedCube");
    }

    void Update(lenz::TimeStep& ts, const lenz::Camera& camera) {
        UpdateSpawnTimers(ts, camera);
        UpdatePhysics(ts);
        CleanupCubes();
        drawcall.UpdateAll();
    }

    void Render(const lenz::Camera& camera) {
        camera.UploadTo({ &shader }, "u_camera");
        drawcall.Draw(shader);
    }

    void HandleKeyPress(int keyCode) {
        if (keyCode == LZ_KEY_SPACE) {
            SpawnRandomCube();
        }
        else if (keyCode == LZ_KEY_1) {
            key1_pressed = true;
        }
    }

    void HandleKeyRelease(int keyCode) {
        if (keyCode == LZ_KEY_1) {
            key1_pressed = false;
        }
    }

    size_t GetCubeCount() const {
        return falling_cubes.size();
    }

private:
    void SetupCube(lenz::Entity& entity, const glm::vec3& position, const std::string& name) {
        // Set transformation
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
        entity.GetComponent<RendererComponent::Transformation>().matrix = transform;

        // Random color
        entity.GetComponent<RendererComponent::Color>().color = {
            color_dist(rng), color_dist(rng), color_dist(rng), 1.0f
        };

        // Add to physics
        physics_system->AddEntity(entity);
        entity.GetComponent<lenz::PhysicsSpecification>().setColliderBox(
            { position.x, position.y, position.z }, { 0, 0, 0 }, { 1, 1, 1 }, 1.0f
        );

        // Add entity name
        entity.AddComponent<EntityName>(name);
        entity.GetComponent<lenz::PhysicsSpecification>().setUserPointer(&entity.GetComponent<EntityName>());

        // Add to tracking list
        float lifetime = lifetime_dist(rng);
        falling_cubes.emplace_back(entity, lifetime);
    }

    void UpdateSpawnTimers(lenz::TimeStep& ts, const lenz::Camera& camera) {
        // Periodic spawning
        cube_spawn_timer += ts.GetDeltaSecond();
        if (cube_spawn_timer >= cube_spawn_interval) {
            SpawnRandomCube();
            cube_spawn_timer = 0.0f;
            cube_spawn_interval = 0.8f + pos_dist(rng) * 0.1f; // Vary interval
        }

        // Key 1 continuous spawning
        if (key1_pressed) {
            key1_spawn_timer += ts.GetDeltaSecond();
            if (key1_spawn_timer >= key1_spawn_interval) {
                SpawnCubeInFrontOfCamera(camera);
                key1_spawn_timer = 0.0f;
            }
        }
    }

    void UpdatePhysics(lenz::TimeStep& ts) {
        for (auto& cube : falling_cubes) {
            cube.UpdateLifetime(ts.GetDeltaSecond());

            // Update transformation from physics
            auto& physics = cube.entity.GetComponent<lenz::PhysicsSpecification>();
            if (physics.body) {
                cube.entity.GetComponent<RendererComponent::Transformation>().matrix =
                    physics.getWorldTransformation();
            }
        }
    }

    void CleanupCubes() {
        auto it = falling_cubes.begin();
        while (it != falling_cubes.end()) {
            if (it->ShouldBeDestroyed()) {
                physics_system->RemoveEntity(it->entity);
                drawcall.RemoveEntity(it->entity);
                it = falling_cubes.erase(it);
            }
            else {
                ++it;
            }
        }
    }
};

// ============================================================================
// SKYBOX RENDERER CLASS
// ============================================================================
class SkyboxRenderer
{
private:
    lenz::Scene* scene;
    lenz::SceneDrawcall<TexturedCubeShape, RendererComponent::TexturePosition> drawcall;
    lenz::Shader shader;
    lenz::Cubemap cubemap;

public:
    SkyboxRenderer(lenz::Scene* scene_ptr)
        : scene(scene_ptr), drawcall(scene, TexturedCubeShape()), cubemap({
            "Images/skybox/right.jpg",
            "Images/skybox/left.jpg",
            "Images/skybox/top.jpg",
            "Images/skybox/bottom.jpg",
            "Images/skybox/front.jpg",
            "Images/skybox/back.jpg"
            }, 0)
    {
        shader.Init("Shaders/skybox.shader");

        cubemap.BindToSlot(5);
        cubemap.UploadToShader(shader, "u_texture");

        drawcall.CreateEntity();
        drawcall.UpdateAll();
    }

    ~SkyboxRenderer() {
        cubemap.Delete();
    }

    void Render(const lenz::Camera& camera) {
        glDepthFunc(GL_LEQUAL);
        camera.UploadOrientationMatrixTo({ &shader }, "u_camera");
        cubemap.BindToSlot(7);
        cubemap.UploadToShader(shader, "u_texture");
        drawcall.Draw(shader);
        glDepthFunc(GL_LESS);
    }
};

// ============================================================================
// GROUND RENDERER CLASS
// ============================================================================
class GroundRenderer
{
private:
    lenz::Scene* scene;
    lenz::SceneDrawcall<Square, RendererComponent::TexturePosition> drawcall;
    lenz::Shader shader;

public:
    GroundRenderer(lenz::Scene* scene_ptr)
        : scene(scene_ptr), drawcall(scene, Square())
    {
        shader.Init("Shaders/groundgridplane.shader");

        drawcall.CreateEntity();
        drawcall.UpdateAll();
    }

    void Render(const lenz::Camera& camera) {
        glDisable(GL_DEPTH_TEST);
        camera.UploadInverseTo({ &shader }, "u_invcamera");
        camera.UploadPositionTo({ &shader }, "u_camPos");
        drawcall.Draw(shader);
        glEnable(GL_DEPTH_TEST);
    }
};

// ============================================================================
// MODEL RENDERER CLASS
// ============================================================================
class ExampleModelRenderer
{
public:
    ExampleModelRenderer(lenz::Scene* scene_ptr, lenz::PhysicsSystem* physics_ptr)
        : scene(scene_ptr), physics_system(physics_ptr)
    {
        shader.Init("./Shaders/default.shader");
        lenz::Model model = lenz::Model("./Models/dragon.obj");
        lenz::Shape* shape = &model.meshes[0];
        texture = shape->textures[0].get();

        drawcall = scene->CreateDrawcall<ExampleModelRenderer, RendererComponent::Transformation>(std::move(*shape));

        entity = drawcall.CreateEntity();
        physics_system->AddEntity(entity);

        // Setup physics collision
        std::vector<float> points;
        points.reserve(model.meshes[0].verticies.size() / model.meshes[0].vertexsize * 3);

        for (unsigned int i = 0; i < model.meshes[0].verticies.size(); i += model.meshes[0].vertexsize) {
            points.push_back(model.meshes[0].verticies[i + 0]);
            points.push_back(model.meshes[0].verticies[i + 1]);
            points.push_back(model.meshes[0].verticies[i + 2]);
        }

        entity.GetComponent<lenz::PhysicsSpecification>().setConvexHullCollider(
            { 0, 500, 0 }, { 0, 0, 0 }, points, 100
        );
    }

    void Update(lenz::TimeStep& timestep) {
        entity.GetComponent<lenz::PhysicsSpecification>().setStatus(true);
        entity.GetComponent<RendererComponent::Transformation>().matrix =
            entity.GetComponent<lenz::PhysicsSpecification>().getWorldTransformation();
        drawcall.UpdateAll();
    }

    void Render(const lenz::Camera& camera) {
        camera.UploadTo({ &shader }, "u_camera");
        texture->BindToSlot(2);
        shader.UploadUniformInt("u_diffuse", 2);
        drawcall.Draw(shader);
    }

private:
    lenz::Scene* scene;
    lenz::PhysicsSystem* physics_system;
    lenz::Shader shader;
    lenz::Texture* texture;
    lenz::Entity entity;
    lenz::SceneDrawcall<ExampleModelRenderer, RendererComponent::Transformation> drawcall;
};

// ============================================================================
// UI RENDERER CLASS
// ============================================================================
class UIRenderer
{
public:
    UIRenderer() {
        textrenderer128.LoadFont("./Fonts/Luminous.otf", 128);
        textrenderer32.LoadFont("./Fonts/Luminous.otf", 32);

        UpdateScreenMatrix();
    }

    void UpdateScreenMatrix() {
        float width = static_cast<float>(lenz::App::GetWindow().GetWidth());
        float height = static_cast<float>(lenz::App::GetWindow().GetHeight());
        screen_mat = glm::ortho(0.0f, width, 0.0f, height, -0.0f, 1.0f);
    }

    void Render(const lenz::Camera& camera, size_t cubeCount) {
        float width = static_cast<float>(lenz::App::GetWindow().GetWidth());
        float height = static_cast<float>(lenz::App::GetWindow().GetHeight());

        // Upload camera matrix for text rendering
        camera.UploadTo({ textrenderer128.activeShader }, "u_camera");

        // Render title
        textrenderer128.RenderText("Falling Cubes Demo", { 0, 0 }, { 0.2, 0.2, 0.2 }, 0.08);

        // Render info text
        textrenderer32.activeShader->UploadUniformMat4("u_camera", screen_mat);

        std::ostringstream info;
        info << "Cubes: " << cubeCount << " | SPACE: random spawn | Hold 1: spawn in front";
        textrenderer32.RenderText(info.str(), { 20, height - 50 }, { 0, 0, 0 }, 1);

        std::ostringstream camera_info;
        camera_info << "Camera: (" << camera.Position.x << ", "
            << camera.Position.y << ", " << camera.Position.z << ")";
        textrenderer32.RenderText(camera_info.str(), { 20, height - 80 }, { 0, 0, 0 }, 1);
    }
};



// ============================================================================
// MAIN EXAMPLE LAYER CLASS
// ============================================================================
class ExampleLayer : public lenz::Layer
{
private:
    // Core systems
    lenz::Camera camera;
    lenz::Scene scene;
    lenz::PhysicsSystem physics_system;

    // Renderers
    std::unique_ptr<CubeManager> cube_manager;
    std::unique_ptr<SkyboxRenderer> skybox_renderer;
    std::unique_ptr<GroundRenderer> ground_renderer;
    std::unique_ptr<ExampleModelRenderer> model_renderer;
    std::unique_ptr<UIRenderer> ui_renderer;

    float tick = 0;

public:
    ExampleLayer(std::string name = "layer") : lenz::Layer(name) {
        InitializeRenderers();
        SetupCamera();
        SetupRendering();
    }

    bool OnEvent(lenz::Event& e) override {
        camera.OnEvent(e);
        lenz::Layer::OnEvent(e);
        return false;
    }

    bool OnKeyPressed(lenz::KeyPressedEvent& e) {
        cube_manager->HandleKeyPress(e.GetKeyCode());
        return false;
    }

    bool OnKeyReleased(lenz::KeyReleasedEvent& e) {
        cube_manager->HandleKeyRelease(e.GetKeyCode());
        return false;
    }

    void OnUpdate(lenz::TimeStep& ts) override {
        tick += ts.GetDeltaSecond();

        // Update systems
        physics_system.Update(ts.GetDeltaSecond());
        cube_manager->Update(ts, camera);
        model_renderer->Update(ts);
        camera.OnUpdate(ts);

        // Render everything
        lenz::RendererCommands::Clear();

        skybox_renderer->Render(camera);
        ground_renderer->Render(camera);
        model_renderer->Render(camera);
        cube_manager->Render(camera);
        ui_renderer->Render(camera, cube_manager->GetCubeCount());
    }

private:
    void InitializeRenderers() {
        cube_manager = std::make_unique<CubeManager>(&scene, &physics_system);
        skybox_renderer = std::make_unique<SkyboxRenderer>(&scene);
        ground_renderer = std::make_unique<GroundRenderer>(&scene);
        model_renderer = std::make_unique<ExampleModelRenderer>(&scene, &physics_system);
        ui_renderer = std::make_unique<UIRenderer>();
    }

    void SetupCamera() {
        camera.SetSpeed(40.0f);
        camera.Position = glm::vec3(0, 10, 20);
    }

    void SetupRendering() {
        lenz::RendererCommands::EnableDepthTest();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

private:
    lenz::TextRenderer textrenderer128;
    lenz::TextRenderer textrenderer32;
    glm::mat4 screen_mat;
};

// ============================================================================
// APPLICATION CLASS
// ============================================================================
class Simulator : public lenz::App
{
public:
    Simulator() : App() {
        PushLayer(new ExampleLayer());
    }
};

// ============================================================================
// ENTRY POINTS
// ============================================================================
lenz::Window* GetAppWindow() {
    return new lenz::Window();
}

lenz::Camera* GetAppCamera() {
    return new lenz::Camera();
}

lenz::App* CreateApp() {
    return new Simulator();
}

#endif