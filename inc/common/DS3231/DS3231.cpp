#include "DS3231.h"

using namespace digitalcave;

DS3231::DS3231(I2C* i2c) :
	i2c(i2c)
{
	;
}

static inline uint8_t decimal_to_bcd(uint8_t decimal){
	return (decimal / 10) << 4 | (decimal % 10);
}

static inline uint8_t bcd_to_decimal(uint8_t bcd){
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

ds3231_time_t DS3231::getTime(){
	uint8_t data[7];
	data[0] = 0x00;
	I2CMessage message(data, sizeof(data));
	message.setLength(1);
	i2c->write(DS3231_ADDRESS, &message);

	message.setLength(7);
	i2c->read(DS3231_ADDRESS, &message);

	ds3231_time_t result;
	result.year = bcd_to_decimal(data[6]);
	result.month = bcd_to_decimal(data[5]);
	result.day_of_month = bcd_to_decimal(data[4]);
	result.day_of_week = bcd_to_decimal(data[3]);
	if (data[2] & (1 << 6)){		//12 hour mode
		if (data[2] & (1 << 5)){
			result.mode = DS3231_MODE_PM;
		}
		else {
			result.mode = DS3231_MODE_AM;
		}
		data[2] &= ~((1 << 6) | (1 << 5));
	}
	else {
		result.mode = DS3231_MODE_24;
	}
	result.hour = bcd_to_decimal(data[2]);
	result.minute = bcd_to_decimal(data[1]);
	result.second = bcd_to_decimal(data[0]);

	return result;
}

void DS3231::setTime(ds3231_time_t time){
	uint8_t data[8];
	data[0] = 0x00;		//Register address
	data[1] = decimal_to_bcd(time.second);
	data[2] = decimal_to_bcd(time.minute);
	data[3] = decimal_to_bcd(time.hour);
	if (time.mode == DS3231_MODE_AM){
		data[3] |= (1 << 6);	//Set the 12 hour flag (6)
	}
	else if (time.mode == DS3231_MODE_PM){
		data[3] |= (1 << 6) | (1 << 5);	//Set the 12 hour flag (6) and the PM flag (5)
	}
	data[4] = decimal_to_bcd(time.day_of_week);
	data[5] = decimal_to_bcd(time.day_of_month);
	data[6] = decimal_to_bcd(time.month);
	data[7] = decimal_to_bcd(time.year);
	I2CMessage message(data, sizeof(data));
	i2c->write(DS3231_ADDRESS, &message);
}
//
// void DS3231::read_registers(){
// 	serial.write("read_registers\n\r");
// 	uint8_t data[12];
// 	data[0] = 0x00;
// 	I2CMessage message(data, sizeof(data));
// 	message.setLength(1);
// 	i2c->write(DS3231_ADDRESS, &message);
//
// 	message.setLength(12);
// 	i2c->read(DS3231_ADDRESS, &message);
// 	char buffer[128];
// 	serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11]));
// }
//
// void DS3231::write_registers(){
// 	I2CMessage message(registers, sizeof(registers));
// 	i2c->write(DS3231_ADDRESS, &message);
// }
