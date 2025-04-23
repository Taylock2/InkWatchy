#include "timer.h"

#if TIME_TOOLS

bool timerRunning = false;        // Tracks if the timer is running
int64_t timerLastSecond = 0;      // Tracks the last displayed UNIX
int64_t timerStartTime = 0;         // Start time of the stopwatch (UNIX time)
int64_t timerElapsedTime = 0;       // Total elapsed time (in seconds)
int8_t timerElapsedMinutes = 0; // How many minutes should be removed from the total time

int64_t timerTime = TIME_TOOLS_DEFAULT_TIMER; //How long the timer is set to
int64_t remainingTime = TIME_TOOLS_DEFAULT_TIMER; //How long in minutes the timer has left

String formatTimer()
{
    String minutes = String(remainingTime);

    if (minutes.length() == 1)
    {
        minutes = "0" + minutes;
    }

    String output = minutes;
    return output;
}

void writeTimer(){
    setTextSize(3);
    setFont(getFont("UbuntuMono-Regular10"));
    writeTextReplaceBack(formatTimer(), 100 -30, 115, GxEPD_WHITE, GxEPD_BLACK); 
    setTextSize(1);
    dUChange = true; 
}

void adjustTimer(int minutes)
{
    int previousSegmentCount = remainingTime;
    minutes = constrain(timerTime + minutes, 0, 60) - timerTime;

    remainingTime += minutes;
    timerTime += minutes;

    int newSegmentCount = remainingTime;

    // Only update changed segments
    if (newSegmentCount > previousSegmentCount) {
        // Fill additional segments
        for (int i = previousSegmentCount; i < newSegmentCount; i++) {
            drawSingleSegment(59 - i, true); // Fill the new segments
        }
    } else if (newSegmentCount < previousSegmentCount) {
        // Clear segments no longer needed
        for (int i = newSegmentCount; i < previousSegmentCount; i++) {
            drawSingleSegment(59 - i, false); // Clear the excess segments
        }
    }

    writeTimer();
}

void initTimer()
{
    drawEmptyCircle();

    // Fill the segments
    for (int i = 0; i < timerTime; i++) {
        drawSingleSegment(59 - i, true); // Fill from the last segment down
    }

    writeTimer();
    writeImageN(TIME_TOOLS_PLAY_PAUSE_CORD, getImg("timetools/play"), GxEPD_BLACK, GxEPD_WHITE);

    resetSleepDelay(SLEEP_EVERY_MS);
    disUp(true);
}



void loopTimer()
{
    if (timerRunning)
    {
        if(remainingTime > 0)
        {
            readRTC();

            if (timerLastSecond != getUnixTime(timeRTCUTC0))
            {
                timerLastSecond = getUnixTime(timeRTCUTC0);

                timerElapsedTime += (timerLastSecond - timerStartTime);   
                // Calculate elapsed minutes
                int64_t currentMinutes = timerElapsedTime / 60;

                timerStartTime = timerLastSecond;

                if (currentMinutes > 0 & currentMinutes > timerElapsedMinutes) {
                    timerElapsedMinutes = currentMinutes;
                    remainingTime = timerTime - timerElapsedMinutes;
                    drawSingleSegment(59 - remainingTime, false); // Draw only the new segment
                    writeTimer();       
                }
            }
        }
        else 
        {
            vibrateMotor(VIBRATION_BUTTON_LONG_TIME * 10);
            delayTask(VIBRATION_BUTTON_LONG_TIME * 10);
        }
        
    }

    // Handle button presses
    switch (useButton()) {
        case Menu: {
            if (timerRunning) {
                // Pause the stopwatch
                timerRunning = false;
                writeImageN(TIME_TOOLS_PLAY_PAUSE_CORD, getImg("timetools/play"), GxEPD_BLACK, GxEPD_WHITE);
                dUChange = true;    
            } else {
                readRTC();
                // Start or resume the stopwatch
                timerStartTime = getUnixTime(timeRTCUTC0);
                timerRunning = true;
                dis->fillRect(TIME_TOOLS_PAUSED_CORD,30,30,GxEPD_BLACK);
                writeImageN(TIME_TOOLS_PLAY_PAUSE_CORD, getImg("timetools/pause"), GxEPD_BLACK, GxEPD_WHITE);
                dUChange = true; 
            }
            break;
        }
        case LongMenu: {
            // Reset the stopwatch
            exitTimer();
            initTimer();
            break;
        }
        case Up: {
            adjustTimer(1);
            break;
        }
        case LongUp: {
            adjustTimer(TIME_TOOLS_BIG_STEPS);
            break;
        }
        case Down: {
            adjustTimer(-1);
            break;
        }
        case LongDown: {
            adjustTimer(-1 * TIME_TOOLS_BIG_STEPS);
            break;
        }
        default: {
            break;
        }
    }

    resetSleepDelay(SLEEP_EVERY_MS);
    delayTask(200);
    disUp();
}


void exitTimer()
{
    timerRunning = false;
    timerLastSecond = 0;
    timerStartTime = 0;
    timerElapsedTime = 0;
    timerElapsedMinutes = 0;
    remainingTime = timerTime;
}

#endif