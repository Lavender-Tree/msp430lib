#include "oled.h"
#include "font.h"
#include <stdarg.h>
#include <string.h>

static int pos_x = 0, pos_y = 0; 

/* send command */
void oled_send_cmd(char cmd)
{
    IIC_start();
	IIC_writebyte(0x78);    // Slave address, SA0=0
	IIC_writebyte(0x00);    // write command
	IIC_writebyte(cmd);
	IIC_stop();
}

/* send data to RAM */
void oled_send_data(char data)
{
    IIC_start();
	IIC_writebyte(0x78);
	IIC_writebyte(0x40);
	IIC_writebyte(data);
	IIC_stop();
}

/* oled init */
void oled_init(void) 
{
    IIC_ioinit();

	/* display off */
	oled_send_cmd(0xAE); 
	
	/* set memory addressing mode */
    oled_set_addr_mode(ADDR_MODE_PAGE);
	
	/* Set Page Start Address for Page Addressing Mode */
	oled_send_cmd(0xb0); 	// 0-7

	/* Set Common Ouput Scan Direction */
	oled_send_cmd(0xc8);		// COM[N-1] ~ COM[0]

	/* set start line address */
	oled_send_cmd(0x40);		// 0

	/* contrast control register */
	oled_send_cmd(0x81); 
	oled_send_cmd(0x7f);

	/* set segment re-map  */
	oled_send_cmd(0xa1); 	// reverse direction

	/* Set Normal/Reverse Display */
	oled_send_cmd(0xa6); 	// normal

	/* set multiplex ratio */
	oled_send_cmd(0xa8);
	oled_send_cmd(0x3F); 	// 64

	/* Set Entire Display */
	oled_send_cmd(0xa4); 	// (off) Output follows RAM content

	/* (14) set display offset */
	oled_send_cmd(0xd3); 
	oled_send_cmd(0x00); 	// 0

	/* (15) set display clock divide ratio/oscillator frequency */
	oled_send_cmd(0xd5);
	oled_send_cmd(0xf0); 	// set divide ratio
	
	/* (16) Set Dis-charge/Pre-charge Period */
	oled_send_cmd(0xd9);	
	oled_send_cmd(0x22); 	// pre-charge: 2 DCLKs (POR) | dis-charge: 2 DCLKs (POR)

	/* (17) Set Common pads hardware configuration */
	oled_send_cmd(0xda);
	oled_send_cmd(0x12);		// H, Alternative. (POR)

	/* (18) Set VCOM Deselect Level */
	oled_send_cmd(0xdb);
	oled_send_cmd(0x20); 	// 0x20, 0.77 x Vcc

	// /* (10) set DC-DC enable */
	// oled_send_cmd(0x8d);
	// oled_send_cmd(0x14);

	/* display on */
	oled_send_cmd(0xaf); 

	oled_cls();
	oled_set_pos(0, 0);
}

/* set current postion */
void oled_set_pos(int x, int y)
{
    oled_send_cmd(0xb0 | y);
	oled_send_cmd(((x & 0xf0) >> 4) | 0x10);
	oled_send_cmd(((x + 2) & 0x0f));        // unknown bug: "+2" ?
}

/* clear screen */
void oled_cls(void)
{
    int y, x;
    pos_x = 0, pos_y = 0;
	for (y = 0; y < OLED_HEIGHT / 8; y++) {
        oled_set_pos(0, y);
		for (x = 0; x < OLED_WIDTH; x++)
			oled_send_data(0);
	}
    oled_set_pos(0, 0);
}

/* clear one line */
void oled_clear_line(char line)
{
    oled_set_pos(0, line);
    for (pos_x = 0; pos_x < OLED_WIDTH; pos_x++)
        oled_send_data(0);
    pos_x = 0, pos_y = line;
    oled_set_pos(0, line);
}

/* display an image */
void oled_display_img(char *img, int width, int height, int _x, int _y)
{
    int x = 0, y = 0;
	height /= 8;
	for (y = 0; y < height; y++) {
		oled_set_pos(_x, y + _y);
		for (x = 0; x < width; x++) {
			oled_send_data(img[x + y * width]);
		}
	}
    oled_set_pos(pos_x, pos_y);
}

static void __out_char(char c)
{
    int x = 0;
    if (c < F6x8_OFFSET) return;
    c -= F6x8_OFFSET;
    for (; x < 6; x++) 
        oled_send_data(F6x8[c * 6 + x]);
}

/* display one char at (x, y) */
void oled_display_chr(char c, int _x, int _y)
{
    oled_set_pos(_x, _y);
    __out_char(c);
    oled_set_pos(pos_x, pos_y);
}

void oled_display_short(unsigned short num, int x, int y)
{
    int i = 4;
    char buf[6] = {0};
    while (num) {
        buf[i--] = num % 10 + 0x30;
        num /= 10;
    }
    while (i >= 0) buf[i--] = '0';
    oled_set_pos(x, y);
    for (i = 0; i < 5; i++)
        __out_char(buf[i]);
    oled_set_pos(pos_x, pos_y);
}

void oled_display_str(char *s, int n, int x, int y)
{
    oled_set_pos(x, y);
    while (n-- && *s) __out_char(*s++);
    oled_set_pos(pos_x, pos_y);
}

/* put char */
void oled_putc(char c)
{
    int x = 0;
    __out_char(c);
    pos_x += 6;
}

/* puts */
void oled_puts(char *s, int n)
{
    while (n-- && *s) 
        oled_putc(*s++);    
}


char* i16tostr(unsigned int num, char *buf)
{
    /* buf -> char[6] */
    int i = 4;
    buf[5] = 0;

    /* 0 */
    if (!num) {
        buf[0] = '0', buf[1] = 0;
        return buf;
    }
    /* ! 0 */
    while (num) {
        buf[i--] = num % 10 + 0x30;
        num /= 10;
    }
    return &buf[++i];
}

void __internal_printf(char *fmt, va_list args)
{
    char buf[0x20] = {0}, op = 0;
    char *p = 0;
    union 
    {
        unsigned short vunsigned;
    } value;

    while ( (op = *fmt++) ) {
        /* char cannot be printed */
        if (op < 0x20) {
            switch (op) {
            case '\n':
                oled_set_pos(pos_x, ++pos_y);
                break;
            default:
                break;
            }
        } else if (op == '%') {
            op = *fmt++;
            switch (op) {
            case 'c':
                oled_putc(va_arg(args, char));
                break;
            case 'd':
            {
                value.vunsigned = va_arg(args, short);
                p = i16tostr(value.vunsigned, buf);
                oled_puts(p, 6);
                break;
            }
            case 's':
            {
                p = va_arg(args, char *);
                oled_puts(p, 100);
                break;
            }
            default:
                break;
            }
        } else {
            oled_putc(op);
        }
    }
}

void oled_printf(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __internal_printf(fmt, args);
    va_end(args);
}


void oled_printfln(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __internal_printf(fmt, args);
    va_end(args);

    while (pos_x < OLED_WIDTH) {
        oled_send_data(0);
        pos_x++;
    }
    pos_x = 0, pos_y++;
    oled_set_pos(pos_x, pos_y);
}