#pragma once

// Ball-following camera for small screens (built for the Anbernic RG Rotate, 720x720).
//
// The motion model is a port of the camera from Pokemon Pinball: Ruby & Sapphire (GBA),
// taken from the pret decompilation (src/all_board_process2.c,
// UpdateFrameProcess2_CameraShakeDrain_MainBoards). See camera.cpp for the details.

class camera
{
public:
	// Zoom relative to "whole table fits on screen". <= 1 disables the camera
	// and falls back to the original full-window rendering (table + sidebar).
	static float Zoom;

	static bool Enabled() { return Zoom > 1.0f; }

	// Output pixels at the top of the screen covered by the Android HUD bar (score, ball,
	// mission text). The table is drawn below it; the zoom level is unaffected.
	static int HudTop;

	// Snap to the plunger and play the board-intro pan (top of table -> plunger),
	// like the GBA game does when a board starts.
	static void StartIntro();

	// Advance the camera by realDtMs and compute the table-space view to draw.
	// tableW/tableH: playfield size in vscreen pixels.
	// outW/outH: output size in screen pixels (the view is placed below HudTop).
	// shakeX/shakeY: nudge offset from render::shift (vscreen pixels).
	// Outputs the integer source rect plus a sub-pixel destination rect.
	static void Update(float realDtMs, int tableW, int tableH, int outW, int outH,
	                   int shakeX, int shakeY, struct SDL_Rect* src, struct SDL_FRect* dst);

private:
	static bool initialized, introPending;
	static float accumulatorMs;
	static float camX, camY;       // camera target position (top-left of view), table px
	static float viewX, viewY;     // rendered position, rate limited, table px
	static float lookAhead;        // vertical look-ahead, table px
	static float scrollOffset;     // intro pan offset, table px (negative = above)
	static float prevBallX, prevBallY;
	static bool prevBallValid;
	static float ballVelY;         // px per GBA frame

	static bool FindBall(float* x, float* y);
	static void Step(float viewW, float viewH, int tableW, int tableH, bool haveBall, float ballX, float ballY);
};
