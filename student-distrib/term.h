#ifndef _TERM_H
#define _TERM_H

#include "types.h"
#include "functions.h"

#define NUM_COLS						80
#define NUM_ROWS						25
#define MAX_BUFFER						128

int32_t term_write(unsigned char* buf, uint32_t nbytes);
void uh_oh_backspace();
void tabitha();
void term_clear();
int32_t term_open();
int32_t term_close();
int32_t term_read(unsigned char* buf, int32_t nbytes);
void one_line_up();

typedef struct termData {
	int x_pos;
	int y_pos;
} termData;

#endif