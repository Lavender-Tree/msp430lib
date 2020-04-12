#ifndef OLED_H_
#define OLED_H_

/* my oled info 
 * 128 * 64 
 * sh1106 
 */

#include "IIC.h"

#define     OLED_WIDTH      128
#define     OLED_HEIGHT     64

void oled_send_cmd(char cmd);
void oled_send_data(char data);
void oled_init(void);
void oled_set_pos(int x, int y);
void oled_cls(void);
void oled_clear_line(char line);
void oled_display_img(char *img, int width, int height, int _x, int _y);
void oled_display_chr(char c, int _x, int _y);
void oled_display_short(unsigned short num, int x, int y);
void oled_display_str(char *s, int n, int x, int y);
void oled_putc(char c);
void oled_puts(char *s, int n);
void oled_printf(char *fmt, ...);


#define ADDR_MODE_HORIZONTAL    0       // Horizontal Addressing Mode
#define ADDR_MODE_VERTICAL      1       // Vertical Addressing Mode
#define ADDR_MODE_PAGE          0x10    // Page Addressing Mode (RESET)
#define ADDR_MODE_INVALID       0x11    
/* set addressing mode */
static inline oled_set_addr_mode(char mode)
{
    oled_send_cmd(0x20); 
	oled_send_cmd(mode);
}


char * i16tostr(unsigned int num, char *buf); 

#endif