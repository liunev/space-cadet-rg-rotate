#pragma once

// Pokemon Pinball: Ruby & Sapphire (GBA) controller layout.
//
// Default GBA button config, from the pret/pokepinballrs decompilation
// (src/options.c, SetButtonConfigInputs / BUTTON_CONFIG_RESET):
//   Left flipper  = D-pad Left
//   Right flipper = A
//   Tilt left     = L
//   Tilt right    = R
//   Tilt up       = L + R together
// Ball launch is on B here.
//
// Tilt timing follows ProcessTiltInput (src/all_board_process2.c): a tilt fires 4 frames after
// the press, the last direction pressed in that window wins (so L and R pressed together give
// an upward tilt), and no new tilt can start for 24 frames afterwards.

class gba_controls
{
public:
	static bool Enabled;

	// Return true when the controller button was handled by this layout.
	static bool InputDown(int button);
	static bool InputUp(int button);
	static void Tick(float dtMs);
	static void Reset();
};
