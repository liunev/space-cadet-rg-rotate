<!-- markdownlint-disable-file MD033 -->

# Space Cadet RG Rotate

A fork of [fexed/Pinball-on-Android](https://github.com/fexed/Pinball-on-Android) tuned for the
**Anbernic RG Rotate** (3.5" 720x720 square screen) and other small displays.

## Install

Download the APK from the [latest release](https://github.com/liunev/space-cadet-rg-rotate/releases/latest)
and open it on the device, or install it from a computer with `adb install space-cadet-rg-rotate-v1.0.apk`.
It installs as `com.liunev.spacecadetrgrotate`, next to the original app if you have it.
Settings tells you when a newer release is available.

What's different from the original Android port:

- Ball-following camera (from Pokémon Pinball: Ruby & Sapphire) with a score/mission bar
- Pokémon Pinball (GBA) gamepad layout, Start/Select for pause/new game
- High scores stored on the device; no online leaderboard, no analytics
- Upstream flipper fix ([k4zmu2a#150](https://github.com/k4zmu2a/SpaceCadetPinball/pull/150))

## Ball camera

The whole table plus the score sidebar is tiny on a 3.5" screen, so this fork adds a zoomed
camera that follows the ball. It's a port of the camera from **Pokemon Pinball: Ruby & Sapphire**
(GBA), taken from the [pret/pokepinballrs](https://github.com/pret/pokepinballrs) decompilation
(`src/all_board_process2.c`, `UpdateFrameProcess2_CameraShakeDrain_MainBoards`):

- **Look-ahead:** the camera centres on a point 16 px below the ball plus 10 frames of the ball's
  vertical velocity, so it leads toward the flippers when the ball falls and toward the top when
  it is shot up. The look-ahead eases in (1/10 per frame).
- **Easing:** each frame the camera closes 1/12 of the distance to that point.
- **Dead zone:** steps smaller than 80/256 px are dropped, so the view stays still when settled.
- **Scroll limit:** the drawn view moves at most 8 px per frame.
- **Board intro:** when a game starts, the view pans from the top of the table down to the plunger.
- **Nudges** still shake the table.

Distances are scaled by `view height / 160` (the GBA screen height), so the motion feels the same
relative to the screen as on the Game Boy Advance. The GBA board is only 16 px wider than its
screen; Space Cadet zoomed in is much wider than the view, so horizontally the camera pans in
proportion to the ball's x position, with the same easing, dead zone and speed limit. During
multiball it follows the lowest ball.

Zoom is set in **Settings > Ball camera zoom** (1.0x = original whole-table view, default 1.6x,
max 3.0x). While zoomed, the score, ball number and the info/mission text from the game's
sidebar move into a two-row bar at the top of the screen, and the table is drawn below it, so nothing
covers the playfield. Setting the zoom to 1.0x brings back the original layout.

Code: [`SpaceCadetPinball/camera.cpp`](SpaceCadetPinball/camera.cpp).

## Pokemon Pinball (GBA) controls

On by default (Settings > Pokemon Pinball (GBA) controls). Uses the GBA game's default layout
from the decompilation (`src/options.c`, `SetButtonConfigInputs`):

| Button | Action |
| --- | --- |
| D-pad Left | Left flipper |
| A | Right flipper |
| B | Launch ball |
| L / R | Nudge left / right |
| L + R together | Nudge up |
| Start | Pause / resume (F3) |
| Select | New game (F2): one press after game over, two presses within 3 s during a game |
| Home | Back to the Android home screen; the game keeps its state |

Nudge timing takes the GBA's `ProcessTiltInput` delay: a nudge starts 4 frames after the press
and the last direction pressed in that window wins (that's how L+R becomes "up"). The GBA's
cooldown between tilts is left out. The nudge lasts as long as you hold the button, like
Space Cadet's keyboard nudge keys, so its tilt meter works as in the original: short taps are
safe, while holding a nudge or tapping rapidly shows "Careful..." and then tilts the table.

Turn the setting off for Space Cadet's original
controller layout (L1/R1 flippers, A plunger, D-pad nudge).

The in-game "F3 to Resume" / "F2 Starts New Game" messages name Start and Select instead.
While it's on, the on-screen touch zone labels (Left/Right/Tilt) and the "keep pressed to
launch" hint are hidden; the touch zones themselves still work.

Code: [`SpaceCadetPinball/gba_controls.cpp`](SpaceCadetPinball/gba_controls.cpp).

## High scores and updates

The upstream online leaderboard is removed: nothing is uploaded or downloaded. Finished games
are stored on the device instead (top 20, with name, date and whether cheats were used) and
shown under **Settings > Show high scores**, where they can also be cleared.

**Settings** checks this fork's [GitHub releases](https://github.com/liunev/space-cadet-rg-rotate/releases)
for updates. Tag releases `v<versionName>` (e.g. `v1.1`); a release is offered when its tag is a
higher version than the installed `versionName`.

## Building

Requires JDK 17-21, Android SDK platform 35, NDK `25.2.9519653` and CMake 4.1.2 (via the SDK
manager). Then `./gradlew assembleDebug`. The fork installs as `com.liunev.spacecadetrgrotate`, so it
can sit next to the original app. Firebase analytics was removed.

Release builds (`./gradlew assembleRelease`) are signed when a git-ignored `keystore.properties`
in the repo root provides `storeFile`, `storePassword`, `keyAlias` and `keyPassword`; without
it the release APK is unsigned. To publish: bump `versionCode`/`versionName` in
`app/build.gradle`, add `fastlane/metadata/android/en-US/changelogs/<versionCode>.txt`, and
attach the APK to a GitHub release tagged `v<versionName>`.

## Credits

- 3D Pinball for Windows – Space Cadet: Cinematronics / Maxis, published by Microsoft (1995)
- Decompilation and reverse engineering: [k4zmu2a/SpaceCadetPinball](https://github.com/k4zmu2a/SpaceCadetPinball) (MIT, see [LICENSE](LICENSE))
- Original Android port: [Iscle](https://github.com/Iscle)
- Android port: [fexed/Pinball-on-Android](https://github.com/fexed/Pinball-on-Android) (fexed, th3vanger)
- Camera and controls reference: [pret/pokepinballrs](https://github.com/pret/pokepinballrs)
- RG Rotate port: liunev

The game data (table, sounds, music) belongs to its original copyright holders.
