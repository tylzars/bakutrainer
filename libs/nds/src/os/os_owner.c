#include <nitro/os.h>
#include <nitro/mi.h>

void OS_GetMacAddress(u8 *macAddress) {
    MI_CpuCopy8((const void *) 0x27ffcf4, macAddress, 6);
}

void OS_GetOwnerInfo(OSOwnerInfo *info) {
    vu8 *src = (vu8 *) 0x27ffc80;
    info->language = ((u32) * (vu16 *) (src + 0x64) << 29) >> 29;
    info->favoriteColor = ((u32) src[2] << 28) >> 28;
    info->birthMonth = src[3];
    info->birthDay = src[4];
    info->nickNameLength = src[0x1a];
    info->commentLength = src[0x50];
    MIi_CpuCopy16((const void *) (src + 6), info->nickName, 0x14);
    MIi_CpuCopy16((const void *) (src + 0x1c), info->comment, 0x34);
    info->nickNameEnd = 0;
    info->commentEnd = 0;
}
