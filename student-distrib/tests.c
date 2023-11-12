#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "term.h"
#include "fs.h"

#include "syscall.h"

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
// int idt_test(){
// 	TEST_HEADER;

// 	int i;
// 	int result = PASS;
// 	for (i = 0; i < 10; ++i){
// 		if ((idt[i].offset_15_00 == NULL) && 
// 			(idt[i].offset_31_16 == NULL)){
// 			assertion_failure();
// 			result = FAIL;
// 		}
// 	}

// 	return result;
// }

// //add more tests here

// int test_divzero(){
// 	TEST_HEADER;
// 	float i;
// 	int result; 
// 	i = 2 / 0; 
// 	assertion_failure();
// 	result = FAIL;
// 	return result;
// }

// int test_debug_exception(){
// 	TEST_HEADER;
// 	int result; 
// 	asm("int $3");	
// 	assertion_failure();
// 	result = FAIL;	
// 	return result;
// }

// int test_null(){
// 	TEST_HEADER;
// 	int result; 
// 	int* ptr = 0x40000000;
// 	result = *ptr;
// 	assertion_failure();
// 	result = FAIL;	
// 	return result;
// }

// // TESTING
// int paging_test() {
//     TEST_HEADER;
//     //Used to test dereference locations.
//     char result;
//     char* pointer = (char*)0x400000;    //Kernel memory
//     result = *pointer;

//     pointer = (char*)0xB8000;                    //Video memory address
//     result = *pointer;

//     pointer = (char*)0x7FFFFF;                 //Bottom of kernel memory
//     result = *pointer;

//     pointer = (char*)0xB8FFF;                 //Bottom of video memory
//     result = *pointer;

//     return PASS; // If exception BSODs, we never get here
// }
// //TESTING

// int paging_exec(){
// 	TEST_HEADER;
// 	uint32_t result;
// 	uint32_t* ptr = (uint32_t*) 138412028;
// 	result = *ptr;
// 	return PASS;
// }

// /* Checkpoint 2 tests */

// // read small files
// int read_small_file() {
// 	TEST_HEADER;
// 	int result = PASS; 

// 	dentry_t myDentry = {
//         .filename = "",      // Empty string, initializes all characters to 0 ('\0')
//         .filetype = 0,       // 0 for uint32_t
//         .inode_num = 0,      // 0 for uint32_t
//         .reserved = {0}      // Initializes all elements of the reserved array to 0
//     };
	
// 	dentry_t* dir_entry = &myDentry; 
// 	uint8_t fname[32] = "frame0.txt";
// 	uint32_t f_size = 187; 
//     uint8_t buf[187]; 
//     int i;
// 	uint32_t test_idx;

//     for (i = 0; i < DIR_ENTRIES_LEN; i++) {
//         if (strncmp((char*)boot_block->dir_entries[i].filename, (const char*)fname, FILENAME_LEN) == 0) {
//             test_idx = i;
//         }
//     }
	
// 	if (0 == read_dentry_by_name((uint8_t*)&fname, dir_entry)) {
// 		inode_t* test1 = (inode_t*)(inode_blocks + (dir_entry->inode_num));
//         if (test1->length != f_size) result = FAIL; 
		
// 		if(read_dentry_by_index(test_idx, dir_entry) == -1) result = FAIL;
// 		inode_t* test2 = (inode_t*)(inode_blocks + (dir_entry->inode_num));
// 		if (test2->length != f_size) result = FAIL;
		
//         uint32_t read_size = read_data(dir_entry->inode_num, 0, buf, f_size); 
//         if (read_size != f_size) result = FAIL; 
//         for (i=0; i<f_size; i++) {
//             putc(buf[i]); 
//         }
		
//     } else {
//         result = FAIL; 
//     }
//     return result; 	
// }


// // read executables
// int read_executables() {
// 	TEST_HEADER;
// 	int result = PASS; 

// 	dentry_t myDentry = {
//         .filename = "",      // Empty string, initializes all characters to 0 ('\0')
//         .filetype = 0,       // 0 for uint32_t
//         .inode_num = 0,      // 0 for uint32_t
//         .reserved = {0}      // Initializes all elements of the reserved array to 0
//     };
	
// 	dentry_t* dir_entry = &myDentry; 
// 	uint8_t fname[32] = "grep";
// 	uint32_t f_size = 6149; 
//     uint8_t buf[6149]; 
//     int i;
// 	uint32_t test_idx;

//     for (i = 0; i < DIR_ENTRIES_LEN; i++) {
//         if (strncmp((char*)boot_block->dir_entries[i].filename, (const char*)fname, FILENAME_LEN) == 0) {
//             test_idx = i;
//         }
//     }

// 	if (0 == read_dentry_by_name((uint8_t*)&fname, dir_entry)) {
// 		inode_t* test1 = (inode_t*)(inode_blocks + (dir_entry->inode_num));
//         if (test1->length != f_size) result = FAIL; 
		
// 		if(read_dentry_by_index(test_idx, dir_entry) == -1) result = FAIL;
// 		inode_t* test2 = (inode_t*)(inode_blocks + (dir_entry->inode_num));
// 		if (test2->length != f_size) result = FAIL;
//         uint32_t read_size = file_read(dir_entry->inode_num, 0, buf, f_size); 
//         if (read_size != f_size) result = FAIL;

// 		printf("First chars: "); 
// 		for (i=0; i<10; i++) {
//             printf("%c",buf[i]); 
//         }

// 		printf(" Mid chars: "); 
// 		for (i=3000; i<3010; i++) {
//             printf("%c",buf[i]); 
//         }
// 		printf(" Last chars: "); 
//         for (i=f_size-10; i<f_size; i++) {
//             printf("%c",buf[i]); 
//         }
// 		printf("\n"); 
		
//     } else {
//         result = FAIL; 
//     }
//     return result; 	
// }

// // read large files
// int read_large_file() {
// 	TEST_HEADER;
// 	int result = PASS; 

// 	dentry_t myDentry = {
//         .filename = "",      // Empty string, initializes all characters to 0 ('\0')
//         .filetype = 0,       // 0 for uint32_t
//         .inode_num = 0,      // 0 for uint32_t
//         .reserved = {0}      // Initializes all elements of the reserved array to 0
//     };
	
// 	dentry_t* dir_entry = &myDentry; 
// 	uint8_t fname[32] = "verylargetextwithverylongname.tx";
// 	uint32_t f_size = 5277; 
//     uint8_t buf[5277]; 
//     int i;
// 	uint32_t test_idx;

//     for (i = 0; i < DIR_ENTRIES_LEN; i++) {
//         if (strncmp((char*)boot_block->dir_entries[i].filename, (const char*)fname, FILENAME_LEN) == 0) {
//             test_idx = i;
//         }
//     }

// 	if (0 == read_dentry_by_name((uint8_t*)&fname, dir_entry)) {
// 		inode_t* test1 = (inode_t*)(inode_blocks + (dir_entry->inode_num));
//         if (test1->length != f_size) result = FAIL; 
		
// 		if(read_dentry_by_index(test_idx, dir_entry) == -1) result = FAIL;
// 		inode_t* test2 = (inode_t*)(inode_blocks + (dir_entry->inode_num));
// 		if (test2->length != f_size) result = FAIL;


//         uint32_t read_size = file_read(dir_entry->inode_num, 0, buf, f_size); 
//         if (read_size != f_size) result = FAIL;

// 		printf("First chars: "); 
// 		for (i=0; i<10; i++) {
//             printf("%c",buf[i]); 
//         }

// 		printf(" Mid chars: "); 
// 		for (i=2500; i<2510; i++) {
//             printf("%c",buf[i]); 
//         }
// 		printf(" Last chars: "); 
//         for (i=f_size-10; i<f_size; i++) {
//             printf("%c",buf[i]); 
//         }
// 		printf("\n"); 
		
//     } else {
//         result = FAIL; 
//     }
//     return result; 	
// }

// // directory read reads one file at a time

// int read_directory_test() {
// 	TEST_HEADER;
// 	int result = PASS; 
// 	uint32_t ret; 
// 	uint32_t i; 
// 	int j; 
// 	uint8_t buf[32]; 

// 	for (i = 0; i < FILENAME_LEN-1; i++) {
// 		ret = directory_read(i, buf); 
// 		if (ret > 0 && ret <= FILENAME_LEN) {
// 			for (j = 0; j < ret; j++) {
// 				putc(buf[j]); 
// 			}
// 			putc('\n'); 
// 		} else {
// 			if (i < 15) { // 15 = file size of the directory (read only)
// 				result=FAIL; 
// 			}
// 		}
// 	}

// 	return result; 
// }

// int rtcopening(){
// 	rtc_open(0);
// 	while(1){
// 		rtc_read(0,0,0);
// 	}
// 	return 0;
// }
// int rtcwriting(int val){
// 	int* ptr = &val;
// 	rtc_write(0, ptr, 4);
// 	while(1){
// 		rtc_read(0,ptr,0);
// 	}
// 	return 0;
// }


// int checkKeys(){
// 	term_write(2,(unsigned char*)"Yo, what's good?\n", 18);
// 	return 0;
// }

// int execheck(){
// 	uint8_t fname[32] = "shell";
// 	int val =  execute((const uint8_t*) fname);
// 	// 255 because -1 in execheck which is uint8_t
// 	if(val == 255 || val == 0) return FAIL;
// 	return PASS;
// }

/* Checkpoint 2 tests */

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
int cat_check(){
	execute("cat frame0.txt");
	return 0;
}
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("div zero", test_divzero());
	// TEST_OUTPUT("debug exception", test_debug_exception());
	// TEST_OUTPUT("paging tests", paging_test());
	// TEST_OUTPUT("above kern", above_kern());
	// TEST_OUTPUT("below kern", below_kern());
	// TEST_OUTPUT("below video", below_video());
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("read_executables test", read_executables());
	//TEST_OUTPUT("read_large_file test", read_large_file());
	//TEST_OUTPUT("read_small_file test", read_small_file());
	//TEST_OUTPUT("read_directory_test test", read_directory_test());
	//TEST_OUTPUT("above video", above_video());
	// TEST_OUTPUT("deref null", test_null());
	// TEST_OUTPUT("test_debug_exception", test_debug_exception());
	//TEST_OUTPUT("rtc functionality", rtcwriting(32));
	 //TEST_OUTPUT("rtc functionality", rtcopening());
	 //TEST_OUTPUT("key test", checkKeys());
	//TEST_OUTPUT("execheck", execheck());
	//TEST_OUTPUT("execheck", paging_exec());
	TEST_OUTPUT("execheck", cat_check());
	//putc('@');
}


