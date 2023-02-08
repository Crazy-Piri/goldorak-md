#include "goldorak.h"

void waitPressSTART()
{
    // Wait until START gets released
    while (JOY_readJoypad(JOY_1) & BUTTON_START)
        SYS_doVBlankProcess();

    // Wait until START is pressed
    while (!(JOY_readJoypad(JOY_1) & BUTTON_START))
        SYS_doVBlankProcess();
}
