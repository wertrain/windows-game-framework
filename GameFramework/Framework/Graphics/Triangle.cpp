#include "../Common/Includes.h"
#include "../System/Includes.h"
#include <d3d11.h>

#include "Triangle.h"

namespace Framework {
namespace Graphics {

Triangle::Triangle()
{

}

Triangle::~Triangle()
{

}

bool Triangle::Create(ID3D11Device* device, ID3D11DeviceContext* context)
{
    // �|���S�����_�\����
    struct Vertex3D {
        float pos[3];	//x-y-z
        float col[4];	//r-g-b-a
    };

    const int TYOUTEN = 3;	//�|���S���̒��_��

    // ���_�f�[�^(�O�p�|���S��1��)
    Vertex3D hVectorData[TYOUTEN] = {
        { { +0.0f, +0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
        { { +0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
        { { -0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } }
    };

    // ���_���C�A�E�g
    // �T�Ԗڂ̃p�����[�^�͐擪����̃o�C�g���Ȃ̂ŁCCOLOR�ɂ�POSITION��float�^4�o�C�g�~3���L�q
    D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // ���_�o�b�t�@�쐬
    D3D11_BUFFER_DESC hBufferDesc;
    hBufferDesc.ByteWidth = sizeof(Vertex3D) * TYOUTEN;
    hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    hBufferDesc.CPUAccessFlags = 0;
    hBufferDesc.MiscFlags = 0;
    hBufferDesc.StructureByteStride = sizeof(float);

    D3D11_SUBRESOURCE_DATA hSubResourceData;
    hSubResourceData.pSysMem = hVectorData;
    hSubResourceData.SysMemPitch = 0;
    hSubResourceData.SysMemSlicePitch = 0;

    ID3D11Buffer* hpBuffer;
    if (FAILED(device->CreateBuffer(&hBufferDesc, &hSubResourceData, &hpBuffer))) {
        return false;
    }
     
    // ���̒��_�o�b�t�@���R���e�L�X�g�ɐݒ�
    UINT hStrides = sizeof(Vertex3D);
    UINT hOffsets = 0;
    context->IASetVertexBuffers(0, 1, &hpBuffer, &hStrides, &hOffsets);

    // �v���~�e�B�u(�|���S���̌`��)���R���e�L�X�g�ɐݒ�
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    size_t vs_buffer_size;
    void* vs_buffer = Framework::System::File::Read("vs_triangle.cso", vs_buffer_size);

    size_t ps_buffer_size;
    void* ps_buffer = Framework::System::File::Read("ps_triangle.cso", ps_buffer_size);

    // ���_���C�A�E�g�쐬
    ID3D11InputLayout* hpInputLayout = NULL;
    if (FAILED(device->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), vs_buffer, vs_buffer_size, &hpInputLayout))) {
        return false;
    }

    // ���_���C�A�E�g���R���e�L�X�g�ɐݒ�
    context->IASetInputLayout(hpInputLayout);

    // ���_�V�F�[�_�[����
    ID3D11VertexShader* hpVertexShader;
    if (FAILED(device->CreateVertexShader(vs_buffer, vs_buffer_size, NULL, &hpVertexShader))) {
        return false;
    }

    // ���_�V�F�[�_�[���R���e�L�X�g�ɐݒ�
    context->VSSetShader(hpVertexShader, NULL, 0);

    // �s�N�Z���V�F�[�_�[����
    ID3D11PixelShader* hpPixelShader;
    if (FAILED(device->CreatePixelShader(ps_buffer, ps_buffer_size, NULL, &hpPixelShader))) {
        return false;
    }

    // �s�N�Z���V�F�[�_�[���R���e�L�X�g�ɐݒ�
    context->PSSetShader(hpPixelShader, NULL, 0);

    return true;
}

} // namespace Graphics
} // namespace Framework
