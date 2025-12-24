#include "Text.h"
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include <fstream>


namespace lenz
{
    TextRenderer::TextRenderer()
    {
        if (DEFAULTSHADER.GetID() == -1u)
            DEFAULTSHADER = Shader({
        R"(
// text.vert
#version 330 core
layout(location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 u_camera;

void main() {
    gl_Position = u_camera * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)",
        R"(
)",
        R"(
// text.frag
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    float alpha = texture(text, TexCoords).r;
    FragColor = vec4(textColor, alpha);
}
)"
            });
    }

    bool TextRenderer::LoadFont(const std::string& fontPath, int fontSize) {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cerr << "Could not init FreeType Library\n";
            return false;
        }

        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            std::cerr << "Failed to load font\n";
            return false;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

        for (GLubyte c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cerr << "Failed to load Glyph: " << c << "\n";
                continue;
            }

            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                face->glyph->bitmap.width, face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (GLuint)face->glyph->advance.x
            };
            Characters.insert(std::pair<GLchar, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // Setup VAO/VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

    void TextRenderer::RenderText(const std::string& text, glm::vec2 position, glm::vec3 color, float scale) {
        activeShader->Bind();
        activeShader->UploadUniformFloat3("textColor", color);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        for (auto c : text) {
            Character ch = Characters[c];

            float xpos = position.x + ch.Bearing.x * scale;
            float ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Update content
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            position.x += (ch.Advance >> 6) * scale; // advance is in 1/64 pixels
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Shader TextRenderer::DEFAULTSHADER;






    SDFTextRenderer::SDFTextRenderer()
    {
        if (DEFAULTSHADER.GetID() == -1u)
            DEFAULTSHADER = Shader(
            {
                R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;
uniform mat4 u_camera;

void main() {
    gl_Position = u_camera * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)",

R"()",

R"(
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sdfAtlas;
uniform vec3 textColor = vec3(0.2);

void main() {
    float dist = texture(sdfAtlas, TexCoords).r;
    float alpha = smoothstep(0.48, 0.52, dist);
    FragColor = vec4(textColor, alpha);
}
)"
            });

    }




    std::string readFile(const char* filename) {
        FILE* file = std::fopen(filename, "rb"); // "rb" = read binary
        if (!file) return "";

        std::fseek(file, 0, SEEK_END);
        size_t size = std::ftell(file);
        std::rewind(file);

        std::vector<char> buffer(size + 1);
        std::fread(buffer.data(), 1, size, file);
        buffer[size] = '\0';

        std::fclose(file);
        return std::string(buffer.data());
    }



    void SDFTextRenderer::LoadFont(const std::string& jsonPath, const std::string& texturePath)
    {
        texture.Init(texturePath, 0);
        LoadFontData(jsonPath);
        SetupBuffers();
    }

    void SDFTextRenderer::LoadFontData(const std::string& jsonPath) {
        if (!std::filesystem::exists(jsonPath)) {
            std::cerr << "File not found: " << jsonPath << "\n";
        }

        nlohmann::json j = nlohmann::json::parse(readFile(jsonPath.c_str()));

        auto atlasW = j["atlas"]["width"].get<float>();
        auto atlasH = j["atlas"]["height"].get<float>();

        for (auto& g : j["glyphs"]) {
            char ch = static_cast<char>(g["unicode"].get<int>());
            Glyph glyph;
            glyph.uvMin = glm::vec2(g["atlasBounds"]["left"], g["atlasBounds"]["top"]) / glm::vec2(atlasW, atlasH);
            glyph.uvMax = glm::vec2(g["atlasBounds"]["right"], g["atlasBounds"]["bottom"]) / glm::vec2(atlasW, atlasH);

            glyph.size = glm::vec2(g["planeBounds"]["right"], g["planeBounds"]["bottom"]) -
                glm::vec2(g["planeBounds"]["left"], g["planeBounds"]["top"]);
            glyph.offset = glm::vec2(g["planeBounds"]["left"], g["planeBounds"]["top"]);
            glyph.advance = g["advance"];

            m_Glyphs[ch] = glyph;
        }
    }


    void SDFTextRenderer::SetupBuffers() {
        m_Buffer = Buffer(BufferType::VertexBuffer);
        m_Buffer.Init(sizeof(float) * 6 * 4);
        BufferLayout vbl;
        vbl.push_back(BufferElement(GL_FLOAT, 3, GL_FALSE));
        m_ArrayBuffer.Init();
        m_ArrayBuffer.Compile(&m_Buffer, nullptr, nullptr, &vbl, nullptr, nullptr);
    }

    void SDFTextRenderer::RenderText(const std::string& text, glm::vec2 position, glm::vec3 color, float scale) {
        activeShader->Bind();

        texture.BindToSlot(0);
        m_ArrayBuffer.Bind();

        float x = position.x;
        float y = position.y;

        for (char c : text) {
            auto it = m_Glyphs.find(c);
            if (it == m_Glyphs.end()) continue;
            Glyph& g = it->second;

            float xpos = x + g.offset.x * scale;
            float ypos = y - g.offset.y * scale;

            float w = g.size.x * scale;
            float h = g.size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h, g.uvMin.x, g.uvMax.y },
                { xpos,     ypos,     g.uvMin.x, g.uvMin.y },
                { xpos + w, ypos,     g.uvMax.x, g.uvMin.y },
                { xpos,     ypos + h, g.uvMin.x, g.uvMax.y },
                { xpos + w, ypos,     g.uvMax.x, g.uvMin.y },
                { xpos + w, ypos + h, g.uvMax.x, g.uvMax.y },
            };

            m_Buffer.InvalidateData(&vertices, sizeof(vertices));
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += g.advance * scale;
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }


    Shader SDFTextRenderer::DEFAULTSHADER;
}