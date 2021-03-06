/**
 * @file GameMain.h
 * @brief ゲームのメイン処理を記述する
 */
#ifndef GAMEMAIN_H_
#define GAMEMAIN_H_

bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
void Update(const uint64_t elapsedTime, const uint64_t nowTime);
void Draw(ID3D11DeviceContext* context);
void Destroy();

#endif // GAMEMAIN_H_