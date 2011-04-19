#include <avr/io.h>
#include <avr/pgmspace.h> 

#include "../draw.h"

/*
 * The font's code page to map chars to glyphs.  The index of this array is the value of 
 * each char in the string; the value of the array will point to an entry in the font table.
 * 0xFF has the special meaning of "Not implemented; leave blank".
 */
prog_uchar codepage_medium[] PROGMEM = {
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, //0x00-0x0F
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, //0x10-0x1F
	0xFF,0x24,0xFF,0xFF,0xFF,0x2A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x26,0xFF,0x25,0x28, //0x20-0x2F
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0x27, //0x30-0x3F
	0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18, //0x40-0x4F
	0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0xFF,0x29,0xFF,0xFF,0xFF, //0x50-0x5F
	0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18, //0x60-0x6F
	0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0xFF,0xFF,0xFF,0xFF,0xFF, //0x70-0x7F
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF  //0x80-0x8F
};

/*
 * A 7x5 pixel font.  The bits are read across from top left to top right, then down
 * to the next line.  Each character is represented by five 8bit unsigned integers, with 
 * bit 8 of the first ignored.
 */
prog_uchar font_medium[] PROGMEM = {
	0x03, 0xa3, 0x3a, 0xe6, 0x2e,	//0, 0x0
	0x01, 0x18, 0x42, 0x10, 0x8e,	//1
	0x03, 0x21,	0x11, 0x11, 0x1f,	//2
	0x07, 0xc4,	0x41, 0x06, 0x2e,	//3
	0x00, 0x8c,	0xa9, 0x7c, 0x42,	//4
	0x07, 0xe1, 0xe0, 0x86, 0x2e,	//5
	0x01, 0x91, 0x0f, 0x46, 0x2e,	//6
	0x07, 0xc2, 0x22, 0x21, 0x08,	//7
	0x03, 0xe3, 0x17, 0x46, 0x2e,	//8
	0x03, 0xe3, 0x17, 0x04, 0x4c,	//9, 0x9
	0x03, 0xa3, 0x18, 0xfe, 0x31,	//A, 0xA
	0x07, 0xa3, 0x1f, 0x46, 0x3e,	//B, 0xB
	0x03, 0xa3, 0x08, 0x42, 0x2e,	//C
	0x07, 0x25, 0x18, 0xc6, 0x5c,	//D
	0x07, 0xe1, 0x0f, 0xc2, 0x1f,	//E
	0x07, 0xe1, 0x0f, 0x42, 0x10,	//F
	0x03, 0xa3, 0x0b, 0xc6, 0x2f,	//G, 0x10
	0x04, 0x63, 0x1f, 0xc6, 0x31,	//H, 0x11
	0x03, 0x88, 0x42, 0x10, 0x8e,	//I
	0x01, 0xc4, 0x21, 0x0a, 0x4c,	//J
	0x04, 0x65, 0x4c, 0x52, 0x51,	//K
	0x04, 0x21, 0x08, 0x42, 0x1f,	//L
	0x04, 0x77, 0x5a, 0xc6, 0x31,	//M
	0x04, 0x63, 0x9a, 0xce, 0x31,	//N
	0x03, 0xa3, 0x18, 0xc6, 0x2e,	//O
	0x07, 0xa3, 0x1f, 0x42, 0x10,	//P
	0x03, 0xa3, 0x18, 0xd6, 0x4d,	//Q
	0x07, 0xa3, 0x1f, 0x52, 0x51,	//R
	0x03, 0xe1, 0x07, 0x04, 0x3e,	//S
	0x07, 0xc8, 0x42, 0x10, 0x84,	//T
	0x04, 0x63, 0x18, 0xc6, 0x2e,	//U
	0x04, 0x63, 0x18, 0xc5, 0x44,	//V
	0x04, 0x63, 0x1a, 0xd6, 0xaa,	//W, 0x20
	0x04, 0x62, 0xa2, 0x2a, 0x31,	//X
	0x04, 0x62, 0xa2, 0x10, 0x84,	//Y
	0x07, 0xc2, 0x22, 0x22, 0x1f,	//Z, 0x23
	0x01, 0x08, 0x42, 0x10, 0x04,	//!, 0x24
	0x00, 0x00, 0x00, 0x31, 0x80,	//.
	0x00, 0x00, 0x06, 0x11, 0x00,	//,
	0x03, 0xa2, 0x11, 0x10, 0x04,	//?
	0x00, 0x02, 0x22, 0x22, 0x00,	///
	0x00, 0x20, 0x82, 0x08, 0x20,	//\, 0x29
	0x06, 0x32, 0x22, 0x22, 0x63	//%, 0x2A
	
};
