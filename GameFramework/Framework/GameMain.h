#ifndef GAMEMAIN_H_
#define GAMEMAIN_H_

bool Create(ID3D11Device* device, ID3D11DeviceContext* context);
void Update(const DWORD nowTime);
void Draw(const HDC hDC);
void Destroy();

#endif // GAMEMAIN_H_