/**
 * @file Defines.h
 * @brief 共通定義
 */
#ifndef COMMON_DEFINES_H_
#define COMMON_DEFINES_H_

/// <summary>
/// Framework 名前空間定義
/// </summary>
#ifndef FW_UNUSED_NAMESPACE
#define NS_DEF_FW fw
#define NS_DEF_FW_FND fnd
#define NS_DEF_FW_GFX gfx
#define NS_DEF_FW_SYS sys
#define NS_DEF_FW_UTIL util
#define NS_DEF_FW_GAME game
#define NS_DEF_FW_CONST cst /// 廃止予定
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
/// よくあるタイプを定義する
/// </summary>
/// cinttypes などを使うべきかもしれないが
/// どうするのがベストか模索中。
//#define FRAMEWORK_COMMON_TYPES_

/// <summary>
/// DirectX の使用を明示する定義
/// </summary>
/// Types.h で DirectX の vector や matrix のラップを判定するのに使っているが
/// そもそも、フレームワークでは Device などを、生のまま使用しているので、DirectXを使用する前提のコードになっている。
/// 気が向けば、そのあたりもラップするかもしれないが、今のところはこの定義はあってないようなもの。
#ifdef _WIN64
//#define FRAMEWORK_USE_DIRECT_X_
#define FRAMEWORK_USE_SIMPLE_MATH_
#endif // _WIN64

/// <summary>
/// UNUSED マクロ
/// </summary>
#define NNFW_UNUSED(x) ((void)(x))

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
