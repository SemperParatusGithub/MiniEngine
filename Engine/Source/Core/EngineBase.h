#pragma once

// Check for Platforms
#ifdef _WIN32
	#ifdef _WIN64
		#define ME_PLATFORM_WINDOWS
	#else
		#error "Win32 not supported"
	#endif
#else
	#error "Unsupported Platform"
#endif

// Logging
#ifdef ME_DEBUG
    #include <stdio.h>
    #define ME_TRACE(...) { printf("\033[1m\033[37m[TRACE]: "); printf(__VA_ARGS__); printf("\033[0m \n"); }
    #define ME_INFO(...)  { printf("\033[1m\033[32m[INFO]: ");  printf(__VA_ARGS__); printf("\033[0m \n"); }
    #define ME_WARN(...)  { printf("\033[1m\033[33m[WARN]: ");  printf(__VA_ARGS__); printf("\033[0m \n"); }
    #define ME_ERROR(...) { printf("\033[1m\033[31m[ERROR]: "); printf(__VA_ARGS__); printf("\033[0m \n"); }
#else
    #define ME_TRACE(...)
    #define ME_INFO(...) 
    #define ME_WARN(...) 
    #define ME_ERROR(...)
#endif

#if (defined(ME_PLATFORM_WINDOWS) && defined(_MSC_VER))
	#define ME_BREAKDEBUGGER __debugbreak();
#else
	#define ME_BREAKDEBUGGER
#endif

// Temporary
#ifdef ME_DEBUG
	#define ME_ENABLE_ASSERTS
#endif

// Asserts
#ifdef ME_ENABLE_ASSERTS
    #define ME_ASSERT(x)                  \
        if (!(x))                         \
        {                                 \
            ME_ERROR("Assertion failed"); \
            ME_BREAKDEBUGGER              \
        }
#else
    #define ME_ASSERT(x)
#endif

#include <iostream>

// Types
// Primitives
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using RendererID = u32;

// const references
template<typename Type>
using ConstRef = const Type &;


// Pointers
template<typename Type>
using UniquePtr = std::unique_ptr<Type>;

template<typename Type, typename ... Args>
constexpr UniquePtr<Type> MakeUnique(Args && ... args)
{
	return std::make_unique<Type>(std::forward<Args>(args)...);
}

template<typename Type>
using SharedPtr = std::shared_ptr<Type>;

template<typename Type, typename ... Args>
constexpr SharedPtr<Type> MakeShared(Args && ... args)
{
	return std::make_shared<Type>(std::forward<Args>(args)...);
}