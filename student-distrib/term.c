#include "term.h"
#include "lib.h"
#include "paging.h"
#include "syscall.h"
#include "functions.h"

#define ATTRIB      0x7

termData term;
// unsigned char* curmem; if this doesnt work bring this back
// int termIdx = 0;
/* term_init
 * 
 * takes clears and initializes terminal by setting cursor position to the top left
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 * Coverage: term
 */
void term_init(){
	int i = 0;
	curmem = (unsigned char*)0xB8000;
	for(i = 0; i < 3; i++){
		curr_term[i].buf_count = 0;
		curr_term[i].x_pos = 0;
		curr_term[i].y_pos = 0;
		clear();
		set_curr_pos(0, 0);
	}
	curr_term[0].status = 1;
}

/* term_open
 * 
 * returns 0
 * Inputs: file name
 * Outputs: None
 * Side Effects: None
 * Coverage: term
 */
int32_t term_open(const uint8_t* file_name){
	return 0;
}

/* term_open
 * 
 * clears terminal then 0
 * Inputs: file descriptor
 * Outputs: None
 * Side Effects: None
 * Coverage: term
 */
int32_t term_close(int32_t fd){
	term_clear();
	return 0;
}

/* term_read
 * 
 * reads buffer once enter key is pressed and prints a newline
 * Inputs: buffer , number bytes in buffer
 * Outputs: number of bytes
 * Side Effects: None
 * Coverage:
 */
int32_t term_read(int32_t fd, int32_t* offset, void* buf, int32_t length) {
	int i;
	uint8_t* char_buf = (uint8_t*)buf;
	curr_term[scheduleIdx].x_pos = get_curr_pos()[0];					//grab current position on screen
	curr_term[scheduleIdx].y_pos = get_curr_pos()[1];
	if(char_buf == NULL){
		return -1;
	}
	if(length <= 0){
		return 0;
	}
	if(length > MAX_BUFFER){
		length = MAX_BUFFER;
	}
	while(curr_term[scheduleIdx].buf_count == 0 || curr_term[scheduleIdx].line_buf[curr_term[scheduleIdx].buf_count - 1] != '\n'){
		// if(dequeue() != -1){
		// 	shell_execute();
		// }
		// if(call == 2){
		// 	// TODO fix monkeying with pid
		// 	set_exe_page(curr_term[termIdx].term_pcb->pid);
		// 	call = 1;
		// }
		// if(call == 3){
		// 	set_exe_page(curr_term[termIdx].term_pcb->pid);
		// 	call = 1;
		// }
	};
	// here
	for(i = 0; i < length && curr_term[scheduleIdx].line_buf[i] != '\0'; i++){
		char_buf[i] = curr_term[scheduleIdx].line_buf[i];
	}
	char_buf[i] = '\n';
	memset(curr_term[scheduleIdx].line_buf, '\0', sizeof(curr_term[scheduleIdx].line_buf));
	curr_term[scheduleIdx].buf_count = 0;
	// for (i = 0; i < nbytes; i++) {
    //     if (buf[i] == '\n') {
    //         term.x_pos = 0;
    //         term.y_pos++;
    //         if (term.y_pos >= NUM_ROWS - 1) {
    //             term.y_pos--;
    //             one_line_up();
    //         }
    //         memset(line_buf, '\0', sizeof(line_buf));
    //         buf_count = 0;
    //         printf("%c",'\n');
    //         set_curr_pos(term.x_pos, term.y_pos);
    //         return i + 1; // Return the number of characters processed
    //     }
	// }
	// // moved this down 
	// set_curr_pos(term.x_pos, term.y_pos);
	// term_write(1,buf, nbytes);
	// set_curr_pos(term.x_pos, term.y_pos);			//set current position on screen
	return i;
}


/* keyboard_read
 * 
 * Reads the character and updates cursor position
 * Inputs: char buffer
 * Outputs: None
 * Side Effects: sets cursor and blink
 * Coverage: term
 */
void keyboard_read(unsigned char* buf){
	curr_term[termIdx].x_pos = get_curr_pos()[0];					//grab current position on screen
	curr_term[termIdx].y_pos = get_curr_pos()[1];

	if (buf[0] == '\n') {
		curr_term[termIdx].x_pos = 0;
		curr_term[termIdx].y_pos++;
		if (curr_term[termIdx].y_pos >= NUM_ROWS - 1) {
			curr_term[termIdx].y_pos--;
			one_line_up();
		}
		printf("%c", '\n');
		set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
		set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos); // this must be changing the global cursor position so that we can type
	}
	set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
	set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
	keyboard_write(buf, 1);
	set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
	set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
}

/* keyboard_write
 * 
 * writes the buffer to the current terminal for a specified number of bytes
 * Inputs: char buffer
 * Outputs: None
 * Side Effects: currTerm array
 * Coverage: term
 */
void keyboard_write(const unsigned char* buf, int nbytes){
	int i;
	//loop over all characters in buffer
	for(i = 0; i < nbytes; i++){
		curr_term[termIdx].x_pos = get_curr_pos()[0];					// get current postiion
		curr_term[termIdx].y_pos = get_curr_pos()[1];
		set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
		set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);			//update current position
		
		if((curr_term[termIdx].x_pos == NUM_COLS - 1)){				//if x position is at edge of screen, move down a line
			curr_term[termIdx].x_pos = 0;
			if(curr_term[termIdx].y_pos < NUM_ROWS - 1){
				curr_term[termIdx].y_pos++;
			}
			else{
				one_line_up();
			}
		}
		if(curr_term[termIdx].buf_count < MAX_BUFFER){						//if we have not reached buffer limit
			set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);		//update current position
			set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
			putc(buf[i]);								//print char to screen
			curr_term[termIdx].line_buf[curr_term[termIdx].buf_count] = buf[i];				//add char to buffer
			curr_term[termIdx].buf_count++;								//increase count
			if(curr_term[termIdx].x_pos < NUM_COLS - 1 && buf[i] != '\n'){				//if we have not reached the edge of screen increase position by 1
				curr_term[termIdx].x_pos++;
			}
		}
		set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);		
		set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);	//update current posiion
	}
}

/* term_keyboard_write
 * 
 * loop through characters in buffer and write to terminal
 * Inputs: char buffer, num bytes to write
 * Outputs: None
 * Side Effects: currTerm array
 * Coverage: term
 */
void term_keyboard_write(const unsigned char* buf, int nbytes){
	int i;
	//loop over all characters in buffer
	for(i = 0; i < nbytes; i++){
		curr_term[scheduleIdx].x_pos = get_curr_pos()[0];					// get current postiion
		curr_term[scheduleIdx].y_pos = get_curr_pos()[1];
		set_curr_pos(curr_term[scheduleIdx].x_pos, curr_term[scheduleIdx].y_pos);			//update current position
		
		if((curr_term[scheduleIdx].x_pos == NUM_COLS)){				//if x position is at edge of screen, move down a line
			curr_term[scheduleIdx].x_pos = 0;
			if(curr_term[scheduleIdx].y_pos < NUM_ROWS - 1){
				curr_term[scheduleIdx].y_pos++;
			}
			else{
				one_line_up();
			}
		}
		if (buf[i] == '\n') {
			curr_term[scheduleIdx].x_pos = 0;
			curr_term[scheduleIdx].y_pos++;
			if (curr_term[scheduleIdx].y_pos >= NUM_ROWS - 1) {
				curr_term[scheduleIdx].y_pos--;
				one_line_up();
			}
			//printf("%c", '\n');
		set_curr_pos(curr_term[scheduleIdx].x_pos, curr_term[scheduleIdx].y_pos);
		}	
		set_curr_pos(curr_term[scheduleIdx].x_pos, curr_term[scheduleIdx].y_pos);			//update current posiion
		if(curr_term[scheduleIdx].buf_count < MAX_BUFFER && buf[i] != '\n'){						//if we have not reached buffer limit
			set_curr_pos(curr_term[scheduleIdx].x_pos, curr_term[scheduleIdx].y_pos);		//update current position
			putc(buf[i]);								//print char to screen
			//line_buf[buf_count] = buf[i];				//add char to buffer
			//buf_count++;								//increase count
			if(curr_term[scheduleIdx].x_pos < NUM_COLS - 1){				//if we have not reached the edge of screen increase position by 1
				curr_term[scheduleIdx].x_pos++;
			}
		}
	}
}
/* term_write
 * 
 * writes buffer to terminal from RTC, Keyboard, or File System
 * Inputs: buffer , number bytes in buffer
 * Outputs: number of bytes
 * Side Effects: None
 * Coverage:
 */
int32_t term_write(int32_t fd, const void* buf, int32_t nbytes) {
	term_keyboard_write(buf, nbytes);
	//loop over all characters in buffer
	// for(i = 0; i < nbytes; i++){
	// 	term.x_pos = get_curr_pos()[0];					// get current postiion
	// 	term.y_pos = get_curr_pos()[1];
	// 	set_curr_pos(term.x_pos, term.y_pos);			//update current position
		
	// 	if((term.x_pos == NUM_COLS - 1)){				//if x position is at edge of screen, move down a line
	// 		term.x_pos = 0;
	// 		if(term.y_pos < NUM_ROWS - 1){
	// 			term.y_pos++;
	// 		}
	// 		else{
	// 			one_line_up();
	// 		}
	// 	}
	// 	if(buf_count < MAX_BUFFER){						//if we have not reached buffer limit
	// 		set_curr_pos(term.x_pos, term.y_pos);		//update current position
	// 		putc(buf[i]);								//print char to screen
	// 		line_buf[buf_count] = buf[i];				//add char to buffer
	// 		buf_count++;								//increase count
	// 		if(term.x_pos < NUM_COLS - 1){				//if we have not reached the edge of screen increase position by 1
	// 			term.x_pos++;
	// 		}
	// 	}
	// 	set_curr_pos(term.x_pos, term.y_pos);			//update current posiion
	// }
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

	curr_term[termIdx].x_pos = get_curr_pos()[0]; // get current screen x and y
	curr_term[termIdx].y_pos = get_curr_pos()[1];

	if(curr_term[termIdx].x_pos == 0){ // if we're at the left most end and we press backspace we go to the previous line. if no previous line, we end
		if(curr_term[termIdx].y_pos > 0){
			curr_term[termIdx].y_pos--;
			curr_term[termIdx].x_pos = NUM_COLS - 1;
		}
		else{return;}
	}
	else{ // if there's characters behind us we go back.
		if(curr_term[termIdx].buf_count > 0)
			curr_term[termIdx].x_pos--;
	}


	if(curr_term[termIdx].buf_count > 0){  // non empty buffer
	// 	line_buf[buf_count - 1] = '\0';
		int i;
		for(i = 1; i < 5; i++){ // check the last four charecters to the cursor and see if they are 4 spaces. if they are then we have a tab character
			if(curr_term[termIdx].line_buf[curr_term[termIdx].buf_count - i] != ' '){
				curr_term[termIdx].tab_flag = 0;
				break;
			}
			curr_term[termIdx].tab_flag = 1; // set the flag
		}
		if(curr_term[termIdx].tab_flag){
			curr_term[termIdx].buf_count-=4;
			int id = curr_term[termIdx].buf_count;
			curr_term[termIdx].x_pos = id % NUM_COLS; // calculate the x and y position of the cursor in the buffer using our id
			curr_term[termIdx].y_pos = id / NUM_COLS;
			set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
			set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
			curr_term[termIdx].tab_flag = 0;
			return;
		}
		curr_term[termIdx].buf_count--;
	}
	// backspace fills the buffer location with a space
	set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
	set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
	putc(' ');
	if(curr_term[termIdx].buf_count == 0){
		memset(curr_term[termIdx].line_buf, '\0', sizeof(curr_term[termIdx].line_buf));
	}
	set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
	set_blink(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);

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
	curr_term[termIdx].x_pos = 0;
	curr_term[termIdx].y_pos = 0;
	//buf_count=0;
	//memset(line_buf, '\0', sizeof(line_buf));
	set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
	clear();
	set_curr_pos(curr_term[termIdx].x_pos, curr_term[termIdx].y_pos);
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
	term_write(2,buf, 4);
}

void one_line_up(){
	int i;
	int vid_offset = NUM_COLS * 2;
	unsigned char* dest = curmem;
    unsigned char* src = curmem + vid_offset;
	unsigned char * bottom = curmem + (NUM_ROWS - 1) * (vid_offset);
    unsigned int size = vid_offset * (NUM_ROWS - 1);
    memmove(dest, src, size);

	for(i = 0; i < NUM_COLS; i++){
		bottom[i*2] = ' ';
		bottom[i*2 + 1] = ATTRIB;
	}
}
