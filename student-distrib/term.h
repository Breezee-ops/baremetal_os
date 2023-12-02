#ifndef _TERM_H
#define _TERM_H

#include "types.h"
#include "functions.h"
#include "syscall.h"

#define NUM_COLS						80
#define NUM_ROWS						25
#define MAX_BUFFER						128

// TODO update this number inside term_read / write

void term_init();
void uh_oh_backspace();
void tabitha();
void term_clear();
int32_t term_open(const uint8_t* file_name);
int32_t term_close(int32_t fd);
int32_t term_read(int32_t fd, int32_t* offset, void* buf, int32_t length);
int32_t term_write(int32_t fd, const void* buf, int32_t nbytes); 
void keyboard_read(unsigned char* buf);
void keyboard_write(const unsigned char* buf, int nbytes);
void one_line_up();
typedef struct termData {
	int x_pos;
	int y_pos;
	char status;
	pcb_t term_pcb;
	unsigned char line_buf[MAX_BUFFER];
	int buf_count;
	unsigned char tab_flag;
} termData;

volatile int termIdx;
volatile termData curr_term[3];

#endif
