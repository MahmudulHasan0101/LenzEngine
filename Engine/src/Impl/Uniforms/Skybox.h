#pragma once
#include"Impl/Texture/Cubemap.h"
#include"Impl/Shapes/BasicShapes.h"
#include"Lenz/Scene/Shape.h"
#include"Lenz/Renderer/Defines.h"
#include"Lenz/Renderer/Renderer.h"

namespace lenz
{
	/*
	class Skybox
	{
	public:
		Skybox()
		{
		}

		void Init(std::initializer_list<char*> faces, Shape&& ashape, const char* shaderpath, const char* uniformname = "uSkybox", uint32_t texslot = 0)
		{
			shader = Shader(shaderpath);
			cubemap = Cubemap(faces, texslot);
			shape = ashape;
			//shape.Init();

			uniformKey = shader.GetUniformLocation(uniformname);
			shader.UploadUniformInt(uniformKey, texslot);
			//cubemap.Bind(texslot, shader, uniformname);
			arraybuffer.Compile(shape.VertexBuffer, shape.IndexBuffer, shape.VertexLayout);
		}

		void Run()
		{
			//cubemap.Bind(shader);
			RendererCommands::DepthTestType(DepthTestTypes::LEQUAL);
			Renderer::Draw(shape.IndexBuffer.GetStride() / sizeof(uint32_t), arraybuffer, shader);
			RendererCommands::DepthTestType(DepthTestTypes::LESS);
		}

		uint32_t uniformKey;

		Shape shape;
		Shader shader;
		Cubemap cubemap;
		VertexArray arraybuffer;
	};

	*/
}




