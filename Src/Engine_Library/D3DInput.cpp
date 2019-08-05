/*
 * FILENAME:
 *			Input.cpp
 *
 * DESCRIPTION:
 *			
 *
 * AUTHOR:	Programming Role-Playing Games with DirectX 8.0 by Jim A. Adams
 *
 * HISTORY:
 *			2004.04.02 책의 내용을 그대로 사용함.
 */

#include "StdAfx.h"
#include "D3DInput.h"

#define ReleaseCOM( x ) if( x ) { x->Release(); x = NULL; }

/*-----------------------------------------------------------------------------
 * cInput CLASS
 *---------------------------------------------------------------------------*/

cInput::cInput()
{
	// Only need to clear the DirectInput interface pointer
	m_pDI = NULL;
}

cInput::~cInput()
{
	// Force a shutdown
	release();
}

HWND cInput::GethWnd()
{
	// return the owner window handle
	return m_hWnd;
}

IDirectInput8 *cInput::GetDirectInputCOM()
{
	// return a pointer to IDirectInput8 object
	return m_pDI;
}

BOOL cInput::initialize( HWND hWnd, HINSTANCE hInstance )
{
	// free a prior initialization if any.
	release();

	// record parent window handle
	m_hWnd = hWnd;

	// create a DirectInput interface
	if( FAILED( DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL ) ) ) return FALSE;

	return TRUE;
}

BOOL cInput::release()
{
	ReleaseCOM( m_pDI );

	m_hWnd = NULL;

	return TRUE;
}


/*-----------------------------------------------------------------------------
 * cInputDevice CLASS
 *---------------------------------------------------------------------------*/

cInputDevice::cInputDevice()
{
	// Clear parent cInput object pointer
	m_Input = NULL;

	// Setup device to none
	m_Type = NONE;

	// Set windowed usage to TRUE
	m_Windowed = TRUE;

	// Clear the DirectInput interface pointer
	m_pDIDevice = NULL;

	// Point the mouse and joystick structures to the state buffer
	m_MouseState = (DIMOUSESTATE *)&m_State;
	m_JoystickState = (DIJOYSTATE *)&m_State;

	// Clear the device variables
	Clear();
}

cInputDevice::~cInputDevice()
{
	// Free a prior install
	Free();
}

IDirectInputDevice8 *cInputDevice::DeviceCOM()
{
	// Return the pointer to the IDirectDevice8 object
	return m_pDIDevice;
}

BOOL cInputDevice::Create( cInput *Input, short Type, BOOL Windowed )
{
	DIDATAFORMAT	*DataFormat;
	DIPROPRANGE		 DIprg;
	DIPROPDWORD		 DIpdw;

	// Free a prior device
	Free();

	// Check for a valid parent cInput class.
	if( ( m_Input = Input ) == NULL ) return FALSE;

	// Create the device and remember device data format
	switch( Type )
	{
		case KEYBOARD:
			if( FAILED( m_Input->GetDirectInputCOM()->CreateDevice( GUID_SysKeyboard, &m_pDIDevice, NULL ) ) ) return FALSE;
			DataFormat = (DIDATAFORMAT *)&c_dfDIKeyboard;
			break;

		case MOUSE:
			if( FAILED( m_Input->GetDirectInputCOM()->CreateDevice( GUID_SysMouse, &m_pDIDevice, NULL ) ) ) return FALSE;
			DataFormat = (DIDATAFORMAT *)&c_dfDIMouse;
			break;

		case JOYSTICK:
			if( FAILED( m_Input->GetDirectInputCOM()->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticks, this, DIEDFL_ATTACHEDONLY ) ) ) return FALSE;
			if( m_pDIDevice == NULL ) return FALSE;
			DataFormat = (DIDATAFORMAT *)&c_dfDIJoystick;
			break;

		default: return FALSE;
	}

	// Set the windowed usage
	m_Windowed = Windowed;

	// Set the data format of keyboard
	if( FAILED( m_pDIDevice->SetDataFormat( DataFormat ) ) ) return FALSE;

	// Set the cooperative level - Foreground & Nonexclusive
	if( FAILED( m_pDIDevice->SetCooperativeLevel( m_Input->GethWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) ) return FALSE;

	// Set some properties if it's a mouse.
	if( Type == MOUSE )
	{
		// Do nothing.
	}

	// Set the special properties if it's a joystick.
	if( Type == JOYSTICK )
	{
		// Set the special properties of the joystick - range
		DIprg.diph.dwSize		= sizeof( DIPROPRANGE );
		DIprg.diph.dwHeaderSize	= sizeof( DIPROPHEADER );
		DIprg.diph.dwHow		= DIPH_BYOFFSET;
		DIprg.lMin				= -1024;
		DIprg.lMax				= +1024;

		// Set X range
		DIprg.diph.dwObj		= DIJOFS_X;
		if( FAILED( m_pDIDevice->SetProperty( DIPROP_RANGE, &DIprg.diph ) ) ) return FALSE;

		// Set Y range
		DIprg.diph.dwObj		= DIJOFS_Y;
		if( FAILED( m_pDIDevice->SetProperty( DIPROP_RANGE, &DIprg.diph ) ) ) return FALSE;

		// Set the special properties of the joystick - deadzone 12%
		DIpdw.diph.dwSize		= sizeof( DIPROPDWORD );
		DIpdw.diph.dwHeaderSize	= sizeof( DIPROPHEADER );
		DIpdw.diph.dwHow		= DIPH_BYOFFSET;
		DIpdw.dwData			= 128;

		// Set X deadzone
		DIpdw.diph.dwObj		= DIJOFS_X;
		if( FAILED( m_pDIDevice->SetProperty( DIPROP_DEADZONE, &DIpdw.diph ) ) ) return FALSE;
		
		// Set Y deadzone
		DIpdw.diph.dwObj		= DIJOFS_Y;
		if( FAILED( m_pDIDevice->SetProperty( DIPROP_DEADZONE, &DIpdw.diph ) ) ) return FALSE;
	}

	// Set the device type
	m_Type = Type;

	// Clear the device information
	Clear();

	// Acquire the device for use.
	if( FAILED( m_pDIDevice->Acquire() ) ) return FALSE;

	// Return a success.
	return TRUE;
}

BOOL cInputDevice::Free()
{
	// Unacquire and release the object
	if( m_pDIDevice != NULL )
	{
		m_pDIDevice->Unacquire();
		ReleaseCOM( m_pDIDevice );
	}

	// Set to no device installed
	m_Type = NONE;

	// Clear the data
	Clear();

	return TRUE;
}

BOOL cInputDevice::Clear()
{
	ZeroMemory( &m_State, 256 );

	for( int i = 0; i < 256; i++ ) m_Locks[ i ] = FALSE;

	m_XPos = m_YPos = 0;

	return TRUE;
}

BOOL cInputDevice::Read()
{
	short i;
	HRESULT hr;
	long BufferSizes[3] = { 256, sizeof(DIMOUSESTATE), sizeof(DIJOYSTATE) };

	// Make sure to have a valid IDirectInputDevice8 object
	if( m_pDIDevice == NULL ) return FALSE;

	// Make sure device type is in range.
	if( m_Type < 1 || m_Type > 3 ) return FALSE;

	// Loop polling and reading until succeeded or unknown error
	// Also take care of lost-focus problems
	while( TRUE )
	{
		// Poll
		m_pDIDevice->Poll();

		// Read in state
		if( SUCCEEDED( hr = m_pDIDevice->GetDeviceState( BufferSizes[ m_Type - 1 ], (LPVOID)&m_State ) ) ) break;

		// Return on an unknown error
		if( hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED ) return FALSE;

		// Reacquire and try again
		if( FAILED( m_pDIDevice->Acquire() ) ) return FALSE;
	}

	// Since only the mouse coordinates are relative, you'll have to deal with them now
	if( m_Type == MOUSE )
	{
		// If windowed usage, ask windows for coordinates.
		if( m_Windowed == TRUE )
		{
			POINT pt;
			GetCursorPos( &pt );
			ScreenToClient( m_Input->GethWnd(), &pt );
			m_XPos = pt.x;
			m_YPos = pt.y;
		}
		else
		{
			// 아래의 코드는 절대좌표를 계산하는 식으로써 적합하지 않다.
			//m_XPos += m_MouseState->lX;
			//m_YPos += m_MouseState->lY;
			POINT pt;
			GetCursorPos( &pt );
			m_XPos = pt.x;
			m_YPos = pt.y;
		}
	}

	// Released Keys and button need to be unlocked
	switch( m_Type )
	{
		case KEYBOARD:
			for( i = 0; i < 256; i++ )
				if( !( m_State[ i ] & 0x80 ) ) m_Locks[ i ] = FALSE;
			break;
		case MOUSE:
			for( i = 0; i < 4; i++ )
				if( !( m_MouseState->rgbButtons[ i ] ) ) m_Locks[ i ] = FALSE;
			break;
		case JOYSTICK:
			for( i = 0; i < 32; i++ )
				if( !( m_JoystickState->rgbButtons[ i ] ) ) m_Locks[ i ] = FALSE;
			break;
	}

	// return a success
	return TRUE;
}

BOOL cInputDevice::Acquire( BOOL Active )
{
	if( m_pDIDevice == NULL ) return FALSE;

	if( Active == TRUE ) m_pDIDevice->Acquire();
	else                 m_pDIDevice->Unacquire();

	return TRUE;
}

BOOL cInputDevice::GetLock( BYTE Num )
{
	return m_Locks[ Num ];
}

BOOL cInputDevice::SetLock( BYTE Num, BOOL State )
{
	m_Locks[ Num ] = State;
	return TRUE;
}

long cInputDevice::GetXPos()
{
	// Update coordinates if a joystick
	if( m_Type == JOYSTICK ) m_XPos = m_JoystickState->lX;
	return m_XPos;
}

BOOL cInputDevice::SetXPos( long XPos )
{
	m_XPos = XPos;
	return TRUE;
}

long cInputDevice::GetYPos()
{
	// Update coordinates if a joystick
	if( m_Type == JOYSTICK ) m_YPos = m_JoystickState->lY;
	return m_YPos;
}

BOOL cInputDevice::SetYPos( long YPos )
{
	m_YPos = YPos;
	return TRUE;
}

long cInputDevice::GetXDelta()
{
	switch( m_Type )
	{
		case MOUSE:		return m_MouseState->lX;
		case JOYSTICK:	return m_JoystickState->lX - m_XPos;
		default:		return 0;
	}
}

long cInputDevice::GetYDelta()
{
	switch( m_Type )
	{
		case MOUSE:		return m_MouseState->lY;
		case JOYSTICK:	return m_JoystickState->lY - m_YPos;
		default:		return 0;
	}
}

BOOL cInputDevice::GetKeyState( BYTE Num )
{
	// Check if key/button is pressed
	if( ( m_State[ Num ] & 0x80 ) && m_Locks[ Num ] == FALSE ) return TRUE;
	return FALSE;
}

BOOL cInputDevice::SetKeyState( BYTE Num, BOOL State )
{
	m_State[ Num ] = State;
	return TRUE;
}

BOOL cInputDevice::GetPureKeyState( BYTE Num )
{
	return ( m_State[ Num ] & 0x80 ) ? TRUE : FALSE;
}

short cInputDevice::GetKeypress( long TimeOut )
{
	static HKL KeyboardLayout = GetKeyboardLayout( 0 );
	BYTE  WinKeyStates[256], DIKeyStates[256];
	unsigned short ScanCode, VirtualKey, Keys, Num;
	unsigned long EndTime;

	// Make sure it's a keyboard and it has initialized.
	if( ( m_Type != KEYBOARD ) || ( m_pDIDevice == NULL ) ) return 0;

	// Calculate end time for TimeOut.
	EndTime = GetTickCount() + TimeOut;

	// Loop until timeout or key pressed.
	while( TRUE )
	{
		// Get Windows keyboard state
		GetKeyboardState( WinKeyStates );

		// Get DirectInput keyboard state
		m_pDIDevice->GetDeviceState( 256, DIKeyStates );

		// Scan through looking for key presses
		for( int i = 0;i < 256; i++ )
		{
			// If one found, try to convert it
			if( DIKeyStates[ i ] & 0x80 )
			{
				// Get virtual key code
				if( ( VirtualKey = MapVirtualKeyEx( (ScanCode = i ), 1, KeyboardLayout ) ) )
				{
					// Get ASCII code of key and return it.
					Num = ToAsciiEx( VirtualKey, ScanCode, WinKeyStates, &Keys, 0, KeyboardLayout );
					if( Num ) return Keys;
				}
			}
		}

		// Check for TimeOut
		if( TimeOut )
		{
			if( GetTickCount() > EndTime ) return 0;
		}
	} // end of while( TRUE )

	return 0;
}

long cInputDevice::GetNumKeyPresses()
{
	long Num = 0;

	for( int i = 0; i < 256; i++ ) if( ( m_State[ i ] & 0x80 ) && m_Locks[ i ] == FALSE ) Num++;
	return Num;
}

long cInputDevice::GetNumPureKeyPresses()
{
	long Num = 0;

	for( int i = 0; i < 256; i++ ) if( m_State[ i ] & 0x80 ) Num++;
	return Num;
}

BOOL cInputDevice::GetButtonState( BYTE Num )
{
	char State = 0;

	if( m_Type == MOUSE ) State = m_MouseState->rgbButtons[ Num ];
	if( m_Type == JOYSTICK ) State = m_JoystickState->rgbButtons[ Num ];

	// Check if key/button is pressed
	if( ( State & 0x80 ) && m_Locks[ Num ] == FALSE ) return TRUE;

	return FALSE;
}

BOOL cInputDevice::SetButtonState( BYTE Num, BOOL State )
{
	if( m_Type == MOUSE )
	{
		m_MouseState->rgbButtons[ Num ] = State;
		return TRUE;
	}
	if( m_Type == JOYSTICK )
	{
		m_JoystickState->rgbButtons[ Num ] = State;
		return TRUE;
	}

	return FALSE;
}

BOOL cInputDevice::GetPureButtonState( BYTE Num )
{
	if( m_Type == MOUSE ) return m_MouseState->rgbButtons[ Num ];
	if( m_Type == JOYSTICK ) return m_JoystickState->rgbButtons[ Num ];

	return FALSE;
}

long cInputDevice::GetNumButtonPresses()
{
	long i, Num = 0;

	if( m_Type == MOUSE )
	{
		for( i = 0; i < 4; i++ )
			if( ( m_MouseState->rgbButtons[ i ] & 0x80 ) && m_Locks[ i ] == FALSE ) Num++;
	}
	else if( m_Type == JOYSTICK )
	{
		for( i = 0; i < 32; i++ )
			if( ( m_JoystickState->rgbButtons[ i ] & 0x08 ) && m_Locks[ i ] == FALSE ) Num++;
	}

	return Num;
}

long cInputDevice::GetNumPureButtonPresses()
{
	long i, Num = 0;

	if( m_Type == MOUSE )
	{
		for( i = 0; i < 4; i++ ) if( m_MouseState->rgbButtons[ i ] & 0x80 ) Num++;
	}
	else if( m_Type == JOYSTICK )
	{
		for( i = 0; i < 32; i++ ) if( m_JoystickState->rgbButtons[ i ] & 0x08 ) Num++;
	}

	return Num;
}

BOOL FAR PASCAL cInputDevice::EnumJoysticks( LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef )
{
	cInputDevice *Input;

	// Stop enumeration if no parent cInputDevice pointer
	if( ( Input = (cInputDevice *)pvRef ) == NULL ) return DIENUM_STOP;

	// Try to create a joystick interface
	if( FAILED( Input->m_Input->GetDirectInputCOM()->CreateDevice( pdInst->guidInstance, &Input->m_pDIDevice, NULL ) ) ) return DIENUM_CONTINUE;

	// All done - stop enumeration
	return DIENUM_STOP;
}

