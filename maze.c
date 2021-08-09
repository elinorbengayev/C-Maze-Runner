#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maze.h"

const char *msg_quit[] = {"Bye", "Goodbye!", "Farewell!","Go learn for the test...","Come visit"};
const char *msg_wall[] = {"Ouch!", "That's Hurts","Please be gentle", "It's hurts like building this game","I've got a headache","Maybe you forgot to wear your glasses?"};
const char *msg_win[] = {"You rock!","You won!","That was hard, I think we deserve a bonus","You are the champion, my friend :)"};
const char *msg_key_error[] = {"Go find more keys","Maybe you dropped it...","Look harder"};
const char *msg_target[] = {"Keep going","You got this!","Don't quit","Just a little bit more"};
char *msg_another_way[] = {"Try another way!","Not from here","That's not the way"};
const char *msg_unknown[] = {"Unknown key pressed","No habla espanol","I don't recognize it"};
const char supported_tiles[] = {'@','$','-','|','>','<','v','^','+','&','#',' ','\t'};
const unsigned tiles_len = 13;
enum tile {quit, win, target, space};
enum tile current_tile = space;
char *str;

int main(int argc, char *argv[])
{
    set_terminal(true);
    maze *m = read_maze(argv[1]);
    if (m == NULL) {
        printf("Something went wrong! Please try again\n");
        set_terminal(false);
        return 0;
    }

    bool game_over = false;
    print_maze(m);

    do
    {
        handle_input(m);
        if (current_tile == quit || current_tile == win) {
            game_over=true;
        }
        print_maze(m);
    } while (!game_over);
    
    delete_maze(m);
    set_terminal(false);
    return 0;
}

#define puts_no_newline(s) fputs(s, stdout)

void print_maze(maze *m)
{
    static unsigned frame_num; // this variable retains its value between calls
    // clear terminal
    puts_no_newline(CLEAR_SCREEN);
    // print frame number and message
    printf("%-*.*s%4d keys: %d\n", m->cols - 2, m->cols - 2, m->msg, frame_num++, m->key_counter);
    // print header row
    for (int j = 0; j < m->cols + 2; j++)
        print_tile(S_WALL_SYM,S_WALL_COL);
    putchar('\n');
    // print maze rows (including player)
    for (int i = 0; i < m->rows; i++) {
        print_tile(S_WALL_SYM,S_WALL_COL);
        for (int j = 0; j < m->cols; j++) {
            if (m->tiles[i][j] == '@') {
                print_tile(S_PLAYER_SYM,S_PLAYER_COL);
            } else if (m->tiles[i][j] == ' ') {
                puts_no_newline(S_FLOOR);
            } else if (m->tiles[i][j] == '#') {
                print_tile(S_WALL_SYM,S_WALL_COL);
            } else {
                if (i == m->player.row_loc && j == m->player.col_loc && m->on_top_tile == true) {
                    char str[2] = "\0"; //converting to string
                    str[0] = m->tiles[i][j];
                    if (m->tiles[i][j] == 't' || m->tiles[i][j] == 'n') {
                        print_tile(S_TARGET_SYM, S_PLAYER_COL);
                    } else {
                        print_tile(str,S_PLAYER_COL);
                    }
                    m->on_top_tile = false;
                } else {
                    switch(m->tiles[i][j]) {
                    case 'n':
                        print_tile(S_TARGET_SYM,S_TARGET_NOT_VISITED_COL);
                        break;
                    case 't':
                        print_tile(S_TARGET_SYM,S_TARGET_VISITED_COL);
                        break;
                    case '-':
                        print_tile(S_HORIZONTAL_WALL_SYM,S_HORIZONTAL_WALL_COL);
                        break;
                    case '|':
                        print_tile(S_VERTICAL_WALL_SYM,S_VERTICAL_WALL_COL);
                        break;
                    case '^':
                        print_tile(S_UP_ARROW,S_ARROW_COL);
                        break;
                    case 'v':
                        print_tile(S_DOWN_ARROW,S_ARROW_COL);
                        break;
                    case '>':
                        print_tile(S_RIGHT_ARROW,S_ARROW_COL);
                        break;
                    case '<':
                        print_tile(S_LEFT_ARROW,S_ARROW_COL);
                        break;
                    case '&':
                        print_tile(S_KEY_SYM,S_KEY_COL);
                        break;
                    case '+':
                        print_tile(S_DOOR_SYM,S_DOOR_COL);
                        break;
                    }
                }
            }
        }
        print_tile(S_WALL_SYM,S_WALL_COL);
        putchar('\n');
    }

    // print footer row
    for (int j = 0; j < m->cols + 2; j++) {
        print_tile(S_WALL_SYM,S_WALL_COL);
    }
    putchar('\n');
}

void print_tile(char *symbol,char *color)
{
    puts_no_newline(color);
    puts_no_newline(symbol);
    puts_no_newline(COLOR_OFF);
}

void handle_input(maze *m)
{
    str = (char*)MSG_NORMAL;
    strcpy(m->msg, str);

    // get user input
    int key = getchar();
    location new_location={m->player.row_loc,m->player.col_loc};
    char temp=' ';
    char current = m->tiles[m->player.row_loc][m->player.col_loc];
    switch(key) {
        case EOF:
        case 'q':
        case 'Q':   // exit game
            current_tile = quit;
            str = (char*)MSG_QUIT;
            strcpy(m->msg, str);
            break;
        case 'a':
        case 'A':   // go left
            if (m->player.col_loc == 0) {
                str = (char*)MSG_WALL_ERR;
                strcpy(m->msg, str); 
            }
            else {
                temp = m->tiles[m->player.row_loc][m->player.col_loc-1];
                if (!walls_and_door(m,temp)) { 
                    if(temp == '|' || temp == '>' || temp == '^' || temp == 'v') {
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else if (current == '^' || current == '>' || current == 'v') {
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else {
                    new_location.col_loc--;
                    }
                }
            }
            break;
        case 'd':
        case 'D':   // go right
            if(m->player.col_loc == m->cols-1) {
                str = (char*)MSG_WALL_ERR;
                strcpy(m->msg, str);
            } 
            else
            {
                temp = m->tiles[m->player.row_loc][m->player.col_loc+1];
                if (!walls_and_door(m,temp)) { 
                    if(temp == '|' || temp == '<' || temp == '^' || temp == 'v') {
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else if (current == '^' || current == '<' || current == 'v') {
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else { 
                    new_location.col_loc++;
                    }
                }
            }
            break;
        case 'w':
        case 'W':   // go up
            if (m->player.row_loc == 0) { 
                str = (char*)MSG_WALL_ERR;
                strcpy(m->msg, str);
            } 
            else
            { 
                temp = m->tiles[m->player.row_loc-1][m->player.col_loc];
                if (!walls_and_door(m,temp)) { 
                    if(temp == '-' || temp == '<' || temp == '>' || temp == 'v' ) { 
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else if (current == '>' || current == '<' || current == 'v') {
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else { 
                    new_location.row_loc--;
                    }
                }
            }
            break;
        case 's':
        case 'S':   // go down
            if (m->player.row_loc == m->rows-1) { 
                str = (char*)MSG_WALL_ERR;
                strcpy(m->msg, str);
            } 
            else {
                temp = m->tiles[m->player.row_loc+1][m->player.col_loc];
                if (!walls_and_door(m,temp)) {
                    if(temp == '-' || temp == '<' || temp == '>' || temp == '^' ) { 
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else if (current == '^' || current == '<' || current == '>') {
                        str = (char*)MSG_ANOTHER_WAY;
                        strcpy(m->msg, str);
                    } else { 
                        new_location.row_loc++;
                    }
                }
           }
            break;
        default:
            str = (char*)MSG_UNKNOWN;
            strcpy(m->msg, str);
    }
    if (current_tile != quit) { // if not quit
        update_tiles(m, new_location);
        m->player.row_loc=new_location.row_loc;
        m->player.col_loc=new_location.col_loc;
        if (m->tiles[m->player.row_loc][m->player.col_loc] == 't') {
            if (m->target_counter == 0) {
                str = (char*)MSG_WIN;
                strcpy(m->msg, str);
                current_tile = win;
            } else {
                str = (char*)MSG_TARGET;
                strcpy(m->msg, str);
                current_tile = target;
                m->on_top_tile = true;
            }
        } else if (m->tiles[m->player.row_loc][m->player.col_loc] != ' ') {
            m->on_top_tile = true;
        }
    } 
}

bool walls_and_door(maze *m, char temp) {

    bool flag = false;
    if(temp == '#') {
        str = (char*)MSG_WALL_ERR;
        strcpy(m->msg, str);
        flag = true;
    } else if (temp == '+' && m->key_counter == 0) {
        str = (char*)MSG_KEY_ERROR;
        strcpy(m->msg, str);
        flag = true; 
    }
    return flag;
        
}

maze *initialize_maze(unsigned rows, unsigned cols, char msg[], location player, int target_counter, int key_counter, char **tiles, char *data) 
{
    //cols and rows without walls
    maze *m = malloc(sizeof(maze));
    if (m == NULL) {
        return NULL;
    }
    m->rows = rows;
    m->cols = cols;
    strcpy(m->msg, msg);
    m->player = player;
    m->target_counter = target_counter;
    m->key_counter = key_counter;
    m->data = data;
    m->tiles = tiles;
    m->on_top_tile = false;

    return m; 
}

void delete_maze(maze *m)
{
    free(m->data);
    free(m->tiles);
}

maze *read_maze (char *filename)
{
    FILE *fp1 = fopen(filename, "r");
    if (fp1 == NULL) {
        printf("Error while opening file\n");
        exit(-1);
    }
    int msg_len = 0;
    while (fgetc(fp1)!='\r')
    {
        msg_len++;
    }
    char message[msg_len]; 
    fseek(fp1, -(msg_len+1), SEEK_CUR);  
    int i = 0;
    while (true) { //reading the welcome message
        char temp = fgetc(fp1);
        if (temp != '\r') {
            message[i++] = temp;
        }
        else {
            break;
        }
    }
    
    fgetc(fp1);
    unsigned rows, cols;
    fscanf(fp1,"%d %d",&rows,&cols);
    char temp = fgetc(fp1);
    while(temp != '\n') { //forward fp1 to the next line
        temp = fgetc(fp1);
    }

    char *data = malloc(rows * cols * sizeof(int));
    if (data == NULL)
        return NULL;
    char **tiles = malloc(rows * sizeof(int*));
    if (tiles == NULL) {
        return NULL;
    }
    i=0;
    int j = 0;
    int flag_p = 0, target_counter=0,key_counter=0;
    location player;

    int flag_err = 0;
    while (!feof(fp1)) { //reading the internal maze
        char temp = fgetc(fp1);
        tiles[i] = data + cols * i;
        while (temp != '\r') {
            if (temp == '@') {
                if (flag_p == 0) {
                    flag_p++;
                } else {
                    printf("There are more than 1 player signs\n");
                    flag_err = 1;
                }
                tiles[i][j] = '@';
                player.col_loc = j;
                player.row_loc = i;
            } else if (temp == '$') {
                tiles[i][j] = 'n'; //not visited target
                target_counter++;
            } else if(supported_tile(temp)){
                tiles[i][j] = temp;
            } else {
                    printf("Unsupported sign - %d\nin location %d %d\n",temp,i,j);
                    flag_err = 1; 
            }

            if (flag_err == 1) { 
                free(data);
                free(tiles);
                exit(-1);
            }
            j++;
            if (i == rows - 1 && j == cols) {
                break;
            }
            temp = fgetc(fp1);
        }
        j=0;
        i++;
        fgetc(fp1);
    }

    maze *m = initialize_maze(rows, cols,message, player,target_counter,key_counter, tiles, data);
    return m;
}
void update_tiles(maze *m,location new_location)
{
    location player = m->player;
    char new_loc_temp = m->tiles[new_location.row_loc][new_location.col_loc];
    char current_temp = m->tiles[player.row_loc][player.col_loc];

    if (new_loc_temp == '&') {
        if (current_temp == '@') {
            m->tiles[player.row_loc][player.col_loc] = ' ';
        }
        else {
             m->tiles[new_location.row_loc][new_location.col_loc] = '@';
        }
        m->key_counter++;
    } 
    if (current_temp == '&' && new_loc_temp == ' '){
        m->tiles[new_location.row_loc][new_location.col_loc] = '@';
        m->tiles[player.row_loc][player.col_loc] = ' ';
    } else if (current_temp == '&' && new_loc_temp != ' ') {
        m->tiles[player.row_loc][player.col_loc] = ' ';
    }
    if (new_loc_temp == '+'){
        if (m->key_counter >0) {
            if (new_loc_temp == ' ') {
                m->tiles[player.row_loc][player.col_loc]=' ';
            }
            m->key_counter--;
            str = (char*)MSG_USED_KEY;
            strcpy(m->msg, str);
        }
    }
    if (current_temp == '+') {
        m->tiles[player.row_loc][player.col_loc]=' ';
        m->tiles[new_location.row_loc][new_location.col_loc] = '@';
    }
    if (current_temp == '-' || current_temp == '|' || current_temp == '^' || current_temp == 'v' || current_temp == '>' || current_temp == '<' || current_temp == 't') {
        if (new_loc_temp == ' ') {
            m->tiles[new_location.row_loc][new_location.col_loc] = '@';
        }
        if (new_loc_temp == 'n') {
            m->tiles[new_location.row_loc][new_location.col_loc] = 't';
            m->target_counter--;
        }
    } else if (new_loc_temp == '-' || new_loc_temp == '|' || new_loc_temp == 't' || new_loc_temp == '^' || new_loc_temp =='v' || new_loc_temp == '>' || new_loc_temp == '<') {
        if (current_temp == '@') {
            m->tiles[player.row_loc][player.col_loc]=' ';
        }
    } else if (new_loc_temp == 'n') {
        m->tiles[new_location.row_loc][new_location.col_loc] = 't';
        m->tiles[player.row_loc][player.col_loc]=' ';
        m->target_counter--;
    } 
    else {
        m->tiles[player.row_loc][player.col_loc]=' ';
        m->tiles[new_location.row_loc][new_location.col_loc]='@';
    }
}

bool supported_tile(char tile) {
    for (int i = 0; i < tiles_len; i++) {
        if (tile == supported_tiles[i])
            return true;
    }
    return false;
}