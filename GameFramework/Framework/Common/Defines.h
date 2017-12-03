#ifndef DEFINES_H_
#define DEFINES_H_

/// <summary>
/// よくあるタイプを定義する
/// </summary>
/// cinttypes などを使うべきかもしれないが
/// どうするのがベストか模索中。
#define FRAMEWORK_COMMON_TYPES_

/// <summary>
/// DirectX の使用を明示する定義
/// </summary>
/// Types.h で DirectX の vector や matrix のラップを判定するのに使っているが
/// そもそも、フレームワークでは Device などを、生のまま使用しているので、DirectXを使用する前提のコードになっている。
/// 気が向けば、そのあたりもラップするかもしれないが、今のところはこの定義はあってないようなもの。
#ifdef _WIN64
#define FRAMEWORK_USE_DIRECT_X_
#endif // _WIN64

#endif // DEFINES_H_
