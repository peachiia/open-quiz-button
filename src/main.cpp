#include <Arduino.h>

#define STATIC_TIMER_UPDATE timer += (unsigned long) period;      // Reschedule for next calling
#define STATIC_TIMER_INIT static unsigned long timer = millis();  // Schedule Variable Declaration
#define STATIC_TIMER_CHECK (millis() >= timer)                    // Schedule Checking

#define TEAM_MAX 6
int teamInput_threshold = 512;

bool teamInput_uniqueActiveFlag = false;
int teamInput_uniqueActiveID = 0;

bool teamInput_isActive[TEAM_MAX];
bool teamOutput_isActive[TEAM_MAX];
int teamInput_Pin[] = {A0, A1, A2, A3, A4, A5, A6};
int teamOutput_Pin[] = {3, 4, 5, 6, 7, 8};



bool boxButton_isActive;
bool boxBuzzer_isActive;
bool boxLed_isActive;

void init_teamio();
void task_teamio_update(int period);



void setup() 
{
    init_teamio();
}

void loop() 
{ 
    task_teamio_update(5);
}



void init_teamio()
{
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
                if (activeFlag == false && activeID == -1) {
                    activeFlag = true;
                    activeID = i;
                }
                else {
                    activeFlag = false;
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

