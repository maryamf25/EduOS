#include "game.h"
#include "../drivers/screen.h"
#include "../drivers/pit.h"
#include "../libc/string.h"

static int active = 0;
static int target = 0;
static int tries = 0;

static int to_int(char* s) {
    int n = 0; int i = 0;
    while (s[i]) {
        if (s[i] < '0' || s[i] > '9') return -1;
        n = n*10 + (s[i]-'0');
        i++;
    }
    return n;
}

int game_is_active() { return active; }

void game_start() {
    active = 1;
    tries = 5;
    // Pseudo-random from PIT ticks
    target = (pit_ticks() % 100) + 1;
    clear_screen();
    kprint("=== Guess The Number ===\n");
    kprint("I picked a number 1-100. You have 5 tries.\n");
    kprint("Type a number or 'quit' to exit.\n\n");
    kprint("Your guess: ");
}

int game_handle_input(char* input) {
    if (!active) return 1;
    if (strcasecmp(input, "quit") == 0) {
        kprint("Exiting game.\n");
        active = 0; return 1;
    }
    int guess = to_int(input);
    if (guess < 1 || guess > 100) {
        kprint("Enter 1-100.\n");
        kprint("Your guess: ");
        return 0;
    }
    tries--;
    if (guess == target) {
        kprint("Correct! You win.\n");
        active = 0; return 1;
    } else if (guess < target) {
        kprint("Too low.\n");
    } else {
        kprint("Too high.\n");
    }
    if (tries == 0) {
        kprint("Out of tries. You lose. Number was: ");
        char buf[12]; int_to_ascii(target, buf); kprint(buf); kprint("\n");
        active = 0; return 1;
    }
    kprint("Tries left: ");
    char buf[12]; int_to_ascii(tries, buf); kprint(buf); kprint("\n");
    kprint("Your guess: ");
    return 0;
}
