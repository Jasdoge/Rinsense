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

bool ON;

const uint8_t NUM_LEDS = 12;
uint8_t pixels[NUM_LEDS*3];
tinyNeoPixel leds = tinyNeoPixel(NUM_LEDS, PIN_NEO_DTA, NEO_GRB, pixels);


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



void bigpp( bool on = false ){
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

	bigpp(true);
	
	setPixels(50);
	delay(500);
	setPixels(0,50);
	delay(500);
	setPixels(0,0,50);
	delay(500);
	setPixels(1,1,1);
	delay(100);



}


void loop(){

	setPixels();
	/*
	ON = !ON;

	digitalWrite(PIN_IR_OUT, ON ? HIGH : LOW);
	delay(2000);
	*/

}


