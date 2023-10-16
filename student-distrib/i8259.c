/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define PIC1					0x20		/* IO base address for master PIC */
#define PIC2					0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND			PIC1
#define PIC1_DATA				(PIC1+1)
#define PIC2_COMMAND			PIC2
#define PIC2_DATA				(PIC2+1)


#define ICW1_ICW4				0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE				0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4			0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL				0x08		/* Level triggered (edge) mode */
#define ICW1_INIT				0x10		/* Initialization - required! */
 
#define ICW4_8086				0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO				0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE			0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER			0x0C		/* Buffered mode/master */
#define ICW4_SFNM				0x10		/* Special fully nested (not) */

#define MASTER_OFFSET			0x08
#define SLAVE_OFFSET			0x70

#define PIC_EOI					0x60		/* End-of-interrupt command code */


/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

static inline void io_wait(void)
{
    outb(0x80, 0);
}

/* Initialize the 8259 PIC */
/* Taken largely from OSdevwiki */
void i8259_init(void) {

	unsigned char a1, a2;
 
	a1 = 0xFF;                       		 // save masks
	a2 = 0xFF;
 
	outb(0x11, PIC1_COMMAND);   					// starts the initialization sequence (in cascade mode)
	//io_wait();
	outb(0x11, PIC2_COMMAND);
	//io_wait();
	outb(0x20, PIC1_DATA);               		// ICW2: Master PIC vector offset
	//io_wait();
	outb(0x28, PIC2_DATA);                		// ICW2: Slave PIC vector offset
	//io_wait();
	outb(0x04, PIC1_DATA);                     // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	//io_wait();
	outb(0x02, PIC2_DATA);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	//io_wait();
 
	outb(0x01, PIC1_DATA);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	//io_wait();
	outb(0x01, PIC2_DATA);
	//io_wait();
 
	outb(a1, PIC1_DATA);   // restore saved masks.
	outb(a2, PIC2_DATA);

	enable_irq(2); 
}

/* Enable (unmask) the specified IRQ */
/* Taken largely from OSdevwiki */
void enable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
	//check if interrupt came from master
    if(irq_num < 8) {
        port = PIC1_DATA;			//grab data port from master
    } 
	//otherwise interrupt came from slave
	else { 
        port = PIC2_DATA;			//grab data port from slave
        irq_num -= 8;				//wrap irq number to match with slave 0-7
    }
	
    value = inb(port) & ~(1 << irq_num);		//grab interrupt value and send it to master or slave data port
    outb(value, port);        
}

/* Disable (mask) the specified IRQ */
/* Taken largely from OSdevwiki */
void disable_irq(uint32_t irq_num) {
	uint16_t port;
    uint8_t value;
 
	//same master or slave logic as above
    if(irq_num < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_num -= 8;
    }
    value = inb(port) | (1 << irq_num);		//value is using | to send a disable signal through the irq line to the master or slave port
    outb(value, port); 
}

/* Send end-of-interrupt signal for the specified IRQ */
/* Taken largely from OSdevwiki */
void send_eoi(uint32_t irq_num) {
	//start of critical section clear interrupts
	cli(); 
	//if EOI came from slave
	if(irq_num >= 8){
		outb(PIC_EOI | (irq_num - 8), PIC2_COMMAND);		//send the end signal through slave data port
		send_eoi((uint32_t) 2);								//send it through IRQ2 which is the IRQ on master that connects to slave
	}
	outb((PIC_EOI | (irq_num)), PIC1_COMMAND);				//if master send the EOI through master port
	sti(); 													//resume interrupts
}


