#pragma once
#include "Lenz/Core.h"
#include "Lenz/Renderer/Shader.h"
#include "Lenz/Renderer/Buffer.h"
#include "Lenz/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <ft2build.h>
#include <string>
#include <map>
#include FT_FREETYPE_H


namespace lenz {
    struct Character {
        GLuint TextureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        GLuint Advance;
    };

    class TextRenderer {
    public:
        TextRenderer();
        std::map<GLchar, Character> Characters;
        GLuint VAO, VBO;

        bool LoadFont(const std::string& fontPath, int fontSize);
        void RenderText(const std::string& text, glm::vec2 position, glm::vec3 color, float scale);

        static Shader DEFAULTSHADER;
        Shader* activeShader{&DEFAULTSHADER};
    };


    struct Glyph {
        glm::vec2 uvMin, uvMax;
        glm::vec2 size;
        glm::vec2 offset;
        float advance;
    };

    class SDFTextRenderer {
    public:
        SDFTextRenderer();
        void LoadFont(const std::string& jsonPath, const std::string& texturePath);
        void RenderText(const std::string& text, glm::vec2 position, glm::vec3 color, float scale);

        Shader* activeShader{ &DEFAULTSHADER };
        Texture2D texture;
        static Shader DEFAULTSHADER;
    private:
        std::unordered_map<char, Glyph> m_Glyphs;
        Buffer m_Buffer;
        ArrayBuffer m_ArrayBuffer;

        void LoadFontData(const std::string& jsonPath);
        void SetupBuffers();
    };
}

