// BlockSignallingV4 PrimaryProcess Written In June 2023 By Ben Forster

// First, Define some things

#include "ArduinoJson.h";

// Sensor Debounce Time - how long a sensor must be clear until the block behind it is cleared
unsigned long SensorDebounceTime = 4000;

// RelayHoldTime - this is passed to the delay() function so it is blocking.
// used to turn the relay on and off. Should be kept as low as possible (milliseconds)
const int RelayHoldTime = 100;

// Lines List - Track - See Documentation
const int LinesList[6] = {1,2,3,4,5,6};

// Blocks Array
int Blocks[3][7] = {
  // 0  1  2  3  4  5  6
    {0, 2, 0, 0, 0, 2, 1},
    {1, 3, 0, 0, 0, 0, 1},
    {2, 4, 0, 0, 0, 1, 1}
};

// SensorLastTriggeredTime
unsigned long SensorLastTriggeredTime[3] = {0, 0, 0};

// Signals Array
int Signals[3][11] = {
  // 0  1  2  3  4   5   6  7   8  9  10
    {0, 0, 5, 0, 0, 14, 15, 0, 16, 0, 2},
    {1, 1, 5, 0, 0, 17, 20, 0, 21, 3, 5},
    {2, 2, 5, 0, 0, 22, 23, 0, 24, 6, 8}
};

// Signal Instructions Array
const int SignalInstructions[9][11] = {
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
        document["instruction"] = 0; // SetPinMode
        document["data1"] = pin;
        document["data2"] = mode;
        
        // then send this to Serial1
        serializeJson(document, Serial1);
    }
    else {
        // then check what mode it is
        // mode == 0 = INPUT_PULLUP
        if(mode == 0) {
            pinMode(pin, INPUT_PULLUP);
        }
        // mode == 1 = INPUT
        else if(mode == 1) {
            pinMode(pin, INPUT);
        }
        // mode == 2 = OUTPUT
        else if(mode == 2) {
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
        StaticJsonDocument<48> document;
        document["instruction"] = 1; // SetPin
        document["data1"] = pin;
        document["data2"] = high;

        // then send this to Serial1
        serializeJson(document, Serial1);
    }
    else {
        // then write the pin high or low
        if(high == true) {
            digitalWrite(pin, HIGH);
            Serial.print("PIN: ");
            Serial.print(pin);
            Serial.println(" SET HIGH");
        }
        else if(high == false) {
            digitalWrite(pin, LOW);
            Serial.print("PIN: ");
            Serial.print(pin);
            Serial.println(" SET LOW");
        }
    }
}

// changeSignal function
// this function takes a signal index and checks what type it is and then sets it to the required state
void changeSignal(int signal, int state) {
    // then check what type the signal is
    int signalType = Signals[signal][2];
    int signalState = Signals[signal][3];
    // then check if the signal is a home semaphore [0] or a distant semaphore [1]
    if(signalType == 0 || signalType == 1) {
        // then check the state of the signal to make sure it isnt the same as the requested state
        if(signalState != state) {
            // then change the signal
            setPin(Signals[signal][4], true);
            // then delay for the RelayHoldTime
            delay(RelayHoldTime);
            setPin(Signals[signal][4], false);
        }
    }
    // then check if the signal is a colour light signal
    else if(signalType == 2 || signalType == 3 || signalType == 4 || signalType == 5 || signalType == 6) {
        // then check that the state is different so we are not wasting time
        if(signalState != state) {
            // then turn the signal to the requested state
            // danger [0]
            if(state == 0) {
                // then turn the danger pin on
                setPin(Signals[signal][5], true);
            }
            // warning [1]
            else if(state == 1) {
                // then turn the warning pin on
                setPin(Signals[signal][6], true);
            }
            // advance warning [2]
            else if(state == 2) {
                // then turn the warning pin on
                setPin(Signals[signal][6], true);
                // then turn the advance warning pin on
                setPin(Signals[signal][7], true);
            }
            // pass [3]
            else if(state == 3) {
                // then turn the pass pin on
                setPin(Signals[signal][8], true);
            }

            // then check what state the signal is currently set to and turn that aspect off
            // is currently danger [0]
            if(signalState == 0) {
                // then turn the danger aspect off
                setPin(Signals[signal][5], false);
            }
            // is currently warning [1]
            else if(signalState == 1) {
                // then turn the warning aspect off
                setPin(Signals[signal][6], false);
            }
            // is currently advance warning [2]
            else if(signalState == 2) {
                // then turn the warning aspect off
                setPin(Signals[signal][6], false);
                // then turn the advance warning aspect off
                setPin(Signals[signal][7], false);
            }
            // is currently pass [3]
            else if(signalState == 3) {
                // then turn the pass aspect off
                setPin(Signals[signal][8], false);
            }
        }
    }

    // Then update the signal state as it has now been changed
    Signals[signal][3] = state;

}



void setup() {
    // then start the programming and debugging serial port
    Serial.begin(9600);
    // then start the expansion board serial port at a higher data rate
    Serial1.begin(115200);

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

        // then initialise the signal as danger
        changeSignal(i, 0);
    }

};

void loop() {
    // this part of the code continually sets the states of the IsSensorActivated and IsBlockOccupied
    for(int i = 0; i < sizeof(Blocks)/sizeof(Blocks[0]); i++) {
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


        // then print out if the block is occupied and if the sensor is triggered
        Serial.print("BLOCK: ");
        Serial.print(i);
        Serial.print(" OCCUPIED=");
        Serial.print(Blocks[i][4]);
        Serial.print( " SENSOR=");
        Serial.println(Blocks[i][3]);
    }
    
    // this part of the code executes the signal instructions
    // 1 - go through the Signals array
    for(int i = 0; i < sizeof(Signals)/sizeof(Signals[0]); i++) {
        // 2 - then get the signal instruction start at index and the stop after index
        int startAt = Signals[i][9];
        int stopAfter = Signals[i][10];
        // 3 - then go through each signal instruction
        for(int j = startAt; j <= stopAfter; j++) {
            // 4 - then check what type of instruction it is
            int instructionType = SignalInstructions[j][3];
            int i1d1 = SignalInstructions[j][4];
            int i1d2 = SignalInstructions[j][5];
            int i2d1 = SignalInstructions[j][6];
            int i2d2 = SignalInstructions[j][7];
            int i3d1 = SignalInstructions[j][8];
            int i3d2 = SignalInstructions[j][9];
            int stateToSet = SignalInstructions[j][2];
            int instructionBooleanOperator = SignalInstructions[j][10];
            if(instructionType == 0) {
                // No Instruction
                // do nothing
            }
            else if(instructionType == 1) {
                // Check And Change Signal (1 Check Instruction)
                // then take the instruction in i1d1 and i1d2 and
                // check the state of the blocks index in i1d1 occupied state against i1d2
                if(Blocks[i1d1][4] == i1d2) {
                    // then the statement is true so change the signal
                    changeSignal(i, stateToSet);
                }
            }
            else if(instructionType == 2) {
                // Check And Change Signal (2 Check Instructions)
                // then do the same as above but instead store the result in a bool value
                // instruction 1
                bool i1Result = Blocks[i1d1][4] == i1d2;
                // instruction 2
                bool i2Result = Blocks[i2d1][4] == i2d2;
                // then do the logic
                bool combinedResult = false;
                if(instructionBooleanOperator == 1) {
                    // then this is the AND operator so AND the results
                    combinedResult = i1Result && i2Result;
                }
                else if(instructionBooleanOperator == 2) {
                    // then this is the OR operator so OR the results
                    combinedResult = i1Result || i2Result;
                }
                // then check whether to change the signal
                if(combinedResult == true) {
                    // then change the signal
                    changeSignal(i, stateToSet);
                }
            }
            else if(instructionType == 3) {
                // Check And Change Signal (3 Check Instructions)
                // then do the same as above but instead store the result in a bool value
                // instruction 1
                bool i1Result = Blocks[i1d1][4] == i1d2;
                // instruction 2
                bool i2Result = Blocks[i2d1][4] == i2d2;
                // instruction 3
                bool i3Result = Blocks[i3d1][4] == i3d2;
                // then do the logic
                bool combinedResult = false;
                if(instructionBooleanOperator == 1) {
                    // then this is the AND operator so AND the results
                    combinedResult = i1Result && i2Result && i3Result;
                }
                else if(instructionBooleanOperator == 2) {
                    // then this is the OR operator so OR the results
                    combinedResult = i1Result || i2Result || i3Result;
                }
                // then check whether to change the signal
                if(combinedResult == true) {
                    // then change the signal
                    changeSignal(i, stateToSet);
                }
            }
            else if(instructionType == 4) {
                // Change Signals To Pass
                // 1 - then check how many blocks behind are to be set clear behind
                if(i1d1 == 1) {
                    // then set the block at i1d2 to clear
                    Blocks[i1d2][4] = 0;
                }
                else if(i1d1 == 2) {
                    // then set the block at i1d2 to clear
                    Blocks[i1d2][4] = 0;
                    // then set the block at i2d1 to clear
                    Blocks[i2d1][4] = 0;
                }
                else if(i1d1 == 3) {
                    // then set the block at i1d2 to clear
                    Blocks[i1d2][4] = 0;
                    // then set the block at i2d1 to clear
                    Blocks[i2d1][4] = 0;
                    // then set the block at i2d2 to clear
                    Blocks[i2d2][4] = 0;
                }
                else if(i1d1 == 4) {
                    // then set the block at i1d2 to clear
                    Blocks[i1d2][4] = 0;
                    // then set the block at i2d1 to clear
                    Blocks[i2d1][4] = 0;
                    // then set the block at i2d2 to clear
                    Blocks[i2d2][4] = 0;
                    // then set the block at i3d1 to clear
                    Blocks[i3d1][4] = 0;
                }
                else if(i1d1 == 5) {
                    // then set the block at i1d2 to clear
                    Blocks[i1d2][4] = 0;
                    // then set the block at i2d1 to clear
                    Blocks[i2d1][4] = 0;
                    // then set the block at i2d2 to clear
                    Blocks[i2d2][4] = 0;
                    // then set the block at i3d1 to clear
                    Blocks[i3d1][4] = 0;
                    // then set the block at i3d2 to clear
                    Blocks[i3d2][4] = 0;
                }
            }
        }
    }

    delay(10);

};
