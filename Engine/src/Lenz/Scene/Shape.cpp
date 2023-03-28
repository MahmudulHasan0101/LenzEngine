#include "Shape.h"
#include "Scene.h"

namespace lenz
{
	Shape::Shape()
		: VertexBuffer(BufferType::VertexBuffer), IndexBuffer(BufferType::IndexBuffer)
	{
	}

	void Shape::Destroy()
	{
		VertexBuffer.Destroy();
		IndexBuffer.Destroy();
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
		
		VertexBuffer.Init(data.begin(), sizeof(float) * data.size(), fluidity);
		//Verticies.Print<float>();
	}

	void Shape::SetIndecies(std::initializer_list<uint32_t> data, uint32_t fluidity)
	{
		IndexBuffer.Init(data.begin(), sizeof(uint32_t) * data.size(), fluidity);
		//Indecies.Print<uint32_t>();
	}


}