#include"Uniform.h"

namespace lenz
{
	/*

	void UniformBuffer::Init(Shader* shader, const char* name, uint32_t size)
	{
		Array.Init(size);
		Name = name;
		ShaderPtr = shader;

		glGenBuffers(1, &ID);
		Bind();
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ID, 0, size);

		BlockIndex = (uint32_t)glGetUniformBlockIndex(ShaderPtr->GetID(), Name.c_str());

		if (BlockIndex != GL_INVALID_INDEX) {
			LZ_CORE_ASSERT(false);
			glGetActiveUniformBlockiv(ShaderPtr->GetID(), BlockIndex, GL_UNIFORM_BLOCK_BINDING, (GLint*)&BindingPoint);
			glUniformBlockBinding(ShaderPtr->GetID(), BlockIndex, BindingPoint);
		}
		Unbind();

	}


	void UniformBuffer::Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ID);
	}

	void UniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBuffer::Upload()
	{
		Bind();
		glBufferSubData(ID, 0, Array.Stride(), Array.Data());
		Unbind();
	}

	void UniformBuffer::ChangeShader(Shader* shader)
	{
		ShaderPtr = shader;
		BindingPoint = glGetUniformBlockIndex(ShaderPtr->GetID(), Name.c_str());
		glUniformBlockBinding(ShaderPtr->GetID(), BlockIndex, BindingPoint);
	}


	*/
	/*


	template<>
	void UploadUniform<(uint32_t)UniformType::None>(void* uniform)
	{
	}

	template<>
	void UploadUniform<UniformType::Int>(void* uniform)
	{
		UniformInt* resolved_uniform = (UniformInt*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformInt(resolved_uniform->Location, resolved_uniform->Data);
	}

	template<>
	void UploadUniform<UniformType::Float>(void* uniform)
	{
		UniformFloat* resolved_uniform = (UniformFloat*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformFloat(resolved_uniform->Location, resolved_uniform->Data);
	}

	template<>
	void UploadUniform<UniformType::IntArray>(void* uniform)
	{
		UniformIntArray<1>* resolved_uniform = (UniformIntArray<1>*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformIntArray(resolved_uniform->Location, resolved_uniform->Data, resolved_uniform->GetSize());
	}

	template<>
	void UploadUniform<UniformType::Float2>(void* uniform)
	{
		UniformFloat2* resolved_uniform = (UniformFloat2*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformFloat2(resolved_uniform->Location, resolved_uniform->Data);
	}

	template<>
	void UploadUniform<UniformType::Float3>(void* uniform)
	{
		UniformFloat3* resolved_uniform = (UniformFloat3*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformFloat3(resolved_uniform->Location, resolved_uniform->Data);
	}

	template<>
	void UploadUniform<UniformType::Float4>(void* uniform)
	{
		UniformFloat4* resolved_uniform = (UniformFloat4*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformFloat4(resolved_uniform->Location, resolved_uniform->Data);
	}

	template<>
	void UploadUniform<UniformType::Mat3>(void* uniform)
	{
		UniformMat3* resolved_uniform = (UniformMat3*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformMat3(resolved_uniform->Location, resolved_uniform->Data);
	}

	template<>
	void UploadUniform<UniformType::Mat4>(void* uniform)
	{
		UniformMat4* resolved_uniform = (UniformMat4*)uniform;
		resolved_uniform->ShaderPtr->UploadUniformMat4(resolved_uniform->Location, resolved_uniform->Data);
	}


	*/
}