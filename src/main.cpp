/*  Copyright 2023 shutterbug2000
    Copyright 2019 Maschell

    Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby
    granted, provided that the above copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
    INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.
*/

#include <wups.h>
#include <utils/logger.h>
#include <coreinit/screen.h>
#include <mocha/mocha.h>

/**
    Mandatory plugin information.
    If not set correctly, the loader will refuse to use the plugin.
**/
WUPS_PLUGIN_NAME("CrashSplash");
WUPS_PLUGIN_DESCRIPTION("Displays a silly splash screen when the Wii U crashes");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("Damienne");
WUPS_PLUGIN_LICENSE("ISC");

extern "C" {
    void OSSendAppSwitchRequest(uint32_t rampid, void* args, uint32_t argsSize);
}

DECL_FUNCTION(void, PPCHalt) {
    OSScreenInit();
    OSScreenEnableEx(SCREEN_TV, 1);
    OSScreenEnableEx(SCREEN_DRC, 1);
    uint32_t tvSize = OSScreenGetBufferSizeEx(SCREEN_TV);
    OSScreenSetBufferEx(SCREEN_TV, (unsigned char*)0xf4000000);
    OSScreenSetBufferEx(SCREEN_DRC, (unsigned char*)0xf4000000+tvSize);
    OSScreenClearBufferEx(SCREEN_TV,0x007ad2ff);
    OSScreenClearBufferEx(SCREEN_DRC,0x007ad2ff);
    OSScreenPutFontEx(SCREEN_TV,0,0,":(");
    OSScreenPutFontEx(SCREEN_TV,0,1,"Your Nintendo Wii U ran into a problem, and needs to restart.");
    OSScreenPutFontEx(SCREEN_TV,0,2,"Please hold down the power button.");
    OSScreenPutFontEx(SCREEN_DRC,0,0,":(");
    OSScreenPutFontEx(SCREEN_DRC,0,1,"Your Nintendo Wii U ran into a problem, and needs to restart.");
    OSScreenPutFontEx(SCREEN_DRC,0,2,"Please hold down the power button.");
    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);
    OSSleepTicks(OSMillisecondsToTicks(5000));
    
    return;
}

INITIALIZE_PLUGIN() {
}

DEINITIALIZE_PLUGIN() {
}

ON_APPLICATION_START() {
    WHBLogUdpInit();
    WHBLogCafeInit();
    DEBUG_FUNCTION_LINE("HaltFix loaded");

    auto res = Mocha_InitLibrary();

    if (res != MOCHA_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE("Mocha init failed with code %d!", res);
        return;
    }
}

ON_APPLICATION_ENDS() {
    WHBLogUdpDeinit();
    WHBLogCafeDeinit();
    Mocha_DeInitLibrary();
}

WUPS_MUST_REPLACE_FOR_PROCESS(PPCHalt, WUPS_LOADER_LIBRARY_COREINIT, PPCHalt, WUPS_FP_TARGET_PROCESS_GAME);
