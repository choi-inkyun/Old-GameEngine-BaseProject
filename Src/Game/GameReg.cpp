
#include "stdafx.h"
#include "Game.h"

// 레지스트리에 옵션을 저정함

void CGame::LoadSetupInfo()
{
	HKEY hKey;
	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_Twogo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwType;
		DWORD dwBytes=100;
		char temp[100];
		RegQueryValueEx( hKey, "EFFECT_VOLUME", 0, &dwType,(LPBYTE)temp, &dwBytes );
		sscanf(temp, "%d", &m_GameOption.effect_vol);
		RegCloseKey(hKey);
	}
	else
		m_GameOption.effect_vol = 90;

	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_Twogo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwType;
		DWORD dwBytes=100;
		char temp[100];
		RegQueryValueEx( hKey, "EFFECT_MODE", 0, &dwType,(LPBYTE)temp, &dwBytes );
		sscanf(temp, "%d", &m_GameOption.effect_mode);
		RegCloseKey(hKey);
	}
	else
		m_GameOption.effect_mode = 1;

	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_Twogo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwType;
		DWORD dwBytes=100;
		char temp[100];
		RegQueryValueEx( hKey, "BGM_VOLUME", 0, &dwType,(LPBYTE)temp, &dwBytes );
		sscanf(temp, "%d", &m_GameOption.bgm_vol);
		RegCloseKey(hKey);
	}
	else
		m_GameOption.bgm_vol=70;

	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_Twogo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwType;
		DWORD dwBytes=100;
		char temp[100];
		RegQueryValueEx( hKey, "BGM_MODE", 0, &dwType,(LPBYTE)temp, &dwBytes );
		sscanf(temp, "%d", &m_GameOption.bgm_mode);
		RegCloseKey(hKey);

		if (m_GameOption.bgm_mode == true)
			DSBGMManager.PauseSound();
	}
	else
		m_GameOption.bgm_mode = 1;
}

void CGame::SaveSetupInfo()
{
	HKEY hKey;
	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_TwoGo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwBytes=100;
		char temp[100];
		sprintf(temp, "%d", m_GameOption.effect_vol);
		RegSetValueEx(hKey, "EFFECT_VOLUME", 0, REG_SZ, (LPBYTE)temp, strlen(temp));
		RegCloseKey(hKey);
	}
	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_Twogo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwBytes=100;
		char temp[100];
		sprintf(temp, "%d", m_GameOption.effect_mode);
		RegSetValueEx(hKey, "EFFECT_MODE", 0, REG_SZ, (LPBYTE)temp, strlen(temp));
		RegCloseKey(hKey);
	}
	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_Twogo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwBytes=100;
		char temp[100];
		sprintf(temp, "%d", m_GameOption.bgm_vol);
		RegSetValueEx(hKey, "BGM_VOLUME", 0, REG_SZ, (LPBYTE)temp, strlen(temp));
		RegCloseKey(hKey);
	}
	if( RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\etnews\\etnews_Twogo\\Options", 0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS ) 
	{
		DWORD dwBytes=100;
		char temp[100];
		sprintf(temp, "%d", m_GameOption.bgm_mode);
		RegSetValueEx(hKey, "BGM_MODE", 0, REG_SZ, (LPBYTE)temp, strlen(temp));
		RegCloseKey(hKey);
	}
}