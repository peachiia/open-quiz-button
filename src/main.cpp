#include <Arduino.h>

#define STATIC_TIMER_UPDATE timer += (unsigned long) period;               // Reschedule for next calling
#define STATIC_TIMER_INIT static unsigned long timer = millis()  // Schedule Variable Declaration
#define STATIC_TIMER_CHECK (millis() >= timer)                   // Schedule Checking

void setup() {
    // put your setup code here, to run once:
}

void loop() {
    // put your main code here, to run repeatedly:
}

void task_input_update(int period)
{
    STATIC_TIMER_INIT;
    if (STATIC_TIMER_CHECK)
    {
        // execute

        STATIC_TIMER_UPDATE;
    }
}