#pragma once

#include <chrono>

#define P2PN_VERSION_MAJOR 0
#define P2PN_VERSION_MINOR 0
#define P2PN_VERSION_PATCH 0

#ifdef BUILD_STATIC
#define P2PN_STATIC
#endif

#ifndef P2PN_STATIC
#if defined(P2PN_LIB)
#define P2PN_EXPORT __declspec(dllexport)
#else
#define P2PN_EXPORT __declspec(dllimport)
#endif
#else
#define P2PN_EXPORT
#endif


// MSVC Compiler
#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__
typedef std::chrono::steady_clock::time_point TimePoint;
#else
typedef std::chrono::system_clock::time_point TimePoint;
#endif