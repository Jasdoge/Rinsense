
// PIN MAPPING
const byte PIN_RED = 1;
const byte PIN_GREEN = 0;

const byte PIN_IR_OUT = 2;
const byte PIN_IR_IN = 3;
const byte PIN_BUZZER = 4;

const byte STATE_TRACKING = 0;		// Search for hands!
const byte STATE_FOUND = 1;			// Found hands! Waiting for hands to be removed (caaaarl)
const byte STATE_TIMER = 2;			// Hands removed, blink the red LEDs on and off in a clock motion
const byte STATE_DONE = 3;			// All done! Glow green! Good job!
byte STATE = STATE_TRACKING;




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

	digitalWrite(PIN_IR_OUT, HIGH);
	delayMicroseconds(10);
	bool near = digitalRead(PIN_IR_IN);
	digitalWrite(PIN_IR_IN, LOW);

	digitalWrite(PIN_GREEN, near);

	// Todo: Sleep
	delay(100);

}

