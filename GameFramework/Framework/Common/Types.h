#ifndef TYPES_H_
#define TYPES_H_

#ifdef FRAMEWORK_USE_DIRECT_X_

namespace DirectX
{

class XMFLOAT3;
class XMFLOAT4;
class XMMATRIX;

};

typedef DirectX::XMFLOAT3 Vector3;
typedef DirectX::XMFLOAT4 Vector4;
typedef DirectX::XMMATRIX Matrix44;

#endif // FRAMEWORK_USE_DIRECT_X_

#endif // TYPES_H_
