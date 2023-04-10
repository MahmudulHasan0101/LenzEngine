#include "Shape.h"

namespace lenz
{
	Shape::Shape()
		: vertexBuffer(BufferType::VertexBuffer), indexBuffer(BufferType::IndexBuffer)
	{
	}

	void Shape::SetName(const std::string& str)
	{
		name = str;
	}

	void Shape::Destroy()
	{
		vertexBuffer.Destroy();
		indexBuffer.Destroy();
		//Verticies.Delete();
		//Indecies.Delete();
	}

	//void Shape::Init(ShapeType type)
	//{
	//	uint32_t drawType = GL_STATIC_DRAW;
	//	if (type == ShapeType::Dynamic)
	//		drawType = GL_DYNAMIC_DRAW;
	//
	//	VertexBuffer.Init(Verticies.Data(), Verticies.Size<void>(), drawType);
	//	IndexBuffer.Init(Indecies.Data(), Indecies.Size<void>(), drawType);
	//}

	void Shape::SetVerticies(std::initializer_list<float> data, uint32_t fluidity)
	{
		
		vertexBuffer.Init(data.begin(), sizeof(float) * data.size(), fluidity);
		//Verticies.Print<float>();
	}

	void Shape::SetIndecies(std::initializer_list<uint32_t> data, uint32_t fluidity)
	{
		indexBuffer.Init(data.begin(), sizeof(uint32_t) * data.size(), fluidity);
		//Indecies.Print<uint32_t>();
	}


}