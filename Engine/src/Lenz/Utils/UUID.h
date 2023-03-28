#pragma once
#include"Lenz/Core.h"

namespace lenz {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};



#define LZ_CREATE_LOCAL_UNIQUE_ID(classname)\
	struct classname\
	{\
		classname()\
		{\
			m_ID = s_Index++;\
		}\
\
		classname(uint32_t id)\
			: m_ID(id)\
		{\
		}\
\
		inline operator uint32_t() const { return m_ID; }\
		inline uint32_t GetID() const { return m_ID; }\
		static inline uint32_t GetComponentStructCount() { return s_Index; }\
\
		static uint32_t s_Index;\
	private:\
		uint32_t m_ID;\
	};

}