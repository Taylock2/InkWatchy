#include "stopwatch.h"

#if TIME_TOOLS

bool isRunning = false;  // Tracks if the stopwatch is running
int64_t startTime = 0;   // Start time of the stopwatch (UNIX time)
int64_t elapsedTime = 0; // Total elapsed time (in seconds)
int64_t lastSecond = 0;  // Tracks the last displayed UNIX

int8_t elapsedMinutes = 0; // Reset elapsed minutes

int centerX = dis->width() / 2;
int centerY = dis->height() / 2;
int radius = 96;
int innerRadius = 56;

void exitStopwatch()
{
    elapsedTime = 0;
    startTime = 0;
    elapsedMinutes = 0;
    isRunning = false;
}

String formatElapsedTime()
{
    String minutes = String(elapsedTime / 60);
    String seconds = String(elapsedTime % 60);

    if (minutes.length() == 1)
    {
        minutes = "0" + minutes;
    }

    if (seconds.length() == 1)
    {
        seconds = "0" + seconds;
    }

    String output = minutes + ":" + seconds + "";
    return output;
}

// Function to apply dithering within the 4 coordinates of a segment
void applyDithering(int x1Inner, int y1Inner, int x2Inner, int y2Inner, int x1Outer, int y1Outer, int x2Outer, int y2Outer)
{
    // Determine the min and max y values
    int minY = min(min(y1Inner, y2Inner), min(y1Outer, y2Outer));
    int maxY = max(max(y1Inner, y2Inner), max(y1Outer, y2Outer));

    // Loop through each row (y-value) within the segment
    for (int y = minY; y <= maxY; y++)
    {
        // Calculate left (minX) and right (maxX) boundaries for this row
        int minX = 200; // Arbitrary high value
        int maxX = -1;  // Arbitrary low value

        // Check intersections of the row with all 4 edges of the quadrilateral
        if (y1Inner != y1Outer)
        {
            int x = x1Inner + (y - y1Inner) * (x1Outer - x1Inner) / (y1Outer - y1Inner);
            if (y >= min(y1Inner, y1Outer) && y <= max(y1Inner, y1Outer))
            {
                minX = min(minX, x);
                maxX = max(maxX, x);
            }
        }
        if (y2Inner != y2Outer)
        {
            int x = x2Inner + (y - y2Inner) * (x2Outer - x2Inner) / (y2Outer - y2Inner);
            if (y >= min(y2Inner, y2Outer) && y <= max(y2Inner, y2Outer))
            {
                minX = min(minX, x);
                maxX = max(maxX, x);
            }
        }
        if (y1Inner != y2Inner)
        {
            int x = x1Inner + (y - y1Inner) * (x2Inner - x1Inner) / (y2Inner - y1Inner);
            if (y >= min(y1Inner, y2Inner) && y <= max(y1Inner, y2Inner))
            {
                minX = min(minX, x);
                maxX = max(maxX, x);
            }
        }
        if (y1Outer != y2Outer)
        {
            int x = x1Outer + (y - y1Outer) * (x2Outer - x1Outer) / (y2Outer - y1Outer);
            if (y >= min(y1Outer, y2Outer) && y <= max(y1Outer, y2Outer))
            {
                minX = min(minX, x);
                maxX = max(maxX, x);
            }
        }

        // Apply dithering within the row boundaries
        for (int x = minX; x <= maxX; x++)
        {
            if (((y % 2 == 0) && (x % 2 == 0)) || ((y % 2 == 1) && (x % 2 == 1)))
            {
                dis->drawPixel(x, y, GxEPD_BLACK); // Draw black
            }
        }
    }
}

// Draws a single segment with a checker pattern if fill true, otherwise clears
void drawSingleSegment(int segment, bool fill)
{
    // Adjust angles to start from 12 o'clock
    float angleStart = (segment * 6 - 90) * (PI / 180);     // Start angle in radians
    float angleEnd = ((segment + 1) * 6 - 90) * (PI / 180); // End angle in radians

    // Points on the outer circle
    int x1Outer = centerX + (radius - 1) * cos(angleStart);
    int y1Outer = centerY + (radius - 1) * sin(angleStart);
    int x2Outer = centerX + (radius - 1) * cos(angleEnd);
    int y2Outer = centerY + (radius - 1) * sin(angleEnd);

    // Points on the inner circle
    int x1Inner = centerX + innerRadius * cos(angleStart);
    int y1Inner = centerY + innerRadius * sin(angleStart);
    int x2Inner = centerX + innerRadius * cos(angleEnd);
    int y2Inner = centerY + innerRadius * sin(angleEnd);

    // Fill or clear the segment
    if (fill)
    {
        applyDithering(x1Inner, y1Inner, x2Inner, y2Inner, x1Outer, y1Outer, x2Outer, y2Outer);
    }
    else
    {
        // Clear segment by filling from outer to inner circle only
        dis->fillTriangle(x1Outer, y1Outer, x2Outer, y2Outer, x1Inner, y1Inner, GxEPD_WHITE);
        dis->fillTriangle(x1Inner, y1Inner, x2Inner, y2Inner, x2Outer, y2Outer, GxEPD_WHITE);
    }
}

// Draws the circle with all segments cleared
void drawEmptyCircle()
{
    // Outer ring
    dis->fillCircle(centerX, centerY, radius + 1, GxEPD_BLACK);
    dis->fillCircle(centerX, centerY, radius - 1, GxEPD_WHITE);

    // Inner circle
    dis->fillCircle(centerX, centerY, innerRadius, GxEPD_BLACK);
}

// Initialize the stopwatch display
void initStopwatch()
{
    drawEmptyCircle();

    setTextSize(2);
    setFont(getFont("UbuntuMono-Regular10"));
    writeTextReplaceBack(formatElapsedTime(), 100 - 48, 110, GxEPD_WHITE, GxEPD_BLACK);
    dis->setTextSize(1);

    writeImageN(TIME_TOOLS_PLAY_PAUSE_CORD, getImg("timetools/play"), GxEPD_BLACK, GxEPD_WHITE);

    resetSleepDelay(SLEEP_EVERY_MS);
    disUp(true);
}

// Stopwatch loop
void loopStopwatch()
{
    if (isRunning)
    {
        readRTC();

        if (lastSecond != getUnixTime(timeRTCUTC0))
        {
            lastSecond = getUnixTime(timeRTCUTC0);

            elapsedTime += (lastSecond - startTime);
            // Calculate elapsed minutes
            int64_t currentMinutes = elapsedTime / 60;

            startTime = lastSecond;

            if (currentMinutes > 0 & currentMinutes > elapsedMinutes)
            {
                elapsedMinutes = currentMinutes;
                drawSingleSegment(elapsedMinutes - 1, true); // Draw only the new segment
            }

            setTextSize(2);

            setFont(getFont("UbuntuMono-Regular10"));
            writeTextReplaceBack(formatElapsedTime(), 100 - 48, 110, GxEPD_WHITE, GxEPD_BLACK);
            setTextSize(1);
            dUChange = true;
        }
    }

    // Handle button presses
    switch (useButton())
    {
    case Menu:
    {
        if (isRunning)
        {
            // Pause the stopwatch
            isRunning = false;
            writeImageN(TIME_TOOLS_PLAY_PAUSE_CORD, getImg("timetools/play"), GxEPD_BLACK, GxEPD_WHITE);
            writeImageN(TIME_TOOLS_PAUSED_CORD, getImg("timetools/paused"), GxEPD_WHITE, GxEPD_BLACK);
            dUChange = true;
        }
        else
        {
            readRTC();
            // Start or resume the stopwatch
            startTime = getUnixTime(timeRTCUTC0);
            isRunning = true;
            dis->fillRect(TIME_TOOLS_PAUSED_CORD, 30, 30, GxEPD_BLACK);
            writeImageN(TIME_TOOLS_PLAY_PAUSE_CORD, getImg("timetools/pause"), GxEPD_BLACK, GxEPD_WHITE);
            dUChange = true;
        }
        break;
    }
    case LongMenu:
    {
        // Reset the stopwatch
        exitStopwatch();
        initStopwatch();
        break;
    }
    default:
    {
        break;
    }
    }

    resetSleepDelay();
    delayTask(200);
    disUp();
}

#endif
