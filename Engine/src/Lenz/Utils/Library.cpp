#include "Library.h"
//
//namespace lenz
//{
//	template<typename T>
//	Library<T>::Library()
//	{
//	}
//
//	template<typename T>
//	void Library<T>::Add(const T& item, const std::string& name)
//	{
//		if (m_Items.find(name) != m_Items.end())
//		{
//			LZ_CORE_WARN(name + " Already exist!");
//			return;
//		}
//		m_Items[name] = item;
//	}
//
//	template<typename T>
//	T& Library<T>::Add(const std::string& name)
//	{
//		if (m_Items.find(name) != m_Items.end())
//		{
//			LZ_CORE_WARN(name + " Already exist!");
//			return;
//		}
//		return m_Items[name];
//	}
//
//
//	template<typename T>
//	void Library<T>::Remove(const std::string& name)
//	{
//		if (m_Items.find(name) != m_Items.end())
//		{
//			LZ_CORE_WARN(name + " Shader already doesn't exist!");
//			return;
//		}
//		m_Items.erase(name);
//	}
//}