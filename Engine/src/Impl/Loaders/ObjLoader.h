#pragma once
#include"Lenz.h"
/*#define LZ_EXPORTING_MODLES

namespace lenz
{
	class ObjModel
	{
	public:
		ObjModel(const char* path)
			: Verticies(10000),
			Indecies(10000)
		{
			FILE* file = fopen(path, "r");
			LZ_ASSERT(file == NULL, "Could not open the file.");

			uint64_t line = 0,
				vline = 0, 
				vtline = 0,
				vnline = 0,
				fline = 0;

			while (true) {
				char lineHeader[128];
				// read the first word of the line
				int res = fscanf(file, "%s", lineHeader);

				if (res == EOF)
					break; // EOF = End Of File. Quit the loop.

				if (strcmp(lineHeader, "v") == 0) {
					glm::vec3 vertex;
					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					Verticies.Write(vline, ).push_back(vertex);
					vline++;
				}
				else if (strcmp(lineHeader, "vt") == 0) {
					glm::vec2 uv;
					fscanf(file, "%f %f\n", &uv.x, &uv.y);
					Verticies.push_back(uv);
					vtline++;
				}
				else if (strcmp(lineHeader, "vn") == 0) {
					glm::vec3 normal;
					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
					Verticies.push_back(normal);
					vnline++
				}
				else if (strcmp(lineHeader, "f") == 0) {
					std::string vertex1, vertex2, vertex3;
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (matches != 9) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
					fline++;
				}

				line++;
			}
		}


		VoidArray Indecies;
		BlockArray Verticies;
		std::vector<BufferElement> Elements;
	};

}
*/