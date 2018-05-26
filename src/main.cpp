#include <Arduino.h>

#define STATIC_TIMER_UPDATE timer += (unsigned long) period;      // Reschedule for next calling
#define STATIC_TIMER_INIT static unsigned long timer = millis();  // Schedule Variable Declaration
#define STATIC_TIMER_CHECK (millis() >= timer)                    // Schedule Checking

#define TEAMIO_TASK_PERIOD 5
void init_teamio();
void task_teamio(int period);
void clearTeamIOFlag();
void printTeamIOStatus();

#define BOXIO_TASK_PERIOD 20
void init_boxio();
void task_boxio(int period);
void clearBoxIOFlag();


#define QUIZ_TASK_PERIOD 20
void init_quiz();
void task_quiz(int period);




#define TEAM_MAX 6
int teamInput_threshold = 512;

bool teamInput_uniqueActiveFlag = false;
int teamInput_uniqueActiveID = 0;

bool teamInput_isActive[TEAM_MAX];
bool teamOutput_isActive[TEAM_MAX];
int teamInput_Pin[] = {A0, A1, A2, A3, A4, A5};
int teamOutput_Pin[] = {2, 3, 4, 5, 6, 7};



bool boxButton_isActive; // Current State
bool boxButton_isHold;   // Holding State

bool boxButton_longPressedFlag;             // Raised if hold > threshold
bool boxButton_longPressedCounter = 0;
bool boxButton_longPressedThreshold = 2000 / BOXIO_TASK_PERIOD; // 2000 ms

bool boxBuzzer_isActive;
bool boxLed_isActive;

int boxButton_Pin = 10;
int boxBuzzer_Pin = 8;
int boxLed_Pin = 9;



bool quiz_displayFlag = false;

int val[TEAM_MAX];
char serialbuffer[128];



void setup() 
{
    Serial.begin(115200);
    init_boxio();
    init_teamio();
    init_quiz();
}

void loop() 
{ 
    task_teamio(TEAMIO_TASK_PERIOD);
    task_boxio(BOXIO_TASK_PERIOD);
    task_quiz(QUIZ_TASK_PERIOD);
}



void init_teamio()
{
    // Inputs don't have to init because it's analogPin
    for (int i = 0; i < TEAM_MAX; i++) {
        pinMode(teamOutput_Pin[i], OUTPUT);
    }
}


void task_teamio(int period)
{
    STATIC_TIMER_INIT;
    if (STATIC_TIMER_CHECK)
    {
        bool activeFlag = false;
        int activeID = -1;
        
        for (int i = 0; i < TEAM_MAX; i++) {
            val[i] = analogRead(teamInput_Pin[i]);
            teamInput_isActive[i] = val[i] > teamInput_threshold;
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

        for (int i = 0; i < TEAM_MAX; i++) {
            digitalWrite(teamOutput_Pin[i], teamOutput_isActive[i]);
        }

        printTeamIOStatus();

        STATIC_TIMER_UPDATE;
    }
}


void init_boxio()
{
    pinMode(boxButton_Pin, INPUT);
    // digitalWrite(boxButton_Pin, HIGH); // Internal Puul-Up Enabled
    pinMode(boxBuzzer_Pin, OUTPUT);
    pinMode(boxLed_Pin, OUTPUT);
}


void task_boxio(int period)
{
    STATIC_TIMER_INIT;
    if (STATIC_TIMER_CHECK)
    {
        // boxButton_isActive = (digitalRead(boxButton_Pin) == LOW); // For Active Low
        boxButton_isActive = (digitalRead(boxButton_Pin) == HIGH); // For Active High
        if (boxButton_isActive) {
            boxButton_longPressedCounter++;
            if (boxButton_longPressedCounter >= boxButton_longPressedThreshold) {
                boxButton_longPressedFlag = true;
            }
        }
        else {
            boxButton_longPressedCounter = 0;
        }

        boxLed_isActive = boxButton_isActive; // Led will follow the Button State

        digitalWrite(boxLed_Pin, boxLed_isActive);
        digitalWrite(boxBuzzer_Pin, boxBuzzer_isActive);

        STATIC_TIMER_UPDATE;
    }
}



void init_quiz()
{

}


void task_quiz(int period)
{
    STATIC_TIMER_INIT;
    if (STATIC_TIMER_CHECK)
    {
        if (quiz_displayFlag) {
            if (boxButton_isActive) {
                clearTeamIOFlag();
                quiz_displayFlag = false;
            }
        }
        else {
            if (teamInput_uniqueActiveFlag) {
                quiz_displayFlag = true;
                teamOutput_isActive[teamInput_uniqueActiveID] = true;
            }
        }

        boxBuzzer_isActive = quiz_displayFlag;

        STATIC_TIMER_UPDATE;
    }
}


void clearTeamIOFlag()
{
    teamInput_uniqueActiveFlag = false;
    for (int i = 0; i < TEAM_MAX; i++) {
        teamOutput_isActive[i] = false;
    }
}
 
void clearBoxIOFlag()
{
    boxButton_longPressedFlag = 0;
} 

void printTeamIOStatus()
{
    sprintf(serialbuffer, "%4d %4d %4d %4d %4d %4d\n", val[0], val[1], val[2], val[3], val[4], val[5]);
    Serial.println(serialbuffer);
}