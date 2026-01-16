/*
	Notes:
	- 20mhz is REQUIRED
	- BOD is optional since replaceable batteries
*/
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
uint8_t colors[6] = {0b001, 0b010, 0b011, 0b100, 0b101, 0b110};

// Needs to be a divisor of 5 and 2
const uint8_t BRIGHTNESS = 150;

const uint8_t NUM_LEDS = 12;
uint8_t pixels[NUM_LEDS*3];
tinyNeoPixel leds = tinyNeoPixel(NUM_LEDS, PIN_NEO_DTA, NEO_GRB, pixels);

uint8_t timeLeft = 0;
uint8_t chargeDot = 0;

const uint8_t HALF_BRIGHTNESS = BRIGHTNESS/2;

void setPixel( const uint8_t nr, const uint8_t r = 0, const uint8_t g = 0, const uint8_t b = 0 ){
	const uint8_t begin = nr*3;
	pixels[begin] = g;
	pixels[begin+1] = r;
	pixels[begin+2] = b;
}

void setPixels( const uint8_t r = 0, const uint8_t g = 0, const uint8_t b = 0 ){

	for( uint8_t i = 0; i < NUM_LEDS; ++i )
		setPixel(i, r, g, b);
	leds.show();

}



bool handsPresent(){
	

	// Take 5 readings
	for( uint8_t i = 0; i < 5; ++i ){

		const int16_t base = analogRead(0);			// Take a baseline IR reading
		digitalWrite(PIN_IR_OUT, HIGH);
		delay(2);
		const int16_t onReading = analogRead(0);
		digitalWrite(PIN_IR_OUT, LOW);
		if( onReading-base < IR_SENSITIVITY )
			return false;
		
		
		delay(5);


	}
	return true;
	
}

void bigpp( const bool on = false ){
	digitalWrite(PIN_BIGPP, on ? LOW : HIGH);
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

	//randomSeed(analogRead(PIN_IR_IN));
	
	setPixels(50);
	delay(255);
	setPixels(0,50);
	delay(255);
	setPixels(0,0,50);
	delay(255);
	setPixels();
	delay(255);

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

			const uint8_t NUM_SEGS = 20;
			const uint8_t NUM_FLARES = 5;
			const uint8_t LEDS_PER_BLOCK = NUM_LEDS/2;
			const float pixelDist = 1.0f/LEDS_PER_BLOCK;

			for( uint8_t i = 0; i < 6; ++i ){
				uint8_t n = random(0,6);
				uint8_t t = colors[n];
				colors[n] = colors[i];
				colors[i] = t;
			}

			for( uint8_t i = 0; i < NUM_FLARES; ++i ){
				uint8_t COLOR = colors[i];
				uint8_t 
					baseR = (COLOR & 0b1)*255, 
					baseG = ((COLOR>>1) & 0b1)*255, 
					baseB = ((COLOR>>2) & 0b1)*255
				;
				const uint8_t SEGS = (NUM_SEGS+10) + (10*(i==NUM_FLARES-1));

				// Each seg is 10ms. Need to use x2 for the segment to finish fading out
				for( uint8_t seg = 0; seg < SEGS; ++seg ){

					// Burst over 250 ms
					const float animPerc = (float)seg/NUM_SEGS;
					for( uint8_t led = 0; led < NUM_LEDS; ++led ){

						// D1 is the top diode
						float localPerc = 0;
						if( led ){ // First one is top diode, always 0
							localPerc = (float)(led) / LEDS_PER_BLOCK;
							if( led > 6 ){
								localPerc = 1.0f - (float)(led-6)/LEDS_PER_BLOCK;
							}
						}

						const float dist = animPerc - localPerc;
						uint8_t r = 0, g = 0, b = 0;
						if( dist > -pixelDist && led != 6 ){	// 6 is bottom, should never be on
							float mul = max(0, 1.0-dist);
							if( dist < 0 )
								mul = 1.0-fabs(dist)/pixelDist;
							mul *= mul;
							r = baseR*mul;
							g = baseG*mul;
							b = baseB*mul;
						}

						setPixel(led, r, g, b);
						
					}				
					leds.show();
					delay(10);
				}
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

		delay(10);

		// Fade in all red
		for( uint8_t i = 0; i < BRIGHTNESS; ++i ){

			setPixels(i+1);
			delay(2);
			
		}

		// Check battery
		const float BAT_DIVIDER_A = 1000;	// kiloohm
		const float BAT_DIVIDER_B = 294;	// k
		const float BAT_MAX = 4.2;
		float voltage = (analogRead(PIN_BAT_LV)+0.5)*BAT_MAX/1024.0; // Get actual voltage on pin
		voltage *= (float)(BAT_DIVIDER_A+BAT_DIVIDER_B)/BAT_DIVIDER_B;    // Ratio is (R1+R2)/R2, so multiply by that to get the actual voltage

		if( voltage < 3.2 ){

			// Blink red
			setPixels(HALF_BRIGHTNESS);
			delay(255);
			setPixels();
			delay(255);
			setPixels(HALF_BRIGHTNESS);
			delay(255);
			setPixels();
			delay(255);
			setPixels(HALF_BRIGHTNESS);
			delay(255);
			setPixels();
			delay(255);
			
		}


		return;

	}
	else if( !hands && !finished )
		finished = true;
	

	bigpp();	// Always disable bigpp when sleeping
	sleep_cpu();
	

}


