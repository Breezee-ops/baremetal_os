#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "fs.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

//add more tests here

int test_divzero(){
	TEST_HEADER;
	float i;
	int result; 
	i = 2 / 0; 
	assertion_failure();
	result = FAIL;
	return result;
}

int test_debug_exception(){
	TEST_HEADER;
	int result; 
	asm("int $3");	
	assertion_failure();
	result = FAIL;	
	return result;
}

int test_null(){
	TEST_HEADER;
	int result; 
	int* ptr = 0x40000000;
	result = *ptr;
	assertion_failure();
	result = FAIL;	
	return result;
}

// TESTING
int paging_test() {
    TEST_HEADER;
    //Used to test dereference locations.
    char result;
    char* pointer = (char*)0x400000;    //Kernel memory
    result = *pointer;

    pointer = (char*)0xB8000;                    //Video memory address
    result = *pointer;

    pointer = (char*)0x7FFFFF;                 //Bottom of kernel memory
    result = *pointer;

    pointer = (char*)0xB8FFF;                 //Bottom of video memory
    result = *pointer;

    return PASS; // If exception BSODs, we never get here
}
//TESTING

/* Checkpoint 2 tests */

// read small files
int read_small_file() {
	TEST_HEADER;
	int result; 

	dentry_t myDentry = {
        .filename = "",      // Empty string, initializes all characters to 0 ('\0')
        .filetype = 0,       // 0 for uint32_t
        .inode_num = 0,      // 0 for uint32_t
        .reserved = {0}      // Initializes all elements of the reserved array to 0
    };
	
	dentry_t* dir_entry = &myDentry; 
	uint8_t fname[32] = "grep";

	if (0 == read_dentry_by_name((uint8_t*)&fname, dir_entry)) {
		result = PASS; 
	} else {
		result = FAIL; 
	}
	return result; 


	
}

// read executables


// read large files

// directory read reads one file at a time

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("file system test", read_small_file());
	// launch your tests here
	// TEST_OUTPUT("test_debug_exception", test_debug_exception());
	putc('@');


}


