#pragma once

#include "GLFW/glfw3.h"
#include <iostream>

namespace GolfInput
{
	struct Gamepad
	{
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

		int PrevState[Button::NUM_BUTTONS];
		int CurrentState[Button::NUM_BUTTONS];

		bool Pressed[Button::NUM_BUTTONS];
		bool Released[Button::NUM_BUTTONS];
		bool Held[Button::NUM_BUTTONS];

		glm::vec2 LeftStick;
		glm::vec2 RightStick;

		float LeftTrigger;
		float RightTrigger;

		void Update()
		{

			int count;
			const unsigned char* Hit = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

			for (int i = 0; i < Button::NUM_BUTTONS; i++)
			{
				PrevState[i] = CurrentState[i];
				CurrentState[i] = Hit[i];

				Pressed[i] = PrevState[i] == 0 && CurrentState[i] == 1;
				Released[i] = CurrentState[i] == 0;
				Held[i] = CurrentState[i] == 1;
			}

			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

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
			LeftStick = glm::vec2(axes[0], axes[1]);
			if (glm::length(LeftStick) < 0.1f)
				LeftStick = {0,0};

			RightStick = glm::vec2(axes[2], axes[3]);
			if (glm::length(RightStick) < 0.1f)
				RightStick = {0,0};

			LeftTrigger = axes[4];
			RightTrigger = axes[5];
#endif
		}
	};
}
