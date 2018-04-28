/**
 * @file GameMain.h
 * @brief �Q�[���̃��C���������L�q����
 */
#ifndef GAMEMAIN_H_
#define GAMEMAIN_H_

bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
void Update(const u64 elapsedTime, const u64 nowTime);
void Draw(ID3D11DeviceContext* context);
void Destroy();

#endif // GAMEMAIN_H_