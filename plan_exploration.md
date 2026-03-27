# Wait, let's fix these code review points!
1. `qmi8658_init()` was NOT in `main.cpp`, the grep shows `//#include "qmi8658/qmi8658.h" // imu`.
My previous replace failed or was reverted.
2. `drawStartScreen(animTime)` is in `game_init()` in `car_game.cpp`. It blocks for 3 seconds. I must remove the `while` loop!
3. `ui_Player_Game_Display` hiding logic missing from `livestreamMode` and `musicPlayerMode`. I will add it.
4. `Spotify` constructor signature in `SpotifyEsp32.h` is:
   `Spotify(const char* client_id, const char* client_secret, int max_num_retry = 3)`
   or `Spotify(const char* client_id, const char* client_secret, const char* refresh_token, int max_num_retry = 3)`
Wait, the grep output showed:
```cpp
  Spotify(const char* client_id, const char* client_secret, int max_num_retry = 3);
```
Let me grep the constructor with `refresh_token`.
