#pragma once
#include"Lenz/Core.h"
#include"Lenz/Renderer/Buffer.h"
#include"Lenz/Inputes.h"

namespace lenz
{

	class Shape
	{
		friend class DrawCall;

	public:
		Shape();

		void Destroy();

		void SetName(const std::string&);

		//void Init(ShapeType type = ShapeType::Static);

		void SetVerticies(std::initializer_list<float> data, uint32_t bufferfluidity = BufferFluidity::Static);

		void SetIndecies(std::initializer_list<uint32_t> data, uint32_t bufferfluidity = BufferFluidity::Static);


		void SetAttrib(lenz::BufferElement& element)
		{
			vertexLayout.push_back(element);
		}

		template<typename T>
		void SetAttrib(uint32_t count)
		{
			uint32_t type = GetOpenGLType<T>();
			vertexLayout.AddVertexElement(type, count, type == GL_FLOAT ? GL_FALSE : GL_TRUE);
		}

		template<typename T, typename SubT = float>
		void SetAttrib()
		{
			uint32_t type = GetOpenGLType<SubT>();
			uint32_t count = sizeof(T) / sizeof(SubT);
			vertexLayout.push_back(type, count, type == GL_FLOAT ? GL_FALSE : GL_TRUE);
		}

		template<typename ... Args>
		void SetAttribs()  // Warning: Always counts subT = float
		{
			(SetAttrib<Args>(), ...);
		}

		//void SetFigure(const Shape& t)
		//{
		//	vertexLayout = t.vertexLayout;
		//	vertexBuffer = t.vertexBuffer;
		//	indexBuffer = t.indexBuffer;
		//}
		//
		//void SetFigure(Shape&& t)
		//{
		//	vertexLayout = std::move(t.vertexLayout);
		//	vertexBuffer = std::move(t.vertexBuffer);
		//	indexBuffer = std::move(t.indexBuffer);
		//}

		void operator = (Shape&& t)
		{
			vertexLayout = std::move(t.vertexLayout);
			vertexBuffer = std::move(t.vertexBuffer);
			indexBuffer = std::move(t.indexBuffer);
			name = std::move(t.name);
		}

		void operator = (const Shape& t)
		{
			vertexLayout = t.vertexLayout;
			vertexBuffer = t.vertexBuffer;
			indexBuffer = t.indexBuffer;
			name = t.name;
		}

		VertexBufferLayout vertexLayout;
		Buffer vertexBuffer;
		Buffer indexBuffer;

		std::string name;
	};


}
