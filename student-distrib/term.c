#include "term.h"
#include "lib.h"


termData term;

int terminal_init(){
    term.x_pos = 0;
    term.y_pos = 0;
    clear();
    set_curr_pos(0, 0);
    return 0;
}

void terminal_write(){
	term.x_pos = get_curr_pos()[0];
	term.y_pos = get_curr_pos()[1];
	set_curr_pos(term.x_pos, term.y_pos);
}

void uh_oh_backspace(){

	term.x_pos = get_curr_pos()[0];
	term.y_pos = get_curr_pos()[1];

	if(term.x_pos == 0){
		if(term.y_pos > 0){
			term.y_pos--;
		}
		term.x_pos = NUM_COLS - 1;
	}
	term.x_pos--;
	set_curr_pos(term.x_pos, term.y_pos);
	putc(' ');
	set_curr_pos(term.x_pos, term.y_pos);
}

// void tabitha(){
// 	term.x_pos = get_curr_pos()[0];
// 	term.y_pos = get_curr_pos()[1];
// 	term.x_pos += 4;
// 	set_curr_pos(term.x_pos, term.y_pos);
// }

void clear_term(){
	term.x_pos = 0;
	term.y_pos = 0;
	set_curr_pos(term.x_pos, term.y_pos);
	clear();
	set_curr_pos(term.x_pos, term.y_pos);
}

