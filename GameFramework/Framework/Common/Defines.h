#ifndef DEFINES_H_
#define DEFINES_H_

/// <summary>
/// �悭����^�C�v���`����
/// </summary>
/// cinttypes �Ȃǂ��g���ׂ���������Ȃ���
/// �ǂ�����̂��x�X�g���͍����B
#define FRAMEWORK_COMMON_TYPES_

/// <summary>
/// DirectX �̎g�p�𖾎������`
/// </summary>
/// Types.h �� DirectX �� vector �� matrix �̃��b�v�𔻒肷��̂Ɏg���Ă��邪
/// ���������A�t���[�����[�N�ł� Device �Ȃǂ��A���̂܂܎g�p���Ă���̂ŁADirectX���g�p����O��̃R�[�h�ɂȂ��Ă���B
/// �C�������΁A���̂���������b�v���邩������Ȃ����A���̂Ƃ���͂��̒�`�͂����ĂȂ��悤�Ȃ��́B
#ifdef _WIN64
#define FRAMEWORK_USE_DIRECT_X_
#endif // _WIN64

#endif // DEFINES_H_
