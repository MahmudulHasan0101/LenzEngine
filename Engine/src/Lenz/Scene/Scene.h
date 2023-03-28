#pragma once
#include "Lenz/Utils/TimeStep.h"
#include "Lenz/Renderer/Renderer.h"
#include "Entity.h"

#include<string>
#include<unordered_map>

#include "Shape.h"
#include "DrawCall.h"

namespace lenz {


	class Scene
	{
	public:
		Scene();
		~Scene();

		//template<typename ...Args>
		//Entity CreateEntity(Args&&... components)
		//{
		//	Entity entity = { m_Registry.create(), this };
		//	(entity.AddComponent<Args>(components),...);
		//
		//	return entity;
		//}

		Entity& CreateEntity();

		template<typename T> // T = any Shape
		Entity& CreateEntity()
		{
			//LZ_CORE_ASSERT(m_DrawCalls.find(GetClassUUID<T>()) != m_DrawCalls.end(), " This DrawCall haven't been specified by this scene, run CreateDrawCall<T>() by this to solve.")

			DrawCall& sn = m_DrawCalls[GetClassUUID<T>()];
			//IntellizeDrawCall<T>(sn);

			Entity& e = CreateEntity();
			e.AddVoidComponent<VOID_CLASS_BINDER<T>>();
			sn.Entities.push_back(e.GetHandle());

			return e;
		}


		template<typename T> // T = any Shape
		void IntellizeDrawCall(DrawCall& sn)
		{
			if (sn.Scene == nullptr)
			{
				sn.Init(this);

				if (sn.ShapeID == 0)
				{
					sn.ShapeID = GetClassUUID<T>();
					sn.Arraybuffer.Init();
					sn.Shape = T();
					#ifdef LZ_DEBUG
						sn.ShapeName = typeid(T).name();
					#endif
					//sn.Shape.Init();

					sn.Layout.JoinVerticies(sn.Shape.VertexLayout);
				}
				LZ_CORE_ASSERT(sn.ShapeID == GetClassUUID<T>(), "The DrawCall is binded to another shape");
			}
		}


		template<typename T>
		DrawCall& CreateDrawCall()
		{
			DrawCall& sn = m_DrawCalls[GetClassUUID<T>()];
			IntellizeDrawCall<T>(sn);

			return sn;
		}

		template<typename T>
		void RemoveDrawCall()
		{
			m_DrawCalls.erase(GetClassUUID<T>());
		}


		template<typename ... Args>
		void Update()
		{
			for (auto& pair : m_DrawCalls)
			{
				pair.second.Update<Args...>();
			}
		}

		template<typename ... Args>
		void Update(std::initializer_list<uint32_t> shapeIDs)
		{
			for (uint32_t u = 0; u < shapeIDs.size(); u++)
			{
				m_DrawCalls[shapeIDs[u]].Update<Args...>();
			}
		}

		template<typename T>
		void DrawShape(const Shader& shader)
		{
			DrawCall& drawcall = m_DrawCalls[GetClassUUID<T>()];
			//ShapeBinder<T>(std::ref(drawcall));
			drawcall.Run(shader);
		}

		template<typename...Args>
		void Draw(Shader& shader)
		{
			//(m_DrawCalls[GetClassUUID<Args>()].Run(shader), ...);
			(DrawShape<Args>(shader), ...);
		}

		void Draw(Shader& shader)
		{
			for (auto& pair : m_DrawCalls)
				pair.second.Run(shader);
		}


		/*
		template<typename...Args>
		void RunAndUpdate(Shader& shader)
		{
			if (ShaderChache != &shader)
			{
				ShaderChache = &shader;
				Uniforms.ChangeShader(shader);
				Uniforms.UploadUniforms();
			}

			Draw<Args...>(shader);
		}

		void RunAndUpdate(Shader& shader)
		{
			if (ShaderChache != &shader)
			{
				ShaderChache = &shader;
				DynamicUniforms.ChangeShader(shader);
				StaticUniforms.ChangeShader(shader);
				StaticUniforms.UploadUniforms();
			}

			DynamicUniforms.UploadUniforms();
			Draw(shader);
		}
		*/


		inline entt::registry& GetRegistry() { return m_Registry; }

		inline std::unordered_map<uint32_t, DrawCall>& GetDrawCalls() { return m_DrawCalls; }

		template<typename T>
		inline DrawCall& GetDrawCall() { return m_DrawCalls[GetClassUUID<T>()]; }

		Shader* ShaderChache;
	private:
		entt::registry m_Registry;
		std::unordered_map<uint32_t, DrawCall> m_DrawCalls;

		friend class Entity; 
	};  

}