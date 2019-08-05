
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "..\define.h"

class CPlayer
{
public:
	CPlayer(void);
	~CPlayer(void);

	void Init();	// 초기화

public:	// 기본정보
	char	m_ID[256];			// 아이디 
	char	m_NickName[256];	// 닉네임
};

#endif