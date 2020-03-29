#include "drivers/screen.h"
#include "util/memcpy.h"

const int MAX_ROWS = 25;
const int MAX_COLS = 80;
const int VIDEO_MEMORY = 0xb8000;
const int WHITE_ON_BLACK = 0x0f;

int row = 0;
int col = 0;

void clear_screen() {
    char* video_memory = (char *)VIDEO_MEMORY;
    int i;
    int screen_size = MAX_ROWS * MAX_COLS;
    for (i = 0; i < screen_size; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = WHITE_ON_BLACK;
    }
    row = 0;
    col = 0;
}

void scroll() {
    char* video_memory = (char *)VIDEO_MEMORY;
    for (int row = 1; row < MAX_ROWS; row++){
        memcpy(&video_memory[row * 2 * MAX_COLS], &video_memory[(row - 1)*2*MAX_COLS], MAX_COLS*2);
    }
}

void increment_row(){
    if(row == MAX_ROWS - 1)
        scroll();
    else
        row++;
}


void print(char* buffer) {
    char* video_memory = (char *)VIDEO_MEMORY;
    char c = buffer[0];
    int i = 0;
    while (c != '\0') {
        if (c == '\n') {
            increment_row();
            col = 0;
        }
        else {
            video_memory[col + row* 2 * MAX_COLS] = c;
            video_memory[col + 1 + row * 2 * MAX_COLS] = WHITE_ON_BLACK;
            col += 2;
        }
        i += 1;
        c = buffer[i];
    }
}