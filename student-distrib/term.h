#ifndef _TERM_H
#define _TERM_H

#include "types.h"
#include "functions.h"

#define NUM_COLS						80
#define NUM_ROWS						25
#define MAX_BUFFER						128

void term_init();
int32_t term_write(int32_t fd, const void* buf, int32_t nbytes); 
void uh_oh_backspace();
void tabitha();
void term_clear();
int32_t term_open(const uint8_t* file_name);
int32_t term_close(int32_t fd);
int32_t term_read(int32_t f_desc, uint32_t offset, uint8_t* buf, int32_t length); 
// int32_t term_read(int32_t fd, unsigned char* buf, int32_t nbytes);
void keyboard_read(unsigned char* buf);
void keyboard_write(const unsigned char* buf, int nbytes);
void one_line_up();


typedef struct termData {
	int x_pos;
	int y_pos;
} termData;

#endif
