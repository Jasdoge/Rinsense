#include <tinyNeoPixel_Static.h>
#include <avr/sleep.h>

ISR(RTC_PIT_vect){
	RTC.PITINTFLAGS = RTC_PI_bm;
}

const uint8_t PIN_BAT_LV = 1;
const uint8_t PIN_IR_IN = 0;
const uint8_t PIN_IR_OUT = 4;
const uint8_t PIN_BIGPP = 3;
const uint8_t PIN_NEO_DTA = 2;

const uint8_t IR_SENSITIVITY = 30;
bool finished = true;

// Needs to be a divisor of 5 and 2
const uint8_t BRIGHTNESS = 100;

const uint8_t NUM_LEDS = 12;
uint8_t pixels[NUM_LEDS*3];
tinyNeoPixel leds = tinyNeoPixel(NUM_LEDS, PIN_NEO_DTA, NEO_GRB, pixels);

uint8_t timeLeft = 0;
uint8_t chargeDot = 0;


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
		delay(35);

	}
}

// Helper function that draws a percentage, nr being 0-12
void drawRange( uint8_t nr, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0 ){
	
	for( uint8_t i = 0; i < NUM_LEDS; ++i ){

		if( nr > i )
			setPixel(i, r, g, b);
		else
			setPixel(i);

	}

	leds.show();
	
}



bool handsPresent(){


	// Take 5 readings
	for( uint8_t i = 0; i < 5; ++i ){

		int16_t base = analogRead(0);			// Take a baseline IR reading
		digitalWrite(PIN_IR_OUT, HIGH);
		delay(2);
		int16_t onReading = analogRead(0);
		digitalWrite(PIN_IR_OUT, LOW);
		if( onReading-base < IR_SENSITIVITY )
			return false;
		
		
		delay(5);


	}
	return true;
	
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
  	RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc /* RTC Clock Cycles = 2Hz */
  	| RTC_PITEN_bm;  

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	pinMode(PIN_BIGPP, OUTPUT);
	pinMode(PIN_IR_OUT, OUTPUT);
	pinMode(PIN_IR_IN, INPUT);
	pinMode(PIN_BAT_LV, INPUT);
	pinMode(PIN_NEO_DTA, OUTPUT);

	
	setPixels(50);
	delay(500);
	setPixels(0,50);
	delay(500);
	setPixels(0,0,50);
	delay(500);
	setPixels();
	delay(100);

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

		
		delay(1100);


		if( timeLeft == 0 ){

			setPixels();
			
			colorCycle(0, BRIGHTNESS, 0);
			colorCycle(BRIGHTNESS, 0, BRIGHTNESS);
			colorCycle(0, 0, BRIGHTNESS);


			// Cycle out
			for( uint8_t i =0; i < NUM_LEDS; ++i ){

				setPixel(i);
				leds.show();
				delay(35);

			}
			
		}

		return;

	}
	
	// Prevents restarting until the sensor is unblocked
	const bool hands = handsPresent();
	if( hands && finished ){

		timeLeft = 24;
		finished = false;
		bigpp(true);

		delay(10);	// Either keep this in or add a 10uF capacitor. It draws too much power otherwise.

		// Fade in
		for( uint8_t i = 0; i < BRIGHTNESS; ++i ){

			setPixels(i+1);
			delay(2);
			
		}

		// Check battery
		const float BAT_DIVIDER_A = 1000;	// kiloohm
		const float BAT_DIVIDER_B = 294;	// k
		const float BAT_MAX = 4.2;
		float voltage = (analogRead(PIN_BAT_LV)+0.5)*BAT_MAX/1024.0; // Get actual voltage on pin
		voltage *= (BAT_DIVIDER_A+BAT_DIVIDER_B)/BAT_DIVIDER_B;    // Ratio is (R1+R2)/R2, so multiply by that to get the actual voltage

		if( voltage < 3 ){

			// Blink red
			setPixels(BRIGHTNESS/2);
			delay(250);
			setPixels();
			delay(250);
			setPixels(BRIGHTNESS/2);
			delay(250);
			setPixels();
			delay(250);
			setPixels(BRIGHTNESS/2);
			delay(250);
			setPixels();
			delay(250);
			
		}


		return;

	}
	else if( !hands && !finished )
		finished = true;
	

	sleep();
	

}


