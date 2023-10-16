1) OS boots and immideatly skips to start screen. GDT init wrong. Had to convert the start of the gdt_desc to a word instead of a long
2) unable to interact with assembly wrapped functions. including our functions.h was causing those functions to come up as being declared twice. making global symbols and accessing them for assembly linkage through teh header file fixed this.
3) Keyboard was only registering a single interrupt. gdb showed hanging after interrupt returned. error in sendEOI function so we never finished servicing our first intnerrupt
4) RTC did not register any interrupts. We were calling the init function in the IDT instead of sending the handler.
5) Interrupts not working. fixed by rearranging order of init in kernel.c
 