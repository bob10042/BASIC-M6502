/*
 * SNAKE - Classic graphical arcade game for 6502
 *
 * Use arrow keys (WASD or arrow keys) to control the snake.
 * Eat food (*) to grow longer and score points.
 * Don't hit the walls or yourself!
 *
 * Demonstrates:
 * - Real-time graphics
 * - Keyboard input
 * - Collision detection
 * - Score keeping
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_LENGTH 200

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[MAX_LENGTH];
    int length;
    int dx, dy;  // Direction
} Snake;

typedef struct {
    char screen[HEIGHT][WIDTH];
    Snake snake;
    Point food;
    int score;
    int game_over;
    int speed;
} Game;

/* Terminal control */
struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void clear_screen() {
    printf("\033[2J\033[H");
}

void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

void set_color(int fg, int bg) {
    printf("\033[%d;%dm", fg, bg);
}

/* Game functions */

void init_game(Game *game) {
    game->snake.length = 3;
    game->snake.body[0].x = WIDTH / 2;
    game->snake.body[0].y = HEIGHT / 2;
    game->snake.body[1].x = WIDTH / 2 - 1;
    game->snake.body[1].y = HEIGHT / 2;
    game->snake.body[2].x = WIDTH / 2 - 2;
    game->snake.body[2].y = HEIGHT / 2;
    game->snake.dx = 1;
    game->snake.dy = 0;

    game->food.x = rand() % (WIDTH - 2) + 1;
    game->food.y = rand() % (HEIGHT - 2) + 1;
    game->score = 0;
    game->game_over = 0;
    game->speed = 150000;  // microseconds
}

void spawn_food(Game *game) {
    int valid = 0;
    while (!valid) {
        game->food.x = rand() % (WIDTH - 2) + 1;
        game->food.y = rand() % (HEIGHT - 2) + 1;

        valid = 1;
        for (int i = 0; i < game->snake.length; i++) {
            if (game->snake.body[i].x == game->food.x &&
                game->snake.body[i].y == game->food.y) {
                valid = 0;
                break;
            }
        }
    }
}

void handle_input(Game *game) {
    char c = getchar();

    if (c == 'w' || c == 'W' || c == 'A') {  // Up
        if (game->snake.dy == 0) {
            game->snake.dx = 0;
            game->snake.dy = -1;
        }
    } else if (c == 's' || c == 'S' || c == 'B') {  // Down
        if (game->snake.dy == 0) {
            game->snake.dx = 0;
            game->snake.dy = 1;
        }
    } else if (c == 'a' || c == 'A' || c == 'D') {  // Left
        if (game->snake.dx == 0) {
            game->snake.dx = -1;
            game->snake.dy = 0;
        }
    } else if (c == 'd' || c == 'D' || c == 'C') {  // Right
        if (game->snake.dx == 0) {
            game->snake.dx = 1;
            game->snake.dy = 0;
        }
    } else if (c == 'q' || c == 'Q') {
        game->game_over = 1;
    }
}

void update_game(Game *game) {
    // Calculate new head position
    int new_x = game->snake.body[0].x + game->snake.dx;
    int new_y = game->snake.body[0].y + game->snake.dy;

    // Check wall collision
    if (new_x <= 0 || new_x >= WIDTH - 1 ||
        new_y <= 0 || new_y >= HEIGHT - 1) {
        game->game_over = 1;
        return;
    }

    // Check self collision
    for (int i = 0; i < game->snake.length; i++) {
        if (game->snake.body[i].x == new_x &&
            game->snake.body[i].y == new_y) {
            game->game_over = 1;
            return;
        }
    }

    // Check food collision
    int ate_food = (new_x == game->food.x && new_y == game->food.y);

    // Move body
    if (!ate_food) {
        for (int i = game->snake.length - 1; i > 0; i--) {
            game->snake.body[i] = game->snake.body[i - 1];
        }
    } else {
        // Grow snake
        for (int i = game->snake.length; i > 0; i--) {
            game->snake.body[i] = game->snake.body[i - 1];
        }
        game->snake.length++;
        game->score += 10;
        spawn_food(game);

        // Speed up slightly
        game->speed -= 2000;
        if (game->speed < 50000) game->speed = 50000;
    }

    // Move head
    game->snake.body[0].x = new_x;
    game->snake.body[0].y = new_y;
}

void render_game(Game *game) {
    // Clear screen buffer
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            game->screen[y][x] = ' ';
        }
    }

    // Draw walls
    for (int x = 0; x < WIDTH; x++) {
        game->screen[0][x] = '#';
        game->screen[HEIGHT - 1][x] = '#';
    }
    for (int y = 0; y < HEIGHT; y++) {
        game->screen[y][0] = '#';
        game->screen[y][WIDTH - 1] = '#';
    }

    // Draw snake
    for (int i = 0; i < game->snake.length; i++) {
        int x = game->snake.body[i].x;
        int y = game->snake.body[i].y;
        if (i == 0) {
            game->screen[y][x] = '@';  // Head
        } else {
            game->screen[y][x] = 'O';  // Body
        }
    }

    // Draw food
    game->screen[game->food.y][game->food.x] = '*';

    // Display
    clear_screen();

    printf("╔════════════════════════════════════════════╗\n");
    printf("║            SNAKE GAME - 6502               ║\n");
    printf("╠════════════════════════════════════════════╣\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("║");
        for (int x = 0; x < WIDTH; x++) {
            char c = game->screen[y][x];
            if (c == '@') {
                printf("\033[1;32m%c\033[0m", c);  // Green head
            } else if (c == 'O') {
                printf("\033[0;32m%c\033[0m", c);  // Green body
            } else if (c == '*') {
                printf("\033[1;31m%c\033[0m", c);  // Red food
            } else if (c == '#') {
                printf("\033[0;36m%c\033[0m", c);  // Cyan walls
            } else {
                printf("%c", c);
            }
        }
        printf("║\n");
    }

    printf("╠════════════════════════════════════════════╣\n");
    printf("║ Score: %-6d  Length: %-6d           ║\n",
           game->score, game->snake.length);
    printf("║ Controls: WASD or Arrow Keys, Q to quit   ║\n");
    printf("╚════════════════════════════════════════════╝\n");

    fflush(stdout);
}

int main() {
    Game game;

    srand(time(NULL));
    enable_raw_mode();

    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║          SNAKE - 6502 CLASSIC              ║\n");
    printf("║                                            ║\n");
    printf("║  Use WASD or Arrow Keys to move           ║\n");
    printf("║  Eat food (*) to grow and score           ║\n");
    printf("║  Don't hit walls or yourself!              ║\n");
    printf("║                                            ║\n");
    printf("║  Press any key to start...                ║\n");
    printf("╚════════════════════════════════════════════╝\n");

    getchar();

    init_game(&game);

    while (!game.game_over) {
        handle_input(&game);
        update_game(&game);
        render_game(&game);
        usleep(game.speed);
    }

    // Game over screen
    clear_screen();
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║              GAME OVER!                    ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║                                            ║\n");
    printf("║  Final Score: %-6d                       ║\n", game.score);
    printf("║  Final Length: %-6d                      ║\n", game.snake.length);
    printf("║                                            ║\n");
    if (game.score >= 100) {
        printf("║  ★★★ EXCELLENT! ★★★                     ║\n");
    } else if (game.score >= 50) {
        printf("║  ★★ GOOD JOB! ★★                        ║\n");
    } else {
        printf("║  ★ NICE TRY! ★                          ║\n");
    }
    printf("║                                            ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n");

    disable_raw_mode();

    return 0;
}
