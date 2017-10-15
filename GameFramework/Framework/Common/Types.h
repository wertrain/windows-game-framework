#ifndef TYPES_H_
#define TYPES_H_

#ifdef FRAMEWORK_COMMON_TYPES_

typedef double f64;
typedef float f32;
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef short f16;
typedef char s8;

static_assert(sizeof(f64) == 8, "sizeof f64 == 8");
static_assert(sizeof(f32) == 4, "sizeof f32 == 4");
static_assert(sizeof(s32) == 4, "sizeof s32 == 4"); 
static_assert(sizeof(u32) == 4, "sizeof u32 == 4");
static_assert(sizeof(s16) == 2, "sizeof u32 == 2");
static_assert(sizeof(f16) == 2, "sizeof s16 == 2");
static_assert(sizeof( s8) == 1, "sizeof  s8 == 1");

#endif // FRAMEWORK_COMMON_TYPES_

#ifdef FRAMEWORK_USE_DIRECT_X_

namespace DirectX
{

struct XMFLOAT3;
struct XMFLOAT4;
struct XMMATRIX;

};

typedef DirectX::XMFLOAT3 Vector3;
typedef DirectX::XMFLOAT4 Vector4;
typedef DirectX::XMMATRIX Matrix44;

#endif // FRAMEWORK_USE_DIRECT_X_



#endif // TYPES_H_
