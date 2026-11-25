#include "pch.h"
#include "camera.h"

#include "render.h"

// ---------------------------------------------------------------------------------------------
// Reference: Pokemon Pinball: Ruby & Sapphire (GBA), pret/pokepinballrs decompilation,
// src/all_board_process2.c, UpdateFrameProcess2_CameraShakeDrain_MainBoards().
//
// What the GBA game does each frame (59.73 Hz, 240x160 screen, ball physics in 4 substeps):
//
//   lookTarget = 0x1000 + velocity.y * 40          // Q8: 16px below the ball, plus 40 substeps
//                                                  //     (= 10 frames) of vertical velocity
//   lookAhead += (lookTarget - lookAhead) / 10     // look-ahead eases in over ~10 frames
//   focus      = clamp(ball.y + lookAhead, minY, maxY) - 80   // 80 = half the screen height
//   step       = (focus - cameraY) / 12            // camera closes 1/12 of the gap per frame
//   if (|step| < 80/256 px) step = 0               // dead zone: no micro-jitter when settled
//   cameraY   += step
//   bgOffsetY  moves toward cameraY (+ tilt/intro offset) by at most 8 px per frame
//   cameraX    slides 1 px/frame toward 16 if the ball is in the plunger lane (x >= 230), else 0
//   On board start: an intro offset of -244 px pans the view from the top to the plunger at
//   1 px/frame while the ball waits in the plunger.
//
// All distances here are expressed in "GBA pixels" and scaled by k = viewHeight / 160, so the
// camera behaves the same relative to the screen as it does on the Game Boy Advance.
//
// Horizontal: the GBA board is only 16 px wider than the screen, so a two-position slide is
// enough. When zoomed in on Space Cadet the overflow is much larger, so the horizontal target
// is proportional to the ball's x position (the ball can never leave the screen) and uses the
// same 1/12 easing, dead zone and 8 px/frame limit as the vertical axis.
// ---------------------------------------------------------------------------------------------

namespace
{
	constexpr float GbaFrameMs = 1000.0f / 59.7275f;
	constexpr float GbaScreenH = 160.0f;
	constexpr float LookAheadBase = 16.0f;            // 0x1000 Q8
	constexpr float LookAheadFrames = 10.0f;          // velocity.y * 40 substeps / 4 substeps per frame
	constexpr float LookAheadSmoothing = 10.0f;
	constexpr float FollowDivisor = 12.0f;
	constexpr float DeadZoneStep = 80.0f / 256.0f;
	constexpr float MaxScrollPerFrame = 8.0f;
	constexpr float MaxBallSpeed = 272.0f * 4.0f / 256.0f; // speed cap from UPDATE_BALL_POSITION, px/frame
	constexpr float IntroPanPerFrame = 1.0f;
	constexpr int MaxStepsPerPresent = 8;

	float clampf(float v, float lo, float hi)
	{
		return v < lo ? lo : (v > hi ? hi : v);
	}

	float applyDeadZone(float step, float k)
	{
		return std::fabs(step) < DeadZoneStep * k ? 0.0f : step;
	}
}

float camera::Zoom = 1.6f;
int camera::HudTop = 0;
bool camera::initialized = false, camera::introPending = true;
float camera::accumulatorMs = 0;
float camera::camX = 0, camera::camY = 0;
float camera::viewX = 0, camera::viewY = 0;
float camera::lookAhead = 0;
float camera::scrollOffset = 0;
float camera::prevBallX = 0, camera::prevBallY = 0;
bool camera::prevBallValid = false;
float camera::ballVelY = 0;

void camera::StartIntro()
{
	introPending = true;
}

bool camera::FindBall(float* x, float* y)
{
	return render::get_focus_ball(x, y);
}

void camera::Step(float viewW, float viewH, int tableW, int tableH, bool haveBall, float ballX, float ballY)
{
	const float k = viewH / GbaScreenH;
	const float maxCamY = tableH - viewH;
	const float maxCamX = tableW - viewW;

	// Board intro pan (cameraScrollOffset -> 0 at 1 px/frame).
	if (scrollOffset < 0)
		scrollOffset = std::min(0.0f, scrollOffset + IntroPanPerFrame * k);

	// Vertical look-ahead, eased.
	float lookTarget = LookAheadBase * k;
	if (haveBall)
		lookTarget += clampf(ballVelY, -MaxBallSpeed * k, MaxBallSpeed * k) * LookAheadFrames;
	lookAhead += (lookTarget - lookAhead) / LookAheadSmoothing;

	if (maxCamY <= 0)
	{
		camY = maxCamY / 2; // view taller than table: center it
	}
	else if (haveBall)
	{
		float focus = clampf(ballY + lookAhead, viewH / 2, viewH / 2 + maxCamY) - viewH / 2;
		camY += applyDeadZone((focus - camY) / FollowDivisor, k);
	}

	if (maxCamX <= 0)
	{
		camX = maxCamX / 2;
	}
	else if (haveBall)
	{
		float target = clampf(ballX / tableW, 0.0f, 1.0f) * maxCamX;
		camX += applyDeadZone((target - camX) / FollowDivisor, k);
	}

	// Rendered scroll follows the camera, limited to 8 px per frame.
	float limit = MaxScrollPerFrame * k;
	viewY += clampf(camY + scrollOffset - viewY, -limit, limit);
	viewX += clampf(camX - viewX, -limit, limit);
}

void camera::Update(float realDtMs, int tableW, int tableH, int outW, int outH,
                    int shakeX, int shakeY, SDL_Rect* src, SDL_FRect* dst)
{
	// Magnification is based on the whole screen so the HUD doesn't change the zoom level;
	// the HUD only takes away some of the view's height.
	const float fit = std::min(static_cast<float>(outW) / tableW, static_cast<float>(outH) / tableH);
	const float scale = fit * Zoom;
	const int hudTop = std::max(0, std::min(HudTop, outH / 2));
	const float viewW = outW / scale;
	const float viewH = (outH - hudTop) / scale;
	const float k = viewH / GbaScreenH;

	float ballX = 0, ballY = 0;
	bool haveBall = FindBall(&ballX, &ballY);

	if (introPending || !initialized)
	{
		// GBA starts at cameraStartX = 16 (plunger side), near the bottom, with the intro pan.
		camX = std::max(0.0f, tableW - viewW);
		camY = std::max(0.0f, tableH - viewH);
		if (tableW - viewW < 0) camX = (tableW - viewW) / 2;
		if (tableH - viewH < 0) camY = (tableH - viewH) / 2;
		scrollOffset = introPending && camY > 0 ? -camY : 0;
		viewX = camX;
		viewY = camY + scrollOffset;
		lookAhead = LookAheadBase * k;
		accumulatorMs = 0;
		prevBallValid = false;
		introPending = false;
		initialized = true;
	}

	// Ball vertical velocity in px per GBA frame, measured from the rendered sprite.
	if (haveBall && prevBallValid && realDtMs > 0)
	{
		float dy = ballY - prevBallY;
		float dx = ballX - prevBallX;
		if (std::fabs(dy) > viewH / 2 || std::fabs(dx) > viewW / 2)
			ballVelY = 0; // teleported (kickout, new ball): don't fling the look-ahead
		else
			ballVelY = dy * (GbaFrameMs / realDtMs);
	}
	else
	{
		ballVelY = 0;
	}
	prevBallValid = haveBall;
	prevBallX = ballX;
	prevBallY = ballY;

	// Run the camera at the GBA frame rate so the per-frame constants keep their meaning.
	accumulatorMs = std::min(accumulatorMs + realDtMs, GbaFrameMs * MaxStepsPerPresent);
	while (accumulatorMs >= GbaFrameMs)
	{
		Step(viewW, viewH, tableW, tableH, haveBall, ballX, ballY);
		accumulatorMs -= GbaFrameMs;
	}

	// Nudge shake moves the table, i.e. the view moves the opposite way.
	const float vx = viewX - shakeX;
	const float vy = viewY - shakeY;

	// Integer source rect covering the view (clamped to the playfield), drawn at a sub-pixel
	// destination offset so scrolling stays smooth at high zoom.
	int sx0 = static_cast<int>(std::floor(std::max(vx, 0.0f)));
	int sy0 = static_cast<int>(std::floor(std::max(vy, 0.0f)));
	int sx1 = static_cast<int>(std::ceil(std::min(vx + viewW, static_cast<float>(tableW))));
	int sy1 = static_cast<int>(std::ceil(std::min(vy + viewH, static_cast<float>(tableH))));
	sx1 = std::max(sx1, sx0 + 1);
	sy1 = std::max(sy1, sy0 + 1);

	*src = SDL_Rect{sx0, sy0, sx1 - sx0, sy1 - sy0};
	*dst = SDL_FRect
	{
		(sx0 - vx) * scale,
		(sy0 - vy) * scale + hudTop,
		(sx1 - sx0) * scale,
		(sy1 - sy0) * scale
	};
}
