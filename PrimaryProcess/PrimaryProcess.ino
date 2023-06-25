// BlockSignallingV4 PrimaryProcess Written In June 2023 By Ben Forster

// First, Define some things

#include "ArduinoJson.h";

// Sensor Debounce Time
unsigned long SensorDebounceTime = 4000;

// Lines List - Track - See Documentation
int LinesList[6] = {1,2,3,4,5,6};

// Blocks Array
int Blocks[3][7] = {
  // 0  1  2  3  4  5  6
    {0, 2, 0, 0, 0, 2, 1},
    {1, 3, 0, 0, 0, 0, 1},
    {2, 4, 0, 0, 0, 1, 1}
};

// SensorLastTriggeredTime
unsigned long SensorLastTriggeredTime[3] = {
    0, 0, 0
}

// Signals Array
int Signals[3][11] = {
  // 0  1  2  3  4   5   6  7   8  9  10
    {0, 0, 5, 0, 0, 14, 15, 0, 16, 0, 2},
    {1, 1, 5, 0, 0, 17, 18, 0, 19, 3, 5},
    {2, 2, 5, 0, 0, 20, 21, 0, 22, 6, 8}
};

// Signal Instructions Array
int SignalInstructions[9][11] = {
  // 0  1  2  3  4  5  6  7  8  9  10
    {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}, //signal 0 danger if block infront occupied
    {1, 0, 1, 2, 1, 0, 2, 1, 0, 0, 1}, //signal 0 warning if block infront clear and block infront of that occupied
    {2, 0, 3, 2, 1, 0, 2, 0, 0, 0, 1}, //signal 0 pass if block infront clear and block infront of that clear
    {3, 1, 0, 1, 2, 1, 0, 0, 0, 0, 0}, //signal 1 danger if block infront occupied
    {4, 1, 1, 2, 2, 0, 0, 1, 0, 0, 1}, //signal 1 warning if block infront clear and block infront of that occupied
    {5, 1, 3, 2, 2, 0, 0, 0, 0, 0, 1}, //signal 1 pass if block infront clear and block infront of that clear
    {6, 2, 0, 1, 0, 1, 0, 0, 0, 0, 0}, //signal 2 danger if block infront occupied
    {7, 2, 1, 2, 0, 0, 1, 1, 0, 0, 1}, //signal 2 warning if block infront clear and block infront of that occupied
    {8, 2, 3, 2, 0, 0, 1, 0, 0, 0, 1}  //signal 2 pass if block infront clear and block infront of that clear
};

// setPinMode function
// this function takes a pin and then checks if it is locally on this board or whether it is on the other board
// if it is on the other board then it will send it down the Serial1
void setPinMode(int pin, int mode) {
    // check if the pin is on the expansion board
    if(pin >= 100) {
        // then it is so make the JSON
        StaticJsonDocument<48> document;
        document["instruction"] = "SPM";
        document["data1"] = pin;
        document["data2"] = mode;
        
        // then send this to Serial1
        seialiseJson(document, Serial1);
    }
    else {
        // then check what mode it is
        // mode == 1 = INPUT_PULLUP
        if(mode == 1) {
            pinMode(pin, INPUT_PULLUP);
        }
        // mode == 2 = INPUT
        else if(mode == 2) {
            pinMode(pin, INPUT);
        }
        // mode == 3 = OUTPUT
        else if(mode == 3) {
            pinMode(pin, OUTPUT);
        }
    }

}

// setPin function
// this function takes a pin and then checks if it is locally on this board or whether it is on the other board
// if it is on the other board then it will send it down the Serial1
void setPin(int pin, bool high) {
    // then check if it is on the expansion board
    if(pin >= 100) {
        // then it is so make the JSON
        StaticJsonDocument<> document;
        document["instruction"] = "SP";
        document["data1"] = pin;
        document["data2"] = high;

        // then send this to Serial1
        serialiseJson(document, Serial1);
    }
    else {
        // then write the pin high or low
        if(high == true) {
            digitalWrite(pin, HIGH);
        }
        else if(high == false) {
            digitalWrite(pin, LOW);
        }
    }
}



void setup() {
    // then start the programming and debugging serial port
    Serial.begin(9600);
    // then start the expansion board serial port at a higher data rate
    Serial1.begin(115200)

    // then initialise all of the pins as input or output
    // start with the Blocks
    for(int i = 0; i < sizeof(Blocks)/sizeof(Blocks[0]); i++) {
        // then set the 1st index to input pullup for the sensor
        setPinMode(Blocks[i][1], 0);
    }
    
    // then do the Signals
    for(int i = 0; i < sizeof(Signals)/sizeof(Signals[0]); i++) {
        // then dependant on the signal type set different indexes to output
        // signal type Semaphore [0/1]
        if(Signals[i][2] == 0 || Signals[i][2] == 1) {
            // then set the SignalChangePin [4] to output
            setPinMode(Signals[i][4], 2);
        }
        // signal type 2 Aspect Colour Light Signal Danger & Pass [2]
        else if(Signals[i][2] == 2) {
            // then set the SignalDangerPin [5] and SignalPassPin [8] to output
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][8], 2);
        }
        // signal type 2 Aspect Colour Light Signal Danger & Warning [3]
        else if(Signals[i][2] == 3) {
            // then set the SignalDangerPin [5] and SignalWarningPin [6]
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][6], 2); 
        }
        // signal type 2 Aspect Colour Light Signal Warning & Pass [4]
        else if(Signals[i][2] == 4) {
            // then set the SignalWarningPin [6] and SignalPassPin [8]
            setPinMode(Signals[i][6], 2);
            setPinMode(Signals[i][8], 2);
        } 
        // signal type 3 Aspect Colour Light Signal [5]
        else if(Signals[i][2] == 5) {
            // then set the SignalDangerPin [5] and SignalWarningPin [6] and SignalPassPin [8]
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][6], 2);
            setPinMode(Signals[i][8], 2);
        }
        // signal type 4 Aspect Colour Light Signal [5]
        else if(Signals[i][2] == 6) {
            // then set the SignalDangerPin [5] and SignalWarningPin [6] and Signal Advanced Warning Pin [7]
            // and SignalPassPin [8]
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][6], 2);
            setPinMode(Signals[i][7], 2);
            setPinMode(Signals[i][8], 2);
        }
    }

};

void loop() {
    // this part of the code continually sets the states of the IsSensorActivated and IsBlockOccupied
    for(var i = 0; i < sizeof(Blocks)/sizeof(Blocks[0]); i++) {
        // 1 - read the sensor
        // isTriggered value - will report if the sensor is triggered - default to true for failsafe
        bool isTriggered = true;
        if(digitalRead(Blocks[i][1]) == LOW) {
            // then the sensor is LOW so check if the sensor is HIGH on activation or not
            if(Blocks[i][2] == 1) {
                // then the sensor is active on a HIGH signal so this sensor is NOT triggered
                isTriggered = false;
            }
            else if(Blocks[i][2] == 0) {
                // the the sensor is active on a LOW signal so this sensor IS triggered
                isTriggered = true;
            }
        }
        else {
            // then the sensor is HIGH so check if the sensor is HIGH on activation or not
            if(Blocks[i][2] == 1) {
                // then the sensor is active on a HIGH signal so this sensor IS triggered
                isTriggered = true;
            }
            else if(Blocks[i][2] == 0) {
                // then the sensor is active on a LOW signal so this sensor is NOT triggered
                isTriggered = false;
            }
        }

        // then after that we should know if the sensor is triggered or not
        // 2 - if the sensor IS triggered
        if(isTriggered == true) {
            // 1 - Set the isSensorActivated variable true (1)
            Blocks[i][3] = 1;
            // 2 = Set the IsBlockOccupied variable true (1)
            Blocks[i][4] = 1;
            // 3 - Set the SensorLastTriggeredTime variable to current millis()
            SensorLastTriggeredTime[i] = millis();
        }
        // 3 - if the sensor is NOT triggered
        else if(isTriggered == false) {
            // 1 - Set the isSensorActivated variable false (1)
            Blocks[i][3] = 0;
            // 2 - then attempt to prove that that the block behind can be considered clear
            // by checking if millis() - SensorLastTriggeredTime > SensorDebounceTime
            if(millis() - SensorLastTriggeredTime[i] >= SensorDebounceTime) {
                // then the block behind can be considered clear however make sure that this
                // sensor was activated is newer than the sensor behind's activation time
                if(SensorLastTriggeredTime[i] > SensorLastTriggeredTime[Blocks[i][5]]) {
                    // then the block behind can now officially be considered clear so set it
                    Blocks[Blocks[i][5]][4] = 0;
                }
            }   
        }
    }
    
    // this part of the code executes the signal instructions
    // 1 - go through the Signals array
    for(int i = 0; i < sizeof(Signals)/sizeof(Signals[0]); i++) {
        
    }

};