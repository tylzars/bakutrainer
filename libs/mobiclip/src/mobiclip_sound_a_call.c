#include <mobiclip.h>

// Decodes one block of a channel of the first audio type; returns the number of input bytes the block used.
// The decoder itself is func_02052c78; with interworking off the call becomes a jump.
#pragma interworking off
int func_02052468(Chan *ch) {
    return func_02052c78(ch);
}
#pragma interworking reset
