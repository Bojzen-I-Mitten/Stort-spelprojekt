#include "Input.h"
#include "ThomasTime.h"
#include "Common.h"
#include "Window.h"
#include "ThomasCore.h"
namespace thomas
{
	//Keyboard
	std::unique_ptr<Keyboard> Input::s_keyboard;
	
	//Mouse
	std::unique_ptr<Mouse> Input::s_mouse;

	//Gamepad
	std::unique_ptr<GamePad> Input::s_gamePad;	

	Input::Input()
	{
		m_lastKey = 0;
		m_mouseMode = MouseMode::POSITION_ABSOLUTE;
		Reset();
	}

	bool Input::Init()
	{
		//Init objects
		m_initialized = false;
		s_vibrateTimeLeft = 0.f;
		
		if(!s_keyboard)
			s_keyboard = std::make_unique<Keyboard>();
		
		if (!s_mouse)
			s_mouse = std::make_unique<Mouse>();
		
		if (!s_gamePad)
		s_gamePad = std::make_unique<GamePad>();
			
		m_initialized = true;
		LOG("Initiating Input");
		return m_initialized;
	}

	void Input::Update()
	{
		if (m_initialized)
		{
			
			s_mouse->SetMode((Mouse::Mode)m_mouseMode);

			m_keyboardState = s_keyboard->GetState();
			m_gamePadState = s_gamePad->GetState(0);	
			m_mouseState = s_mouse->GetState();
			
			s_vibrateTimeLeft -= ThomasTime::GetDeltaTime();

			if(s_vibrateTimeLeft < 0.f)
				s_gamePad->SetVibration(0, 0.f, 0.f);

			m_keyboardTracker.Update(m_keyboardState);
			m_mouseTracker.Update(m_mouseState);
			m_gamePadTracker.Update(m_gamePadState);
			
			// Set mouse mode position
			m_mousePosition = math::Vector2((float)m_mouseState.x, (float)m_mouseState.y);
			
			

			// Update absolute position (if not in relative mode)
			if (m_mouseState.positionMode == Mouse::Mode::MODE_ABSOLUTE)
				m_absolutePosition = m_mousePosition;

		}
	}

	void Input::ProcessKeyboard(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (m_initialized)
			s_keyboard->ProcessMessage(message, wParam, lParam);
	}

	void Input::ProcessMouse(UINT message, WPARAM wParam, LPARAM lParam, HWND handle)
	{
		if (m_initialized)
		{
			s_mouse->SetWindow(handle);
			s_mouse->ProcessMessage(message, wParam, lParam);
		}			
	}

	void Input::ProcessGamePad(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (m_initialized)
		{
			//Needs fixing
	/*		switch (message)
			{

			case WM_SETFOCUS:
			{
				s_gamePad->Resume();
			}
				
			case WM_KILLFOCUS:
				s_gamePad->Suspend();
				break;
			}*/
		}
	}

	//Gamepad
	float Input::GetLeftStickY()
	{
		return m_gamePadState.thumbSticks.leftY;
	}

	float Input::GetLeftStickX()
	{
		return m_gamePadState.thumbSticks.leftX;
	}

	float Input::GetRightStickY()
	{
		return m_gamePadState.thumbSticks.rightY;
	}

	float Input::GetRightStickX()
	{
		return m_gamePadState.thumbSticks.rightX;
	}

	float Input::GetLeftTriggerDelta()
	{
		return m_gamePadState.triggers.left;
	}

	float Input::GetRightTriggerDelta()
	{
		return m_gamePadState.triggers.right;
	}

	//Mouse
	void Input::ResetScrollWheelValue()
	{
		s_mouse->ResetScrollWheelValue();
	}

	void Input::SetMouseMode(MouseMode mode)
	{
		m_mouseMode = mode;
		if (m_mouseState.positionMode == (Mouse::Mode)mode)
			return;

		// Reset mouse pos
		if (mode == MouseMode::POSITION_RELATIVE)
			m_mousePosition = math::Vector2();
		else
			m_mousePosition = m_absolutePosition;
	}

	math::Vector2 Input::GetMousePosition()
	{
		return m_mousePosition;
	}
	math::Vector2 Input::GetAbsolutePosition()
	{
		return m_absolutePosition;
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

	bool Input::GetMouseButtonDown(MouseButtons button)
	{
		switch (button)
		{
		case MouseButtons::LEFT:
			return (m_mouseTracker.leftButton == m_mouseTracker.PRESSED);
		case MouseButtons::RIGHT:
			return (m_mouseTracker.rightButton == m_mouseTracker.PRESSED);
		case MouseButtons::MIDDLE:
			return (m_mouseTracker.middleButton == m_mouseTracker.PRESSED);
		}
		return false;
	}

	bool Input::GetMouseButtonUp(MouseButtons button)
	{
		switch (button)
		{
		case MouseButtons::LEFT:
			return (m_mouseTracker.leftButton == m_mouseTracker.RELEASED);
		case MouseButtons::RIGHT:
			return (m_mouseTracker.rightButton == m_mouseTracker.RELEASED);
		case MouseButtons::MIDDLE:
			return (m_mouseTracker.middleButton == m_mouseTracker.RELEASED);
		}
		return false;
	}

	bool Input::GetMouseButton(MouseButtons button)
	{
		switch (button)
		{
		case MouseButtons::LEFT:
			return m_mouseState.leftButton;
		case MouseButtons::RIGHT:
			return m_mouseState.rightButton;
		case MouseButtons::MIDDLE:
			return m_mouseState.middleButton;
		}
		return false;
	}

	int Input::GetMouseScrollWheel()
	{
		return m_mouseState.scrollWheelValue;
	}

	//Gamepad
	bool Input::GetButtonDown(Buttons button)
	{
		if (!m_gamePadState.IsConnected()) //Always false if no gamePad.
			return false;

		switch (button)
		{
		case Buttons::A:
			return (m_gamePadTracker.a == m_gamePadTracker.PRESSED);
		case Buttons::B:
			return (m_gamePadTracker.b == m_gamePadTracker.PRESSED);
		case Buttons::X:
			return (m_gamePadTracker.x == m_gamePadTracker.PRESSED);
		case Buttons::Y:
			return (m_gamePadTracker.y == m_gamePadTracker.PRESSED);
		case Buttons::DPAD_UP:
			return (m_gamePadTracker.dpadUp == m_gamePadTracker.PRESSED);
		case Buttons::DPAD_DOWN:
			return (m_gamePadTracker.dpadDown == m_gamePadTracker.PRESSED);
		case Buttons::DPAD_LEFT:
			return (m_gamePadTracker.dpadLeft == m_gamePadTracker.PRESSED);
		case Buttons::DPAD_RIGHT:
			return (m_gamePadTracker.dpadRight == m_gamePadTracker.PRESSED);
		case Buttons::LS:
			return (m_gamePadTracker.leftStick == m_gamePadTracker.PRESSED);
		case Buttons::RS:
			return (m_gamePadTracker.rightStick == m_gamePadTracker.PRESSED);
		case Buttons::LB:
			return (m_gamePadTracker.leftShoulder == m_gamePadTracker.PRESSED);
		case Buttons::RB:
			return (m_gamePadTracker.rightShoulder == m_gamePadTracker.PRESSED);
		case Buttons::LT:
			return (m_gamePadTracker.leftTrigger == m_gamePadTracker.PRESSED);
		case Buttons::RT:
			return (m_gamePadTracker.rightTrigger == m_gamePadTracker.PRESSED);
		case Buttons::START:
			return (m_gamePadTracker.start == m_gamePadTracker.PRESSED);
		case Buttons::BACK:
			return (m_gamePadTracker.back == m_gamePadTracker.PRESSED);
		}
		return false;
	}

	bool Input::GetButtonUp(Buttons button)
	{
		if (!m_gamePadState.IsConnected()) //Always false if no gamePad.
			return false;

		switch (button)
		{
		case Buttons::A:
			return (m_gamePadTracker.a == m_gamePadTracker.RELEASED);
		case Buttons::B:
			return (m_gamePadTracker.b == m_gamePadTracker.RELEASED);
		case Buttons::X:
			return (m_gamePadTracker.x == m_gamePadTracker.RELEASED);
		case Buttons::Y:
			return (m_gamePadTracker.y == m_gamePadTracker.RELEASED);
		case Buttons::DPAD_UP:
			return (m_gamePadTracker.dpadUp == m_gamePadTracker.RELEASED);
		case Buttons::DPAD_DOWN:
			return (m_gamePadTracker.dpadDown == m_gamePadTracker.RELEASED);
		case Buttons::DPAD_LEFT:
			return (m_gamePadTracker.dpadLeft == m_gamePadTracker.RELEASED);
		case Buttons::DPAD_RIGHT:
			return (m_gamePadTracker.dpadRight == m_gamePadTracker.RELEASED);
		case Buttons::LS:
			return (m_gamePadTracker.leftStick == m_gamePadTracker.RELEASED);
		case Buttons::RS:
			return (m_gamePadTracker.rightStick == m_gamePadTracker.RELEASED);
		case Buttons::LB:
			return (m_gamePadTracker.leftShoulder == m_gamePadTracker.RELEASED);
		case Buttons::RB:
			return (m_gamePadTracker.rightShoulder == m_gamePadTracker.RELEASED);
		case Buttons::LT:
			return (m_gamePadTracker.leftTrigger == m_gamePadTracker.RELEASED);
		case Buttons::RT:
			return (m_gamePadTracker.rightTrigger == m_gamePadTracker.RELEASED);
		case Buttons::START:
			return (m_gamePadTracker.start == m_gamePadTracker.RELEASED);
		case Buttons::BACK:
			return (m_gamePadTracker.back == m_gamePadTracker.RELEASED);
		}
		return false;
	}

	bool Input::GetButton(Buttons button)
	{
		if (!m_gamePadState.IsConnected()) //Always false if no gamePad.
			return false;

		switch (button)
		{
		case Buttons::A:
			//return m_gamePadState.IsAPressed();
			return (m_gamePadTracker.a == m_gamePadTracker.HELD);
		case Buttons::B:
			return (m_gamePadTracker.b == m_gamePadTracker.HELD);
		case Buttons::X:
			return (m_gamePadTracker.x == m_gamePadTracker.HELD);
		case Buttons::Y:
			return (m_gamePadTracker.y == m_gamePadTracker.HELD);
		case Buttons::DPAD_UP:
			return (m_gamePadTracker.dpadUp == m_gamePadTracker.HELD);
		case Buttons::DPAD_DOWN:
			return (m_gamePadTracker.dpadDown == m_gamePadTracker.HELD);
		case Buttons::DPAD_LEFT:
			return (m_gamePadTracker.dpadLeft == m_gamePadTracker.HELD);
		case Buttons::DPAD_RIGHT:
			return (m_gamePadTracker.dpadRight == m_gamePadTracker.HELD);
		case Buttons::LS:
			return (m_gamePadTracker.leftStick == m_gamePadTracker.HELD);
		case Buttons::RS:
			return (m_gamePadTracker.rightStick == m_gamePadTracker.HELD);
		case Buttons::LB:
			return (m_gamePadTracker.leftShoulder == m_gamePadTracker.HELD);
		case Buttons::RB:
			return (m_gamePadTracker.rightShoulder == m_gamePadTracker.HELD);
		case Buttons::LT:
			return (m_gamePadTracker.leftTrigger == m_gamePadTracker.HELD);
		case Buttons::RT:
			return (m_gamePadTracker.rightTrigger == m_gamePadTracker.HELD);
		case Buttons::START:
			return (m_gamePadTracker.start == m_gamePadTracker.HELD);
		case Buttons::BACK:
			return (m_gamePadTracker.back == m_gamePadTracker.HELD);
		}
		return false;
	}

	void Input::Vibrate(float left, float right, float time)
	{
		if (m_gamePadState.IsConnected()) //Always false if no gamePad.
		{
			s_gamePad->SetVibration(0, left, right);
			s_vibrateTimeLeft = time;
		}		
	}

	void Input::Reset()
	{
		m_keyboardTracker.Reset();
		m_mouseTracker.Reset();
		m_gamePadTracker.Reset();
		m_keyboardState = Keyboard::State();
		m_gamePadState = GamePad::State();
		m_mouseState = Mouse::State();
		m_mouseMode = MouseMode::POSITION_ABSOLUTE;
	}

	//Keyboard
	bool Input::GetKeyDown(Keys key)
	{
		return m_keyboardTracker.IsKeyPressed((Keyboard::Keys)key);
	}	

	bool Input::GetKeyUp(Keys key)
	{
		return m_keyboardTracker.IsKeyReleased((Keyboard::Keys)key);
	}

	bool Input::GetKey(Keys key)
	{
		return m_keyboardState.IsKeyDown((Keyboard::Keys)key);
	}

	void Input::SetLastKey(unsigned short key)
	{
		m_lastKey = key;
	}

	unsigned short Input::GetLastKey() const
	{
		return m_lastKey;
	}

	Input::MouseMode Input::GetMouseMode()
	{
		return m_mouseMode;
	}
}
