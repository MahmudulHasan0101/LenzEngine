#pragma once
#include"Lenz/Core.h"
#include"Lenz/Renderer/Buffer.h"
#include"Lenz/Renderer/Texture.h"
#include"Lenz/Inputs.h"

namespace lenz
{
	class Shape
	{
		friend class DrawCall;

	public:
		Shape();
		Shape(const Shape&) = delete;
		Shape(Shape&&);
		Shape(const char* binaryFile);
		~Shape();

		//void ChacheBinary(const char* filename = "");

		void Destroy();

		void SetName(const std::string&);

		//void Init(ShapeType type = ShapeType::Static);

		void SetVerticies(std::initializer_list<float> data, uint32_t bufferfluidity = BufferFluidity::Static);

		void SetVerticies(std::vector<float> data, uint32_t fluidity = BufferFluidity::Static);

		void SetIndecies(std::initializer_list<uint32_t> data, uint32_t bufferfluidity = BufferFluidity::Static);

		void SetIndecies(std::vector<uint32_t> data, uint32_t fluidity = BufferFluidity::Static);

		void SetAttrib(lenz::BufferElement& element)
		{
			vertexLayout.push_back(element);
		}

		template<typename T>
		void SetAttrib(uint32_t count)
		{
			uint32_t type = OpenGLType<T>;
			vertexLayout.push_back(type, count, type == GL_FLOAT ? GL_FALSE : GL_TRUE);
		}

		template<typename T, typename SubT = float>
		void SetAttrib()
		{
			uint32_t type = OpenGLType<SubT>;
			uint32_t count = sizeof(T) / sizeof(SubT);
			vertexLayout.push_back(type, count, type == GL_FLOAT ? GL_FALSE : GL_TRUE);
		}

		template<typename ... Args>
		void SetAttribs()  // Warning: Always counts subT = float
		{
			(SetAttrib<Args>(), ...);
		}


		void operator = (Shape&& t)
		{
			vertexLayout = std::move(t.vertexLayout);
			vertexBuffer = std::move(t.vertexBuffer);
			indexBuffer = std::move(t.indexBuffer);
			textures = std::move(t.textures);

			name = std::move(t.name);
		}
		
		void operator = (const Shape& t) = delete;
		
		Buffer vertexBuffer;
		Buffer indexBuffer;
		BufferLayout vertexLayout;

		std::vector<std::shared_ptr<Texture>> textures;
		std::string name;
	};


};
