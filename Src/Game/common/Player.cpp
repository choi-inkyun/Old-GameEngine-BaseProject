
// 플레이어에 대한 정보나 처리를 할 수있는 클레스 입니다.

#include "StdAfx.h"
#include "Player.h"

CPlayer::CPlayer()
{
	Init();
}

CPlayer::~CPlayer()
{

}


void CPlayer::Init()
{
	memset(m_ID, 0, sizeof(char)*256);
	memset(m_NickName, 0, sizeof(char)*256);
}