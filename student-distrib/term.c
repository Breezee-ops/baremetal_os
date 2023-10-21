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

void uh_oh_backspace(){

	term.x_pos = get()[0];
	term.y_pos = get()[1];
	term.x_pos--;
	putc(' ');
	set_curr_pos(term.x_pos, term.y_pos);
}

