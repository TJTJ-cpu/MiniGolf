#pragma once

#include "GLFW/glfw3.h"
#include <iostream>

namespace GolfInput
{
	struct Gamepad
	{
#ifdef __linux__
		enum Button {
			A_BUTTON,
			B_BUTTON,
			X_BUTTON,
			Y_BUTTON,
			LEFT_SHOULDER_BUTTON,
			RIGHT_SHOULDER_BUTTON,
			SELECT,
			START,
			HOME_BUTTON,
			LEFT_STICK_CLICK,
			RIGHT_STICK_CLICK,
			DPAD_UP,
			DPAD_RIGHT,
			DPAD_DOWN,
			DPAD_LEFT,
			NUM_BUTTONS
		};
#elif _WIN32
		enum Button {
			A_BUTTON,
			B_BUTTON,
			X_BUTTON,
			Y_BUTTON,
			LEFT_SHOULDER_BUTTON,
			RIGHT_SHOULDER_BUTTON,
			SELECT,
			START,
			LEFT_STICK_CLICK,
			RIGHT_STICK_CLICK,
			DPAD_UP,
			DPAD_RIGHT,
			DPAD_DOWN,
			DPAD_LEFT,
			NUM_BUTTONS
		};
#endif

		int PrevState[Button::NUM_BUTTONS];
		int CurrentState[Button::NUM_BUTTONS];

		bool Pressed[Button::NUM_BUTTONS]{ false };
		bool Released[Button::NUM_BUTTONS]{ false };
		bool Held[Button::NUM_BUTTONS]{ false };
		bool JustReleased[Button::NUM_BUTTONS]{ false };

		glm::vec2 LeftStick{ 0, 0 };
		glm::vec2 RightStick{ 0, 0 };

		float LeftTrigger{ 0 };
		float RightTrigger{ 0 };

		int CurrentJoystick = GLFW_JOYSTICK_1;
		bool ANY_BUTTON = false;

		void Update()
		{

			int count;
			const unsigned char* Hit = glfwGetJoystickButtons(CurrentJoystick, &count);

			if (Hit == nullptr)
				return;

			ANY_BUTTON = false;
			for (int i = 0; i < Button::NUM_BUTTONS; i++)
			{
				PrevState[i] = CurrentState[i];
				CurrentState[i] = Hit[i];

				Pressed[i] =		CurrentState[i] == GLFW_PRESS	&& PrevState[i] == GLFW_RELEASE;
				Released[i] =		CurrentState[i] == GLFW_RELEASE;
				Held[i] =			CurrentState[i] == GLFW_PRESS;
				JustReleased[i] =	CurrentState[i] == GLFW_RELEASE && PrevState[i] == GLFW_PRESS;
				ANY_BUTTON |= Pressed[i] == 1;
			}

			const float* axes = glfwGetJoystickAxes(CurrentJoystick, &count);

#ifdef __linux__
			LeftStick = glm::vec2(axes[0], -axes[1]);
			if (glm::length(LeftStick) < 0.1f)
				LeftStick = {0,0};

			RightStick = glm::vec2(axes[3], -axes[4]);
			if (glm::length(RightStick) < 0.1f)
				RightStick = {0,0};

			LeftTrigger = axes[2];
			RightTrigger = axes[5];
#elif _WIN32
			LeftStick = glm::vec2(axes[0], -axes[1]);
			if (glm::length(LeftStick) < 0.1f)
				LeftStick = {0,0};

			RightStick = glm::vec2(axes[2], -axes[3]);
			if (glm::length(RightStick) < 0.1f)
				RightStick = {0,0};

			LeftTrigger = axes[4];
			RightTrigger = axes[5];
#endif
		}
	};
}
