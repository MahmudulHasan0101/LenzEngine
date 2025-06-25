#pragma once
#include "Lenz/Core.h"
#include "Lenz/Renderer/Shader.h"  

namespace lenz
{

	class DeferredFramebuffer
	{
	public:
		DeferredFramebuffer();
		DeferredFramebuffer(uint32_t width, uint32_t height);

		void Init(uint32_t width, uint32_t height);

		inline uint32_t getID() const { return mID; }
		inline uint32_t getPositionTexture() const { return mPosition; }
		inline uint32_t getNormalTexture() const { return mNormal; }
		inline uint32_t getAlbedoSpecTexture() const { return mAlbedoSpec; }

		void Bind();
		void Unbind();
		void BindPositionTextureTo(uint8_t slot);
		void BindNormalTextureTo(uint8_t slot);
		void BindAlbedoSpecTextureTo(uint8_t slot);

		void BindTextures(uint8_t positionslot, uint8_t normalslot, uint8_t albedospecslot);

	private:
		uint32_t mID;
		uint32_t mPosition;
		uint32_t mNormal;
		uint32_t mAlbedoSpec;
	};

}
