#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi.h>

#define G3X_GXSTAT_BUSY 0x08000000
#define G3X_GXSTAT_ERROR_CLEAR 0x8000

void G3X_Init(void) {
    G3X_ClearFifo();
    reg_G3X_END_VTXS = 0;
    while (reg_G3X_GXSTAT & G3X_GXSTAT_BUSY) {
    }
    reg_GX_DISP3DCNT = 0;
    reg_G3X_GXSTAT = 0;
    reg_G2_BG0OFS = 0;
    reg_GX_DISP3DCNT |= 0x2000;
    reg_GX_DISP3DCNT |= 0x1000;
    reg_GX_DISP3DCNT &= ~0x3002;
    reg_GX_DISP3DCNT = (u16) ((reg_GX_DISP3DCNT & ~0x3000) | 0x10);
    reg_GX_DISP3DCNT &= 0xcffb;
    reg_G3X_GXSTAT |= G3X_GXSTAT_ERROR_CLEAR;
    reg_G3X_GXSTAT = (reg_G3X_GXSTAT & ~0xc0000000) | 0x80000000;
    G3X_InitMtxStack();
    reg_G3X_CLEAR_COLOR = 0;
    reg_G3X_CLEAR_DEPTH = 0x7fff;
    reg_G3X_CLRIMAGE_OFFSET = 0;
    reg_G3X_FOG_COLOR = 0;
    reg_G3X_FOG_OFFSET = 0;
    reg_G2_BG0CNT &= ~3;
    G3X_InitTable();
    *(vu32 *) 0x040004a4 = 0x1f0080;
    *(vu32 *) 0x040004a8 = 0;
    *(vu32 *) 0x040004ac = 0;
}

// Resets the geometry engine: waits for it to go idle, clears the error flags and matrix stacks.
void func_0203cbe0(void) {
    while (reg_G3X_GXSTAT & G3X_GXSTAT_BUSY) {
    }
    reg_G3X_GXSTAT |= G3X_GXSTAT_ERROR_CLEAR;
    reg_GX_DISP3DCNT |= 0x2000;
    reg_GX_DISP3DCNT |= 0x1000;
    func_0203cd0c();
    *(vu32 *) 0x040004a4 = 0x1f0080;
    *(vu32 *) 0x040004a8 = 0;
    *(vu32 *) 0x040004ac = 0;
}

void G3X_ClearFifo(void) {
    GXi_NopClearFifo128_(&reg_G3X_GXFIFO);
    while (reg_G3X_GXSTAT & G3X_GXSTAT_BUSY) {
    }
}

void G3X_InitMtxStack(void) {
    u32 pv;
    u32 pj;
    reg_G3X_GXSTAT |= G3X_GXSTAT_ERROR_CLEAR;
    while (G3X_GetMtxStackLevelPV(&pv) != 0) {
    }
    while (G3X_GetMtxStackLevelPJ(&pj) != 0) {
    }
    reg_G3X_MTX_MODE = 3;
    reg_G3X_MTX_IDENTITY = 0;
    reg_G3X_MTX_MODE = 0;
    if (pj != 0) {
        reg_G3X_MTX_POP = pj;
    }
    reg_G3X_MTX_IDENTITY = 0;
    reg_G3X_MTX_MODE = 2;
    reg_G3X_MTX_POP = pv;
    reg_G3X_MTX_IDENTITY = 0;
}

// Resets the projection and position/vector matrix stacks to identity, popping each back to its base level.
void func_0203cd0c(void) {
    u32 pv;
    u32 pj;
    reg_G3X_GXSTAT |= G3X_GXSTAT_ERROR_CLEAR;
    while (G3X_GetMtxStackLevelPV(&pv) != 0) {
    }
    while (G3X_GetMtxStackLevelPJ(&pj) != 0) {
    }
    reg_G3X_MTX_MODE = 3;
    reg_G3X_MTX_IDENTITY = 0;
    reg_G3X_MTX_MODE = 0;
    if (pj != 0) {
        reg_G3X_MTX_POP = pj;
    }
    reg_G3X_MTX_MODE = 2;
    reg_G3X_MTX_POP = pv;
    reg_G3X_MTX_IDENTITY = 0;
}

// Fog control: enable, blend mode, depth shift and offset.
void func_0203cd9c(BOOL enable, int mode, int shift, int offset) {
    if (enable) {
        reg_G3X_FOG_OFFSET = (u16) offset;
        reg_GX_DISP3DCNT = (u16) ((shift << 8) | (mode << 6) | 0x80 | (reg_GX_DISP3DCNT & ~0x3f40));
    } else {
        reg_GX_DISP3DCNT &= 0xcf7f;
    }
}

// Writes the 32-entry fog density table.
void func_0203cdf4(const u16 *table) {
    func_0203fda0(table, (void *) 0x04000360);
}

void G3X_SetClearColor(int rgb, int alpha, int depth, int polygonID, BOOL fog) {
    u32 v = rgb | (alpha << 16) | (polygonID << 24);
    if (fog) {
        v |= 0x8000;
    }
    reg_G3X_CLEAR_COLOR = v;
    reg_G3X_CLEAR_DEPTH = (u16) depth;
}

void G3X_InitTable(void) {
    int i;
    if (data_020644a4 != -1) {
        MI_DmaFill32Async(data_020644a4, (void *) 0x04000330, 0, 0x10, 0, 0);
        MI_DmaFill32(data_020644a4, (void *) 0x04000360, 0, 0x60);
    } else {
        MIi_CpuClear32(0, (void *) 0x04000330, 0x10);
        MIi_CpuClear32(0, (void *) 0x04000360, 0x60);
    }
    for (i = 0; i < 32; i++) {
        *(vu32 *) 0x040004d0 = 0;
    }
}

int G3X_GetMtxStackLevelPV(u32 *level) {
    if (reg_G3X_GXSTAT & 0x4000) {
        return -1;
    }
    *level = (reg_G3X_GXSTAT & 0x1f00) >> 8;
    return 0;
}

int G3X_GetMtxStackLevelPJ(u32 *level) {
    if (reg_G3X_GXSTAT & 0x4000) {
        return -1;
    }
    *level = (reg_G3X_GXSTAT & 0x2000) >> 13;
    return 0;
}

int G3X_GetBoxTestResult(u32 *result) {
    if (reg_G3X_GXSTAT & 1) {
        return -1;
    }
    *result = reg_G3X_GXSTAT & 2;
    return 0;
}

// Reads the vertex position test result; -1 while the test is running.
int func_0203cf5c(VecFx32 *pos, fx32 *w) {
    if (reg_G3X_GXSTAT & 1) {
        return -1;
    }
    pos->x = *(vu32 *) 0x04000620;
    pos->y = *(vu32 *) 0x04000624;
    pos->z = *(vu32 *) 0x04000628;
    *w = *(vu32 *) 0x0400062c;
    return 0;
}

// Writes 32 groups of four zero words (128 words in all) to the same write-only FIFO port.
void GXi_NopClearFifo128_(volatile void *fifo) {
    typedef struct {
        u32 w[4];
    } Words4;
    Words4 zero = {0, 0, 0, 0};
    int i;
    for (i = 0; i < 32; i++) {
        *(volatile Words4 *) fifo = zero;
    }
}
