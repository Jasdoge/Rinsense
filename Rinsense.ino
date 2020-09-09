// Include AVR sleep stuff
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

// Macros to toggle ADC (turn off for lower battery consumption)
#define adc_disable() ADCSRA &= ~ bit(ADEN) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC



// Needed to handle wakeups
ISR(WDT_vect) {
	wdt_disable();  // disable watchdog
}

// Enter sleep mode
void sleep( uint8_t dur = SLEEP_1S ){

	adc_disable();	
	MCUSR = 0;                          // reset various flags
	WDTCR |= 0b00011000;               // see docs, set WDCE, WDE
	WDTCR =  0b01000000 | dur;    // set WDIE, and 4s delay
	wdt_reset();

	// Enters sleep mode
    sleep_enable();                          // enables the sleep bit in the mcucr register so sleep is possible
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
	sleep_mode();

	adc_enable();

}



// PIN MAPPING
const uint8_t PIN_RED = 1;
const uint8_t PIN_GREEN = 0;

const uint8_t PIN_IR_OUT = 2;
const uint8_t PIN_IR_IN = 3;
#define PIN_IR_IN_A A3

const uint8_t IR_SENSITIVITY = 22;

// The program cycle runs in states. These are them:
const uint8_t STATE_TRACKING = 0;		// Search for hands!
const uint8_t STATE_SOAP = 1;			// Hands removed, blink the red LEDs to signify it's time to soap up
const uint8_t STATE_RINSE = 2;			// Time to rinse, blink yellow LEDs on and off 
const uint8_t STATE_DONE = 3;			// All done! Glow green! Good job!
const uint8_t STATE_RESET = 4;			// Waiting for hands to be removed, just to prevent shenanigans

uint8_t STATE = STATE_TRACKING;

const uint8_t MAX_TICKS_SOAP = 30;			// Ticking at 2hz so 30 = 15 sec
const uint8_t MAX_TICKS_RINSE = 30;		// Same as above, but for the rinse state

uint8_t ticks;							// Tracks the flashing timer

void setup(){

	adc_enable();

	// Setup pin defaults
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_IR_OUT, OUTPUT);
	pinMode(PIN_IR_IN, INPUT);

	digitalWrite(PIN_GREEN, LOW);
	digitalWrite(PIN_RED, LOW);
	digitalWrite(PIN_IR_OUT, LOW);

	// Diode test (flash green -> yellow -> red)
	digitalWrite(PIN_GREEN, HIGH);
	delay(500);
	digitalWrite(PIN_RED, HIGH);
	delay(500);
	digitalWrite(PIN_GREEN, LOW);
	delay(500);
	digitalWrite(PIN_RED, LOW);
	

}

bool handsPresent(){

	// Take 5 readings
	for( uint8_t i = 0; i < 5; ++i ){

		int16_t base = analogRead(PIN_IR_IN_A);			// Take a baseline IR reading
		digitalWrite(PIN_IR_OUT, HIGH);
		delay(1);	// Wait a bit for LED to fully turn on
		
		int16_t onReading = analogRead(PIN_IR_IN_A);
		digitalWrite(PIN_IR_OUT, LOW);

		if( onReading-base < IR_SENSITIVITY )
			return false;
		
		delay(1);

	}
	return true;
	
}

void loop(){

	// Find presence of hands
	if( STATE == STATE_TRACKING ){

		bool hands = handsPresent();
		if( !hands ){

			sleep(SLEEP_05S);
			return;

		}

		digitalWrite(PIN_IR_OUT, LOW);	// Disable sensor LED, sleep also does this

		analogWrite(PIN_RED, 5);
		++STATE;
		ticks = 0;

	}

	// Blinking for 20 seconds
	if( STATE == STATE_SOAP || STATE == STATE_RINSE ){

		const uint8_t mt = STATE == STATE_SOAP ? MAX_TICKS_SOAP : MAX_TICKS_RINSE;

		++ticks;
		digitalWrite(PIN_RED, !(ticks%2) );			// Tick on even
		digitalWrite(PIN_GREEN, !(ticks%2) && STATE == STATE_RINSE );			// Tick on even
		sleep(SLEEP_05S);

		// Finished ticking
		if( ticks >= mt ){

			++STATE;
			ticks = 0;

		}

	}

	// Show green, and then reset
	if( STATE == STATE_DONE ){

		digitalWrite(PIN_RED, LOW);
		digitalWrite(PIN_GREEN, HIGH);

		// Stay green for 4 sec
		sleep(SLEEP_4S);
		++STATE;
		digitalWrite(PIN_GREEN, LOW);

	}

	// Wait for low
	if( STATE == STATE_RESET ){

		if( !handsPresent() ){
			
			STATE = STATE_TRACKING;
			return;

		}

		// Hands are present, wait until they're removed
		sleep(SLEEP_1S);
		

	}

}

