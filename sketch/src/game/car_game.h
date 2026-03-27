#ifndef _CAR_GAME_H_
#define _CAR_GAME_H_

#ifdef __cplusplus
extern "C" {
#endif

void game_init();
void game_loop(float dt);
void* get_game_buffer();

#ifdef __cplusplus
}
#endif

#endif
