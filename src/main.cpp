#include <Arduino.h>

#define STATIC_TIMER_UPDATE timer += (unsigned long) period;      // Reschedule for next calling
#define STATIC_TIMER_INIT static unsigned long timer = millis();  // Schedule Variable Declaration
#define STATIC_TIMER_CHECK (millis() >= timer)                    // Schedule Checking

#define TEAMIO_TASK_PERIOD 5
void init_teamio();
void task_teamio_update(int period);

#define BOXIO_TASK_PERIOD 50
void init_boxio();
void task_boxio_update(int period);


#define TEAM_MAX 6
int teamInput_threshold = 512;

bool teamInput_uniqueActiveFlag = false;
int teamInput_uniqueActiveID = 0;

bool teamInput_isActive[TEAM_MAX];
bool teamOutput_isActive[TEAM_MAX];
int teamInput_Pin[] = {A0, A1, A2, A3, A4, A5};
int teamOutput_Pin[] = {3, 4, 5, 6, 7, 8};




bool boxButton_isActive; // Current State
bool boxButton_isHold;   // Holding State

bool boxButton_longPressedFlag;             // Raised if hold > threshold
bool boxButton_longPressedCounter = 0;
bool boxButton_longPressedThreshold = 2000 / BOXIO_TASK_PERIOD; // 2000 ms

bool boxBuzzer_isActive;
bool boxLed_isActive;

int boxButton_Pin = 0;
int boxBuzzer_Pin = 0;
int boxLed_Pin = 0;



void setup() 
{
    init_boxio();
    init_teamio();
}

void loop() 
{ 
    task_teamio_update(5);
    task_boxio_update(50);
}



void init_teamio()
{
    // Inputs don't have to init because it's analogPin
    for (int i = 0; i < TEAM_MAX; i++) {
        pinMode(teamOutput_Pin[i], OUTPUT);
    }
}


void task_teamio_update(int period)
{
    STATIC_TIMER_INIT;
    if (STATIC_TIMER_CHECK)
    {
        bool activeFlag = false;
        int activeID = -1;
        
        for (int i = 0; i < TEAM_MAX; i++) {
            teamInput_isActive[i] = analogRead(teamInput_Pin[i]) > teamInput_threshold;
            if (teamInput_isActive[i]) 
            {
                if (activeFlag == false) {
                    activeFlag = true;
                    activeID = i;
                }
                else {
                    if (random(99) % 2) {
                        activeID = i;
                    }
                }
            }
        }

        if (teamInput_uniqueActiveFlag == false) {
            teamInput_uniqueActiveFlag = activeFlag;
            teamInput_uniqueActiveID = activeID;
        }

        STATIC_TIMER_UPDATE;
    }
}


void init_boxio()
{
    pinMode(boxButton_Pin, INPUT);
    pinMode(boxBuzzer_Pin, OUTPUT);
    pinMode(boxLed_Pin, OUTPUT);
}


void task_boxio_update(int period)
{
    STATIC_TIMER_INIT;
    if (STATIC_TIMER_CHECK)
    {
        boxButton_isActive = digitalRead(boxButton_Pin);
        if (boxButton_isActive) {
            boxButton_longPressedCounter++;
            if (boxButton_longPressedCounter >= boxButton_longPressedThreshold) {
                boxButton_longPressedFlag = true;
            }
        }
        else {
            boxButton_longPressedCounter = 0;
        }
        digitalWrite(boxLed_Pin, boxLed_isActive);
        digitalWrite(boxBuzzer_Pin, boxBuzzer_isActive);
        STATIC_TIMER_UPDATE;
    }
}

