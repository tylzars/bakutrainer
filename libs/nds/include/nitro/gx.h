#pragma once

#include "types.h"
#include <nitro/fx.h>

#define reg_GX_DISPCNT (*(vu32 *) 0x04000000)
#define reg_GX_DISPSTAT (*(vu16 *) 0x04000004)
#define reg_GX_DISP3DCNT (*(vu16 *) 0x04000060)

#define GX_DISPSTAT_VBLANK_IRQ 0x0008

#define GX_DISPCNT_MODE_MASK 0x30000
#define GX_DISPCNT_MODE_SHIFT 16
#define GX_DISPCNT_MODE_OFF 0
#define GX_DISPCNT_MODE_GRAPHICS 1

#define reg_G2_BG0CNT (*(vu16 *) 0x04000008)
#define reg_G2_BG3CNT (*(vu16 *) 0x0400000e)
#define reg_G2_BG0OFS (*(vu32 *) 0x04000010)

#define reg_G3X_GXFIFO (*(vu32 *) 0x04000400)
#define reg_G3X_CLEAR_COLOR (*(vu32 *) 0x04000350)
#define reg_G3X_CLEAR_DEPTH (*(vu16 *) 0x04000354)
#define reg_G3X_CLRIMAGE_OFFSET (*(vu16 *) 0x04000356)
#define reg_G3X_FOG_COLOR (*(vu32 *) 0x04000358)
#define reg_G3X_FOG_OFFSET (*(vu16 *) 0x0400035c)
#define reg_G3X_MTX_MODE (*(vu32 *) 0x04000440)
#define reg_G3X_MTX_PUSH (*(vu32 *) 0x04000444)
#define reg_G3X_MTX_POP (*(vu32 *) 0x04000448)
#define reg_G3X_MTX_STORE (*(vu32 *) 0x0400044c)
#define reg_G3X_MTX_IDENTITY (*(vu32 *) 0x04000454)
#define reg_G3X_MTX_SCALE (*(vu32 *) 0x0400046c)
#define reg_G3X_MTX_TRANS (*(vu32 *) 0x04000470)
#define reg_G3X_NORMAL (*(vu32 *) 0x04000484)
#define reg_G3X_TEXCOORD (*(vu32 *) 0x04000488)
#define reg_G3X_VTX_16 (*(vu32 *) 0x0400048c)
#define reg_G3X_VTX_XY (*(vu32 *) 0x04000494)
#define reg_G3X_POLYGON_ATTR (*(vu32 *) 0x040004a4)
#define reg_G3X_TEXIMAGE_PARAM (*(vu32 *) 0x040004a8)
#define reg_G3X_PLTT_BASE (*(vu32 *) 0x040004ac)
#define reg_G3X_DIF_AMB (*(vu32 *) 0x040004c0)
#define reg_G3X_SPE_EMI (*(vu32 *) 0x040004c4)
#define reg_G3X_LIGHT_COLOR (*(vu32 *) 0x040004cc)
#define reg_G3X_BEGIN_VTXS (*(vu32 *) 0x04000500)
#define reg_G3X_END_VTXS (*(vu32 *) 0x04000504)
#define reg_G3X_VIEWPORT (*(vu32 *) 0x04000580)
#define reg_G3X_POS_TEST (*(vu32 *) 0x040005c4)
#define reg_G3X_GXSTAT (*(vu32 *) 0x04000600)

#ifdef __cplusplus
extern "C" {
#endif

// DMA channel the GX code fills its tables with; -1 means CPU fills (it starts as 3).
extern s32 data_020644a4;

u16 GX_VBlankIntr(BOOL enable);
void func_0203c994(void);
void GX_DispOn(void);
void GX_SetGraphicsMode(u32 dispMode, u32 bgMode, u32 bg0_2d3d);

// Copies 12 / 16 words to one fixed address (a FIFO port).
void func_0203d178(const void *src, volatile void *dst);
void func_0203d19c(const void *src, volatile void *dst);

void func_0203ca80(const MtxFx44 *mtx);
void G3_LoadMtx43(const MtxFx43 *mtx);
void G3_MultMtx33(const MtxFx33 *mtx);

void G3X_Init(void);
void func_0203cbe0(void);
void G3X_ClearFifo(void);
void G3X_InitMtxStack(void);
void func_0203cd0c(void);
void func_0203cd9c(BOOL enable, int mode, int shift, int offset);
void func_0203cdf4(const u16 *table);
void G3X_SetClearColor(int rgb, int alpha, int depth, int polygonID, BOOL fog);
void G3X_InitTable(void);
int G3X_GetMtxStackLevelPV(u32 *level);
int G3X_GetMtxStackLevelPJ(u32 *level);
int G3X_GetBoxTestResult(u32 *result);
int func_0203cf5c(VecFx32 *pos, fx32 *w);
void GXi_NopClearFifo128_(volatile void *fifo);

void func_0203d030(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scale, BOOL load, MtxFx44 *mtx);
void func_0203d08c(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scale, BOOL load, MtxFx44 *mtx);
void func_0203d0f0(const VecFx32 *pos, const VecFx32 *up, const VecFx32 *target, BOOL load, MtxFx43 *mtx);
void func_0203d13c(fx32 sin, fx32 cos);

#ifdef __cplusplus
}
#endif
