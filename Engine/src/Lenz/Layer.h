#pragma once

#include <Lenz/Core.h>
#include "Lenz/Event/Event.h"
#include "Lenz/Event/EventAdaptor.h"
#include "Lenz/Utils/TimeStep.h"


namespace lenz
{
	class TimeStep;
	class  Layer : public EventAdaptor
	{
	public:
		Layer(const std::string& name = "UntitledLayer")
			:m_DebugName(name)
		{
		}
		virtual ~Layer() {};

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep& ts) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string  m_DebugName;

	};

}
