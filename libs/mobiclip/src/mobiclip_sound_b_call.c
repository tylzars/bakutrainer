#include <mobiclip.h>

// Decodes one block of a channel of the second audio type. The decoder itself is func_02053834; with
// interworking off the call becomes a jump.
#pragma interworking off
void func_02052e28(Chan *ch) {
    func_02053834(ch);
}
#pragma interworking reset
