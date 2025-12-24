#pragma once
#include "Lenz/Renderer/Shader.h"

#include<string>
#include<unordered_map>

namespace lenz
{
	//class T
	//{
	//
	//};
	template<typename T, typename _IdentyType = std::string>
	class Library
	{
	public:
		Library()
		{
		}

		virtual void Add(const T& item, const  _IdentyType& name)
		{
			if (Has(name))
			{
				LZ_CORE_WARN(name + " Already exist!");
				return;
			}
			m_Items[name] = item;
		}

		virtual T& Add(const  _IdentyType& name)
		{
			if (Has(name))
			{
				LZ_CORE_WARN(name + " Already exist!");
			}
			return m_Items[name];
		}


		virtual void Remove(const  _IdentyType& name)
		{
			if (!Has(name))
			{
				LZ_CORE_WARN(name + " Shader already doesn't exist!");
				return;
			}
			m_Items.erase(name);
		}

		T& Get(const  _IdentyType& name) 
		{ 
			if (m_Items.find(name) != m_Items.end())
			{
				return m_Items[name]; 
			}
			LZ_CORE_WARN("'" + name + "' doesn't exists, so referance of a new object will be returned!");
			return m_Items[name];
		}

		bool Has(const _IdentyType it)
		{
			return m_Items.find(it) != m_Items.end();
		}

		inline std::unordered_map< _IdentyType, T>& GetItems() { return m_Items; }
		inline uint32_t size() const { return m_Items.size(); }

		T& operator [] (const  _IdentyType& name) { return Get(name); }

	protected:
		std::unordered_map< _IdentyType, T> m_Items;
	};
}

