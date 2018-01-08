#ifndef GAMEMAIN_H_
#define GAMEMAIN_H_

bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
void Update(const DWORD elapsedTime, const DWORD nowTime);
void Draw(ID3D11DeviceContext* context);
void Destroy();

#endif // GAMEMAIN_H_