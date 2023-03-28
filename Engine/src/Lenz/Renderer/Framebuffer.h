#pragma once
#include<vector>
#include"Lenz/Core.h"

namespace lenz
{
	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;
		bool SwapChainTarget = false;
	};


	class Framebuffer
	{
	public:
		Framebuffer();
		~Framebuffer();

		void Init(const FramebufferSpecification& spec);
		void Invalidate();

		inline uint32_t GetColorAttachmentID() const { return m_ColorAttachment; }
		inline uint32_t GetDepthAttachmentID() const { return m_DepthAttachment; }

		void Bind() const;
		void Unbind() const;

		const inline FramebufferSpecification& GetSpecification() const { return m_Specification; }
		inline uint32_t GetID() const { return m_ID; }

	private:
		uint32_t m_ID;
		uint32_t  m_ColorAttachment;
		uint32_t m_DepthAttachment;
		FramebufferSpecification m_Specification;
	};

}