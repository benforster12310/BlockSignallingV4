# Block Signalling V4
> This Replicates Simple UK Block Signalling On A Signal By Signal Basis

### The program checks each signal's instructions and then checks this from the blocks instead of the blocks changing the signals. Blocks can still change eachother



## Block
>The Block array contains a nested array of information about the block

0. Block ID
1. Sensor Pin
2. Sensor High On Activation
3. Is Sensor Activated
4. Is Block Occupied
5. Block ID Behind


### **Block ID [0]** - This is an **INT** type at index **0** stating what ID number the block is, This must always match it's array index position

### **Sensor Pin [1]** - This is an **INT** type at index **1** stating what digital pin the sensor is connected to

### **Sensor High On Activation [2]** - This is a **BOOL** type stored as an **INT** at index **2** stating whether the logic level HIGH represents a sensor that is detecting something or a sensor that isnt detecting anything

### **Is Sensor Activated [3]** - This is a **BOOL** type stored as an **INT** at index **3** stating whether the sensor is detecting anyting **(TRUE/1)** or not **(FALSE/0)**

### **Is Block Occupied [4]** - This is a **BOOL** type stored as an **INT** at index **4** stating if the sensor has detected anything, this can only be reset when the block infront's sensor is **NOT Activated**

### **Block ID Behind [5]** - This is an **INT** type at index **5** stating what id the block behind is so that it can be cleared when this block's sensor stops being active

## Signal
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
10. Signal Instructions Stop Index

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

### **Signal Instructions Stop Index [10]** - This is an **INT** type at index **10** stating what is the last index in the SignalInstructions array that will be done on this signal