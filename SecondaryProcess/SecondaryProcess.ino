// BlockSignallingV4 Secondary Process

// This program takes instructions and executes them

// Define some things
#include "ArduinoJson.h"

// then define the pin to subtract
// this defines the conversion between the virtual pins on the primary board
// and the actial pins on this secondary board
int pinToSubtract = 100;

void setup() {
    // then setup the debugging serial port
	Serial.begin(9600);
	// then setup the board communication port 
	Serial1.begin(115200);
	
}

void loop() {
	// then use ArduinoJson to check for new commands
	
}

