// BlockSignallingV4 Secondary Process

// This program takes instructions and executes them

// Define some things
#include "ArduinoJson.h"

// then define the pin to subtract
// this defines the conversion between the virtual pins on the primary board
// and the actual pins on this secondary board
int pinToSubtract = 100;

void setup() {
    // then setup the debugging serial port
	Serial.begin(9600);
	// then setup the board communication port 
	Serial1.begin(115200);
	
}

void loop() {
	// then use ArduinoJson to check for new commands
	while(Serial1.available() > 0) {
        // then define the document
        StaticJsonDocument<48> document;
        DeserializationError error = deserializeJson(document, Serial1);
        if(error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            digitalWrite(13, HIGH);
            delay(500);
            digitalWrite(13, LOW);
            delay(500);
            digitalWrite(13, HIGH);
            return;
        }
        
        // then check the instruction
        int instructionType = document["instruction"].as<int>;

        // then get the pin from the data1 variable
        int pin = document["data1"].as<int>;

        pin = pin - pinToSubtract;
        
        // SetPinMode
        if(instructionType == 0) {
            // then get the pin mode from the data2 variable
            int PinMode = document["data2"].as<int>;
            // then get the pin from
            // then check what PinMode to set the pin to
            if(PinMode == 0) {
                // then set the pin to INPUT_PULLUP
                pinMode(pin, INPUT_PULLUP);
            }
            else if(PinMode == 1) {
                // then set the pin to INPUT
                pinMode(pin, INPUT);
            }
            else if(PinMode == 2) {
                // then set the pin to OUTPUT
                pinMode(pin, OUTPUT);
            }
        }
        // SetPin
        else if(instructionType == 1) {
            // then check whether to set the pin high or low from the data2 variable
            bool high = document["data2"].as<bool>;
            if(high == true) {
                // then set the pin high
                digitalWrite(pin, HIGH);
            }
            else if(high == false) {
                // then set the pin low
                digitalWrite(pin, LOW);
            }
        }
    }
}

