#ifndef GAMEMAIN_H_
#define GAMEMAIN_H_

bool Create(const HWND hWnd);
void Update(const DWORD nowTime);
void Draw(const HDC hDC);
void Destroy();

#endif // GAMEMAIN_H_