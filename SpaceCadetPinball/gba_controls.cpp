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
	constexpr float Never = 1e30f;

	enum class Tilt { None, Left, Right, Up };

	// Game time, advanced by Tick.
	float nowMs = 0;
	bool lHeld = false, rHeld = false;

	// Tilt waiting for the GBA's 4-frame window (last direction pressed wins).
	Tilt pendingTilt = Tilt::None;
	float pendingMs = 0, pressAtMs = 0, heldForMs = -1; // heldForMs < 0: still held
	float cooldownMs = 0;

	// Nudge currently applied to the table.
	// Space Cadet's tilt meter fills at 4x real time while a nudge is held ("Careful..." at 0.5,
	// tilt at 1.0) and drains at 1x, so the nudge must end when the button is released, like
	// the keyboard does. Otherwise it runs for nudge.cpp's full 0.4 s cap and tilts at once.
	Tilt activeTilt = Tilt::None;
	float releaseAtMs = Never;
	float fireDelayMs = 0; // press -> nudge start, added to the release too

	bool ButtonsHeld(Tilt direction)
	{
		switch (direction)
		{
		case Tilt::Left: return lHeld;
		case Tilt::Right: return rHeld;
		case Tilt::Up: return lHeld || rHeld;
		default: return false;
		}
	}

	bool NudgeStillOn(Tilt direction)
	{
		switch (direction)
		{
		case Tilt::Left: return nudge::nudged_right != 0;
		case Tilt::Right: return nudge::nudged_left != 0;
		case Tilt::Up: return nudge::nudged_up != 0;
		default: return false;
		}
	}

	void ReleaseNudge()
	{
		// Same calls as Space Cadet's key-up for the table bump keys.
		switch (activeTilt)
		{
		case Tilt::Left:
			nudge::un_nudge_right(0, nullptr);
			break;
		case Tilt::Right:
			nudge::un_nudge_left(0, nullptr);
			break;
		case Tilt::Up:
			nudge::un_nudge_up(0, nullptr);
			break;
		default:
			break;
		}
		activeTilt = Tilt::None;
		releaseAtMs = Never;
	}

	void QueueTilt(Tilt direction)
	{
		if (pendingTilt == Tilt::None)
		{
			if (cooldownMs > 0 || activeTilt != Tilt::None)
				return;
			pendingMs = TiltDelayMs;
			pressAtMs = nowMs;
		}
		pendingTilt = direction;
		heldForMs = -1;
	}

	void OnButtonReleased()
	{
		if (pendingTilt != Tilt::None && heldForMs < 0 && !ButtonsHeld(pendingTilt))
			heldForMs = nowMs - pressAtMs;
		if (activeTilt != Tilt::None && releaseAtMs == Never && !ButtonsHeld(activeTilt))
			releaseAtMs = nowMs + fireDelayMs;
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
			return;
		}
		activeTilt = direction;
		// The nudge lasts as long as the button was held, shifted by the GBA delay.
		fireDelayMs = nowMs - pressAtMs;
		releaseAtMs = heldForMs >= 0 ? nowMs + heldForMs : Never;
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
	if (activeTilt != Tilt::None)
		ReleaseNudge();
	lHeld = rHeld = false;
	pendingTilt = Tilt::None;
	pendingMs = cooldownMs = 0;
	heldForMs = -1;
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
		OnButtonReleased();
		return true;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		rHeld = false;
		OnButtonReleased();
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
	nowMs += dtMs;
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

	if (activeTilt != Tilt::None)
	{
		if (!NudgeStillOn(activeTilt))
		{
			// nudge.cpp's 0.4 s auto-release (or a new nudge) already ended it.
			activeTilt = Tilt::None;
			releaseAtMs = Never;
		}
		else if (nowMs >= releaseAtMs)
		{
			ReleaseNudge();
		}
	}
}
