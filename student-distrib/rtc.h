#ifndef RTC_H
#define RTC_H

#include "lib.h"

#define base_freq 1024

extern void init_rtc();
extern void rtc_handler();

extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_read(int32_t f_desc, int32_t offset, void* buf, int32_t length);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t rtc_close(int32_t fd);

#endif
