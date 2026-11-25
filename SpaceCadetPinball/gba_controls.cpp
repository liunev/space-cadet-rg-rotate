#include "pch.h"
#include "gba_controls.h"

#include "nudge.h"
#include "pb.h"
#include "TPinballTable.h"

namespace
{
	constexpr float GbaFrameMs = 1000.0f / 59.7275f;
	constexpr float TiltDelayMs = 4 * GbaFrameMs;     // boardShakeIntensity = 4
	constexpr float TiltCooldownMs = 24 * GbaFrameMs; // boardShakeTimer = 24

	enum class Tilt { None, Left, Right, Up };

	bool lHeld = false, rHeld = false;
	Tilt pendingTilt = Tilt::None;
	float pendingMs = 0, cooldownMs = 0;

	void QueueTilt(Tilt direction)
	{
		if (pendingTilt == Tilt::None)
		{
			if (cooldownMs > 0)
				return;
			pendingMs = TiltDelayMs;
		}
		pendingTilt = direction;
	}

	void FireTilt(Tilt direction)
	{
		if (!pb::MainTable || pb::MainTable->TiltLockFlag)
			return;

		// Same actions as Space Cadet's Left/Right/Bottom table bump keys.
		switch (direction)
		{
		case Tilt::Left:
			nudge::nudge_right();
			break;
		case Tilt::Right:
			nudge::nudge_left();
			break;
		case Tilt::Up:
			nudge::nudge_up();
			break;
		default:
			break;
		}
	}

	void Message(int code)
	{
		if (pb::MainTable)
			pb::MainTable->Message(code, pb::time_now);
	}
}

bool gba_controls::Enabled = true;

void gba_controls::Reset()
{
	lHeld = rHeld = false;
	pendingTilt = Tilt::None;
	pendingMs = cooldownMs = 0;
}

bool gba_controls::InputDown(int button)
{
	switch (button)
	{
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		Message(1000); // left flipper
		return true;
	case SDL_CONTROLLER_BUTTON_A:
		Message(1002); // right flipper
		return true;
	case SDL_CONTROLLER_BUTTON_B:
		Message(1004); // plunger
		return true;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		lHeld = true;
		QueueTilt(rHeld ? Tilt::Up : Tilt::Left);
		return true;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		rHeld = true;
		QueueTilt(lHeld ? Tilt::Up : Tilt::Right);
		return true;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
	case SDL_CONTROLLER_BUTTON_X:
	case SDL_CONTROLLER_BUTTON_Y:
		return true; // unused in the GBA layout
	default:
		return false; // Start etc. keep their normal behavior
	}
}

bool gba_controls::InputUp(int button)
{
	switch (button)
	{
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		Message(1001);
		return true;
	case SDL_CONTROLLER_BUTTON_A:
		Message(1003);
		return true;
	case SDL_CONTROLLER_BUTTON_B:
		Message(1005);
		return true;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		lHeld = false;
		return true;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		rHeld = false;
		return true;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
	case SDL_CONTROLLER_BUTTON_X:
	case SDL_CONTROLLER_BUTTON_Y:
		return true;
	default:
		return false;
	}
}

void gba_controls::Tick(float dtMs)
{
	if (cooldownMs > 0)
		cooldownMs -= dtMs;

	if (pendingTilt != Tilt::None)
	{
		pendingMs -= dtMs;
		if (pendingMs <= 0)
		{
			FireTilt(pendingTilt);
			pendingTilt = Tilt::None;
			cooldownMs = TiltCooldownMs;
		}
	}
}
