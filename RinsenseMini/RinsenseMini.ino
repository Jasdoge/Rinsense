/*

	Todo: 
	- IR Sensor
	- Print case
	- Test charger
	- Test power draw
	- Design case


*/

#include <tinyNeoPixel_Static.h>
#include <avr/sleep.h>

ISR(RTC_PIT_vect){
	RTC.PITINTFLAGS = RTC_PI_bm;
}

const uint8_t PIN_CHRG_STAT = 1;
const uint8_t PIN_IR_IN = 0;
const uint8_t PIN_IR_OUT = 4;
const uint8_t PIN_BIGPP = 3;
const uint8_t PIN_NEO_DTA = 2;

// Needs to be a divisor of 5 and 2
const uint8_t BRIGHTNESS = 50;

const uint8_t NUM_LEDS = 12;
uint8_t pixels[NUM_LEDS*3];
tinyNeoPixel leds = tinyNeoPixel(NUM_LEDS, PIN_NEO_DTA, NEO_GRB, pixels);

uint8_t timeLeft = 24;


void setPixel( uint8_t nr, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0 ){
	const uint8_t begin = nr*3;
	pixels[begin] = g;
	pixels[begin+1] = r;
	pixels[begin+2] = b;
}

void setPixels( uint8_t r = 0, uint8_t g = 0, uint8_t b = 0 ){

	for( uint8_t i = 0; i < NUM_LEDS; ++i )
		setPixel(i, r, g, b);
	leds.show();

}


void colorCycle( uint8_t r = 0, uint8_t g = 0, uint8_t b = 0 ){
	for( uint8_t i =0; i < NUM_LEDS; ++i ){

		setPixel(i, r, g, b);
		leds.show();
		delay(25);

	}
}


void bigpp( bool on = false ){
	digitalWrite(PIN_BIGPP, on ? LOW : HIGH);
}

void sleep(){

	bigpp();	// Always disable bigpp when sleeping
	sleep_cpu();

}

void setup(){

	/* Initialize RTC: */
  	while (RTC.STATUS > 0){}
  	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;    /* 32.768kHz Internal Ultra-Low-Power Oscillator (OSCULP32K) */
  	RTC.PITINTCTRL = RTC_PI_bm;           /* PIT Interrupt: enabled */
  	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles = 1Hz */
  	| RTC_PITEN_bm;  

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	pinMode(PIN_BIGPP, OUTPUT);
	pinMode(PIN_IR_OUT, OUTPUT);
	pinMode(PIN_IR_IN, INPUT);
	pinMode(PIN_CHRG_STAT, INPUT);
	pinMode(PIN_NEO_DTA, OUTPUT);

	
	setPixels(50);
	delay(100);
	setPixels(0,50);
	delay(100);
	setPixels(0,0,50);
	delay(100);
	setPixels();
	delay(1000);

}


void loop(){

	// Clocking
	if( timeLeft ){

		--timeLeft;

		const bool roundOne = (timeLeft >= 12);
		const uint8_t sub = timeLeft%12;
		
		for( uint8_t i = 0; i < NUM_LEDS; ++i ){


			uint8_t red = BRIGHTNESS;
			uint8_t green = BRIGHTNESS;

			if( roundOne ){

				// This is above, go red
				if( i <= sub )
					green = 0;
				// This is below, go low brightness
				else{
					red /= 5;
					green /= 5;
				}

			}
			else{
				// This is above, stay yellow
				if( i <= sub ){
					red /= 2;
					green /= 2;
				}
				// This is below, turn off
				else{
					red = green = 0;
				}

			}

			setPixel(NUM_LEDS-i-1, red, green);

		}

		leds.show();

		
		delay(1000);


		if( timeLeft == 0 ){

			setPixels();
			
			colorCycle(0, BRIGHTNESS, 0);
			colorCycle(BRIGHTNESS, 0, BRIGHTNESS);
			colorCycle(0, 0, BRIGHTNESS);


			// Cycle out
			for( uint8_t i =0; i < NUM_LEDS; ++i ){

				setPixel(i);
				leds.show();
				delay(25);

			}
			
		}

		return;

	}
	
	sleep();

}


