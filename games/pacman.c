/*
 * PAC-MAN - Classic arcade game for 6502
 *
 * Use WASD or arrow keys to move Pac-Man
 * Eat all dots to win
 * Avoid the ghosts!
 * Eat power pellets to turn the tables
 *
 * Classic 1980 arcade action!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 28
#define HEIGHT 21
#define NUM_GHOSTS 4

typedef struct {
    int x, y;
    int dx, dy;
} Entity;

typedef enum {
    GHOST_CHASE,
    GHOST_SCATTER,
    GHOST_FRIGHTENED
} GhostMode;

typedef struct {
    Entity pos;
    GhostMode mode;
    int color;
    int frighten_timer;
} Ghost;

typedef struct {
    char maze[HEIGHT][WIDTH];
    Entity pacman;
    Ghost ghosts[NUM_GHOSTS];
    int score;
    int lives;
    int dots_remaining;
    int power_mode;
    int power_timer;
    int game_over;
    int won;
} Game;

/* Original Pac-Man maze (simplified) */
const char *maze_template[HEIGHT] = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#*####.#####.##.#####.####*#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.##### ## #####.######",
    "     #.##          ##.#     ",
    "     #.## ###  ### ##.#     ",
    "######.## #      # ##.######",
    "      .   #      #   .      ",
    "######.## #      # ##.######",
    "     #.## ######## ##.#     ",
    "     #.##          ##.#     ",
    "######.## ######## ##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#*..##.......  .......##..*#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
};

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
    // Copy maze template
    game->dots_remaining = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            game->maze[y][x] = maze_template[y][x];
            if (game->maze[y][x] == '.') game->dots_remaining++;
            if (game->maze[y][x] == '*') game->dots_remaining++;  // Power pellets
        }
    }

    // Pac-Man starting position
    game->pacman.x = WIDTH / 2;
    game->pacman.y = HEIGHT - 4;
    game->pacman.dx = 0;
    game->pacman.dy = 0;

    // Initialize ghosts
    int ghost_colors[] = {31, 36, 35, 33};  // Red, Cyan, Magenta, Yellow
    for (int i = 0; i < NUM_GHOSTS; i++) {
        game->ghosts[i].pos.x = WIDTH / 2 - 2 + i;
        game->ghosts[i].pos.y = HEIGHT / 2;
        game->ghosts[i].pos.dx = 0;
        game->ghosts[i].pos.dy = -1;
        game->ghosts[i].mode = GHOST_CHASE;
        game->ghosts[i].color = ghost_colors[i];
        game->ghosts[i].frighten_timer = 0;
    }

    game->score = 0;
    game->lives = 3;
    game->power_mode = 0;
    game->power_timer = 0;
    game->game_over = 0;
    game->won = 0;
}

int can_move(Game *game, int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return 0;
    char tile = game->maze[y][x];
    return (tile != '#' && tile != ' ');
}

void handle_input(Game *game) {
    char c = getchar();

    int new_dx = game->pacman.dx;
    int new_dy = game->pacman.dy;

    if (c == 'w' || c == 'W' || c == 'A') {  // Up
        new_dx = 0; new_dy = -1;
    } else if (c == 's' || c == 'S' || c == 'B') {  // Down
        new_dx = 0; new_dy = 1;
    } else if (c == 'a' || c == 'A' || c == 'D') {  // Left
        new_dx = -1; new_dy = 0;
    } else if (c == 'd' || c == 'D' || c == 'C') {  // Right
        new_dx = 1; new_dy = 0;
    } else if (c == 'q' || c == 'Q') {
        game->game_over = 1;
        return;
    }

    // Check if new direction is valid
    int new_x = game->pacman.x + new_dx;
    int new_y = game->pacman.y + new_dy;
    if (can_move(game, new_x, new_y)) {
        game->pacman.dx = new_dx;
        game->pacman.dy = new_dy;
    }
}

void move_ghost(Game *game, int ghost_idx) {
    Ghost *ghost = &game->ghosts[ghost_idx];

    // Simple AI: try to move towards Pac-Man (or away if frightened)
    int target_x = game->pacman.x;
    int target_y = game->pacman.y;

    if (game->power_mode) {
        // Run away from Pac-Man
        target_x = WIDTH - target_x;
        target_y = HEIGHT - target_y;
    }

    // Try to move closer to target
    int best_dx = ghost->pos.dx;
    int best_dy = ghost->pos.dy;
    int best_dist = 9999;

    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    for (int i = 0; i < 4; i++) {
        int new_x = ghost->pos.x + directions[i][0];
        int new_y = ghost->pos.y + directions[i][1];

        if (can_move(game, new_x, new_y)) {
            int dist = abs(new_x - target_x) + abs(new_y - target_y);
            if (dist < best_dist) {
                best_dist = dist;
                best_dx = directions[i][0];
                best_dy = directions[i][1];
            }
        }
    }

    ghost->pos.dx = best_dx;
    ghost->pos.dy = best_dy;

    // Move
    int new_x = ghost->pos.x + ghost->pos.dx;
    int new_y = ghost->pos.y + ghost->pos.dy;

    if (can_move(game, new_x, new_y)) {
        ghost->pos.x = new_x;
        ghost->pos.y = new_y;
    }
}

void update_game(Game *game) {
    // Move Pac-Man
    int new_x = game->pacman.x + game->pacman.dx;
    int new_y = game->pacman.y + game->pacman.dy;

    // Tunnel wrap-around
    if (new_x < 0) new_x = WIDTH - 1;
    if (new_x >= WIDTH) new_x = 0;

    if (can_move(game, new_x, new_y)) {
        game->pacman.x = new_x;
        game->pacman.y = new_y;

        // Eat dots
        char tile = game->maze[new_y][new_x];
        if (tile == '.') {
            game->maze[new_y][new_x] = ' ';
            game->score += 10;
            game->dots_remaining--;
        } else if (tile == '*') {  // Power pellet
            game->maze[new_y][new_x] = ' ';
            game->score += 50;
            game->dots_remaining--;
            game->power_mode = 1;
            game->power_timer = 50;  // 5 seconds at 10 FPS
        }
    }

    // Update power mode
    if (game->power_mode) {
        game->power_timer--;
        if (game->power_timer <= 0) {
            game->power_mode = 0;
        }
    }

    // Move ghosts
    static int ghost_move_counter = 0;
    ghost_move_counter++;
    if (ghost_move_counter >= 2) {  // Ghosts move slower
        for (int i = 0; i < NUM_GHOSTS; i++) {
            move_ghost(game, i);
        }
        ghost_move_counter = 0;
    }

    // Check collisions with ghosts
    for (int i = 0; i < NUM_GHOSTS; i++) {
        if (game->ghosts[i].pos.x == game->pacman.x &&
            game->ghosts[i].pos.y == game->pacman.y) {

            if (game->power_mode) {
                // Eat ghost
                game->score += 200;
                game->ghosts[i].pos.x = WIDTH / 2;
                game->ghosts[i].pos.y = HEIGHT / 2;
            } else {
                // Lose life
                game->lives--;
                if (game->lives <= 0) {
                    game->game_over = 1;
                } else {
                    // Reset positions
                    game->pacman.x = WIDTH / 2;
                    game->pacman.y = HEIGHT - 4;
                    game->pacman.dx = 0;
                    game->pacman.dy = 0;
                }
            }
        }
    }

    // Check win condition
    if (game->dots_remaining == 0) {
        game->won = 1;
        game->game_over = 1;
    }
}

void render_game(Game *game) {
    clear_screen();

    printf("╔════════════════════════════════════╗\n");
    printf("║       PAC-MAN - 6502 CLASSIC       ║\n");
    printf("╠════════════════════════════════════╣\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("║");
        for (int x = 0; x < WIDTH; x++) {
            int is_pacman = (x == game->pacman.x && y == game->pacman.y);
            int is_ghost = 0;
            int ghost_color = 31;

            for (int g = 0; g < NUM_GHOSTS; g++) {
                if (x == game->ghosts[g].pos.x && y == game->ghosts[g].pos.y) {
                    is_ghost = 1;
                    ghost_color = game->ghosts[g].color;
                    break;
                }
            }

            if (is_pacman) {
                printf("\033[1;33mC\033[0m");  // Yellow Pac-Man
            } else if (is_ghost) {
                if (game->power_mode) {
                    printf("\033[1;34mM\033[0m");  // Blue frightened ghost
                } else {
                    printf("\033[1;%dmM\033[0m", ghost_color);  // Colored ghost
                }
            } else {
                char c = game->maze[y][x];
                if (c == '#') {
                    printf("\033[0;34m#\033[0m");  // Blue walls
                } else if (c == '.') {
                    printf("\033[1;37m·\033[0m");  // White dots
                } else if (c == '*') {
                    printf("\033[1;37mO\033[0m");  // Power pellets
                } else {
                    printf(" ");
                }
            }
        }
        printf("║\n");
    }

    printf("╠════════════════════════════════════╣\n");
    printf("║ Score: %-8d  Lives: ", game->score);
    for (int i = 0; i < game->lives; i++) {
        printf("♥ ");
    }
    for (int i = game->lives; i < 3; i++) {
        printf("  ");
    }
    printf("   ║\n");

    if (game->power_mode) {
        printf("║ \033[1;36mPOWER MODE! %d  \033[0m                ║\n", game->power_timer / 10);
    } else {
        printf("║                                    ║\n");
    }

    printf("║ Controls: WASD, Q to quit          ║\n");
    printf("╚════════════════════════════════════╝\n");

    fflush(stdout);
}

int main() {
    Game game;

    srand(time(NULL));
    enable_raw_mode();

    printf("\n");
    printf("╔════════════════════════════════════╗\n");
    printf("║         PAC-MAN - 6502             ║\n");
    printf("║                                    ║\n");
    printf("║  C = Pac-Man                       ║\n");
    printf("║  M = Ghosts                        ║\n");
    printf("║  · = Dots (10 points)              ║\n");
    printf("║  O = Power Pellets (50 points)     ║\n");
    printf("║                                    ║\n");
    printf("║  Eat all dots to win!              ║\n");
    printf("║  Avoid ghosts... or eat them!      ║\n");
    printf("║                                    ║\n");
    printf("║  Press any key to start...         ║\n");
    printf("╚════════════════════════════════════╝\n");

    getchar();

    init_game(&game);

    while (!game.game_over) {
        handle_input(&game);
        update_game(&game);
        render_game(&game);
        usleep(100000);  // 10 FPS
    }

    // Game over screen
    clear_screen();
    printf("\n");
    printf("╔════════════════════════════════════╗\n");
    if (game.won) {
        printf("║         ★★★ YOU WIN! ★★★           ║\n");
    } else {
        printf("║           GAME OVER!               ║\n");
    }
    printf("╠════════════════════════════════════╣\n");
    printf("║                                    ║\n");
    printf("║  Final Score: %-8d             ║\n", game.score);
    printf("║                                    ║\n");

    if (game.score >= 2000) {
        printf("║  Rating: ★★★ MASTER ★★★            ║\n");
    } else if (game.score >= 1000) {
        printf("║  Rating: ★★ EXPERT ★★              ║\n");
    } else if (game.score >= 500) {
        printf("║  Rating: ★ SKILLED ★               ║\n");
    } else {
        printf("║  Rating: BEGINNER                  ║\n");
    }

    printf("║                                    ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf("\n");

    disable_raw_mode();

    return 0;
}
