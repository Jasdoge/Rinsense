#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

// Sleep mode
#define SLEEP_025S 0b000100
#define SLEEP_05S 0b000101
#define SLEEP_1S 0b000110
#define SLEEP_2S 0b000111
#define SLEEP_4S 0b100000
#define SLEEP_8S 0b100001


#define adc_disable() ADCSRA &= ~ bit(ADEN) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC

ISR(PCINT0_vect){}
ISR(WDT_vect) {
	wdt_disable();  // disable watchdog
}

void sleep( byte dur = SLEEP_1S ){
	
	MCUSR = 0;                          // reset various flags
	WDTCR |= 0b00011000;               // see docs, set WDCE, WDE
	WDTCR =  0b01000000 | dur;    // set WDIE, and 4s delay
	wdt_reset();

	// Enters sleep mode
    sleep_enable();                          // enables the sleep bit in the mcucr register so sleep is possible
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
	sleep_mode();

}


// PIN MAPPING
const byte PIN_RED = 0;
const byte PIN_GREEN = 1;

const byte PIN_IR_OUT = 2;
const byte PIN_IR_IN = 3;
const byte PIN_BUZZER = 4;

const byte STATE_TRACKING = 0;		// Search for hands!
const byte STATE_FOUND = 1;			// Found hands! Waiting for hands to be removed (caaaarl)
const byte STATE_TIMER = 2;			// Hands removed, blink the red LEDs on and off in a clock motion
const byte STATE_DONE = 3;			// All done! Glow green! Good job!
byte STATE = STATE_TRACKING;

const byte MAX_TICKS = 40;			// Ticking at 2hz so 40 = 20 sec
byte ticks;							// Nr 0.5s blinks done so far
byte pump_held;						// Nr cycles you've held your hands in front of the pump. (This is to prevent the "warn" light from getting stuck)
const byte PUMP_HELD_MAX = 100;		// Each cycle is 1/10th of a second. So 10 = 1 sec

void beep( byte times = 1, uint32_t usec = 1000 ){

	for( byte i=0; i<times; ++i ){
		
		digitalWrite(PIN_BUZZER, HIGH);
		delayMicroseconds(usec);
		digitalWrite(PIN_BUZZER, LOW);
		delayMicroseconds(usec);

	}

}

void setup(){

	adc_disable();
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_BUZZER, OUTPUT);
	pinMode(PIN_IR_OUT, OUTPUT);
	pinMode(PIN_IR_IN, INPUT);

	digitalWrite(PIN_GREEN, LOW);
	digitalWrite(PIN_RED, LOW);
	digitalWrite(PIN_BUZZER, LOW);
	digitalWrite(PIN_IR_OUT, LOW);

	// Diode test
	digitalWrite(PIN_GREEN, HIGH);
	delay(500);
	digitalWrite(PIN_RED, HIGH);
	delay(500);
	digitalWrite(PIN_GREEN, LOW);
	delay(500);
	digitalWrite(PIN_RED, LOW);
	

}

void loop(){

	// Find presence of hands
	if( STATE == STATE_TRACKING || STATE == STATE_FOUND ){

		digitalWrite(PIN_IR_OUT, HIGH);
		delay(1);
		bool near = digitalRead(PIN_IR_IN);
		digitalWrite(PIN_IR_OUT, LOW);

		if( near && STATE == STATE_TRACKING ){
			
			// Turn on half red
			++STATE;
			analogWrite(PIN_RED, 5);
			beep(1, 20000);
			pump_held = 0;

		}
		else if( (!near || ++pump_held >= PUMP_HELD_MAX ) && STATE == STATE_FOUND ){
			
			++STATE;
			ticks = 0;	// reset ticks

		}

		// State might have moved on beyond these, which is why it's an else if instead of else
		if( STATE == STATE_TRACKING )
			sleep(SLEEP_05S);		
		else if( STATE == STATE_FOUND )
			delay(100);	// Can't sleep here because of PWM

	}

	if( STATE == STATE_TIMER ){

		++ticks;
		digitalWrite(PIN_RED, !(ticks%2) );			// Tick on even
		sleep(SLEEP_05S);

		// Finished ticking
		if( ticks >= MAX_TICKS )
			++STATE;

	}

	if( STATE == STATE_DONE ){

		digitalWrite(PIN_RED, LOW);
		digitalWrite(PIN_GREEN, HIGH);
		beep(2, 200000);
		sleep(SLEEP_4S);
		STATE = STATE_TRACKING;	 // Reset to basic
		digitalWrite(PIN_GREEN, LOW);

	}

}

