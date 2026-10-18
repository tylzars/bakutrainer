#include <Main/Unknown/functions.h>
#include <Main/Unknown/touch_buttons.h>
#include <Main/Unknown/backup_access.h>
#include <Main/Unknown/globals.h>
#include <Main/Unknown/random_angle.h>
#include <Main/Unknown/system_init.h>
#include <Main/Unknown/save_blocks.h>
#include <Main/Unknown/text_draw.h>
#include <Main/clib.h>
#include <nitro/os.h>
#include <nitro/math.h>

#define POWCNT1_SWAP_LCDS 0x8000

// Store kinds accepted by func_0202002c: blocks of 0x200 bytes and blocks of 0x2000 bytes.
#define SAVE_STORE_SMALL 0x901
#define SAVE_STORE_LARGE 0xd01
#define SMALL_BLOCK_SIZE 0x200
#define LARGE_BLOCK_SIZE 0x2000
#define CRC_POLY 0xa001

// Results of func_02020140.
#define BLOCK_CRC_OK 1
#define BLOCK_ID_OK 2
#define BLOCK_INTACT 3

// Marks of a field in the ordering list built by func_02020354.
#define ORDER_END (-1)
#define ORDER_USED (-2)

// Returns the 16-bit tag of a block: the table bytes at 2 * index and 2 * index + 1, each taken modulo the table's
// length.
u16 func_0201ffd4(int id, u32 index) {
    const u8 *table = func_02021e18(id, data_020c18bc.mUnk_32c + 2);
    u32 count = func_02021f0c(id, data_020c18bc.mUnk_32c + 2, 0);
    if (count < 2) {
        OS_Terminate();
    }
    return (table[(index * 2) % count] << 8) + table[(index * 2 + 1) % count];
}

// Sets up the block store for set a: kind, first and last field, status callback and the value for mUnk_454
// (-1 leaves it). Reads every block and rewrites the ones that do not read back intact, then waits for the store.
void func_0202002c(u32 a, int b, u16 c, int d, void (*fn)(u32), int e) {
    u32 count = func_02021e44(a, d + 1);
    u32 r;
    u32 i;
    if (b != SAVE_STORE_SMALL && b != SAVE_STORE_LARGE) {
        OS_Terminate();
    }
    data_020c18bc.mUnk_31c = fn;
    data_020c18bc.mUnk_32e = 0;
    data_020c18bc.mUnk_32f = data_020c18bc.mUnk_32e;
    data_020c18bc.mUnk_32a = c;
    data_020c18bc.mUnk_32c = d;
    data_020c18bc.mUnk_324 = 0;
    if (e != -1) {
        data_020c18bc.mUnk_454 = e;
        fn(4);
    }
    func_02020ee0(b);
    if (b == SAVE_STORE_SMALL) {
        data_020c18bc.mUnk_328 = SMALL_BLOCK_SIZE;
    } else {
        data_020c18bc.mUnk_328 = LARGE_BLOCK_SIZE;
    }
    // Volatile read of the frame counter; the value is not used.
    data_020c18bc.mUnk_45c;
    for (i = 0; i < count; i++) {
        r = func_02020140(i, 0, a);
        if (r == BLOCK_ID_OK) {
            data_020c18bc.mUnk_31c(1);
        }
        if (r != BLOCK_INTACT) {
            if (data_020c18bc.mUnk_328 > SMALL_BLOCK_SIZE && r != BLOCK_ID_OK) {
                data_020c18bc.mUnk_31c(6);
            }
            func_02020224(i, a);
        }
    }
    func_0201fed4(1);
    if (data_020c18bc.mUnk_32f == 1) {
        data_020c18bc.mUnk_31c(2);
        while (func_02020890() == 0) {
            func_0201bb30();
        }
        data_020c18bc.mUnk_46c &= ~0xc0000;
    }
    data_020c18bc.mUnk_450 |= 4;
}

// Reads block index of set c. Bit 0 of the result: the checksum matches (the record is then unpacked into buf when
// buf is not null); bit 1: the stored tag matches the block's tag.
u32 func_02020140(u32 index, void *buf, int c) {
    u16 table[256];
    u16 crc;
    int size = func_0201ff80(c);
    u8 *data = func_0201c430(size);
    u32 result = 0;
    int check = func_0201ffd4(c, index);
    if (index >= func_02021e44(c, data_020c18bc.mUnk_32c + 1)) {
        OS_Terminate();
    }
    if (func_02020f9c(data, index * size, size) == 0) {
        if (data_020c18bc.mUnk_320 != 0) {
            memcpy(data, data_020c18bc.mUnk_320 + index * size, size);
        } else {
            data_020c18bc.mUnk_31c(10);
        }
    }
    func_02048780(table, CRC_POLY);
    crc = func_02048804(table, data + 4, size - 4);
    if (crc == data[0] + (data[1] << 8)) {
        if (buf != 0) {
            func_0202062c(buf, (u32 *) (data + 4), c);
        }
        result |= BLOCK_CRC_OK;
    }
    if (data[2] == (check & 0xff) && data[3] == (check >> 8)) {
        result |= BLOCK_ID_OK;
    }
    if (data_020c18bc.mUnk_32e != 1) {
        func_0201fed4(0);
    }
    func_0201c4f0(&data);
    return result;
}

// Writes a freshly allocated record to block index of set c and sets the block's bit in mUnk_458.
void func_02020224(u32 index, int c) {
    void *buf = func_0201c430(func_0201ffa0() * 4);
    func_02020258(index, buf, c);
    func_0201c4f0(&buf);
    data_020c18bc.mUnk_458[0] |= 1 << index;
}

// Packs the record in buf into block index of set c with its checksum and tag and writes the block.
void func_02020258(u32 index, void *buf, int c) {
    u16 table[256];
    u16 crc;
    int size = func_0201ff80(c);
    u8 *data = func_0201c430(size);
    int payloadWritten = 1;
    int check = func_0201ffd4(c, index);
    int payload;
    u32 offset;
    int headWritten;
    if (index >= func_02021e44(c, data_020c18bc.mUnk_32c + 1)) {
        OS_Terminate();
    }
    memset(data, 0, size);
    func_0202043c(buf, (u32 *) (data + 4), c);
    func_02048780(table, CRC_POLY);
    payload = size - 4;
    crc = func_02048804(table, data + 4, payload);
    memcpy(data, &crc, 2);
    data[2] = check;
    data[3] = check >> 8;
    if (size > 4) {
        payloadWritten = func_02020f38(data + 4, index * size + 4, payload);
    }
    offset = index * size;
    headWritten = func_02020f38(data, offset, 4);
    if (payloadWritten == 0 || headWritten == 0) {
        if (data_020c18bc.mUnk_320 != 0) {
            memcpy(data_020c18bc.mUnk_320 + offset, data, size);
        } else {
            data_020c18bc.mUnk_31c(4);
        }
    }
    if (data_020c18bc.mUnk_32e != 1) {
        func_0201fed4(1);
    }
    func_0201c4f0(&data);
}

// Clears mUnk_456.
void func_02020348(void) {
    data_020c18bc.mUnk_456 = 0;
}

#pragma thumb off
// Fills order with 0..count-1, ends it with ORDER_END and sorts it by descending key (shell sort).
void func_02020354(u32 *order, const u32 *key, int count) {
    int gap;
    u32 t;
    int j;
    int i;
    int span;
    int a;
    for (i = 0; i < count; i++) {
        order[i] = i;
    }
    order[i] = ORDER_END;
    gap = (1 << (32 - func_02028250(count))) - 1;
    for (; gap > 0; gap = (gap - 1) / 2) {
        span = count - gap;
        for (a = 0; a < span; a++) {
            j = 0;
            if (key[order[a]] < key[order[a + gap]]) {
                do {
                    t = order[a + j];
                    order[a + j] = order[a + gap + j];
                    order[a + gap + j] = t;
                    j -= gap;
                } while (a + j >= 0 && key[order[a + j]] < key[order[a + gap + j]]);
            }
        }
    }
}

// Packs the field values into 32-bit words, mixed radix: each field's radix is its largest value plus one. Returns
// the number of bytes written to out.
int func_0202043c(const u32 *values, u32 *out, u32 id) {
    int fields;
    u32 *radix;
    u32 *order;
    int i;
    int outCount;
    u64 prod;
    u64 old;
    u32 acc;
    fields = func_0201ffa0();
    radix = func_0201c430(fields * 4);
    order = func_0201c430((fields + 1) * 4);
    for (i = 0; i < fields; i++) {
        radix[i] = func_02021e44(id, data_020c18bc.mUnk_32a + i) + 1;
    }
    func_02020354(order, radix, fields);
    outCount = 0;
    while (1) {
        i = 0;
        acc = 0;
        prod = 1;
        if (order[0] != ORDER_END) {
            do {
                if (order[i] != ORDER_USED) {
                    old = prod;
                    prod = prod * radix[order[i]];
                    if (prod - 1 > 0xffffffff) {
                        prod = old;
                    } else {
                        if (values[order[i]] >= radix[order[i]]) {
                            OS_Terminate();
                        }
                        acc = radix[order[i]] * acc + values[order[i]];
                        order[i] = ORDER_USED;
                        if (prod * 2 - 1 > 0xffffffff) {
                            break;
                        }
                    }
                }
                i++;
            } while (order[i] != ORDER_END);
        }
        if (prod == 1) {
            break;
        }
        out[outCount] = acc;
        outCount++;
    }
    func_0201c4f0(&radix);
    func_0201c4f0(&order);
    return outCount * 4;
}

// Unpacks the words written by func_0202043c back into the field values.
void func_0202062c(u32 *out, u32 *packed, u32 id) {
    int fields = func_0201ffa0();
    u32 *radix = func_0201c430(fields * 4);
    u32 *order = func_0201c430((fields + 1) * 4);
    u32 list[32];
    int i;
    int w;
    int n;
    u64 prod;
    for (i = 0; i < fields; i++) {
        radix[i] = func_02021e44(id, data_020c18bc.mUnk_32a + i) + 1;
    }
    func_02020354(order, radix, fields);
    for (w = 0;; w++) {
        i = 0;
        n = 0;
        prod = 1;
        if (order[0] != ORDER_END) {
            do {
                if (order[i] != ORDER_USED) {
                    u32 f = order[i];
                    u32 r = radix[f];
                    u64 old = prod;
                    prod = prod * r;
                    if (prod - 1 > 0xffffffff) {
                        prod = old;
                    } else {
                        order[i] = ORDER_USED;
                        list[n] = f;
                        n++;
                        if (prod * 2 - 1 > 0xffffffff) {
                            break;
                        }
                    }
                }
                i++;
            } while (order[i] != ORDER_END);
        }
        if (n == 0) {
            break;
        }
        while (n-- != 0) {
            u32 f = list[n];
            out[f] = packed[w] % radix[f];
            packed[w] = packed[w] / radix[f];
        }
    }
    func_0201c4f0(&radix);
    func_0201c4f0(&order);
}
#pragma thumb on

// Sets (a == 1) or clears the LCD swap bit of the power control register.
void func_02020854(int a) {
    if (a == 1) {
        *(vu16 *) 0x04000304 |= POWCNT1_SWAP_LCDS;
    } else {
        *(vu16 *) 0x04000304 &= ~POWCNT1_SWAP_LCDS;
    }
}
