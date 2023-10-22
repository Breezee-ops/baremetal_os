#ifndef _TERM_H
#define _TERM_H

#include "types.h"
#include "functions.h"

#define NUM_COLS						80
#define NUM_ROWS						25
#define MAX_BUFFER						128

void term_write(unsigned char* buf, uint32_t nbytes);
void uh_oh_backspace();
//void tabitha();
void term_clear();
int32_t term_read(void* buf, int32_t nbytes);

typedef struct termData {
	int x_pos;
	int y_pos;
} termData;

#endif