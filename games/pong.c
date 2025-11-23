/*
 * PONG - Classic two-player arcade game for 6502
 *
 * Player 1: W/S keys (left paddle)
 * Player 2: Up/Down arrows (right paddle)
 *
 * First to 10 points wins!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 60
#define HEIGHT 20
#define PADDLE_HEIGHT 4
#define WIN_SCORE 10

typedef struct {
    int x, y;
    float dx, dy;
} Ball;

typedef struct {
    int y;
    int score;
} Paddle;

typedef struct {
    char screen[HEIGHT][WIDTH];
    Ball ball;
    Paddle left;
    Paddle right;
    int game_over;
    int winner;
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

/* Game functions */

void init_game(Game *game) {
    game->ball.x = WIDTH / 2;
    game->ball.y = HEIGHT / 2;
    game->ball.dx = (rand() % 2 == 0) ? 1.0 : -1.0;
    game->ball.dy = ((rand() % 100) / 100.0 - 0.5) * 2.0;

    game->left.y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    game->left.score = 0;

    game->right.y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    game->right.score = 0;

    game->game_over = 0;
    game->winner = 0;
}

void reset_ball(Game *game) {
    game->ball.x = WIDTH / 2;
    game->ball.y = HEIGHT / 2;
    game->ball.dx = (rand() % 2 == 0) ? 1.0 : -1.0;
    game->ball.dy = ((rand() % 100) / 100.0 - 0.5) * 2.0;
}

void handle_input(Game *game) {
    char c = getchar();

    // Player 1 (left paddle)
    if (c == 'w' || c == 'W') {
        if (game->left.y > 1) game->left.y--;
    } else if (c == 's' || c == 'S') {
        if (game->left.y < HEIGHT - PADDLE_HEIGHT - 1) game->left.y++;
    }

    // Player 2 (right paddle) - Arrow keys
    else if (c == 'A') {  // Up arrow
        if (game->right.y > 1) game->right.y--;
    } else if (c == 'B') {  // Down arrow
        if (game->right.y < HEIGHT - PADDLE_HEIGHT - 1) game->right.y++;
    }

    // Alternative for Player 2
    else if (c == 'i' || c == 'I') {
        if (game->right.y > 1) game->right.y--;
    } else if (c == 'k' || c == 'K') {
        if (game->right.y < HEIGHT - PADDLE_HEIGHT - 1) game->right.y++;
    }

    else if (c == 'q' || c == 'Q') {
        game->game_over = 1;
    }
}

void update_game(Game *game) {
    // Move ball
    game->ball.x += game->ball.dx;
    game->ball.y += game->ball.dy;

    // Top/bottom wall collision
    if (game->ball.y <= 1 || game->ball.y >= HEIGHT - 2) {
        game->ball.dy = -game->ball.dy;
    }

    // Left paddle collision
    if (game->ball.x <= 3 &&
        game->ball.y >= game->left.y &&
        game->ball.y < game->left.y + PADDLE_HEIGHT) {
        game->ball.dx = -game->ball.dx;
        // Add spin based on where ball hits paddle
        float hit_pos = (game->ball.y - game->left.y) / (float)PADDLE_HEIGHT;
        game->ball.dy = (hit_pos - 0.5) * 2.0;
    }

    // Right paddle collision
    if (game->ball.x >= WIDTH - 4 &&
        game->ball.y >= game->right.y &&
        game->ball.y < game->right.y + PADDLE_HEIGHT) {
        game->ball.dx = -game->ball.dx;
        float hit_pos = (game->ball.y - game->right.y) / (float)PADDLE_HEIGHT;
        game->ball.dy = (hit_pos - 0.5) * 2.0;
    }

    // Score (left side)
    if (game->ball.x < 0) {
        game->right.score++;
        reset_ball(game);
        if (game->right.score >= WIN_SCORE) {
            game->game_over = 1;
            game->winner = 2;
        }
    }

    // Score (right side)
    if (game->ball.x > WIDTH) {
        game->left.score++;
        reset_ball(game);
        if (game->left.score >= WIN_SCORE) {
            game->game_over = 1;
            game->winner = 1;
        }
    }
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
        game->screen[0][x] = '=';
        game->screen[HEIGHT - 1][x] = '=';
    }

    // Draw center line
    for (int y = 0; y < HEIGHT; y++) {
        if (y % 2 == 0) game->screen[y][WIDTH / 2] = '|';
    }

    // Draw left paddle
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        game->screen[game->left.y + i][2] = '#';
    }

    // Draw right paddle
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        game->screen[game->right.y + i][WIDTH - 3] = '#';
    }

    // Draw ball
    int bx = (int)game->ball.x;
    int by = (int)game->ball.y;
    if (bx >= 0 && bx < WIDTH && by >= 0 && by < HEIGHT) {
        game->screen[by][bx] = 'O';
    }

    // Display
    clear_screen();

    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                  PONG - 6502 CLASSIC                         ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║   Player 1: %-2d                           Player 2: %-2d    ║\n",
           game->left.score, game->right.score);
    printf("╠══════════════════════════════════════════════════════════════╣\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("║");
        for (int x = 0; x < WIDTH; x++) {
            char c = game->screen[y][x];
            if (c == '#') {
                printf("\033[1;33m%c\033[0m", c);  // Yellow paddles
            } else if (c == 'O') {
                printf("\033[1;37m%c\033[0m", c);  // White ball
            } else if (c == '|') {
                printf("\033[0;36m%c\033[0m", c);  // Cyan center
            } else {
                printf("%c", c);
            }
        }
        printf("║\n");
    }

    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Player 1: W/S         Player 2: I/K or ↑/↓        Q: Quit   ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    fflush(stdout);
}

int main() {
    Game game;

    srand(time(NULL));
    enable_raw_mode();

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     PONG - 6502                              ║\n");
    printf("║                                                              ║\n");
    printf("║  Player 1 (Left):  W = Up, S = Down                         ║\n");
    printf("║  Player 2 (Right): I = Up, K = Down (or arrow keys)         ║\n");
    printf("║                                                              ║\n");
    printf("║  First to 10 points wins!                                   ║\n");
    printf("║                                                              ║\n");
    printf("║  Press any key to start...                                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    getchar();

    init_game(&game);

    while (!game.game_over) {
        handle_input(&game);
        update_game(&game);
        render_game(&game);
        usleep(30000);  // ~33 FPS
    }

    // Game over screen
    clear_screen();
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     GAME OVER!                               ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║                                                              ║\n");
    if (game.winner == 1) {
        printf("║             ★★★ PLAYER 1 WINS! ★★★                        ║\n");
    } else if (game.winner == 2) {
        printf("║             ★★★ PLAYER 2 WINS! ★★★                        ║\n");
    }
    printf("║                                                              ║\n");
    printf("║  Final Score: Player 1: %-2d  -  Player 2: %-2d              ║\n",
           game.left.score, game.right.score);
    printf("║                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    disable_raw_mode();

    return 0;
}
