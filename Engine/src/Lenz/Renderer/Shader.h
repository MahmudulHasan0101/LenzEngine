#pragma once
#include "Lenz/Core.h"
#include "glm/glm.hpp"
#include "Lenz/Renderer/Defines.h"

namespace lenz
{
	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string GeometrySource;
		std::string FragmentSource;
	};

	class Shader
	{
	public:
		Shader(const char* path = nullptr);
		Shader(const ShaderProgramSource& source);

		void Init(const char* path);
		void Init(const ShaderProgramSource& source);
		virtual void SetDefault() {};

		void Bind() const;
		void Unbind() const;
		void Delete();

		unsigned int GetUniformLocation(const std::string& name);
#ifdef LZ_PRESERVE_SHADER_SOURCE
		LZ_DEBUG_ONLY(
			void PrintSources();
			void DeleteSource()
			);
#endif

		inline unsigned int GetID() const { return m_RendererID; };

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void UploadUniformIntArray(const std::string& name, std::initializer_list<int> value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformUnsignedInt(const std::string& name, uint32_t value);
		void UploadUniformUnsignedInt2(const std::string& name, const glm::uvec2& value);
		void UploadUniformUnsignedInt3(const std::string& name, const glm::uvec3& value);
		void UploadUniformUnsignedInt4(const std::string& name, const glm::uvec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformVec3f(const std::string& name, const glm::vec3& vec);

		void UploadUniformInt(uint32_t key, int value);
		void UploadUniformIntArray(uint32_t key, int* values, uint32_t count);
		void UploadUniformIntArray(uint32_t key, std::initializer_list<int> value);

		void UploadUniformFloat(uint32_t key, float value);
		void UploadUniformFloat2(uint32_t key, const glm::vec2& value);
		void UploadUniformFloat3(uint32_t key, const glm::vec3& value);
		void UploadUniformFloat4(uint32_t key, const glm::vec4& value);

		void UploadUniformUnsignedInt(uint32_t key, uint32_t value);
		void UploadUniformUnsignedInt2(uint32_t key, const glm::uvec2& value);
		void UploadUniformUnsignedInt3(uint32_t key, const glm::uvec3& value);
		void UploadUniformUnsignedInt4(uint32_t key, const glm::uvec4& value);

		void UploadUniformMat3(uint32_t key, const glm::mat3& matrix);
		void UploadUniformMat4(uint32_t key, const glm::mat4& matrix);
		void UploadUniformVec3f(uint32_t, const glm::vec3& vec);



	protected:
		ShaderProgramSource getFileSource(const std::string& filepath);

	public:
		std::unordered_map<std::string, unsigned int> UniformLocationChache;

#ifdef LZ_PRESERVE_SHADER_SOURCE
		LZ_DEBUG_ONLY(ShaderProgramSource source);
#endif

	protected:
		unsigned int m_RendererID{-1u};

		LZ_DEBUG_ONLY(std::string m_Name);
	};
}
