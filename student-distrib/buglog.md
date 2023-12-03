checkpoint 1: the bugman
1) OS boots and immideatly skips to start screen. GDT init wrong. Had to convert the start of the gdt_desc to a word instead of a long
2) unable to interact with assembly wrapped functions. including our functions.h was causing those functions to come up as being declared twice. making global symbols and accessing them for assembly linkage through teh header file fixed this.
3) Keyboard was only registering a single interrupt. gdb showed hanging after interrupt returned. error in sendEOI function so we never finished servicing our first intnerrupt
4) RTC did not register any interrupts. We were calling the init function in the IDT instead of sending the handler.
5) Interrupts not working. fixed by rearranging order of init in kernel.c

checkpoint 2: the bugman returns
1) rtc frequency not updating proerly when set to 1024. was using the wrong comparison statement.
2) issues with virutalizing. switched from working with the rate to working with the frequency itself.
3) not writing properly using the rtc write function. Was storing to the wrong argument had to store as pointer into the buffer instead.
4) Had problem in terminal driver where it would repeatedly spam first keyboard input without ending (EOI issue)
5) Problems with my x_pos of cursor becoming negative values if certain special keys were hit
6) Enter key was not properly storing the buffer and allowed additional keypresses to go past the maximum buffer size
7) If you were to press tab then backspace it would send the cursor to seemingly random locations
8) printing strings into the write_terminal would cause the buffer to behave abnormally
9) file system memcopy was acting extremely buggy, caused a page fault. We eventually found that we could fix by initializing an empty dentry object
10) problems with strncpy, fixing issues with types in comparisons allowed it to work correctly

checkpoint 3: the bugman evolves into a creature I no longer understand
1) The terminal read and write functions were not utilizing the buffer properly
2) Enter key not triggering the buffer to be loaded within term_read
3) Problems with how the pointer structer in pcb.c was being utilized across system calls
4) Problems with assembly linkage for system calls particularly with passing parameters via stack
5) Fixing scrolling from checkpoint 2 was causing problems in the video memory space
6) Problems calling c functions from assembly using assembly linkage and vice versa
7) Problems fixing the format of the inline assembly
8) Problems with keyboard handler not mingling correctly with system calls to terminal read
9) Problem compiling execute system call without errors.

checkpoint 4: the bugman grows old and his wife finds out he cheated on her
1) ls hitting directory read fail due to improper updating of file_position/inode_num in fd
2) ls printing all but last file due to updating file_position every time regardless of success or fail
3) read syscall not working for different filetypes due to pointer issues
4) cat leaving program abnormally. adding endline at end of getargs.
5) enter key prints next line of shell one space off, due to code treating enter key as a character that requires space on screen
6) pingpong error caused by updating screen x and screen y values improperly
7) syserror causes page fault due to poor edge case handling in file open
8) fish not working. vidmap paging structure inaccurate.
9) getargs not parsing arguments in whole. had a problems with terminal read and write
10) pingpong output incorrect. had a test interrupts call in read for some reason.


checkpoint 5: the bugman retires from his silly buggery
1) pagefaults all the time: converted the global active pcb pointer to have it's esp and ebp match that of the program when it got a pit interrupt. instead, we needed to maintain original stack boundries and store the esp ebp values from interrupt seperately
2) cursor position reset to schedulers terminal. had to chang eup control flow.
3) keyboard was writing to every terminal, not just terminal of interest.
4) PIT was at a frequency that was causing page faults
5) Programs on one terminal were leaking to other terminals
6) Overlapping interrupts between the scheduler and termWrite
7) Buggy features when adding the third terminal
8) currMem wasn't updated properly, so fish program was bleeding to other terminals
9) All three blinking cursors shown on same display because of video memory bugs 