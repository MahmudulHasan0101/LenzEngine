#pragma once
/*
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include"Shape.h"
#include"Lenz/Core.h"
//#include"Component.h"

namespace lenz
{

	class Models
	{
	public:
		Models(const std::string& path)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile("my_model.fbx", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				LZ_CORE_ERROR("Couldn't load the model");
				return;
			}

			for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
				const aiMesh* mesh = scene->mMeshes[i];
				
			}

			importer.FreeScene();
		}

	private:
		std::vector<Shape> m_shapes;

	};

}
*/