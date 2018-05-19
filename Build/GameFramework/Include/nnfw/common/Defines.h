/**
 * @file Defines.h
 * @brief ���ʒ�`
 */
#ifndef COMMON_DEFINES_H_
#define COMMON_DEFINES_H_

/// <summary>
/// Framework ���O��Ԓ�`
/// </summary>
#ifndef FW_UNUSED_NAMESPACE
#define NS_DEF_FW fw
#define NS_DEF_FW_FND fnd
#define NS_DEF_FW_GFX gfx
#define NS_DEF_FW_SYS sys
#define NS_DEF_FW_UTIL util
#define NS_DEF_FW_GAME game
#define NS_DEF_FW_CONST cst /// �p�~�\��
#define NS_DEF_FW_TOOL tool

#define NS_FW NS_DEF_FW
#define NS_FW_FND NS_DEF_FW::NS_DEF_FW_FND
#define NS_FW_GFX NS_DEF_FW::NS_DEF_FW_GFX
#define NS_FW_SYS NS_DEF_FW::NS_DEF_FW_SYS
#define NS_FW_UTIL NS_DEF_FW::NS_DEF_FW_UTIL
#define NS_FW_GAME NS_DEF_FW::NS_DEF_FW_GAME
#define NS_FW_CONST NS_DEF_FW::NS_DEF_FW_CONST
#define NS_FW_TOOL NS_DEF_FW::NS_DEF_FW_TOOL

#define NS_FW_BEGIN namespace NS_DEF_FW {
#define NS_FW_END }
#define NS_FW_FND_BEGIN NS_FW_BEGIN namespace NS_DEF_FW_FND {
#define NS_FW_FND_END } NS_FW_END
#define NS_FW_GFX_BEGIN NS_FW_BEGIN namespace NS_DEF_FW_GFX {
#define NS_FW_GFX_END } NS_FW_END
#define NS_FW_SYS_BEGIN NS_FW_BEGIN namespace NS_DEF_FW_SYS {
#define NS_FW_SYS_END } NS_FW_END
#define NS_FW_UTIL_BEGIN NS_FW_BEGIN namespace NS_DEF_FW_UTIL {
#define NS_FW_UTIL_END } NS_FW_END
#define NS_FW_GAME_BEGIN NS_FW_BEGIN namespace NS_DEF_FW_GAME {
#define NS_FW_GAME_END } NS_FW_END
#define NS_FW_CONST_BEGIN NS_FW_BEGIN namespace NS_DEF_FW_CONST {
#define NS_FW_CONST_END } NS_FW_END
#define NS_FW_TOOL_BEGIN NS_FW_BEGIN namespace NS_DEF_FW_TOOL {
#define NS_FW_TOOL_END } NS_FW_END
#else
#error
#endif // FW_UNUSE_NAMESPACE

/// <summary>
/// �悭����^�C�v���`����
/// </summary>
/// cinttypes �Ȃǂ��g���ׂ���������Ȃ���
/// �ǂ�����̂��x�X�g���͍����B
//#define FRAMEWORK_COMMON_TYPES_

/// <summary>
/// DirectX �̎g�p�𖾎������`
/// </summary>
/// Types.h �� DirectX �� vector �� matrix �̃��b�v�𔻒肷��̂Ɏg���Ă��邪
/// ���������A�t���[�����[�N�ł� Device �Ȃǂ��A���̂܂܎g�p���Ă���̂ŁADirectX���g�p����O��̃R�[�h�ɂȂ��Ă���B
/// �C�������΁A���̂���������b�v���邩������Ȃ����A���̂Ƃ���͂��̒�`�͂����ĂȂ��悤�Ȃ��́B
#ifdef _WIN64
#define FRAMEWORK_USE_DIRECT_X_
#endif // _WIN64

NS_FW_BEGIN

template <typename T>
inline void SafeDelete(T*& p)
{
    if (p != nullptr)
    {
        delete (p);
        (p) = nullptr;
    }
}

template <typename T>
inline void SafeDeleteArray(T*& p)
{
    if (p != nullptr)
    {
        delete [] (p);
        (p) = nullptr;
    }
}

template <typename T>
inline void SafeRelease(T*& p)
{
    if (p != nullptr)
    {
        (p)->Release();
        (p) = nullptr;
    }
}

NS_FW_END

//-----------------------------------------------
// for Doug Lea' s malloc
#define ONLY_MSPACES (1)
#define NO_MALLINFO (1)
//-----------------------------------------------

#endif // COMMON_DEFINES_H_
