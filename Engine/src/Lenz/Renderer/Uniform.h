#pragma once
#include"Lenz/Core.h"
#include"Lenz/Renderer/Shader.h"
#include"Lenz/Renderer/Defines.h"
#include"Lenz/Utils/Vector.h"

namespace lenz
{
	class Uniform
	{
	public:
		Uniform(const char* name = "uUniform")
			: Name(name)
		{
		}

		inline void ChangeName(const char* name) { Name = name;	}

		virtual void UploadTo(Shader& shader) = 0;

		std::string Name;
	};

	struct UniformMat3 : public Uniform
	{
		UniformMat3(const char* name = "uUniformMat3")
			: Data(1.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformMat3(Name, Data);
		}

		glm::mat3 Data;
	};
	struct UniformMat4 : public Uniform
	{
		UniformMat4(const char* name = "uUniformInt")
			: Data(1.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformMat4(Name, Data);
		}

		glm::mat4 Data;
	};

	struct UniformInt : public Uniform	
	{
		UniformInt(const char* name = "uUniformInt")
			:Data(0), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformInt(Name, Data);
		}

		int Data;
	};
	struct UniformUnsignedInt : public Uniform
	{
		UniformUnsignedInt(const char* name = "uUniformInt")
			:Data(0), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformInt(Name, Data);
		}

		uint32_t Data;
	};
	struct UniformUnsignedInt2 : public Uniform
	{
		UniformUnsignedInt2(const char* name = "uUniformUnsignedInt2")
			: Data(0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformUnsignedInt2(Name, Data);
		}

		glm::uvec2 Data;
	};
	struct UniformUnsignedInt3 : public Uniform
	{
		UniformUnsignedInt3(const char* name = "uUniformUnsignedInt3")
			: Data(0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformUnsignedInt3(Name, Data);
		}
		glm::uvec3 Data;
	};
	struct UniformUnsignedInt4 : public Uniform
	{
		UniformUnsignedInt4(const char* name = "uUniformUnsignedInt4")
			: Data(0.0f, 0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformUnsignedInt4(Name, Data);
		}


		glm::uvec4 Data;
	};

	struct UniformFloat : public Uniform
	{
		UniformFloat(const char* name = "uUniformFloat")
			: Data(0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat(Name, Data);
		}

		float Data;
	};
	struct UniformFloat2 : public Uniform
	{
		UniformFloat2(const char* name = "uUniformFloat2")
			: Data(0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat2(Name, Data);
		}

		glm::vec2 Data;
	};
	struct UniformFloat3 : public Uniform
	{
		UniformFloat3(const char* name = "uUniformFloat3")
			: Data(0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat3(Name, Data);
		}

		glm::vec3 Data;
	};


	struct UniformFloat4 : public Uniform
	{
		UniformFloat4(const char* name = "uUniformFloat4")
			: Data(0.0f, 0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat4(Name, Data);
		}

		glm::vec4 Data;
	};

	template<uint32_t tsize>
	struct UniformIntArray : public Uniform 
	{
		UniformIntArray(const char* name, std::initializer_list<int> ints)
			: Size(tsize), Uniform(name)
		{
			memcpy(&Data[0], ints.begin(), ints.size());
		}

		void Rewrite(const std::initializer_list<int>& list)
		{
			memcpy(Data, list.begin(), list.size());
		}

		void UploadTo(Shader& shader)
		{
			std::initializer_list<int> list(&Data[0], &Data[tsize]);
			shader.UploadUniformIntArray(Name, list);
		}

		inline uint32_t GetSize() { return Size; }
		inline int& operator [] (size_t index) { return Data[index]; }

		uint32_t Size;
		int Data[tsize];
	};



	/*
		class Uniform
	{
	public:
		Uniform(const char* name = "uUniform")
			: Name(name)
		{
		}

		Uniform(Shader& shader, const char* name)
			: Name(name)
		{
			ChangeShader(shader);
		}

		void AddShader(Shader& shader)
		{
			LocationChache[&shader] = shader.GetUniformLocation(Name);
		}

		void ChangeShader(Shader& shader)
		{
			if (NameChache.find(&shader) == LocationChache.end())
				LocationChache[&shader] = shader.GetUniformLocation(Name);

			CurrentShader = &shader;
			Location = LocationChache[&shader];
		}

		void ChangeName(const char* name)
		{
			Name = name;

			for (auto& pair : LocationChache)
				pair.second = pair.first->GetUniformLocation(Name);

			Location = LocationChache[CurrentShader];
		}

		virtual void UploadTo(Shader& shader) = 0;

		virtual void UploadToAllShaders() = 0;

		uint32_t Location;
		Shader* CurrentShader;
		std::string Name;
		std::unordered_map<Shader*, uint32_t> LocationChache;
	};

	struct UniformMat3 : public Uniform
	{
		UniformMat3(const char* name = "uUniformMat3")
			: Data(1.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformMat3(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformMat3(pair.second, Data);
			}
		}

		glm::mat3 Data;
	};
	struct UniformMat4 : public Uniform
	{
		UniformMat4(const char* name = "uUniformInt")
			: Data(1.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformMat4(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformMat4(pair.second, Data);
			}
		}

		glm::mat4 Data;
	};

	struct UniformInt : public Uniform	
	{
		UniformInt(const char* name = "uUniformInt")
			:Data(0), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformInt(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformInt(pair.second, Data);
			}
		}

		int Data;
	};
	struct UniformUnsignedInt : public Uniform
	{
		UniformUnsignedInt(const char* name = "uUniformInt")
			:Data(0), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformInt(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformInt(pair.second, Data);
			}
		}

		uint32_t Data;
	};
	struct UniformUnsignedInt2 : public Uniform
	{
		UniformUnsignedInt2(const char* name = "uUniformUnsignedInt2")
			: Data(0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformUnsignedInt2(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformUnsignedInt2(pair.second, Data);
			}
		}

		glm::uvec2 Data;
	};
	struct UniformUnsignedInt3 : public Uniform
	{
		UniformUnsignedInt3(const char* name = "uUniformUnsignedInt3")
			: Data(0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformUnsignedInt3(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformUnsignedInt3(pair.second, Data);
			}
		}

		glm::uvec3 Data;
	};
	struct UniformUnsignedInt4 : public Uniform
	{
		UniformUnsignedInt4(const char* name = "uUniformUnsignedInt4")
			: Data(0.0f, 0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformUnsignedInt4(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformUnsignedInt4(pair.second, Data);
			}
		}

		glm::uvec4 Data;
	};

	struct UniformFloat : public Uniform
	{
		UniformFloat(const char* name = "uUniformFloat")
			: Data(0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformFloat(pair.second, Data);
			}
		}

		float Data;
	};
	struct UniformFloat2 : public Uniform
	{
		UniformFloat2(const char* name = "uUniformFloat2")
			: Data(0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat2(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformFloat2(pair.second, Data);
			}
		}

		glm::vec2 Data;
	};
	struct UniformFloat3 : public Uniform
	{
		UniformFloat3(const char* name = "uUniformFloat3")
			: Data(0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat3(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformFloat3(pair.second, Data);
			}
		}

		glm::vec3 Data;
	};
	struct UniformFloat4 : public Uniform
	{
		UniformFloat4(const char* name = "uUniformFloat4")
			: Data(0.0f, 0.0f, 0.0f, 0.0f), Uniform(name)
		{
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformFloat4(Name, Data);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformFloat4(pair.second, Data);
			}
		}

		glm::vec4 Data;
	};

	template<uint32_t tsize>
	struct UniformIntArray : public Uniform 
	{
		UniformIntArray(const char* name = "uUniformIntArray")
			: Size(tsize), Uniform(name)
		{
		}

		void Rewrite(const std::initializer_list<int>& list)
		{
			memcpy(Data, list.begin(), list.size());
		}

		void UploadTo(Shader& shader)
		{
			shader.UploadUniformIntArray(Name, Data, Size);
		}

		void UploadToAllShaders()
		{
			for (auto& pair : LocationChache)
			{
				pair.first->UploadUniformIntArray(pair.second, Data, Size);
			}
		}

		inline uint32_t GetSize() { return Size; }

		uint32_t Size;
		int Data[tsize];
	};
	*/

}