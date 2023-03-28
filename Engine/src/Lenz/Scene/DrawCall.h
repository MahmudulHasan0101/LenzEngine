#pragma once
#include"Lenz/Core.h"
#include"entt.hpp"
#include"Lenz/Renderer/Defines.h"
#include"Lenz/Utils/Vector.h"
#include"Lenz/Renderer/Shader.h"
#include"Lenz/Scene/Shape.h"
#include"Lenz/Renderer/Renderer.h"

namespace lenz
{
	template<typename T>
	struct CLASS_UUID_BINDER
	{
		uint32_t id = GetClassUUID<T>();
		inline operator uint32_t() const { return id; }
	};

	template<typename T>
	struct VOID_CLASS_BINDER
	{
	};

	class Scene;


//#define LZ_DRAWCALL_USE_BLOCKBUFFER  // Directly transfers the data to instanced buffer

#ifndef  LZ_DRAWCALL_USE_BLOCKBUFFER
	#define LZ_DRAWCALL_USE_BLOCKARRAY   // Stores all data in a heap allocated memory and then transfers the data to the instanced buffer (default)
#endif






	class DrawCall
	{
	public:
		DrawCall()
			:
	#ifdef LZ_DRAWCALL_USE_BLOCKARRAY
			AttributeArray(10000),
	#endif
			ShapeID(0),
			Instancebuffer(BufferType::InstanceBuffer),
			//ShaderChache(nullptr),
			Scene(nullptr)
		{
		}

		void Init(Scene* scene = nullptr)
		{
			Scene = scene;
			Instancebuffer.Init(10000);
		}

		~DrawCall()
		{
			Shape.Destroy();
		#ifdef LZ_DRAWCALL_USE_BLOCKARRAY
			AttributeArray.Delete();
		#endif
			Instancebuffer.Destroy();
		}


		void Compile()
		{
			LZ_CORE_ASSERT((Shape.VertexBuffer.GetStride() != 0 && Shape.IndexBuffer.GetStride() != 0), " A Shape must be binded to see the result of DrawCall | Try calling Update functoin before this function. ")

			Arraybuffer.Compile(Shape.VertexBuffer, Instancebuffer, Shape.IndexBuffer, Layout);
		}


		template<typename ... attribs>
		void Update()
		{

		#ifdef LZ_DRAWCALL_USE_BLOCKARRAY
			for (uint32_t u = 0; u < Entities.size(); u++)
			{
				(AttributeArray.Write(
					u,
					&Scene->GetRegistry().get<attribs>(Entities[u]),
					sizeof(attribs),
					GetClassUUID<attribs>()
				), ...);
		}

			Instancebuffer.SetData(0, AttributeArray.GetVoidArray().Data(), AttributeArray.GetVoidArray().Size<void>());

		#endif

		#ifdef LZ_DRAWCALL_USE_BLOCKBUFFER
			for (uint32_t u = 0; u < Entities.size(); u++)
			{
				(Instancebuffer.Write(
					u,
					&Scene->GetRegistry().get<attribs>(Entities[u]),
					sizeof(attribs),
					GetClassUUID<attribs>()
				), ...);
			}
		#endif
		}

		void Print()
		{
			LZ_CORE_WARN("Printing entities:");
		#ifdef LZ_DRAWCALL_USE_BLOCKARRAY
			for (uint32_t u = 0; u < Entities.size(); u++)
			{
				std::cout << (uint32_t)Entities[u] << "\n";
			}
		#else
			LZ_CORE_WARN("LZ_DRAWCALL_USE_BLOCKBUFFER should be defined to use Print() function");
		#endif
		}

		template<typename ... Args>
		void AddAttributes()
		{
			(Layout.AddInstanceElement(Args::GetFrameworkType(), sizeof(Args) / Args::GetTypeSize(), Args::GetFrameworkType() == GL_FLOAT ? GL_FALSE : GL_TRUE), ...);
		#ifdef LZ_DRAWCALL_USE_BLOCKARRAY
			AttributeArray.AddBlocks<Args ...>();
		#endif

		#ifdef LZ_DRAWCALL_USE_BLOCKBUFFER
			Instancebuffer.AddBlocks<Args ...>();
		#endif
			Compile();
		}

		void Run(Shader& shader)
		{
			Renderer::Draw(Entities.size(), Shape.IndexBuffer.GetStride() / sizeof(uint32_t), Arraybuffer, shader);
		}

		void operator = (DrawCall&& drawcall)
		{
			Scene = drawcall.Scene;
			drawcall.Scene = nullptr;

			Shape = std::move(drawcall.Shape);
			Instancebuffer = std::move(drawcall.Instancebuffer);

		#ifdef LZ_DRAWCALL_USE_BLOCKARRAY
			Arraybuffer = std::move(drawcall.Arraybuffer);
		#endif
			Layout = std::move(Layout);

			ShapeID = drawcall.ShapeID;
			drawcall.ShapeID = 0;

			//Entities = std::move(drawcall.Entities);
			Entities = std::move(drawcall.Entities);
			//drawcall.Entities = nullptr;
		}

	#ifdef LZ_DRAWCALL_USE_BLOCKARRAY
		Buffer Instancebuffer;
		BlockArray AttributeArray;
	#endif

	#ifdef LZ_DRAWCALL_USE_BLOCKBUFFER
		BlockBuffer Instancebuffer;
	#endif

		Scene* Scene;
		Shape Shape;
		VertexArray Arraybuffer;
		BufferLayout Layout;

		uint32_t ShapeID;

		LZ_DEBUG_ONLY(std::string ShapeName);
		std::vector<entt::entity> Entities;
	};


	/*

	class DrawCall
	{
	public:
		DrawCall()
			:AttributeArray(10000),
			ShapeID(0),
			Instancebuffer(BufferType::InstanceBuffer),
			//ShaderChache(nullptr),
			Scene(nullptr)
		{
		}

		void Init(Scene* scene = nullptr)
		{
			Scene = scene;
			Instancebuffer.Init(10000);
		}

		~DrawCall()
		{
			Shape.Destroy();
			AttributeArray.Delete();
			Instancebuffer.Destroy();
		}


		void Compile()
		{
			LZ_CORE_ASSERT((Shape.VertexBuffer.GetStride() != 0 && Shape.IndexBuffer.GetStride() != 0), " A Shape must be binded to see the result of DrawCall | Try calling Update functoin before this function. ")

			Arraybuffer.Compile(Shape.VertexBuffer, Instancebuffer, Shape.IndexBuffer, Layout);
		}


		template<typename ... attribs>
		void Update()
		{
			for (uint32_t u = 0; u < Entities.size(); u++)
			{
				(AttributeArray.Write(
					u,
					&Scene->GetRegistry().get<attribs>(Entities[u]),
					sizeof(attribs),
					GetClassUUID<attribs>()
				), ...);
			}
			
			Instancebuffer.SetData(0, AttributeArray.GetVoidArray().Data(), AttributeArray.GetVoidArray().Size<void>());
		}

		void Print()
		{
			LZ_CORE_WARN("Printing entities:");
			for (uint32_t u = 0; u < Entities.size(); u++)
			{
				std::cout << (uint32_t)Entities[u] << "\n";
			}
		}

		template<typename ... Args>
		void AddAttributes()
		{
			(Layout.AddInstanceElement(Args::GetFrameworkType(), sizeof(Args) / Args::GetTypeSize(), Args::GetFrameworkType() == GL_FLOAT ? GL_FALSE : GL_TRUE), ...);
			AttributeArray.AddBlocks<Args ...>();
		}

		void Run(Shader& shader)
		{
			Renderer::Draw(Entities.size(), Shape.IndexBuffer.GetStride() / sizeof(uint32_t), Arraybuffer, shader);
		}

		void operator = (DrawCall&& drawcall)
		{
			Scene = drawcall.Scene;
			drawcall.Scene = nullptr;

			Shape = std::move(drawcall.Shape);
			Instancebuffer = std::move(drawcall.Instancebuffer);
			Arraybuffer = std::move(drawcall.Arraybuffer);
			Layout = std::move(Layout);

			ShapeID = drawcall.ShapeID;
			drawcall.ShapeID = 0;

			//Entities = std::move(drawcall.Entities);
			Entities = std::move(drawcall.Entities);
			//drawcall.Entities = nullptr;
		}


		BlockArray AttributeArray;

		Scene* Scene;
		Shape Shape;
		Buffer Instancebuffer;
		VertexArray Arraybuffer;
		BufferLayout Layout;

		uint32_t ShapeID;

#ifdef LZ_DEBUG
		std::string ShapeName;
#endif
		std::vector<entt::entity> Entities;
	};
*/



};
