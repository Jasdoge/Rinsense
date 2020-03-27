// Sleep mode

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

void beep( byte times = 1, uint32_t usec = 1000 ){

	for( byte i=0; i<times; ++i ){
		
		digitalWrite(PIN_BUZZER, HIGH);
		delayMicroseconds(usec);
		digitalWrite(PIN_BUZZER, LOW);
		delayMicroseconds(usec);

	}

}

void setup(){

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
	delay(100);
	digitalWrite(PIN_RED, HIGH);
	delay(100);
	digitalWrite(PIN_GREEN, LOW);
	delay(100);
	digitalWrite(PIN_RED, LOW);
	

}

void loop(){

	// Find presence of hands
	if( STATE == STATE_TRACKING || STATE == STATE_FOUND ){

		digitalWrite(PIN_IR_OUT, HIGH);
		delayMicroseconds(10);
		bool near = digitalRead(PIN_IR_IN);
		digitalWrite(PIN_IR_IN, LOW);

		if( near && STATE == STATE_TRACKING ){
			
			// Turn on half red
			++STATE;
			analogWrite(PIN_RED, 50);
			beep(1, 10000);

		}
		else if( !near && STATE == STATE_FOUND ){
			
			++STATE;
			ticks = 0;	// reset ticks

		}

		// State might have moved on beyond these, which is why it's an else if instead of else
		if( STATE == STATE_TRACKING )
			delay(1000);							// Todo: Replace with sleep
		else if( STATE == STATE_FOUND )
			delay(100);								// Todo: Replace with sleep

	}

	if( STATE == STATE_TIMER ){

		++ticks;
		digitalWrite(PIN_RED, !(ticks%2) );			// Tick on even
		delay(500);									// Todo: Sleep?

		// Finished ticking
		if( ticks >= MAX_TICKS )
			++STATE;

	}

	if( STATE == STATE_DONE ){

		digitalWrite(PIN_RED, LOW);
		digitalWrite(PIN_GREEN, HIGH);
		beep(2, 100000);
		delay(3000);								// Todo: sleep?
		STATE = STATE_TRACKING;	 // Reset to basic
		digitalWrite(PIN_GREEN, LOW);

	}

}

