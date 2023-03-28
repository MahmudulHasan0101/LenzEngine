
#include"Shader.h"

#include<iostream>
#include<fstream>
#include<sstream>


#include<Lenz/Core.h>
#include<glm/gtc/type_ptr.hpp>


namespace lenz
{
//_______________________________________________________________________________________________________________________
//                                                      S H A D E R														 |
//_______________________________________________________________________________________________________________________|

	Shader::Shader(const char* path)
	{
		if (path != nullptr) Init(path);
	}

	ShaderProgramSource Shader::getFileSource(const std::string& filepath)
	{
		enum class Shadertype
		{
			NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2
		};

		std::fstream stream(filepath);
		if (!stream.is_open()) LZ_ASSERT(false);
		std::stringstream ss[3];
		Shadertype stype = Shadertype::NONE;


		std::string line;
		while (getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					stype = Shadertype::VERTEX;
				else if (line.find("geometry") != std::string::npos)
					stype = Shadertype::GEOMETRY;
				else if (line.find("fragment") != std::string::npos)
					stype = Shadertype::FRAGMENT;
				else if (line.find("end"))
					break;
			}
			else if (stype != Shadertype::NONE)
			{
				ss[(int)stype] << line << "\n";
			}
		}

		stream.close();
		return ShaderProgramSource{ ss[0].str(), ss[1].str(), ss[2].str() };
	}

#ifdef LZ_DEBUG
#ifdef LZ_PRESERVE_SHADER_SOURCE

	void Shader::PrintSources()
	{
		std::cout << "Vertex Shader:\n" << source.VertexSource << "\n";

		if (source.GeometrySource != "")
			std::cout << "Geometry Shader:\n" << source.GeometrySource << "\n";

		std::cout << "Fragment Shader:\n" << source.FragmentSource << "\n";
	}

	void Shader::DeleteSource()
	{
		source.VertexSource.clear();
		source.GeometrySource.clear();
		source.FragmentSource.clear();
	}
#endif


	static void compileErrors(unsigned int shader, const char* type)
	{
		GLint hasCompiled;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << infoLog << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
			if (hasCompiled == GL_FALSE)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "SHADER_LINKING_ERROR for: " << type << "\n" << infoLog << std::endl;
			}
		}
	}
#endif

	static uint32_t CompileShaderPart(uint32_t type, const std::string& source, const char* debugName)
	{
		if (source.size() == 0)
			return -1;

		const char* c_source = source.c_str();

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &c_source, NULL);
		glCompileShader(shader);
		LZ_DEBUG_ONLY(compileErrors(shader, debugName));

		return shader;
	}

	void Shader::Init(const char* path)
	{
		ShaderProgramSource sources = getFileSource(path);

		LZ_DEBUG_ONLY(m_Name = std::string(path).find_last_of("/"));

#ifdef LZ_PRESERVE_SHADER_SOURCE
		LZ_DEBUG_ONLY(this->source = sources);
#endif

		uint32_t vertexShader   = CompileShaderPart(GL_VERTEX_SHADER, sources.VertexSource, (m_Name + "Vertex").c_str());
		uint32_t geometryShader = CompileShaderPart(GL_GEOMETRY_SHADER, sources.GeometrySource, (m_Name + "Geometry").c_str());
		uint32_t fragmentShader = CompileShaderPart(GL_FRAGMENT_SHADER, sources.FragmentSource, (m_Name + "Fragment").c_str());

		m_RendererID = glCreateProgram();
		if (vertexShader)   glAttachShader(m_RendererID, vertexShader);
		if (geometryShader) glAttachShader(m_RendererID, geometryShader);
		if (fragmentShader) glAttachShader(m_RendererID, fragmentShader);

		glLinkProgram(m_RendererID);

		LZ_DEBUG_ONLY(compileErrors(m_RendererID, (m_Name + "SHADER").c_str()));

#ifndef LZ_PRESERVE_SHADER
		if (vertexShader) glDeleteShader(vertexShader);
		if (geometryShader) glDeleteShader(geometryShader);
		if (fragmentShader) glDeleteShader(fragmentShader);
#endif
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Delete()
	{
		glDeleteProgram(m_RendererID);
	}


	uint32_t Shader::GetUniformLocation(const std::string& name)
	{
		if (this == nullptr) LZ_CORE_ASSERT(false);
		if (UniformLocationChache.find(name) != UniformLocationChache.end())
			return UniformLocationChache[name];
		
		uint32_t location = glGetUniformLocation(m_RendererID, name.c_str());

		if (location == -1)
		{
			LZ_DEBUG_ONLY(std::cout << "[" << m_Name << "] Warning: uniform '" << name << "' doesn't existes!\n");
			return -1;
		}

		UniformLocationChache[name] = location;
		return location;
	}



	// Uniforms:

	void Shader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}

	void Shader::UploadUniformIntArray(const std::string& name, std::initializer_list<int> value)
	{
		UploadUniformIntArray(name, (int*)value.begin(), value.size());
	}

	void Shader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}


	void Shader::UploadUniformUnsignedInt(const std::string& name, uint32_t value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1ui(location, value);
	}

	void Shader::UploadUniformUnsignedInt2(const std::string& name, const glm::uvec2& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2ui(location, value.x, value.y);
	}

	void Shader::UploadUniformUnsignedInt3(const std::string& name, const glm::uvec3& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3ui(location, value.x, value.y, value.z);
	}

	void Shader::UploadUniformUnsignedInt4(const std::string& name, const glm::uvec4& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4ui(location, value.x, value.y, value.z, value.w);
	}


	void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}




	void Shader::UploadUniformInt(uint32_t key, int value)
	{
		glUniform1i(key, value);
	}

	void Shader::UploadUniformIntArray(uint32_t key, int* values, uint32_t count)
	{
		glUniform1iv(key, count, values);
	}

	void Shader::UploadUniformIntArray(uint32_t key, std::initializer_list<int> value)
	{
		UploadUniformIntArray(key, (int*)value.begin(), value.size());
	}

	void Shader::UploadUniformFloat(uint32_t key, float value)
	{
		glUniform1f(key, value);
	}

	void Shader::UploadUniformFloat2(uint32_t key, const glm::vec2& value)
	{
		glUniform2f(key, value.x, value.y);
	}

	void Shader::UploadUniformFloat3(uint32_t key, const glm::vec3& value)
	{
		glUniform3f(key, value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(uint32_t key, const glm::vec4& value)
	{
		glUniform4f(key, value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniformUnsignedInt(uint32_t key, uint32_t value)
	{
		glUniform1ui(key, value);
	}

	void Shader::UploadUniformUnsignedInt2(uint32_t key, const glm::uvec2& value)
	{
		glUniform2ui(key, value.x, value.y);
	}

	void Shader::UploadUniformUnsignedInt3(uint32_t key, const glm::uvec3& value)
	{
		glUniform3ui(key, value.x, value.y, value.z);
	}

	void Shader::UploadUniformUnsignedInt4(uint32_t key, const glm::uvec4& value)
	{
		glUniform4ui(key, value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniformMat3(uint32_t key, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(key, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4(uint32_t key, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(key, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}