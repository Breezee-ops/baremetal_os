#include "term.h"
#include "lib.h"


termData term;
unsigned char line_buf[MAX_BUFFER];
int buf_count = 0;
unsigned char tab_flag;

void term_init(){
	int i;
    term.x_pos = 0;
    term.y_pos = 0;
    clear();
    set_curr_pos(0, 0);
}

int32_t term_read(void* buf, int32_t nbytes){
	term.x_pos = get_curr_pos()[0];
	term.y_pos = get_curr_pos()[1];
	term.x_pos = 0;
	term.y_pos++;
	set_curr_pos(term.x_pos, term.y_pos);
}

int32_t term_write(unsigned char* buf, uint32_t nbytes){
	int i;
	for(i = 0; i < nbytes; i++){
		term.x_pos = get_curr_pos()[0];
		term.y_pos = get_curr_pos()[1];
		set_curr_pos(term.x_pos, term.y_pos);
		if(buf[i] == '\n'){
			term_read(line_buf, sizeof(line_buf));
		 	memset(line_buf, '\0', sizeof(line_buf));
		 	buf_count = 0;
		}
		if((term.x_pos == NUM_COLS - 1)){
			term.x_pos = 0;
			term.y_pos++;
		}
		if(buf_count < MAX_BUFFER){
			set_curr_pos(term.x_pos, term.y_pos);
			putc(buf[i]);
			line_buf[buf_count] = buf[i];
			buf_count++;
			if(term.x_pos < NUM_COLS - 1){
				term.x_pos++;
			}
		}
		set_curr_pos(term.x_pos, term.y_pos);
	}
	return 0;
}

void uh_oh_backspace(){

	term.x_pos = get_curr_pos()[0];
	term.y_pos = get_curr_pos()[1];

	if(term.x_pos == 0){
		if(term.y_pos > 0){
			term.y_pos--;
			term.x_pos = NUM_COLS - 1;
		}
		else{return;}
	}
	else{
		term.x_pos--;
	}


	if(buf_count > 0){
	// 	line_buf[buf_count - 1] = '\0';
		int i;
		for(i = 1; i < 5; i++){
			if(line_buf[buf_count - i] != ' '){
				tab_flag = 0;
				break;
			}
			tab_flag = 1;
		}
		if(tab_flag){
			for(i = 0; i < 4; i++){
				buf_count--;
				int id = buf_count;
				term.x_pos = id % NUM_COLS;
				term.y_pos = id / NUM_COLS;
				set_curr_pos(term.x_pos, term.y_pos);
			}
			tab_flag = 0;
			return;
		}
		buf_count--;
	}
	set_curr_pos(term.x_pos, term.y_pos);
	putc(' ');
	set_curr_pos(term.x_pos, term.y_pos);

}

void term_clear(){
	term.x_pos = 0;
	term.y_pos = 0;
	buf_count=0;
	memset(line_buf, '\0', sizeof(line_buf));
	set_curr_pos(term.x_pos, term.y_pos);
	clear();
	set_curr_pos(term.x_pos, term.y_pos);
}


void tabitha(){
	unsigned char buf[4];
	memset(buf, ' ', sizeof(buf));
	term_write(buf, 4);
}