/**
 * @file Types.h
 * @brief ��{�I�Ȍ^��`
 */
#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#ifdef FRAMEWORK_COMMON_TYPES_

typedef double f64;
typedef float f32;
typedef long long s64;
typedef unsigned long long u64;
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned short u16;
typedef short f16;
typedef char s8;

static_assert(sizeof(f64) == 8, "sizeof f64 == 8");
static_assert(sizeof(f32) == 4, "sizeof f32 == 4");
static_assert(sizeof(s64) == 8, "sizeof s64 == 8"); 
static_assert(sizeof(u64) == 8, "sizeof u64 == 8"); 
static_assert(sizeof(s32) == 4, "sizeof s32 == 4");
static_assert(sizeof(u32) == 4, "sizeof u32 == 4");
static_assert(sizeof(s16) == 2, "sizeof s16 == 2");
static_assert(sizeof(u16) == 2, "sizeof u16 == 2");
static_assert(sizeof(f16) == 2, "sizeof f16 == 2");
static_assert(sizeof( s8) == 1, "sizeof  s8 == 1");

#else

#include <cinttypes>

#endif // FRAMEWORK_COMMON_TYPES_

#ifdef FRAMEWORK_USE_DIRECT_X_

namespace DirectX
{

struct XMFLOAT2;
struct XMFLOAT3;
struct XMFLOAT4;
struct XMMATRIX;

};

typedef DirectX::XMFLOAT2 Vector2;
typedef DirectX::XMFLOAT3 Vector3;
typedef DirectX::XMFLOAT4 Vector4;
typedef DirectX::XMMATRIX Matrix44;

#endif // FRAMEWORK_USE_DIRECT_X_

#include <vector>
#include <memory>
#include <map>
#include <mutex>

NS_FW_BEGIN

template <class T>
using vector = std::vector<T>;

template <class T>
using unique_ptr = std::unique_ptr<T>;

template <class K, class V>
using dictonary = std::map<K, V>;

using mutex = std::mutex;
template <class T>
using ScopedLock = std::lock_guard<T>;

NS_FW_END

#endif // COMMON_TYPES_H_
