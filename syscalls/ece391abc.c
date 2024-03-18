#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

// #include "ioctl.h"

uint8_t *vmem_base_addr;
uint8_t *f0[] = {"aframe0.txt", "aframe1.txt", "aframe2.txt", "aframe3.txt", "aframe4.txt", "aframe5.txt", "aframe6.txt", "aframe7.txt", "aframe8.txt", "aframe9.txt", "aframe10.txt", "aframe11.txt", "aframe12.txt", "aframe13.txt", "aframe14.txt", "aframe15.txt", "aframe16.txt", "aframe17.txt", "aframe18.txt", "aframe19.txt", "aframe20.txt", "aframe21.txt", "aframe22.txt", "aframe23.txt", "aframe24.txt", "aframe25.txt", "aframe26.txt", "aframe27.txt", "aframe28.txt", "aframe29.txt", "aframe30.txt", "aframe31.txt", "aframe32.txt"};

int main(){
    int rtc_fd, ret_val, i, j, garbage, eof, ii, col, row, num_bytes;
    int fd;
    uint8_t c = '0';
    if(ece391_vidmap(&vmem_base_addr) == -1){
        return -1;
    }

    rtc_fd = ece391_open((uint8_t*)"rtc");
    ret_val = 32;
    ret_val = ece391_write(rtc_fd, &ret_val, 4);
    for(ii = 0; ii < 33; ii++)
    {
        eof = 0;
        fd = ece391_open(f0[ii]);
        row = 0;
        while(!eof){
            c = '0';
            col = 0;
            while(1){
                num_bytes = ece391_read(fd, &c, 1);
                if(c != '\n'){
                    if(c == 'j'){
                        eof = 1;
                        break;
                    }
                    vmem_base_addr[2 * (row * 80 + col + 40)] = c;
                    col++;
                }
                else break;
            }
            row++;
        }
    for(j=0; j < 4; j++){
        ece391_read(rtc_fd, &garbage, 4);
    }
    ece391_close(fd);
    }

    ece391_close(rtc_fd);

    return 0;
}