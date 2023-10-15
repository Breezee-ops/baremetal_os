/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define PIC1				0x20		/* IO base address for master PIC */
#define PIC2				0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND		PIC1
#define PIC1_DATA			(PIC1+1)
#define PIC2_COMMAND		PIC2
#define PIC2_DATA			(PIC2+1)
#define PIC_EOI				0x20		/* End-of-interrupt command code */

#define MASTER_OFFSET		0x08
#define SLAVE_OFFSET		0x70

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */\
/* Taken largely from OSdevwiki */
void i8259_init(void) {

	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, MASTER_OFFSET);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, SLAVE_OFFSET);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

/* Enable (unmask) the specified IRQ */
/* Taken largely from OSdevwiki */
void enable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_num -= 8;
    }
    value = inb(port) & ~(1 << irq_num);
    outb(port, value);        
}

/* Disable (mask) the specified IRQ */
/* Taken largely from OSdevwiki */
void disable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_num -= 8;
    }
    value = inb(port) | (1 << irq_num);
    outb(port, value); 
}

/* Send end-of-interrupt signal for the specified IRQ */
/* Taken largely from OSdevwiki */
void send_eoi(uint32_t irq_num) {
	if(irq_num >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
	outb(PIC1_COMMAND,PIC_EOI);
}
