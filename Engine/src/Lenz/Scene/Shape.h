#pragma once
#include"Lenz/Core.h"
#include"Lenz/Renderer/Buffer.h"
#include"Lenz/Inputes.h"

namespace lenz
{
	//enum class ShapeType
	//{
	//	None = 0, 
	//	Dynamic, 
	//	Static
	//};


	class Shape
	{
	public:
		Shape();
		Shape(const Shape&);

		void Destroy();

		virtual void OnUpdate() {}

		//void Init(ShapeType type = ShapeType::Static);

		void SetVerticies(std::initializer_list<float> data, uint32_t bufferfluidity = BufferFluidity::Static);

		void SetIndecies(std::initializer_list<uint32_t> data, uint32_t bufferfluidity = BufferFluidity::Static);


		void SetAttrib(lenz::BufferElement& element)
		{
			VertexLayout.AddVertexElement(element);
		}

		template<typename T>
		void SetAttrib(uint32_t count)
		{
			uint32_t type = GetOpenGLType<T>();
			VertexLayout.AddVertexElement(type, count, type == GL_FLOAT ? GL_FALSE : GL_TRUE);
		}

		template<typename T, typename SubT = float>
		void SetAttrib()
		{
			uint32_t type = GetOpenGLType<SubT>();
			uint32_t count = sizeof(T) / sizeof(SubT);
			VertexLayout.AddVertexElement(type, count, type == GL_FLOAT ? GL_FALSE : GL_TRUE);
		}

		template<typename ... Args>
		void SetAttribs()  // Warning: Always counts subT = float
		{
			(SetAttrib<Args>(), ...);
		}

		void SetVertex(uint32_t vertexNo, uint32_t elementNo, std::initializer_list<float> data)
		{
			uint32_t vertexoffset = vertexNo * VertexLayout.GetStride();
			uint32_t elementoffset = &VertexLayout.GetVertexElements()[elementNo] - &VertexLayout.GetVertexElements()[0];
			uint32_t offset = vertexoffset + elementoffset;

			VertexBuffer.SetData(offset, data.begin(), data.size());
		}


		inline operator Shape& () { return *(Shape*)this; }
		
		template<typename T>
		void operator = (T&& t)
		{
			VertexLayout = std::move(t.VertexLayout);
			VertexBuffer = std::move(t.VertexBuffer);
			IndexBuffer = std::move(t.IndexBuffer);
			//Verticies =	std::move(t.Verticies);
			//Indecies = std::move(t.Indecies);
		}

		template<>
		void operator = <LZ_NULL>(LZ_NULL&& t) {}

		BufferLayout VertexLayout;
		Buffer VertexBuffer;
		Buffer IndexBuffer;
		//VoidArray Verticies;
		//BlockArray Verticies;
		//VoidArray Indecies;
	};

	class ComplexShape : public Shape
	{
	public:

	};

}
