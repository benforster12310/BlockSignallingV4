// BlockSignallingV4 PrimaryProcess Written In June 2023 By Ben Forster

// First, Define some things

#include "ArduinoJson.h"
#include "ArduinoTimer.h"

// Sensor Debounce Time - how long a sensor must be clear until the block behind it is cleared
unsigned long SensorDebounceTime = 4000;

// RelayHoldTime - this is passed to the delay() function so it is blocking.
// used to turn the relay on and off. Should be kept as low as possible (milliseconds)
const int RelayHoldTime = 100;

// then define the RedTrackResetButtonPin, BlueTrackResetButtonPin, RedTrackWrongWayRunningLedPin, BlueTrackWrongWayRunningLedPin, RedTrackTrainInCarriageSidingButtonPin, RedTrackTrainInBayPlatformButtonPin
const int RTResetButtonPin = 2;
const int RTWrongWayRunningLedPin = 3;
const int BTResetButtonPin = 4;
const int BTWrongWayRunningLedPin = 5;
const int RTTrainInCarriageSidingButtonPin = 6;
const int RTTrainInBayPlatformButtonPin = 7;

bool RTResetButtonPressed = false;
bool BTResetButtonPressed = false;

unsigned long RTResetButtonPressedTime = 0;
unsigned long BTResetButtonPressedTime = 0;

bool RTAllowWrongWayRunning = false;
bool BTAllowWrongWayRunning = false; 

// Lines List - Track - See Documentation
const int LinesList[4] = {1,2,3,4};

int ignoredLines[4] = {0,0,0,0};

// Blocks Array
int Blocks[20][7] = {
    // When Inputting The Sensor Pin, Make Sure To Use The Sensor Before's Pin
  // 0  1  2  3  4  5  6
    {0, 0, 0, 0, 0, 0, 0},// placeholder
    {1, 0, 0, 0, 0, 0, 0},// placeholder
    {2, 0, 0, 0, 0, 0, 0},// placeholder
    {3, 0, 0, 0, 0, 0, 0},// placeholder
    {4, 0, 0, 0, 0, 0, 0},// placeholder
    {5, 0, 0, 0, 0, 0, 0},// placeholder
    {6, 61, 0, 0, 0, 11, 2}, // REDTRACKTOPLINE
    {7, 63, 0, 0, 0, 6, 2},  // REDTRACKTOPLINE
    {8, 62, 0, 0, 0, 7, 2},  // REDTRACKTOPLINE
    {9, 45, 0, 0, 0, 8, 2},  // REDTRACKTOPLINE
    {10, 44, 0, 0, 0, 9, 2}, // REDTRACKTOPLINE
    {11, 53, 0, 0, 0, 10, 2},// REDTRACKTOPLINE
    {12, 42, 0, 0, 0, 14, 3},// BLUETRACKDOWNSCENICLINE
    {13, 64, 0, 0, 0, 12, 3},// BLUETRACKDOWNSCENICLINE
    {14, 50, 0, 0, 0, 13, 3},// BLUETRACKDOWNSCENICLINE
    {15, 65, 0, 0, 0, 17, 4},// REDTRACKUPSCENICLINE
    {16, 43, 0, 0, 0, 15, 4},// REDTRACKUPSCENICLINE
    {17, 52, 0, 0, 0, 16, 4},// REDTRACKUPSCENICLINE
    {18, 0, 0, 0, 0, 0, 0},// placeholder
    {19, 0, 0, 0, 0, 0, 0}//placeholder
};

// SensorLastTriggeredTime
unsigned long SensorLastTriggeredTime[20] = {};

// Signals Array
int Signals[20][11] = {
  // 0  1  2  3  4   5   6  7   8  9  10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    {6, 6, 5, 0, 0, 165, 164, 0, 163, 18, 20}, // RTTOPLINE
    {7, 7, 5, 0, 0, 162, 161, 0, 160, 21, 23}, // RTTOPLINE
    {8, 8, 5, 0, 0, 137, 139, 0, 138, 24, 26}, // RTTOPLINE
    {9, 9, 5, 0, 0, 132, 127, 0, 131, 27, 29}, // RTTOPLINE
    {10, 10, 5, 0, 0, 128, 130, 0, 129, 30, 32}, // RTTOPLINE
    {11, 11, 5, 0, 0, 159, 158, 0, 156, 33, 35}, // RTTOPLINE
    {12, 12, 5, 0, 0, 168, 167, 0, 166, 36, 38}, // BTDOWNSCENICLINE
    {13, 13, 5, 0, 0, 141, 140, 0, 142, 39, 41}, // BTDOWNSCENICLINE
    {14, 14, 5, 0, 0, 146, 147, 0, 148, 42, 44}, // BTDOWNSCENICLINE
    {15, 15, 5, 0, 0, 149, 150, 0, 151, 45, 47}, // RTUPSCENICLINE
    {16, 16, 5, 0, 0, 143, 144, 0, 145, 48, 50}, // RTUPSCENICLINE
    {17, 17, 5, 0, 0, 152, 153, 0, 169, 51, 53}, // RTUPSCENICLINE
    {18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    {19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},// placeholder
    
};

// Signal Instructions Array
const int SignalInstructions[60][11] = {
  // 0  1  2  3  4  5  6  7  8  9  10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {18, 6, 3, 2, 7, 0, 8, 0, 0, 0, 1}, // signal 6 pass when block infront1 clear and block infront2 clear
    {19, 6, 1, 2, 7, 0, 8, 1, 0, 0, 1}, // signal 6 warning when block infront1 clear and block infront2 occupied
    {20, 6, 0, 1, 7, 1, 0, 0, 0, 0, 0}, // signal 6 danger when block infront1 occupied
    {21, 7, 3, 2, 8, 0, 9, 0, 0, 0, 1}, // signal 7 pass when block infront1 clear and block infront2 clear
    {22, 7, 1, 2, 8, 0, 9, 1, 0, 0, 1}, // signal 7 warning when block infront1 clear and block infront2 occupied
    {23, 7, 0, 1, 8, 1, 0, 0, 0, 0, 0}, // signal 7 danger when block infront1 occupied
    {24, 8, 3, 2, 9, 0, 10, 0, 0, 0, 1}, // signal 8 pass when block infront1 clear and block infront2 clear
    {25, 8, 1, 2, 9, 0, 10, 1, 0, 0, 1}, // signal 8 warning when block infront1 clear and block infront2 occupied
    {26, 8, 0, 1, 9, 1, 0, 0, 0, 0, 0}, // signal 8 danger when block infront1 occupied
    {27, 9, 3, 2, 10, 0, 11, 0, 0, 0, 1}, // signal 9 pass when block infront1 clear and block infront2 clear
    {28, 9, 1, 2, 10, 0, 11, 1, 0, 0, 1}, // signal 9 warning when block infront1 clear and block infront2 occupied
    {29, 9, 0, 1, 10, 1, 0, 0, 0, 0, 0}, // signal 9 danger when block infront1 occupied
    {30, 10, 3, 2, 11, 0, 6, 0, 0, 0, 1}, // signal 10 pass when block infront1 clear and block infront2 clear
    {31, 10, 1, 2, 11, 0, 6, 1, 0, 0, 1}, // signal 10 warning when block infront1 clear and block infront2 occupied
    {32, 10, 0, 1, 11, 1, 0, 0, 0, 0, 0}, // signal 10 danger when block infront1 occupied
    {33, 11, 3, 2, 6, 0, 7, 0, 0, 0, 1}, // signal 11 pass when block infront1 clear and block infront2 clear
    {34, 11, 1, 2, 6, 0, 7, 1, 0, 0, 1}, // signal 11 warning when block infront1 clear and block infront2 occupied
    {35, 11, 0, 1, 6, 1, 0, 0, 0, 0, 0}, // signal 11 danger when block infront1 occupied
    {36, 12, 3, 2, 13, 0, 14, 0, 0, 0, 1}, // signal 12 pass when block infront1 clear and block infront2 clear
    {37, 12, 1, 2, 13, 0, 14, 1, 0, 0, 1}, // signal 12 warning when block infront1 clear and block infront2 occupied
    {38, 12, 0, 1, 13, 1, 0, 0, 0, 0, 0}, // signal 12 danger when block infront1 occupied
    {39, 13, 3, 2, 14, 0, 12, 0, 0, 0, 1}, // signal 13 pass when block infront1 clear and block infront2 clear
    {40, 13, 1, 2, 14, 0, 12, 1, 0, 0, 1}, // signal 13 warning when block infront1 clear and block infront2 occupied
    {41, 13, 0, 1, 14, 1, 0, 0, 0, 0, 0}, // signal 13 danger when block infront1 occupied
    {42, 14, 3, 2, 12, 0, 13, 0, 0, 0, 1}, // signal 14 pass when block infront1 clear and block infront2 clear
    {43, 14, 1, 2, 12, 0, 13, 1, 0, 0, 1}, // signal 14 warning when block infront1 clear and block infront2 occupied
    {44, 14, 0, 1, 12, 1, 0, 0, 0, 0, 0}, // signal 14 danger when block infront1 occupied
    {45, 15, 3, 2, 16, 0, 17, 0, 0, 0, 1}, // signal 15 pass when block infront1 clear and block infront2 clear
    {46, 15, 1, 2, 16, 0, 17, 1, 0, 0, 1}, // signal 15 warning when block infront1 clear and block infront2 occupied
    {47, 15, 0, 1, 16, 1, 0, 0, 0, 0, 0}, // signal 15 danger when block infront1 occupied
    {48, 16, 3, 2, 17, 0, 15, 0, 0, 0, 1}, // signal 16 pass when block infront1 clear and block infront2 clear
    {49, 16, 1, 2, 17, 0, 15, 1, 0, 0, 1}, // signal 16 warning when block infront1 clear and block infront2 occupied
    {50, 16, 0, 1, 17, 1, 0, 0, 0, 0, 0}, // signal 16 danger when block infront1 occupied
    {51, 17, 3, 2, 15, 0, 16, 0, 0, 0, 1}, // signal 17 pass when block infront1 clear and block infront2 clear
    {52, 17, 1, 2, 15, 0, 16, 1, 0, 0, 1}, // signal 17 warning when block infront1 clear and block infront2 occupied
    {53, 17, 0, 1, 15, 1, 0, 0, 0, 0, 0}, // signal 17 danger when block infront1 occupied
    {54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
    {59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // placeholder
};

// ################### Simple Semaphore Signals #############################
int simpleSemaphoreSignals_relayHoldTime = 250;
int simpleSemaphoreSignals_timeUntilReset = 10000;
int simpleSemaphoreSignals_resetToDangerPin = 2;
ArduinoTimer simpleSemaphoreSignals_timersArray[4];

int simpleSemaphoreSignals_semaphoresArray[4][5] = {
// 0: id, 1: semaphoreChangePin, 2: semaphoreSensorPin, 3: semaphoreState, 4: timerStarted
  // 0  1  2  3  4
    {1, 24, 67, 0, 0},
    {2, 35, 66, 0, 0},
    {3, 30, 69, 0, 0},
    {4, 29, 68, 0, 0}
};

int simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin = 28;
int simpleSemaphoreSignals_pointActivatedSemaphorePassInputPin = 17;
int simpleSemaphoreSignals_pointActivatedSemaphoreSignalState = 0;

// ##################### END Simple Semaphore Signals ########################

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
        Serial.println("Set Pin Mode");
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
            // then turn all the pins off
            setPin(Signals[i][5], 0);
            setPin(Signals[i][8], 0);
        }
        // signal type 2 Aspect Colour Light Signal Danger & Warning [3]
        else if(Signals[i][2] == 3) {
            // then set the SignalDangerPin [5] and SignalWarningPin [6]
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][6], 2);
            // then turn all the pins of
            setPin(Signals[i][5], 0);
            setPin(Signals[i][6], 0);
        }
        // signal type 2 Aspect Colour Light Signal Warning & Pass [4]
        else if(Signals[i][2] == 4) {
            // then set the SignalWarningPin [6] and SignalPassPin [8]
            setPinMode(Signals[i][6], 2);
            setPinMode(Signals[i][8], 2);
            // then turn all the pins off
            setPin(Signals[i][6], 0);
            setPin(Signals[i][8], 0);
        } 
        // signal type 3 Aspect Colour Light Signal [5]
        else if(Signals[i][2] == 5) {
            // then set the SignalDangerPin [5] and SignalWarningPin [6] and SignalPassPin [8]
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][6], 2);
            setPinMode(Signals[i][8], 2);
            // then turn all the pins off
            setPin(Signals[i][5], 0);
            setPin(Signals[i][6], 0);
            setPin(Signals[i][8], 0);
        }
        // signal type 4 Aspect Colour Light Signal [5]
        else if(Signals[i][2] == 6) {
            // then set the SignalDangerPin [5] and SignalWarningPin [6] and Signal Advanced Warning Pin [7]
            // and SignalPassPin [8]
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][6], 2);
            setPinMode(Signals[i][7], 2);
            setPinMode(Signals[i][8], 2);
            // then turn all the pins off
            setPin(Signals[i][5], 0);
            setPin(Signals[i][6], 0);
            setPin(Signals[i][7], 0);
            setPin(Signals[i][8], 0);
        }

        // then initialise the signal as danger
        changeSignal(i, 0);
    }


    // ##################### Simple Semaphore Signals #########################

    // Go through and set the pin modes of the signal change pins 

    for(int i = 0; i < sizeof(simpleSemaphoreSignals_semaphoresArray)/sizeof(simpleSemaphoreSignals_semaphoresArray[0]); i++) {
        // then set the sensor pin as INPUT_PULLUP
        pinMode(simpleSemaphoreSignals_semaphoresArray[i][2], INPUT_PULLUP);
        // then set the signalChangePin as OUTPUT
        pinMode(simpleSemaphoreSignals_semaphoresArray[i][1], OUTPUT);
    }

    // then set the reset to danger pin to INPUT_PULLUP
    pinMode(simpleSemaphoreSignals_resetToDangerPin, INPUT_PULLUP);

    // then set the pointActivatedSemaphore relay to output
    pinMode(simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin, OUTPUT);

    // then set the pointActivatedSemaphore pass input to INPUT_PULLUP
    pinMode(simpleSemaphoreSignals_pointActivatedSemaphorePassInputPin, INPUT_PULLUP);

    // ##################### End Of Simple Semaphore Signals ##################

    // set the control panel button inputs and outputs
    pinMode(RTResetButtonPin, INPUT_PULLUP);
    pinMode(RTWrongWayRunningLedPin, OUTPUT);
    pinMode(BTResetButtonPin, INPUT_PULLUP);
    pinMode(BTWrongWayRunningLedPin, OUTPUT);
    pinMode(RTTrainInBayPlatformButtonPin, INPUT_PULLUP);
    pinMode(RTTrainInCarriageSidingButtonPin, INPUT_PULLUP);

};

void loop() {
    // this part of the code continually sets the states of the IsSensorActivated and IsBlockOccupied
    for(int i = 0; i < sizeof(Blocks)/sizeof(Blocks[0]); i++) {
        // check what lines are being ignored 
        bool ignoreBlock = false;
        for(int j = 0; j < sizeof(ignoredLines); j++) {
            // then check if the line that the block is on is being ignored
            if(Blocks[i][6] == ignoredLines[j]) {
                // set ignoreBlock to true and break from the loop
                // SET TO FALSE TEMPORARY
                //ignoreBlock = true;
                ignoreBlock = false;
                //Serial.println("Ignoring Line");
                break;
            }
        }
        // then check if the block was ignored 
        if(ignoreBlock == true) {
            // then set the block to be clear and break from the loop
            Blocks[i][3] = 0;
            Blocks[i][4] = 0;
            break;
        }
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
                // Change Blocks To Clear
                // NEEDS CHANGING TO BE ON A BLOCK INSTEAD OF ON A SIGNAL
            }
        }
    }


    // ################### Simple Semaphore Signals ######################

    // then go through each signal
    for(int i = 0; i < sizeof(simpleSemaphoreSignals_semaphoresArray)/sizeof(simpleSemaphoreSignals_semaphoresArray[0]); i++) {
        //Serial.print("Doing Semaphore: ");
        //Serial.println(i);
        // then check if the sensor before the signal has been activated
        if(digitalRead(simpleSemaphoreSignals_semaphoresArray[i][2]) == LOW) {
            //Serial.println("Sensor Activated");
            // then check if the signal is set to danger
            if(simpleSemaphoreSignals_semaphoresArray[i][3] == 0) {
              //Serial.println("Semaphore Set To Danger");
                // then it is set to danger so change it to pass
                digitalWrite(simpleSemaphoreSignals_semaphoresArray[i][1], HIGH);
                // set the state to pass
                simpleSemaphoreSignals_semaphoresArray[i][3] = 1;
                delay(simpleSemaphoreSignals_relayHoldTime);
                digitalWrite(simpleSemaphoreSignals_semaphoresArray[i][1], LOW);
            }
            else {
                // if the signal is then set to pass then restart the timer
                //Serial.println("Semaphore Set To Pass, Restarting Timer");
                simpleSemaphoreSignals_timersArray[i].Reset();
            }
        }
        else {
            // then make sure that the signal is at pass
            //Serial.println("Sensor Not Activated");
            if(simpleSemaphoreSignals_semaphoresArray[i][3] == 1) {
              //Serial.println("Semaphore At PASS");
                // then check if the timer has been started and if it hasnt then start it
                if(simpleSemaphoreSignals_semaphoresArray[i][4] == 0) {
                    //Serial.println("Timer Not Started, Starting Timer");
                    // then start the timer
                    simpleSemaphoreSignals_timersArray[i].Reset();
                    // then set the timer as started
                    simpleSemaphoreSignals_semaphoresArray[i][4] = 1;
                }
                // then check if the required time has passed and check if the timer has started
                if(simpleSemaphoreSignals_timersArray[i].TimePassed_Milliseconds(simpleSemaphoreSignals_timeUntilReset) && simpleSemaphoreSignals_semaphoresArray[i][4] == 1) {
                    //Serial.println("Timer Finnished And Timer Was Started");
                    // then the time has passed so set the signal to danger
                    digitalWrite(simpleSemaphoreSignals_semaphoresArray[i][1], HIGH);
                    // set the state to danger
                    simpleSemaphoreSignals_semaphoresArray[i][3] = 0;
                    delay(simpleSemaphoreSignals_relayHoldTime);
                    // then set the timer as stopped
                    simpleSemaphoreSignals_semaphoresArray[i][4] = 0;
                    //Serial.println("Set State To Danger And Timer Started To False");
                    digitalWrite(simpleSemaphoreSignals_semaphoresArray[i][1], LOW);
                }
            }
        }
    }


    // then do the pointActivatedSemaphores
    // then check the state of the pass input
    if(digitalRead(simpleSemaphoreSignals_pointActivatedSemaphorePassInputPin) == LOW) {
        // then make sure that it is actually set correctly
        // check if the timer has been started
        // then delay 10ms to make sure it is correct
        delay(10);
        if(digitalRead(simpleSemaphoreSignals_pointActivatedSemaphorePassInputPin) == LOW) {
            Serial.println("Point Set To Pass");
            // then the signal should be set to pass so check if it is not and that 3 seconds has passed since it was last moved
            if(simpleSemaphoreSignals_pointActivatedSemaphoreSignalState != 1) {
                // then change the signal
                Serial.println("Signal Changing To Pass");
                digitalWrite(simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin, HIGH);
                // change the signal state
                simpleSemaphoreSignals_pointActivatedSemaphoreSignalState = 1;
                delay(simpleSemaphoreSignals_relayHoldTime);
                digitalWrite(simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin, LOW);
            }
        }
    }
    else {
        // then if the signal is not activated it can be assumed that the signal should be at danger
        // then wait a period of time to check that the signal is still correct
        delay(10);
        if(digitalRead(simpleSemaphoreSignals_pointActivatedSemaphorePassInputPin) == HIGH) {
            // then the signal needs changing to danger
            if(simpleSemaphoreSignals_pointActivatedSemaphoreSignalState != 0) {
                // then change the signal
                Serial.println("Signal Changing To Danger");
                digitalWrite(simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin, HIGH);
                // change the signal state
                simpleSemaphoreSignals_pointActivatedSemaphoreSignalState = 0;
                delay(simpleSemaphoreSignals_relayHoldTime);
                digitalWrite(simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin, LOW);
            }
        }
    
    }

    // reset semaphore signals to danger
    if(digitalRead(simpleSemaphoreSignals_resetToDangerPin) == LOW) {
        // then go through the semaphore signals in the array and set them to danger
        for(int i = 0; i < sizeof(simpleSemaphoreSignals_semaphoresArray)/sizeof(simpleSemaphoreSignals_semaphoresArray[0]); i++) {
            // then set the signal to danger if it is not already
            if(simpleSemaphoreSignals_semaphoresArray[i][3] != 0) {
                // then change it and set the signal state
                digitalWrite(simpleSemaphoreSignals_semaphoresArray[i][1], HIGH);
                simpleSemaphoreSignals_semaphoresArray[i][3] = 0;
                delay(simpleSemaphoreSignals_relayHoldTime);
                digitalWrite(simpleSemaphoreSignals_semaphoresArray[i][1], LOW);
            }
        }

        if(simpleSemaphoreSignals_pointActivatedSemaphoreSignalState != 0) {
            // then change the signal
            digitalWrite(simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin, HIGH);
            // change the signal state
            simpleSemaphoreSignals_pointActivatedSemaphoreSignalState = 0;
            delay(simpleSemaphoreSignals_relayHoldTime);
            digitalWrite(simpleSemaphoreSignals_pointActivatedSemaphoreRelayPin, LOW);
        }
        
    }

    // ###################### End Of Simple Semaphore Signals ###############################

    // then do the pushbuttons on the main panel

    // Red Track Reset / Allow Wrong Way Running
    // check if the red track reset button has been pressed
    if(digitalRead(RTResetButtonPin) == LOW) {
        // then check if the button has not been set to pressed
        if(RTResetButtonPressed == false) {
            // then set the button to pressed
            RTResetButtonPressed = true;
            RTResetButtonPressedTime = millis();
        }
        else {
            // then check how long the button has been pressed for
            if(millis() - RTResetButtonPressedTime > 1000) {
                // then check if the RT Allow Wrong Way Running Is True
                if(RTAllowWrongWayRunning == true) {
                    // then stop allowing wrong way running
                    digitalWrite(RTWrongWayRunningLedPin, LOW);
                    ignoredLines[1] = 0;
                    RTAllowWrongWayRunning = false;
                    // then set the button pressed variable to false
                    RTResetButtonPressed = false;
                }
                else {
                    // then as it is bigger than 1 second then set the RTAllowWrongWayRunning to true
                    RTAllowWrongWayRunning = true;
                    // then put the led on
                    digitalWrite(RTWrongWayRunningLedPin, HIGH);
                    // then put the red track into the ignoredLines list
                    ignoredLines[1] = 1;
                    // then set the button pressed variable to false
                    RTResetButtonPressed = false;
                }
            }
        }

    }
    else {
        // then the button isnt pressed now so check if it was pressed before
        if(RTResetButtonPressed == true) {
            // then check when it was stopped being pressed
            if(millis() - RTResetButtonPressedTime < 1000) {
                // then it should reset the Red Track
                for(int i = 0; i < sizeof(Blocks)/sizeof(Blocks[0]); i++) {
                    // check if it is the red track
                    if(Blocks[i][6] == 1) {
                        Blocks[i][3] = 0;
                        Blocks[i][4] = 0;
                    }
                }

                // then set the RTResetButton to false
                RTResetButtonPressed = false;
            }
        }
    }

    // Blue Track Reset / Allow Wrong Way Running
    // check if the blue track reset button has been pressed
    if(digitalRead(BTResetButtonPin) == LOW) {
        // then check if the button has not been set to pressed
        if(BTResetButtonPressed == false) {
            // then set the button to pressed
            BTResetButtonPressed = true;
            BTResetButtonPressedTime = millis();
        }
        else {
            // then check how long the button has been pressed for
            if(millis() - BTResetButtonPressedTime > 1000) {
                // then check if the BT Allow Wrong Way Running Is True
                if(BTAllowWrongWayRunning == true) {
                    // then stop allowing wrong way running
                    digitalWrite(BTWrongWayRunningLedPin, LOW);
                    ignoredLines[2] = 0;
                    BTAllowWrongWayRunning = false;
                    // then set the button pressed variable to false
                    BTResetButtonPressed = false;
                }
                else {
                    // then as it is bigger than 1 second then set the BTAllowWrongWayRunning to true
                    BTAllowWrongWayRunning = true;
                    // then put the led on
                    digitalWrite(BTWrongWayRunningLedPin, HIGH);
                    // then put the red track into the ignoredLines list
                    ignoredLines[2] = 2;
                    // then set the button pressed variable to false
                    BTResetButtonPressed = false;
                }
            }
        }

    }
    else {
        // then the button isnt pressed now so check if it was pressed before
        if(BTResetButtonPressed == true) {
            // then check when it was stopped being pressed
            if(millis() - BTResetButtonPressedTime < 1000) {
                // then it should reset the Red Track
                for(int i = 0; i < sizeof(Blocks)/sizeof(Blocks[0]); i++) {
                    // check if it is the blue track
                    if(Blocks[i][6] == 2) {
                        Blocks[i][3] = 0;
                        Blocks[i][4] = 0;
                    }
                }

                // then set the BTResetButton to false
                BTResetButtonPressed = false;
            }
        }
    }

    // then do the RTTrainInCarriageSiding

    if(digitalRead(RTTrainInCarriageSidingButtonPin) == LOW) {
        // then set the 2 blocks that would be occupied to clear
        //Blocks[][3] == 0;
        //Blocks[][4] == 0;
        //Blocks[][3] == 0;
        //Blocks[][4] == 0;
    }

    // then do the RTTrainInBayPlatform
    if(digitalRead(RTTrainInBayPlatformButtonPin) == LOW) {
        // then set the 2 blocks that would be occupied to clear
        //Blocks[][3] == 0;
        //Blocks[][4] == 0;
        //Blocks[][3] == 0;
        //Blocks[][4] == 0;
    }


    //delay(10);

};
