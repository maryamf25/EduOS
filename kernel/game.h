#ifndef GAME_H
#define GAME_H

void game_start();
int game_handle_input(char* input); // returns 1 when game ends
int game_is_active();

#endif
