
// �÷��̾ ���� ������ ó���� �� ���ִ� Ŭ���� �Դϴ�.

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