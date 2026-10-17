#include <nitro/math.h>

void func_02048780(u16 *table, u16 poly) {
    u32 j;
    u32 crc;
    u32 i;

    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ poly;
            } else {
                crc >>= 1;
            }
        }
        table[i] = crc;
    }
}

void func_020487c4(const u16 *table, u16 *currentCrc, const void *data, u32 dataLength) {
    const u8 *p = data;
    u32 crc = *currentCrc;
    u32 i;

    for (i = 0; i < dataLength; i++) {
        crc = table[(crc ^ *p++) & 0xff] ^ (crc >> 8);
    }
    *currentCrc = crc;
}

u16 func_02048804(const u16 *table, const void *data, u32 dataLength) {
    u16 crc = 0;

    func_020487c4(table, &crc, data, dataLength);
    return crc;
}
