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


namespace lenz
{
    class Object : public Shape
    {
    public:
        Object(aiMesh* mesh, const aiScene* scene, const std::string& directory)
        {
            // Setting Indices
            std::vector<uint32_t> indices;
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            SetIndecies(indices);

            // Setting Verticies
            uint32_t vertexsize = 3;
            SetAttrib<float>(3);

            if (mesh->HasNormals())
            {
                vertexsize += 3;
                SetAttrib<float>(3);
            }

            if (mesh->mTextureCoords[0])
            {
                vertexsize += 8;
                SetAttrib<float>(2);
                if (mesh->HasTangentsAndBitangents())
                {
                    SetAttrib<float>(3);
                    SetAttrib<float>(3);
                }
            }

            size_t size = vertexsize * mesh->mNumVertices;

            std::vector<float> vertexbuffer(size);

            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                size_t offset = vertexsize * i;
                uint32_t u = 0;

                vertexbuffer[offset + u++] = mesh->mVertices[i].x;
                vertexbuffer[offset + u++] = mesh->mVertices[i].y;
                vertexbuffer[offset + u++] = mesh->mVertices[i].z;

                // normals
                if (mesh->HasNormals())
                {
                    vertexbuffer[offset + u++] = mesh->mNormals[i].x;
                    vertexbuffer[offset + u++] = mesh->mNormals[i].y;
                    vertexbuffer[offset + u++] = mesh->mNormals[i].z;
                }
                // texture coordinates
                if (mesh->mTextureCoords[0])
                {
                    vertexbuffer[offset + u++] = mesh->mTextureCoords[0][i].x;
                    vertexbuffer[offset + u++] = mesh->mTextureCoords[0][i].y;

                    if (mesh->HasTangentsAndBitangents())
                    {
                        // tangent
                        vertexbuffer[offset + u++] = mesh->mTangents[i].x;
                        vertexbuffer[offset + u++] = mesh->mTangents[i].y;
                        vertexbuffer[offset + u++] = mesh->mTangents[i].z;

                        // bitangent
                        vertexbuffer[offset + u++] = mesh->mBitangents[i].x;
                        vertexbuffer[offset + u++] = mesh->mBitangents[i].y;
                        vertexbuffer[offset + u++] = mesh->mBitangents[i].z;
                    }
                }
            }

            SetVerticies(vertexbuffer);

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

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