// BlockSignallingV4 PrimaryProcess Written In June 2023 By Ben Forster

// First, Define some things

// Lines List - Track - See Documentation
int LinesList[6] = {1,2,3,4,5,6};

// Blocks Array
int Blocks[3][7] = {
  // 0  1  2  3  4  5  6
    {0, 2, 0, 0, 0, 2, 1},
    {1, 3, 0, 0, 0, 0, 1},
    {2, 4, 0, 0, 0, 1, 1}
};

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


void setup() {
    // then start the programming and debugging serial port
    Serial.begin(9600);

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
        // signal type 3 Aspect Colour Light Signal
        else if(Signals[i][2] == 5) {
            // then set the SignalDangerPin [5] and SignalWarningPin [6] and SignalPassPin [8]
            setPinMode(Signals[i][5], 2);
            setPinMode(Signals[i][6], 2);
            setPinMode(Signals[i][8], 2);
        }
        // signal type 4 Aspect Colour Light Signal
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

};