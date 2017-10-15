#ifndef DEFINES_H_
#define DEFINES_H_

/**
 * よくあるタイプを定義する
 * cinttypes などを使うべきかもしれないが
 * どうするのがベストか模索中。
 */
#define FRAMEWORK_COMMON_TYPES_

/**
 * DirectX の使用を明示する定義
 * Types.h で DirectX の vector や matrix のラップを判定するのに使っているが
 * そもそも、フレームワークでは Device などを、生のまま使用しているので、DirectXを使用する前提のコードになっている。
 * 気が向けば、そのあたりもラップするかもしれないが、今のところはこの定義はあってないようなもの。
 */ 
#define FRAMEWORK_USE_DIRECT_X_

#endif // DEFINES_H_
