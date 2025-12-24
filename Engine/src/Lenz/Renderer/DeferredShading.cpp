#include "DeferredShading.h"
#include "Lenz/Renderer/Shader.h"

namespace lenz
{

	DeferredFramebuffer::DeferredFramebuffer()
	{

	}

	DeferredFramebuffer::DeferredFramebuffer(uint32_t width, uint32_t height)
	{
		Init(width, height);
	}

	void DeferredFramebuffer::Init(uint32_t width, uint32_t height)
	{
		glGenFramebuffers(1, &mID);
		glBindFramebuffer(GL_FRAMEBUFFER, mID);

		glGenTextures(1, &mPosition);
		glBindTexture(GL_TEXTURE_2D, mPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPosition, 0);

		glGenTextures(1, &mNormal);
		glBindTexture(GL_TEXTURE_2D, mNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormal, 0);

		glGenTextures(1, &mAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, mAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mAlbedoSpec, 0);

		uint32_t gDepth;
		glGenTextures(1, &gDepth);
		glBindTexture(GL_TEXTURE_2D, gDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		glDrawBuffers(3, attachments);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DeferredFramebuffer::Bind()
	{
		//glBindTexture(GL_TEXTURE_2D, mPosition);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPosition, 0);

		//glBindTexture(GL_TEXTURE_2D, mNormal);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormal, 0);

		//glBindTexture(GL_TEXTURE_2D, mAlbedoSpec);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mAlbedoSpec, 0);

		//unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		//glDrawBuffers(3, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, mID);
	}

	void DeferredFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void DeferredFramebuffer::BindTextures(uint8_t positionslot, uint8_t normalslot, uint8_t albedospecslot)
	{
		glActiveTexture(GL_TEXTURE0 + positionslot);
		glBindTexture(GL_TEXTURE_2D, mPosition);
		glActiveTexture(GL_TEXTURE0 + normalslot);
		glBindTexture(GL_TEXTURE_2D, mNormal);
		glActiveTexture(GL_TEXTURE0 + albedospecslot);
		glBindTexture(GL_TEXTURE_2D, mAlbedoSpec);
	}


	void DeferredFramebuffer::BindPositionTextureTo(uint8_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, mPosition);
	}

	void DeferredFramebuffer::BindNormalTextureTo(uint8_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, mNormal);
	}

	void DeferredFramebuffer::BindAlbedoSpecTextureTo(uint8_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, mAlbedoSpec);
	}


}