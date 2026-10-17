#include <nitro/snd.h>
#include <nitro/os.h>

static BOOL sndInitialized;
static OSMutex sndMutex;

void func_020402f0(void) {
    if (sndInitialized != 0) {
        return;
    }
    sndInitialized = TRUE;
    OS_InitMutex(&sndMutex);
    SND_CommandInit();
    func_020409cc();
}
