#pragma once
#include<vector>
#include"Lenz/Core.h"

namespace lenz
{
	class Framebuffer
	{
	public:
		Framebuffer(size_t width = 0, size_t height = 0);
		~Framebuffer();

		void Init(size_t width, size_t height);

		inline uint32_t GetColorAttachmentID() const { return m_ColorAttachment; }
		inline uint32_t GetDepthAttachmentID() const { return m_DepthAttachment; }

		void Bind() const;
		void Unbind() const;

		inline uint32_t GetID() const { return m_ID; }
		inline size_t GetWidth() const { return Width; }
		inline size_t GetHeight() const { return Height; }

	private:
		size_t Width, Height;
		uint32_t m_ID;
		uint32_t  m_ColorAttachment;
		uint32_t m_DepthAttachment;
	};

}