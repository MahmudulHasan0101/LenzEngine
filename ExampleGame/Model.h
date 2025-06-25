#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Lenz/Scene/Shape.h"
#include "Lenz/Scene/Scene.h"
#include "Lenz/Renderer/Texture.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

namespace lenz
{
    class Object : public Shape
    {
    public:
        std::vector<uint32_t> indices;
        std::vector<float> verticies;
        unsigned int vertexsize;


        Object(aiMesh* mesh, const aiScene* scene, const std::string& directory)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            bool hasNormalMap = HasTexture(material, aiTextureType_NORMALS);

            // Setting Indices
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            SetIndecies(indices);

            // Setting Verticies
            SetAttrib<float>(3);
            vertexsize = 3;

            if (mesh->HasNormals())
            {
                vertexsize += 3;
                SetAttrib<float>(3);
            }

            if (mesh->mTextureCoords[0])
            {
                vertexsize += 2;
                SetAttrib<float>(2);
                if (mesh->HasTangentsAndBitangents() && hasNormalMap)
                {
                    SetAttrib<float>(3);
                    SetAttrib<float>(3);
                    vertexsize += 6;
                }
            }

            size_t size = vertexsize * mesh->mNumVertices;
            verticies.resize(size);

            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                size_t offset = vertexsize * i;
                uint32_t u = 0;

                verticies[offset + u++] = mesh->mVertices[i].x;
                verticies[offset + u++] = mesh->mVertices[i].y;
                verticies[offset + u++] = mesh->mVertices[i].z;

                // normals
                if (mesh->HasNormals())
                {
                    verticies[offset + u++] = mesh->mNormals[i].x;
                    verticies[offset + u++] = mesh->mNormals[i].y;
                    verticies[offset + u++] = mesh->mNormals[i].z;
                }
                // texture coordinates
                if (mesh->mTextureCoords[0])
                {
                    verticies[offset + u++] = mesh->mTextureCoords[0][i].x;
                    verticies[offset + u++] = mesh->mTextureCoords[0][i].y;

                    if (mesh->HasTangentsAndBitangents() && hasNormalMap)
                    {
                        // tangent
                        verticies[offset + u++] = mesh->mTangents[i].x;
                        verticies[offset + u++] = mesh->mTangents[i].y;
                        verticies[offset + u++] = mesh->mTangents[i].z;

                        // bitangent
                        verticies[offset + u++] = mesh->mBitangents[i].x;
                        verticies[offset + u++] = mesh->mBitangents[i].y;
                        verticies[offset + u++] = mesh->mBitangents[i].z;
                    }
                }
            }

            SetVerticies(verticies);


            LoadTextures(material, aiTextureType_DIFFUSE, directory);
            LoadTextures(material, aiTextureType_SPECULAR, directory);
            LoadTextures(material, aiTextureType_AMBIENT, directory);
            LoadTextures(material, aiTextureType_EMISSIVE, directory);
            LoadTextures(material, aiTextureType_HEIGHT, directory);
            LoadTextures(material, aiTextureType_NORMALS, directory);
            LoadTextures(material, aiTextureType_SHININESS, directory);
            LoadTextures(material, aiTextureType_OPACITY, directory);
            LoadTextures(material, aiTextureType_DISPLACEMENT, directory);
            LoadTextures(material, aiTextureType_LIGHTMAP, directory);
            LoadTextures(material, aiTextureType_REFLECTION, directory);
            LoadTextures(material, aiTextureType_BASE_COLOR, directory);
            LoadTextures(material, aiTextureType_NORMAL_CAMERA, directory);
            LoadTextures(material, aiTextureType_EMISSION_COLOR, directory);
            LoadTextures(material, aiTextureType_METALNESS, directory);
            LoadTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, directory);
            LoadTextures(material, aiTextureType_AMBIENT_OCCLUSION, directory);
            LoadTextures(material, aiTextureType_SHEEN, directory);
            LoadTextures(material, aiTextureType_CLEARCOAT, directory);
        }

    private:
        bool HasTexture(aiMaterial* mat, aiTextureType type)
        {
            return 0 < mat->GetTextureCount(type);
        }

        void LoadTextures(aiMaterial* mat, aiTextureType type, const std::string& directory)
        {
            for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);

                textures.push_back(Storage::addTexture2D(directory + std::string(str.C_Str()), 0));
            }
        }
    };

    class Model
    {
    public:
        // constructor, expects a filepath to a 3D model.
        Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                LZ_CORE_ERROR(importer.GetErrorString());
                return;
            }

            directory = path.substr(0, path.find_last_of('/')) + "/";
            processNode(scene->mRootNode, scene);
        }


    private:
        void processNode(aiNode* node, const aiScene* scene)
        {
            for (size_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.emplace_back(mesh, scene, directory);
            }

            for (size_t i = 0; i < node->mNumChildren; i++)
            {
                processNode(node->mChildren[i], scene);
            }

        }

    public:
        std::vector<Object> meshes;
        std::string directory;
        bool gammaCorrection;

    };


}