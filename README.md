## This software is **NOT** intended to be fully scalable without major code changes

# **Block Signalling V4**

> This Replicates Simple (Non route indicating) UK Block Signalling On A Signal By Signal Basis

### The program checks each signal's instructions and then checks this from the blocks instead of the blocks changing the signals. Blocks can still change eachother but mainly for the purpose of checking if a block can be considered clear or not

## **Signal Types**

> These are the number representation of signal types that a signal can be

0. Home Semaphore
1. Distant Semaphore
2. 2 Aspect Colour Light (Danger & Pass)
3. 2 Aspect Colour Light (Danger & Warning)
4. 2 Aspect Colour Light (Warning & Pass)
5. 3 Aspect Colour Light
6. 4 Aspect Colour Light

## **Signal States**

> These are the number representations of signal states that a signal can be

0. Danger
1. Warning
2. Advanced Warning
3. Pass

## **LinesList**

>These are the loops and branches of track that the block is situated on, Contains a simple array of line indexes
**e.g. [1,2,3,4] for 4 separate lines**


## **Block**

>The Block array contains a nested array of information about the block

0. Block ID
1. Sensor Pin
2. Sensor High On Activation
3. Is Sensor Activated
4. Is Block Occupied
5. Block ID Behind
6. The Line That The Block Is On


### **Block ID [0]** - This is an **INT** type at index **0** stating what ID number the block is, This must always match it's array index position

### **Sensor Pin [1]** - This is an **INT** type at index **1** stating what digital pin the sensor is connected to

### **Sensor High On Activation [2]** - This is a **BOOL** type stored as an **INT** at index **2** stating whether the logic level HIGH represents a sensor that is detecting something or a sensor that isnt detecting anything

### **Is Sensor Activated [3]** - This is a **BOOL** type stored as an **INT** at index **3** stating whether the sensor is detecting anyting **(TRUE/1)** or not **(FALSE/0)**

### **Is Block Occupied [4]** - This is a **BOOL** type stored as an **INT** at index **4** stating if the sensor has detected anything, this can only be reset when the block infront's sensor is **NOT Activated**

### **Block ID Behind [5]** - This is an **INT** type at index **5** stating what id the block behind is so that it can be cleared when this block's sensor stops being active

### **The Line That The Block Is On [6]** - This is an **INT** type at index **6** stating the index in the LinesList of which line this block is on, This is used so that it can be cleared and so that it can be ignored with the ignore button

## **SensorLastTriggeredTime**

>This array contains a variable that is set to the millis() when the sensor is considered occupied. Then when the sensor becomes clear and is checked again it can be checked if a predetermined time has passed to state that the sensor is actually clear (effectively a long debounce on the sensor). And if the amount of time has been passed then the block behind can be considered clear

In the check care must be taken to ensure that the last time the sensor was triggered is greater than the time that the sensor behind was triggered whilst also checking for the SensorDebounceTime


**The last time that your sensor was high needs to be newer than the time that the block you are resetting's sensor was high**


## **Signal**

>The Signal array contains a nested array of information about each signal and what block it belongs to and what it's instructions are

0. Signal ID
1. Block ID (that this signal belongs to)
2. Signal Type
3. Signal State
4. Signal Change Pin (Semaphores Only)
5. Signal Danger Pin
6. Signal Warning Pin
7. Signal Advance Warning Pin
8. Signal Pass Pin
9. Signal Instructions Start Index
10. Signal Instructions Stop After Index

### **Signal ID [0]** - This is an **INT** type at index **0** stating what ID number the signal is, This must always match it's array index position

### **Block ID [1]** - This is an **INT** type at index **1** stating what Block ID this signal will have an effect on 

### **Signal Type [2]** - This is an **INT** type at index **2** stating what type of signal it is from the list

### **Signal State [3]** - This is an **INT** type at index **3** stating what state the signal is currently at from the list

### **Signal Change Pin [4]** - This is an **INT** type at index **4** stating what pin is used to change the signal if it is a semaphore

### **Signal Danger Pin [5]** - This is an **INT** type at index **5** stating what pin is used to turn the danger aspect of the signal on

### **Signal Warning Pin [6]** - This is an **INT** type at index **6** stating what pin is used to turn the warning aspect of the signal on

### **Signal Advance Warning Pin [7]** - This is an **INT** type at index **7** stating what pin is used to turn the advanced warning aspect of the signal on

### **Signal Pass Pin [8]** - This is an **INT** Type at index **8** stating what pin is used to turn the pass aspect of the signal on

### **Signal Instructions Start Index [9]** - This is an **INT** type at index **9** stating what index in the SignalInstructions array to start at 

### **Signal Instructions Stop After Index [10]** - This is an **INT** type at index **10** stating what is the last index in the SignalInstructions array that will be done on this signal


## **SignalInstructions**

> This nested array will contain each instruction (case) that will be checked repetadely in the loop

0. Instruction ID
1. Signal ID
2. StateToSet
3. InstructionIndexInSignalInstuctionsList
4. Instruction1Data1
5. Instruction1Data2
6. Instruction2Data1
7. Instruction2Data2
8. Instruction3Data1
9. Instruction3Data2
10. InstructionBooleanOperator

## **SignalInstructionsList**

### InstructionType

0. No Instruction
1. Check And Change Signal (1 Check Instruction)
2. Check And Change Signal (2 Check Instructions)
3. Check And Change Signal (3 Check Instructions)
4. Change Signals To Pass

### **No Instruction [0]** - this **INT** type will state that the instruction will do nothing

### **Check And Change Signal [1]** - this **INT** type states that the instruction will take the **Instruction1Data1** and find the **Block ID** with this data and then check it's occupied status against the value in **Instruction1Data2** and if they are equal then it will change the signal state to the **StateToSet** value

### **Check And Change Signal (2 Check Instructions) [2]** - this **INT** type states that the instruction will take the **Instruction1Data1** and find the **Block ID** with this data and then check it's occupied status against the value in **Instruction1Data2** and save this result. Then it will do the same for the values in **Instruction2Data1** and **Instruction2Data2** and than take these results and then use the **InstructionBooleanOperator** to compare them, if the result returns true then the signal will be changed to the **StateToSet** value

### **Check And Change Signal (3 Check Instructions) [3]** - this **INT** type is exactly the same as the above but with another instruction totalling 3. The boolean operator applies to the result of all these instructions

### **Change Signals To Pass [4]** this takes the Block Id from the 5 available instructions and then sets them to clear, the first instruction data 1 is used to say how many of the 5 blocks behind are to be cleared

## Boolean Operators List

0. No Operator
1. AND
2. OR

## Control Pushbuttons

> Control Buttons are provided to clear a block if pressed and if pressed and held it can allow wrong way running which sets all the signals to red and blocks the sensors so that trains can run in the opposite direction
> Control buttons can also be set to clear the blocks

SetPinMode() 
0 = INPUT_PULLUP
1 = INPUT
2 = OUTPUT
