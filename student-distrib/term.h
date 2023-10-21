#ifndef _TERM_H
#define _TERM_H

#include "types.h"
#include "functions.h"

#define NUM_COLS						80
#define NUM_ROWS						25
#define MAX_BUFFER						128

int terminal_init();
void uh_oh_backspace();

typedef struct termData {
	int x_pos;
	int y_pos;
} termData;

#endif