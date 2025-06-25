
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

	Shader::Shader(const ShaderProgramSource& source)
	{
		Init(source);
	}

	ShaderProgramSource Shader::getFileSource(const std::string& filepath)
	{
		enum class Shadertype
		{
			NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2
		};

		std::fstream stream(filepath);
		if (!stream.is_open()) LZ_ASSERT(false, "Shader file doesn't exist");
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
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << infoLog << std::endl;
		}
	}

	static void compileLinkStatus(unsigned int shader, const char* name)
	{
		GLint hasLinked;
		char infoLog[1024];
		glGetProgramiv(shader, GL_LINK_STATUS, &hasLinked);
		if (hasLinked == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for: " << name << "\n" << infoLog << std::endl;
		}
	}
#endif

	static uint32_t CompileShaderPart(uint32_t type, const std::string& source, const char* debugName)
	{
		if (source.size() < 5)
			return -1;

		const char* c_source = source.c_str();

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &c_source, NULL);
		glCompileShader(shader);
		LZ_DEBUG_ONLY(compileErrors(shader, debugName));

		return shader;
	}

	void Shader::Init(const ShaderProgramSource& sources)
	{
#ifdef LZ_PRESERVE_SHADER_SOURCE
		LZ_DEBUG_ONLY(this->source = sources);
#endif
		LZ_DEBUG_RELEASE_SWAP(
		uint32_t vertexShader = CompileShaderPart(GL_VERTEX_SHADER, sources.VertexSource, (m_Name + "-Vertex").c_str());
		uint32_t geometryShader = CompileShaderPart(GL_GEOMETRY_SHADER, sources.GeometrySource, (m_Name + "-Geometry").c_str());
		uint32_t fragmentShader = CompileShaderPart(GL_FRAGMENT_SHADER, sources.FragmentSource, (m_Name + "-Fragment").c_str());
		,
		uint32_t vertexShader = CompileShaderPart(GL_VERTEX_SHADER, sources.VertexSource, nullptr);
		uint32_t geometryShader = CompileShaderPart(GL_GEOMETRY_SHADER, sources.GeometrySource, nullptr);
		uint32_t fragmentShader = CompileShaderPart(GL_FRAGMENT_SHADER, sources.FragmentSource, nullptr);
		)

		m_RendererID = glCreateProgram();
		if (vertexShader != UINT32_MAX)   glAttachShader(m_RendererID, vertexShader);
		if (geometryShader != UINT32_MAX) glAttachShader(m_RendererID, geometryShader);
		if (fragmentShader != UINT32_MAX) glAttachShader(m_RendererID, fragmentShader);

		glLinkProgram(m_RendererID);

		LZ_DEBUG_ONLY(compileLinkStatus(m_RendererID, (m_Name).c_str()));

#ifndef LZ_PRESERVE_SHADER
		if (vertexShader != UINT32_MAX) glDeleteShader(vertexShader);
		if (geometryShader != UINT32_MAX) glDeleteShader(geometryShader);
		if (fragmentShader != UINT32_MAX) glDeleteShader(fragmentShader);
#endif
	}

	void Shader::Init(const char* path)
	{
		ShaderProgramSource sources = getFileSource(path);

		LZ_DEBUG_ONLY(
			m_Name = std::string(path);
			uint32_t offset = m_Name.find_last_of('/', m_Name.size());
			m_Name = m_Name.substr().substr(offset+1, m_Name.size()-offset);
		);

		Init(sources);
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
		if (this == nullptr) LZ_CORE_ASSERT(false, "Shader not intellized");
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
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}

	void Shader::UploadUniformIntArray(const std::string& name, std::initializer_list<int> value)
	{
		UploadUniformIntArray(name, (int*)value.begin(), value.size());
	}

	void Shader::UploadUniformFloat(const std::string& name, float value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}


	void Shader::UploadUniformUnsignedInt(const std::string& name, uint32_t value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform1ui(location, value);
	}

	void Shader::UploadUniformUnsignedInt2(const std::string& name, const glm::uvec2& value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform2ui(location, value.x, value.y);
	}

	void Shader::UploadUniformUnsignedInt3(const std::string& name, const glm::uvec3& value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform3ui(location, value.x, value.y, value.z);
	}

	void Shader::UploadUniformUnsignedInt4(const std::string& name, const glm::uvec4& value)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform4ui(location, value.x, value.y, value.z, value.w);
	}


	void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformVec3f(const std::string& name, const glm::vec3& vec)
	{
		Bind();
		GLint location = GetUniformLocation(name);
		glUniform3fv(location, 1, glm::value_ptr(vec));
	}



	void Shader::UploadUniformInt(uint32_t key, int value)
	{
		Bind();
		glUniform1i(key, value);
	}

	void Shader::UploadUniformIntArray(uint32_t key, int* values, uint32_t count)
	{
		Bind();
		glUniform1iv(key, count, values);
	}

	void Shader::UploadUniformIntArray(uint32_t key, std::initializer_list<int> value)
	{
		UploadUniformIntArray(key, (int*)value.begin(), value.size());
	}

	void Shader::UploadUniformFloat(uint32_t key, float value)
	{
		Bind();
		glUniform1f(key, value);
	}

	void Shader::UploadUniformFloat2(uint32_t key, const glm::vec2& value)
	{
		Bind();
		glUniform2f(key, value.x, value.y);
	}

	void Shader::UploadUniformFloat3(uint32_t key, const glm::vec3& value)
	{
		Bind();
		glUniform3f(key, value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(uint32_t key, const glm::vec4& value)
	{
		Bind();
		glUniform4f(key, value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniformUnsignedInt(uint32_t key, uint32_t value)
	{
		Bind();
		glUniform1ui(key, value);
	}

	void Shader::UploadUniformUnsignedInt2(uint32_t key, const glm::uvec2& value)
	{
		Bind();
		glUniform2ui(key, value.x, value.y);
	}

	void Shader::UploadUniformUnsignedInt3(uint32_t key, const glm::uvec3& value)
	{
		Bind();
		glUniform3ui(key, value.x, value.y, value.z);
	}

	void Shader::UploadUniformUnsignedInt4(uint32_t key, const glm::uvec4& value)
	{
		Bind();
		glUniform4ui(key, value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniformMat3(uint32_t key, const glm::mat3& matrix)
	{
		Bind();
		glUniformMatrix3fv(key, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4(uint32_t key, const glm::mat4& matrix)
	{
		Bind();
		glUniformMatrix4fv(key, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformVec3f(uint32_t key, const glm::vec3& vec)
	{
		Bind();
		glUniform3fv(key, 1, (GLfloat*)glm::value_ptr(vec));
	}



}