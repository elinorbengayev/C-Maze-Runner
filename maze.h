#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>




#define MSG_NORMAL  "Press A/S/D/W to move, Q to quit"
#define MSG_START   "Welcome! " MSG_NORMAL
#define MSG_UNKNOWN msg_unknown[rand() % (sizeof(msg_unknown) / sizeof(msg_unknown[0]))];
#define MSG_QUIT msg_quit[rand() % (sizeof(msg_quit) / sizeof(msg_quit[0]))];
#define MSG_WALL_ERR msg_wall[rand() % (sizeof(msg_wall) / sizeof(msg_wall[0]))];
#define MSG_WIN  msg_win[rand() % (sizeof(msg_win) / sizeof(msg_win[0]))];
#define MSG_ANOTHER_WAY msg_another_way[rand() % (sizeof(msg_another_way) / sizeof(msg_another_way[0]))];
#define MSG_KEY_ERROR msg_key_error[rand() % (sizeof(msg_key_error) / sizeof(msg_key_error[0]))];
#define MSG_TARGET msg_target[rand() % (sizeof(msg_target) / sizeof(msg_target[0]))];
#define MSG_USED_KEY "You've used one key";

typedef struct {
    unsigned row_loc;
    unsigned col_loc;
} location;

typedef struct {
    unsigned rows;
    unsigned cols;  
    char msg[256];
    char *data; // helper array to tiles
    char **tiles;
    int target_counter;
    int key_counter;
    location player;
    bool on_top_tile;
} maze;


void set_terminal(bool start);
void print_maze(maze *m);
void handle_input(maze *m);
maze *initialize_maze(unsigned rows, unsigned cols, char *msg, location player, int target_counter, int key_counter, char **tiles, char *data);
void print_tile(char *symbol,char *color);
maze *read_maze (char *filename);
void delete_maze(maze *m);
void update_tiles(maze *m,location new_location);
bool supported_tile(char tile);
bool walls_and_door(maze *m, char temp);

/************************************
 * TERMINAL CODES ("MAGIC STRINGS") *
 ************************************/

#define COLOR_OFF       "\e[0m"
#define COLOR_BLACK     "\e[0;30m"
#define COLOR_GRAY      "\e[1;30m"
#define COLOR_RED       "\e[0;31m"
#define COLOR_LT_RED    "\e[1;31m"
#define COLOR_GREEN     "\e[0;32m"
#define COLOR_LT_GREEN  "\e[1;32m"
#define COLOR_BROWN     "\e[0;33m"
#define COLOR_YELLOW    "\e[1;33m"
#define COLOR_BLUE      "\e[0;34m"
#define COLOR_LT_BLUE   "\e[1;34m"
#define COLOR_PURPLE    "\e[0;35m"
#define COLOR_LT_PURPLE "\e[1;35m"
#define COLOR_CYAN      "\e[0;36m"
#define COLOR_LT_CYAN   "\e[1;36m"
#define COLOR_LT_GRAY   "\e[0;37m"
#define COLOR_WHITE     "\e[1;37m"

#define CLEAR_SCREEN    "\e[2J\e[1;1H"

/*****************************
 * SYMBOLS FOR GAME ELEMENTS *
 *****************************/

#define S_FLOOR     " "
#define S_TARGET_SYM "$"
#define S_TARGET_VISITED_COL COLOR_GREEN
#define S_TARGET_NOT_VISITED_COL COLOR_RED
#define S_PLAYER_SYM "@"
#define S_PLAYER_COL COLOR_BLUE
#define S_WALL_SYM "#"
#define S_WALL_COL COLOR_GRAY
#define S_VERTICAL_WALL_SYM "|"
#define S_VERTICAL_WALL_COL COLOR_CYAN
#define S_HORIZONTAL_WALL_SYM "-"
#define S_HORIZONTAL_WALL_COL COLOR_PURPLE
#define S_UP_ARROW "^"
#define S_DOWN_ARROW "v"
#define S_RIGHT_ARROW ">"
#define S_LEFT_ARROW "<"
#define S_ARROW_COL COLOR_GREEN
#define S_KEY_SYM "&"
#define S_KEY_COL COLOR_YELLOW
#define S_DOOR_SYM "+"
#define S_DOOR_COL COLOR_BROWN