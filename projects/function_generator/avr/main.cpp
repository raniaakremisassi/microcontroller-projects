#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h> 
#include <stdio.h>
#include <util/delay.h>

#include "lib/Hd44780/Hd44780_Direct.h"
#include "lib/Button/Buttons.h"

using namespace digitalcave;

#define MODE_SQUARE			0x00
#define MODE_SINE			0x01
#define MODE_TRIANGLE		0x02
#define MODE_SAWTOOTH_UP	0x03
#define MODE_SAWTOOTH_DOWN	0x04
#define MODE_STAIRCASE_UP	0x05
#define MODE_STAIRCASE_DOWN	0x06
#define MODE_LAST 			MODE_STAIRCASE_DOWN

#define BUTTON_MODE			_BV(PORTC0)
#define BUTTON_UP			_BV(PORTC1)
#define BUTTON_DOWN			_BV(PORTC2)
#define BUTTON_OK			_BV(PORTC3)

const uint8_t data_sine[] PROGMEM			=	{0x7f,0x82,0x85,0x88,0x8b,0x8e,0x91,0x94,0x97,0x9b,0x9e,0xa1,0xa4,0xa7,0xaa,0xad,0xaf,0xb2,0xb5,0xb8,0xbb,0xbe,0xc0,0xc3,0xc6,0xc8,0xcb,0xcd,0xd0,0xd2,0xd4,0xd7,0xd9,0xdb,0xdd,0xdf,0xe1,0xe3,0xe5,0xe7,0xe9,0xeb,0xec,0xee,0xef,0xf1,0xf2,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfb,0xfc,0xfd,0xfd,0xfe,0xfe,0xfe,0xfe,0xfe,0xff,0xfe,0xfe,0xfe,0xfe,0xfe,0xfd,0xfd,0xfc,0xfb,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,0xf5,0xf4,0xf2,0xf1,0xef,0xee,0xec,0xeb,0xe9,0xe7,0xe5,0xe3,0xe1,0xdf,0xdd,0xdb,0xd9,0xd7,0xd4,0xd2,0xd0,0xcd,0xcb,0xc8,0xc6,0xc3,0xc0,0xbe,0xbb,0xb8,0xb5,0xb2,0xaf,0xad,0xaa,0xa7,0xa4,0xa1,0x9e,0x9b,0x97,0x94,0x91,0x8e,0x8b,0x88,0x85,0x82,0x7f,0x7c,0x79,0x76,0x73,0x70,0x6d,0x6a,0x67,0x63,0x60,0x5d,0x5a,0x57,0x54,0x51,0x4f,0x4c,0x49,0x46,0x43,0x40,0x3e,0x3b,0x38,0x36,0x33,0x31,0x2e,0x2c,0x2a,0x27,0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,0x15,0x13,0x12,0x10,0x0f,0x0d,0x0c,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x03,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0c,0x0d,0x0f,0x10,0x12,0x13,0x15,0x17,0x19,0x1b,0x1d,0x1f,0x21,0x23,0x25,0x27,0x2a,0x2c,0x2e,0x31,0x33,0x36,0x38,0x3b,0x3e,0x40,0x43,0x46,0x49,0x4c,0x4f,0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x67,0x6a,0x6d,0x70,0x73,0x76,0x79,0x7c};
const uint8_t data_triangle[] PROGMEM		=	{0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,0xff,0xfe,0xfc,0xfa,0xf8,0xf6,0xf4,0xf2,0xf0,0xee,0xec,0xea,0xe8,0xe6,0xe4,0xe2,0xe0,0xde,0xdc,0xda,0xd8,0xd6,0xd4,0xd2,0xd0,0xce,0xcc,0xca,0xc8,0xc6,0xc4,0xc2,0xc0,0xbe,0xbc,0xba,0xb8,0xb6,0xb4,0xb2,0xb0,0xae,0xac,0xaa,0xa8,0xa6,0xa4,0xa2,0xa0,0x9e,0x9c,0x9a,0x98,0x96,0x94,0x92,0x90,0x8e,0x8c,0x8a,0x88,0x86,0x84,0x82,0x80,0x7e,0x7c,0x7a,0x78,0x76,0x74,0x72,0x70,0x6e,0x6c,0x6a,0x68,0x66,0x64,0x62,0x60,0x5e,0x5c,0x5a,0x58,0x56,0x54,0x52,0x50,0x4e,0x4c,0x4a,0x48,0x46,0x44,0x42,0x40,0x3e,0x3c,0x3a,0x38,0x36,0x34,0x32,0x30,0x2e,0x2c,0x2a,0x28,0x26,0x24,0x22,0x20,0x1e,0x1c,0x1a,0x18,0x16,0x14,0x12,0x10,0x0e,0x0c,0x0a,0x08,0x06,0x04,0x02};
const uint8_t data_sawtooth_up[] PROGMEM	=	{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};
const uint8_t data_sawtooth_down[] PROGMEM	=	{0xff,0xfe,0xfd,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,0xf5,0xf4,0xf3,0xf2,0xf1,0xf0,0xef,0xee,0xed,0xec,0xeb,0xea,0xe9,0xe8,0xe7,0xe6,0xe5,0xe4,0xe3,0xe2,0xe1,0xe0,0xdf,0xde,0xdd,0xdc,0xdb,0xda,0xd9,0xd8,0xd7,0xd6,0xd5,0xd4,0xd3,0xd2,0xd1,0xd0,0xcf,0xce,0xcd,0xcc,0xcb,0xca,0xc9,0xc8,0xc7,0xc6,0xc5,0xc4,0xc3,0xc2,0xc1,0xc0,0xbf,0xbe,0xbd,0xbc,0xbb,0xba,0xb9,0xb8,0xb7,0xb6,0xb5,0xb4,0xb3,0xb2,0xb1,0xb0,0xaf,0xae,0xad,0xac,0xab,0xaa,0xa9,0xa8,0xa7,0xa6,0xa5,0xa4,0xa3,0xa2,0xa1,0xa0,0x9f,0x9e,0x9d,0x9c,0x9b,0x9a,0x99,0x98,0x97,0x96,0x95,0x94,0x93,0x92,0x91,0x90,0x8f,0x8e,0x8d,0x8c,0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x84,0x83,0x82,0x81,0x80,0x7f,0x7e,0x7d,0x7c,0x7b,0x7a,0x79,0x78,0x77,0x76,0x75,0x74,0x73,0x72,0x71,0x70,0x6f,0x6e,0x6d,0x6c,0x6b,0x6a,0x69,0x68,0x67,0x66,0x65,0x64,0x63,0x62,0x61,0x60,0x5f,0x5e,0x5d,0x5c,0x5b,0x5a,0x59,0x58,0x57,0x56,0x55,0x54,0x53,0x52,0x51,0x50,0x4f,0x4e,0x4d,0x4c,0x4b,0x4a,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40,0x3f,0x3e,0x3d,0x3c,0x3b,0x3a,0x39,0x38,0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x30,0x2f,0x2e,0x2d,0x2c,0x2b,0x2a,0x29,0x28,0x27,0x26,0x25,0x24,0x23,0x22,0x21,0x20,0x1f,0x1e,0x1d,0x1c,0x1b,0x1a,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};
const uint8_t data_staircase_up[] PROGMEM	=	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0};
const uint8_t data_staircase_down[] PROGMEM	=	{0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uint8_t data[256];	//Copy PROGMEM DDS signals into this buffer

static Hd44780_Direct display(display.FUNCTION_LINE_2 | display.FUNCTION_SIZE_5x8);
static Buttons buttons(&PORTC, BUTTON_MODE | BUTTON_UP | BUTTON_DOWN | BUTTON_OK, 3, 8);

static uint8_t mode = MODE_SQUARE;
static uint16_t frequency = 0;	//Indexed frequencies, different from square vs DDS waveforms

extern "C"{	//This is needed when calling ASM-implemented functions
	void output_square_wave_1khz();
	void output_square_wave_5khz();
	void output_square_wave_10khz();
	void output_square_wave_25khz();
	void output_square_wave_50khz();
	void output_square_wave_100khz();
	void output_square_wave_250khz();
	void output_square_wave_500khz();
	void output_square_wave_1000khz();
	void output_square_wave_2500khz();
	void output_dds_wave();
}

void update_display(){
	display.clear();

	display.setDdramAddress(0x00);
	if (mode == MODE_SQUARE){
		display.setText((char*) "Square", 6);
	}
	else if (mode == MODE_SINE){
		display.setText((char*) "Sine", 4);
	}
	else if (mode == MODE_TRIANGLE){
		display.setText((char*) "Triangle", 8);
	}
	else if (mode == MODE_SAWTOOTH_UP){
		display.setText((char*) "Sawtooth Up", 11);
	}
	else if (mode == MODE_SAWTOOTH_DOWN){
		display.setText((char*) "Sawtooth Dn", 11);
	}
	else if (mode == MODE_STAIRCASE_UP){
		display.setText((char*) "Staircase Up", 12);
	}
	else if (mode == MODE_STAIRCASE_DOWN){
		display.setText((char*) "Staircase Dn", 12);
	}
	else {
		display.setText((char*) "Unknown", 7);
	}
		
	display.setDdramAddress(0x40);
	if (mode == MODE_SQUARE){
		if (frequency == 0) display.setText((char*) "1kHz", 4);
		else if (frequency == 1) display.setText((char*) "5kHz", 4);
		else if (frequency == 2) display.setText((char*) "10kHz", 5);
		else if (frequency == 3) display.setText((char*) "25kHz", 5);
		else if (frequency == 4) display.setText((char*) "50kHz", 5);
		else if (frequency == 5) display.setText((char*) "100kHz", 6);
		else if (frequency == 6) display.setText((char*) "250kHz", 6);
		else if (frequency == 7) display.setText((char*) "500kHz", 6);
		else if (frequency == 8) display.setText((char*) "1MHz", 4);
		else if (frequency == 9) display.setText((char*) "2.5MHz", 6);
		else display.setText((char*) "Unknown", 7);
	}
	else {
		if (frequency == 0) display.setText((char*) "1Hz", 3);
		else if (frequency == 1) display.setText((char*) "2.5Hz", 4);
		else if (frequency == 2) display.setText((char*) "5Hz", 3);
		else if (frequency == 3) display.setText((char*) "10Hz", 4);
		else if (frequency == 4) display.setText((char*) "25Hz", 4);
		else if (frequency == 5) display.setText((char*) "50Hz", 4);
		else if (frequency == 6) display.setText((char*) "100Hz", 5);
		else if (frequency == 7) display.setText((char*) "250Hz", 5);
		else if (frequency == 8) display.setText((char*) "500Hz", 5);
		else if (frequency == 9) display.setText((char*) "1kHz", 4);
	}
}

void pick_menu(){
	update_display();
	while(1){
		uint8_t changed = buttons.changed();
		uint8_t pressed = buttons.pressed() & changed;
		uint8_t held = buttons.pressed() & ~changed;
		uint8_t released = ~buttons.pressed() & changed;

		if (released & BUTTON_MODE){
			mode++;
			if (mode > MODE_LAST){
				mode = MODE_SQUARE;
				frequency = 1;
			}
			else {
				frequency = 1000;
			}
			update_display();
		}
		else if ((held & BUTTON_MODE) && ((pressed & BUTTON_UP) || (pressed & BUTTON_DOWN))){
			update_display();
			return;
		}
		else if (pressed & BUTTON_UP){
			frequency++;
			if (frequency > 9) frequency = 0;
			update_display();
		}
		else if (pressed & BUTTON_DOWN){
			frequency--;
			if (frequency > 9) frequency = 9;
			update_display();			
		}
		
		_delay_ms(12);
		buttons.sample();
	}
}

void output_waveform(){
	display.setDdramAddress(0x0F);
	char temp[1];
	temp[0] = '#';
	display.setText(temp, 1);
	
	if (mode == MODE_SQUARE){
		if (frequency < 7){
			TCCR1A = 0x0;	//Normal mode
			TCCR1B = _BV(CS10);	//No prescaler
			//Set up the frequency.  We multiply by 2 because each time through the interrupt is half a waveform.
			// The negative offset is because the ISR takes that many instructions.
			if (frequency == 0) OCR1A = F_CPU / 2000 - 16;			//1kHz
			else if (frequency == 1) OCR1A = F_CPU / 10000 - 16;	//5kHz
			else if (frequency == 2) OCR1A = F_CPU / 20000 - 16;	//10kHz
			else if (frequency == 3) OCR1A = F_CPU / 50000 - 16;	//25kHz
			else if (frequency == 4) OCR1A = F_CPU / 100000 - 16;	//50kHz
			else if (frequency == 5) OCR1A = F_CPU / 200000 - 16;	//100kHz
			else if (frequency == 6) OCR1A = F_CPU / 500000 - 16;	//250kHz
			TIMSK1 = _BV(OCIE1A);	//Enable OCR1A interrupts
			TCNT1 = 0;
			sei();
			while(1);	//Timers do everything now...
		}
		else if (frequency == 7) output_square_wave_500khz();
		else if (frequency == 8) output_square_wave_1000khz();
		else if (frequency == 9) output_square_wave_2500khz();
	}
	else {
		const uint8_t* progmem_pointer;
		
		//Figure out which DDS mode we are in
		if (mode == MODE_SINE) progmem_pointer = data_sine;
		else if (mode == MODE_TRIANGLE) progmem_pointer = data_triangle;
		else if (mode == MODE_SAWTOOTH_UP) progmem_pointer = data_sawtooth_up;
		else if (mode == MODE_SAWTOOTH_DOWN) progmem_pointer = data_sawtooth_down;
		else if (mode == MODE_STAIRCASE_UP) progmem_pointer = data_staircase_up;
		else if (mode == MODE_STAIRCASE_DOWN) progmem_pointer = data_staircase_down;
		else return;
	
		//Copy from PROGMEM to RAM buffer
		for (uint16_t i = 0; i <= 0xFF; i++){
			data[i] = pgm_read_byte_near(progmem_pointer + i);
		}
	
		//Start generating the waveform
		uint8_t i = 0;
		while(1){
			PORTD = data[i++];
		}
	}
}

int main (void){

	//Init port D in output mode (used to send data to DAC)
	DDRD = 0xFF;

	while (1) {
		//Pick mode / options
		pick_menu();

		//Output waveform
		output_waveform();
	}
}

EMPTY_INTERRUPT(TIMER1_OVF_vect)

/*
 * DDS Waveform Generation
 */
ISR(TIMER1_COMPB_vect){
	static uint8_t i = 0;
	PORTD = data[i++];
	TCNT1 = 0;
}