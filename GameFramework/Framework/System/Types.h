#ifndef TYPES_H_
#define TYPES_H_

#ifdef _WIN64

#include <DirectXMath.h>
typedef DirectX::XMFLOAT3 Vector3;
typedef DirectX::XMFLOAT4 Vector4;
typedef DirectX::XMMATRIX Matrix44;

#endif // _WIN64

#endif // TYPES_H_
