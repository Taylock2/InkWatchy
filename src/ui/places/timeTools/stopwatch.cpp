#include "stopwatch.h"

#if TIME_TOOLS

uint16_t timerHeight;

String twoDigits(String str)
{
    while (str.length() < 2)
    {
        str = "0" + str;
    }
    return str;
}

String getTimer()
{
    return twoDigits(String(timeRTCLocal.Hour)) + ":" + twoDigits(String(timeRTCLocal.Minute)) + ":" + twoDigits(String(timeRTCLocal.Second));
}

String timeSince(int64_t unixTime)
{
    int64_t currentTime = getUnixTime(timeRTCUTC0);
    int64_t elapsedTime = currentTime - unixTime;

    int64_t days = elapsedTime / (24 * 3600);
    elapsedTime %= (24 * 3600);
    int64_t hours = elapsedTime / 3600;
    elapsedTime %= 3600;
    int64_t minutes = elapsedTime / 60;
    int64_t seconds = elapsedTime % 60;

    String output = String(days) + "d " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";
    return output;
}

void initStopwatch()
{
    uint16_t h;
    setFont(&FreeSansBold9pt7b);
    setTextSize(1);
    dis->setCursor(0, 1);
    String menuName = "Time tools: Stopwatch";
    debugLog(menuName);
    getTextBounds(menuName, NULL, NULL, NULL, &h);
    if (containsBelowChar(menuName) == true)
    {
        h = h + 2;
    }
    maxHeight = h;
    uint16_t currentHeight = maxHeight;
    dis->setCursor(0, currentHeight - 3);
    dis->print(menuName);

    dis->fillRect(0, currentHeight, dis->width(), 3, GxEPD_BLACK);
    currentHeight = currentHeight + maxHeight;

    timerHeight = currentHeight;
    readRTC();
    centerText(getTimer(), &currentHeight);

    resetSleepDelay(SLEEP_EVERY_MS);
    disUp(true);
}

uint8_t savedSeconds = 0;
void loopstopwatch()
{
    readRTC();
    if (savedSeconds != timeRTCLocal.Second)
    {
        savedSeconds = timeRTCLocal.Second;
        // debugLog("timeRTCLocal.Second: " + String(timeRTCLocal.Second));
        writeTextCenterReplaceBack(getTimer(), timerHeight);
        dUChange = true;
    }

    switch (useButton())
    {
        case Menu:
        {
            // Start or pause
            break;
        }
        case LongMenu:
        {
            initStopwatch(); // reset on longpress
            break;
        }
        default:
        {
            break;
        }
    }

    resetSleepDelay();
    delayTask(100);
    disUp();
}

#endif