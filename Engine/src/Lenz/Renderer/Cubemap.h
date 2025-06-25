#pragma once
#include"Lenz/Core.h"
#include"stb_image.h"
#include"Lenz/Renderer/Shader.h"
#include"Lenz/Renderer/Texture.h"

namespace lenz
{
    class Cubemap : public Texture
    {
    public:
        Cubemap()
            : Texture(TextureType::CUBEMAP)
        {
        }

        Cubemap(const std::initializer_list<const char*>& faces, uint8_t slot)
            : Texture(TextureType::CUBEMAP)
        {
            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            int width, height, channels;

            for (unsigned int i = 0; i < faces.size(); i++)
            {
                stbi_set_flip_vertically_on_load(false);
                unsigned char* data = stbi_load(faces.begin()[i], &width, &height, &channels, 0);
                if (data)
                {

                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                    );
                    stbi_image_free(data);
                }
                else
                {
                    std::cout << "Cubemap texture failed to load at path: " << faces.begin()[i] << std::endl;
                    stbi_image_free(data);
                }
            }


            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
        }
    /*

        void Bind(uint8_t slot)
        { 
            m_SlotChache = slot;

            glActiveTexture(m_SlotChache);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
            glActiveTexture(0);
        }

        void Bind(uint8_t slot, Shader& shader, const char * name = "uSkyBox")
        {
            shader.Bind();
            Bind(slot);
            glUniform1ui(shader.GetUniformLocation(name), m_SlotChache);
            shader.Unbind();
        }

        void Bind(uint8_t slot, Shader& shader, uint32_t key)
        {
            Bind(slot);
            glUniform1ui(key, m_SlotChache);
        }

        void Bind(Shader& shader, uint32_t key)
        {
            shader.Bind();
            glUniform1ui(key, m_SlotChache);
            shader.Unbind();
        }

        void Bind(Shader& shader, const char* name = "uSkybox")
        {
            shader.Bind();
            glUniform1ui(shader.GetUniformLocation(name), m_SlotChache);
            shader.Unbind();
        }

        void Unbind()
        {
            if (m_SlotChache)
            {
                glActiveTexture(m_SlotChache);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                return;
            }
            LZ_CORE_WARN("Cubemap not binded to any textureSolt, so it cannot be unbinded.");
        }
        */
    };
    


}
