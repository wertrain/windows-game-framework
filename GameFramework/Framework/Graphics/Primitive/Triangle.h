#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Primitive.h"

NS_FW_GFX_BEGIN

/// <summary>
/// 三角形を扱うクラス
/// </summary>
class Triangle : public Primitive
{
public:
    Triangle() ;
    virtual ~Triangle();

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
};

NS_FW_GFX_END

#endif // TRIANGLE_H_
