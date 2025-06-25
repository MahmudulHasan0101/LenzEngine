#pragma once
#include"Log.h"

#ifdef PROJECT_DEBUG || _DEBUG || __DEBUG
	#define LZ_DEBUG
#endif

#ifdef PROJECT_RELEASE || _RELEASE || __RELEASE
	#define LZ_RELEASE
	#define LZ_DISABLE_ASSERTS
#endif


#ifdef LZ_DEBUG

	#ifndef LZ_DISABLE_ASSERT
		#define	LZ_ASSERT(x, ...) { if (!x) { LZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#define	LZ_CORE_ASSERT(x, ...) { if (!x) { LZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#else 
		#define LZ_ASSERT(x, ...)
		#define	LZ_CORE_ASSERT(x, ...)
	#endif

	#define LZ_DEBUG_ONLY(x) x
	#define LZ_DEBUG_RELEASE_SWAP(x, y) x

#else
	#define LZ_ASSERT(x, ...)
	#define	LZ_CORE_ASSERT(x, ...)
	#define LZ_DEBUG_ONLY(x)
	#define LZ_DEBUG_RELEASE_SWAP(x, y) y

#endif

#define BIT(x) (1 << x)
#define LZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

//#define LZ_DYNAMIC_API


#include<list>
#include<string>
#include<vector>
#include<iostream>
#include<stdint.h>
#include<tuple>
#include<memory>
#include<utility>
#include<algorithm>
#include<unordered_map>
#include<functional>

#include<gl/glew.h>
#include<GLFW/glfw3.h>

class LZ_NULL
{
public:
	LZ_NULL() {}
	LZ_NULL(const LZ_NULL&) {}
	LZ_NULL(LZ_NULL&&) {}
};

template<typename Class>
inline constexpr uint32_t GetClassUUID() { return reinterpret_cast<int>(typeid(Class).name()); }

template<typename... Classes>
inline constexpr uint32_t GetCombinedClassUUID()
{
	uint32_t seed = 0;
	std::hash<uint32_t> hasher;
	((seed ^= hasher(GetClassUUID<Classes>()) + 0x9e3779b9 + (seed << 6) + (seed >> 2)), ...);

	return seed;
}

constexpr std::uint64_t fnv1a(const char* str, std::size_t len) {
	std::uint64_t hash = 14695981039346656037ull;
	for (std::size_t i = 0; i < len; ++i) {
		hash ^= static_cast<std::uint64_t>(str[i]);
		hash *= 1099511628211ull;
	}
	return hash;
}

constexpr std::size_t strlen_const(const char* str) {
	std::size_t len = 0;
	while (str[len] != '\0') ++len;
	return len;
}

constexpr std::uint64_t unique_id(const char* name) {
	return fnv1a(name, strlen_const(name));
}

#define GLM_ENABLE_EXPERIMENTAL

//
//template<typename Class, typename... Classes>
//inline constexpr uint32_t AddClassUUIDToHash(uint32_t hash)
//{
//	// Combine the hash with the ID of the class
//	hash ^= GetClassUUID<Class>() + 0x9e3779b9 + (hash << 6) + (hash >> 2);
//
//	// Recursively combine the hash with the IDs of the remaining classes
//	if constexpr (sizeof...(Classes) > 0)
//	{
//		return AddClassUUIDToHash<Classes...>(hash, ids...);
//	}
//	else
//	{
//		return hash;
//	}
//}


template<typename T>
struct VOID_CLASS_BINDER
{
};

#include <iostream>
#include <typeinfo>

template <typename... Types>
struct TypePrinter;

template <typename First, typename... Rest>
struct TypePrinter<First, Rest...> {
	TypePrinter() {
		std::cout << "Class name: " << typeid(First).name() << std::endl;
		TypePrinter<Rest...>();
	}
};

