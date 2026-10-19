#include <nitro/gx.h>

// Nonzero while the displays are on.
u16 data_020644a0 = 1;
// Display mode that GX_DispOn restores.
u16 data_020ca6a4;

#define GX_DISPCNT_KEEP_MASK 0xfff0fff0 // everything except the display mode and BG mode

u16 GX_VBlankIntr(BOOL enable) {
    u16 old = reg_GX_DISPSTAT & GX_DISPSTAT_VBLANK_IRQ;
    if (enable) {
        reg_GX_DISPSTAT |= GX_DISPSTAT_VBLANK_IRQ;
    } else {
        reg_GX_DISPSTAT &= ~GX_DISPSTAT_VBLANK_IRQ;
    }
    return old;
}

// Switches the displays off, remembering the display mode for GX_DispOn.
void func_0203c994(void) {
    u32 dispcnt = reg_GX_DISPCNT;
    data_020644a0 = 0;
    data_020ca6a4 = (dispcnt & GX_DISPCNT_MODE_MASK) >> GX_DISPCNT_MODE_SHIFT;
    reg_GX_DISPCNT = dispcnt & ~GX_DISPCNT_MODE_MASK;
}

void GX_DispOn(void) {
    u16 mode;
    data_020644a0 = 1;
    mode = data_020ca6a4;
    if (mode != 0) {
        reg_GX_DISPCNT = (reg_GX_DISPCNT & ~GX_DISPCNT_MODE_MASK) | (mode << GX_DISPCNT_MODE_SHIFT);
        return;
    }
    reg_GX_DISPCNT |= GX_DISPCNT_MODE_GRAPHICS << GX_DISPCNT_MODE_SHIFT;
}

void GX_SetGraphicsMode(u32 dispMode, u32 bgMode, u32 bg0_2d3d) {
    u16 on = data_020644a0;
    u32 cur = reg_GX_DISPCNT;
    data_020ca6a4 = (u16) dispMode;
    if (on == 0) {
        dispMode = 0;
    }
    reg_GX_DISPCNT = (cur & GX_DISPCNT_KEEP_MASK) | (dispMode << GX_DISPCNT_MODE_SHIFT) | bgMode | (bg0_2d3d << 3);
    if (data_020ca6a4 == 0) {
        data_020644a0 = 0;
    }
}
