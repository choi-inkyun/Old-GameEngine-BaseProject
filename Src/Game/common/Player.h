
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "..\define.h"

class CPlayer
{
public:
	CPlayer(void);
	~CPlayer(void);

	void Init();	// �ʱ�ȭ

public:	// �⺻����
	char	m_ID[256];			// ���̵� 
	char	m_NickName[256];	// �г���
};

#endif