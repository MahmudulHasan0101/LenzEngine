#pragma once
#include"glm/glm.hpp"
#include"Lenz/Renderer/Buffer.h"
#include"Lenz/Renderer/Texture.h"
#include"Lenz/Scene/Shape.h"

struct Vertex
{
	typedef glm::vec2 Position2D;

	typedef glm::vec3 Position,
					  Normal;

	typedef glm::vec4 Color;

	typedef glm::vec2 TextureCoord,
					  NormalmapCoord;

	typedef float TextureSlot,
				  NormapSlot;
};



class Square : public lenz::Shape
{
public:
	Square()
	{
		SetName("Square");

		SetVerticies({
			 -1.0f,  -1.0f,
			 1.0f,  -1.0f,
			 -1.0f,  1.0f,
			 1.0f,  1.0f
			});

		SetIndecies({ 
			0, 1, 2,
			1, 2, 3
			});

		SetAttrib<glm::vec2>();
	}
};




struct TexturedCubeShape : lenz::Shape
{
	TexturedCubeShape()
	{
		SetName("TexturedCubeShape");

		SetVerticies(
			{
				//   Coordinates
				-1.0f, -1.0f,  1.0f,//        7--------6
				 1.0f, -1.0f,  1.0f,//       /|       /|
				 1.0f, -1.0f, -1.0f,//      4--------5 |
				-1.0f, -1.0f, -1.0f,//      | |      | |
				-1.0f,  1.0f,  1.0f,//      | 3------|-2
				 1.0f,  1.0f,  1.0f,//      |/       |/
				 1.0f,  1.0f, -1.0f,//      0--------1
				-1.0f,  1.0f, -1.0f
			});

		SetIndecies(
			{
				// Right
				1, 2, 6,
				6, 5, 1,
				// Left
				0, 4, 7,
				7, 3, 0,
				// Top
				4, 5, 6,
				6, 7, 4,
				// Bottom
				0, 3, 2,
				2, 1, 0,
				// Back
				0, 1, 5,
				5, 4, 0,
				// Front
				3, 7, 6,
				6, 2, 3
			});

		SetAttrib<glm::vec2>();
	}
};



struct Cube : public lenz::Shape
{
	Cube()
	{
		SetName("Cube");

		SetVerticies({
			// front
			-1.0, -1.0,  1.0,
			 1.0, -1.0,  1.0,
			 1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// back
			-1.0, -1.0, -1.0,
			 1.0, -1.0, -1.0,
			 1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0
			});

		SetIndecies({
			// front
			0, 1, 2,
			2, 3, 0,
			// right
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// left
			4, 0, 3,
			3, 7, 4,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// top
			3, 2, 6,
			6, 7, 3
			});

		SetAttrib<glm::vec3>();
	}
};



