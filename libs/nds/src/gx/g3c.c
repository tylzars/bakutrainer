#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi.h>

#define G3_CMD_LOAD_MTX44 0x16
#define G3_CMD_LOAD_MTX43 0x17
#define G3_CMD_MULT_MTX33 0x1a

void func_0203ca80(const MtxFx44 *mtx) {
    reg_G3X_GXFIFO = G3_CMD_LOAD_MTX44;
    func_0203d19c(mtx, &reg_G3X_GXFIFO);
}

void G3_LoadMtx43(const MtxFx43 *mtx) {
    reg_G3X_GXFIFO = G3_CMD_LOAD_MTX43;
    func_0203d178(mtx, &reg_G3X_GXFIFO);
}

void G3_MultMtx33(const MtxFx33 *mtx) {
    reg_G3X_GXFIFO = G3_CMD_MULT_MTX33;
    MI_Copy36B(mtx, &reg_G3X_GXFIFO);
}
