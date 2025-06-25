#include "Shape.h"

#include <memory>

namespace lenz
{
	Shape::Shape()
		: vertexBuffer(BufferType::VertexBuffer), indexBuffer(BufferType::IndexBuffer)
	{
	}


	Shape::Shape(const char* binaryfile)
	{
		
	}


//	void Shape::ChacheBinary(const char* filename)
//	{
//		if (filename == "")
//			filename = (name + ".lsb").c_str();
//
//		int file = open(filename, 'wb');
//
//		if (!file)
//		{
//			LZ_CORE_ASSERT("Could'nt write chache binary file");
//			return;
//		}
//
//		size_t vbsize = vertexBuffer.GetSize();
//		size_t ibsize = indexBuffer.GetSize();
//		size_t vblsize = (vertexLayout.elements.size() * sizeof(BufferElement)) + sizeof(uint32_t);
//		size_t namesize = name.size();
//
//		size_t totalsize = vbsize + ibsize + vblsize + namesize;
//	}


	Shape::Shape(Shape&& shape)
	{
		this->name = std::move(shape.name);
		this->vertexBuffer = std::move(shape.vertexBuffer);
		this->indexBuffer = std::move(shape.indexBuffer);
		this->vertexLayout = std::move(shape.vertexLayout);
		this->textures = std::move(shape.textures);

		LZ_DEBUG_RELEASE_SWAP(shape.name = shape.name + " - moved");
	}

	Shape::~Shape()
	{
		Destroy();
	}

	void Shape::SetName(const std::string& str)
	{
		name = str;
	}

	void Shape::Destroy()
	{
		vertexBuffer.Destroy();
		indexBuffer.Destroy();
		//shape-> Release();
	}

	void Shape::SetVerticies(std::initializer_list<float> data, uint32_t fluidity)
	{
		vertexBuffer.Init(data.begin(), sizeof(float) * data.size(), fluidity);

	}

	void Shape::SetVerticies(std::vector<float> data, uint32_t fluidity)
	{

		vertexBuffer.Init(data.data(), data.size() * sizeof(float), fluidity);
	}

	void Shape::SetIndecies(std::initializer_list<uint32_t> data, uint32_t fluidity)
	{
		indexBuffer.Init(data.begin(), sizeof(uint32_t) * data.size(), fluidity);
	}

	void Shape::SetIndecies(std::vector<uint32_t> data, uint32_t fluidity)
	{
		indexBuffer.Init(data.data(), data.size() * sizeof(uint32_t), fluidity);
	}

}