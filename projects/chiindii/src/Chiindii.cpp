#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <PID.h>

#include "lib/Mpu6050/Mpu6050.h"
#include "lib/timer/timer.h"

#include "Chiindii.h"

#include "motor/motor.h"

using namespace digitalcave;

void drive_motors(double throttle, vector_t rate);

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	timer_init();
	
	Mpu6050 mpu6050;
//	mpu6050.calibrate();

	// TODO supply proper kp, ki, kd
	PID rate_x(1, 0, 0, DIRECTION_NORMAL, 10, 0); // 10 = 10 ms
	PID rate_y(1, 0, 0, DIRECTION_NORMAL, 10, 0);
	PID rate_z(1, 0, 0, DIRECTION_NORMAL, 10, 0);
	
	// TODO supply proper tau
	Complementary c_x(0.075, 10, 0); // 10 = 10 ms
	Complementary c_y(0.075, 10, 0);
	Complementary c_z(0.075, 10, 0);
	
	*(&LED_PORT - 1) |= RED | GREEN | BLUE;
	LED_PORT |= RED | GREEN | BLUE;	//Tuen off active low LED
//	DDRB |= _BV(0) | _BV(1);
	
	double throttle = 0;
	vector_t rate_sp;
	vector_t rate_pv;
	vector_t angle_sp;
	vector_t angle_pv;
	
	motor_start();
	
	uint16_t motor = 0;

	//Main program loop
	while (1) {

		vector_t angle_mv = mpu6050.getAccel();
		vector_t rate_mv = mpu6050.getGyro();

		c_x.compute(rate_mv.x, angle_mv.x, &angle_pv.x, time);
		c_y.compute(rate_mv.y, angle_mv.y, &angle_pv.y, time);
		c_z.compute(rate_mv.z, angle_mv.z, &angle_pv.z, time);

		uint32_t time = timer_millis();
		rate_x.compute(rate_sp.x, rate_mv.x, &rate_pv.x, time);
		rate_y.compute(rate_sp.y, rate_mv.y, &rate_pv.y, time);
		rate_z.compute(rate_sp.z, rate_mv.z, &rate_pv.z, time);
		drive_motors(throttle, rate_pv);

//		PORTB ^= _BV(0) | _BV(1);
//		PORTF ^= _BV(5) | _BV(6) | _BV(7);

		//LED warning for low battery
		LED_PORT |= RED | GREEN | BLUE;	//Turn all lights off
		uint8_t battery_level = battery_read();
		if (battery_level < 2) {
			LED_PORT &= ~RED;
		}
		else if (battery_level < 10){
			LED_PORT &= ~(RED | GREEN);
		}
		else {
			LED_PORT &= ~GREEN;
		}

		motor_set(motor, motor, motor, motor);
		motor++;
		if (motor >= 0x1FF) motor = 0;
		
		_delay_ms(10);
	}
}

void drive_motors(double throttle, vector_t rate_pv) {
	double m1 = throttle - rate_pv.x - rate_pv.y - rate_pv.z;
	double m2 = throttle + rate_pv.x - rate_pv.y + rate_pv.z;
	double m3 = throttle + rate_pv.x + rate_pv.y - rate_pv.z;
	double m4 = throttle - rate_pv.x + rate_pv.y + rate_pv.z;

	motor_set(m1, m2, m3, m4);
} 
