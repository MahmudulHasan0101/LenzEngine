#pragma once
#include "Lenz/Event/EventAdaptor.h"
#include "Lenz/Utils/TimeStep.h"

namespace lenz {

	class Entity : public EventAdaptor
	{
	public:
		Entity()
			: _LocalIndex(0), _DataPtr(nullptr)
		{
		}

		Entity(size_t drawunite_index, void* dataptr)
			: _LocalIndex(drawunite_index), _DataPtr(dataptr)
		{
		}

		operator size_t() { return _LocalIndex; }

		size_t _LocalIndex;
		void* _DataPtr;
	};
}
