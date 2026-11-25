<!-- markdownlint-disable-file MD033 -->

# Space Cadet RG Rotate

A fork of [fexed/Pinball-on-Android](https://github.com/fexed/Pinball-on-Android) tuned for the
**Anbernic RG Rotate** (3.5" 720x720 square screen) and other small displays.

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

Tilt timing matches the GBA's `ProcessTiltInput`: a nudge fires 4 frames after the press, the
last direction pressed in that window wins (that's how L+R becomes "up"), and there's a
24-frame cooldown before the next one. Turn the setting off for Space Cadet's original
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

---

# Pinball-on-Android (upstream README)
[![Android CI](https://github.com/fexed/Pinball-on-Android/actions/workflows/android.yml/badge.svg)](https://github.com/fexed/Pinball-on-Android/actions/workflows/android.yml)
[![Validate Fastlane](https://github.com/fexed/Pinball-on-Android/actions/workflows/build.yml/badge.svg)](https://github.com/fexed/Pinball-on-Android/actions/workflows/build.yml)
## Summary

Android port of `3D Pinball for Windows – Space Cadet`, a game bundled with Windows.

Thanks to [k4zmu2a](https://github.com/k4zmu2a) for the original source code and to [iscle](https://github.com/Iscle) for the original Android porting

## How to play

Download the latest available [APK](https://github.com/fexed/Pinball-on-Android/releases)

## Articles

Some articles were written about this project! Thank you to everyone. Some articles I found are on: [Frandroid](https://www.frandroid.com/android/applications/1384165_un-developpeur-ressuscite-le-pinball-3d-de-windows-sur-android-voici-comment-y-jouer), [TudoCelular](https://www.tudocelular.com/android/noticias/n192126/windows-xp-jogo-classico-pinball-3d-android.html), [Libero Tecnologia](https://tecnologia.libero.it/uno-studente-italiano-ha-portato-il-flipper-di-windows-su-android-58522), [Oxii](https://www.oxii.vn/tech/tua-game-huyen-thoai-tren-windows-pinball-3d-da-co-phien-ban-tren-android-39480.html), [Tugatech](https://tugatech.com.pt/t46867-space-cadet-do-windows-agora-chega-tambem-ao-android), [Robotina](https://www.robotina.us/entretenimiento/El-Pinball-tiene-una-version-disponible-para-celulares-Android-20220701-0004.html), [Androiditaly](https://www.androiditaly.com/2022/07/02/3d-pinball-space-cadet-arriva-su-android-grazie-ad-un-progetto-amatoriale-tutto-italiano/), [Pasion Movil](https://www.pasionmovil.com/videojuegos-2/3d-pinball-el-clasico-juego-de-windows-xp-llega-a-dispositivos-android/), [Clubic](https://www.clubic.com/pro/entreprises/microsoft/actualite-429654-vous-en-reviez-et-ils-l-ont-fait-3d-pinball-de-windows-obtient-un-portage-sur-android.html), [Meteoweek](https://tech.meteoweek.com/2022/07/10/3d-pinball-non-e-piu-unesclusiva-windows-space-cadet-sara-anche-per-android-grazie-ad-uno-studente/), 

## Known source ports

| Platform           | Author          | URL                                                                                                        |
| ------------------ | --------------- | ---------------------------------------------------------------------------------------------------------- |
| Original source    | k4zmu2a         |<https://github.com/k4zmu2a/SpaceCadetPinball>                                                              |
| PS Vita            | Axiom           | <https://github.com/suicvne/SpaceCadetPinball_Vita>                                                        |
| Emscripten         | alula           | <https://github.com/alula/SpaceCadetPinball> <br> Play online: <https://alula.github.io/SpaceCadetPinball> |
| Nintendo Switch    | averne          | <https://github.com/averne/SpaceCadetPinball-NX>                                                           |
| webOS TV           | mariotaku       | <https://github.com/webosbrew/SpaceCadetPinball>                                                           |
| Android (WIP)      | Iscle           | https://github.com/Iscle/SpaceCadetPinball                                                                 |
| Nintendo Wii       | MaikelChan      | https://github.com/MaikelChan/SpaceCadetPinball                                                            |
| Nintendo 3DS       | MaikelChan      | https://github.com/MaikelChan/SpaceCadetPinball/tree/3ds                                                   |
| Nintendo Wii U     | IntriguingTiles | https://github.com/IntriguingTiles/SpaceCadetPinball-WiiU                                                  |
| MorphOS            | BeWorld         | https://www.morphos-storage.net/?id=1688897                                                                |
| AmigaOS 4          | rjd324          | http://aminet.net/package/game/actio/spacecadetpinball-aos4                                                |

Platforms covered by this project: Android.
