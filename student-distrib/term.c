#include "term.h"
#include "lib.h"


termData term;
unsigned char line_buf[MAX_BUFFER];
int buf_count = 0;
unsigned char tab_flag;

/* term_init
 * 
 * takes clears and initializes terminal by setting cursor position to the top left
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: term
 */
void term_init(){
	int i;
    term.x_pos = 0;
    term.y_pos = 0;
    clear();
    set_curr_pos(0, 0);
}

/* term_read
 * 
 * reads buffer once enter key is pressed and prints a newline
 * Inputs: buffer , number bytes in buffer
 * Outputs: number of bytes
 * Side Effects: None
 * Coverage:
 */
int32_t term_read(void* buf, int32_t nbytes){
	term.x_pos = get_curr_pos()[0];					//grab current position on screen
	term.y_pos = get_curr_pos()[1];
	term.x_pos = 0;
	term.y_pos++;
	set_curr_pos(term.x_pos, term.y_pos);			//set current position on screen
	return nbytes;
}
/* term_write
 * 
 * writes buffer to terminal from RTC, Keyboard, or File System
 * Inputs: buffer , number bytes in buffer
 * Outputs: number of bytes
 * Side Effects: None
 * Coverage:
 */
int32_t term_write(unsigned char* buf, uint32_t nbytes){
	int i;
	//loop over all characters in buffer
	for(i = 0; i < nbytes; i++){
		term.x_pos = get_curr_pos()[0];					// get current postiion
		term.y_pos = get_curr_pos()[1];
		set_curr_pos(term.x_pos, term.y_pos);			//update current position
		if(buf[i] == '\n'){								//if enter is pressed reset buffer and call term_read
			term_read(line_buf, sizeof(line_buf));
		 	memset(line_buf, '\0', sizeof(line_buf));
		 	buf_count = 0;
		}
		if((term.x_pos == NUM_COLS - 1)){				//if x position is at edge of screen, move down a line
			term.x_pos = 0;
			term.y_pos++;
		}
		if(buf_count < MAX_BUFFER){						//if we have not reached buffer limit
			set_curr_pos(term.x_pos, term.y_pos);		//update current position
			putc(buf[i]);								//print char to screen
			line_buf[buf_count] = buf[i];				//add char to buffer
			buf_count++;								//increase count
			if(term.x_pos < NUM_COLS - 1){				//if we have not reached the edge of screen increase position by 1
				term.x_pos++;
			}
		}
		set_curr_pos(term.x_pos, term.y_pos);			//update current posiion
	}
	return nbytes;
}

/* uh_oh_backspace
 * 
 * implements backspace functionality
 * Inputs: none
 * Outputs: none
 * Side Effects: sets x and y positions of the term struct. if succesful backspace, elements are removed from buffer and curr position is reset
 * Coverage: video memory buffer and terminal cursor
 */
void uh_oh_backspace(){

	term.x_pos = get_curr_pos()[0]; // get current screen x and y
	term.y_pos = get_curr_pos()[1];

	if(term.x_pos == 0){ // if we're at the left most end and we press backspace we go to the previous line. if no previous line, we end
		if(term.y_pos > 0){
			term.y_pos--;
			term.x_pos = NUM_COLS - 1;
		}
		else{return;}
	}
	else{ // if there's characters behind us we go back.
		term.x_pos--;
	}


	if(buf_count > 0){  // non empty buffer
	// 	line_buf[buf_count - 1] = '\0';
		int i;
		for(i = 1; i < 5; i++){ // check the last four charecters to the cursor and see if they are 4 spaces. if they are then we have a tab character
			if(line_buf[buf_count - i] != ' '){
				tab_flag = 0;
				break;
			}
			tab_flag = 1; // set the flag
		}
		if(tab_flag){
			buf_count-=4;
			int id = buf_count;
			term.x_pos = id % NUM_COLS; // calculate the x and y position of the cursor in the buffer using our id
			term.y_pos = id / NUM_COLS;
			set_curr_pos(term.x_pos, term.y_pos);
			tab_flag = 0;
			return;
		}
		buf_count--;
	}
	// backspace fills the buffer location with a space
	set_curr_pos(term.x_pos, term.y_pos);
	putc(' ');
	set_curr_pos(term.x_pos, term.y_pos);

}
/* term_clear
 * 
 * clears terminal and sets cursor at the top
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage:
 */
void term_clear(){
	term.x_pos = 0;
	term.y_pos = 0;
	//buf_count=0;
	//memset(line_buf, '\0', sizeof(line_buf));
	set_curr_pos(term.x_pos, term.y_pos);
	clear();
	set_curr_pos(term.x_pos, term.y_pos);
}

/* tabitha
 * 
 * inserts 4 spaces and writes to terminal
 * Inputs: None
 * Outputs: number of bytes
 * Side Effects: None
 * Coverage:
 */
void tabitha(){
	unsigned char buf[4];
	memset(buf, ' ', sizeof(buf));
	term_write(buf, 4);
}
