checkpoint 1:
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
8) printing strings into the write_terminal would cause the buffer to behave abnormally\
9) file system memcopy was acting extremely buggy, caused a page fault. We eventually found that we could fix by initializing an empty dentry object
10) problems with strncpy, fixing issues with types in comparisons allowed it to work correctly