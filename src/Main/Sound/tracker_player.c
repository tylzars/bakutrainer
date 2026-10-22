#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/snd.h>
#include <Main/System/system_init.h>
#include <Main/System/file_loader.h>
#include <Main/Sound/wave_player.h>
#include <Main/Sound/tracker_player.h>
#include <Main/clib.h>
#include <stdio.h>

// Periods per note for each of the 16 fine-tune steps.
const u16 gNotePeriods[16][96] = {
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x716, 0x6b0, 0x650, 0x5f4, 0x5a0, 0x54c, 0x500, 0x4b8,
        0x474, 0x434, 0x3f8, 0x3c0, 0x38b, 0x358, 0x328, 0x2fa, 0x2d0, 0x2a6, 0x280, 0x25c, 0x23a, 0x21a, 0x1fc, 0x1e0,
        0x1c5, 0x1ac, 0x194, 0x17d, 0x168, 0x153, 0x140, 0x12e, 0x11d, 0x10d, 0xfe,  0xf0,  0xe2,  0xd6,  0xca,  0xbe,
        0xb4,  0xaa,  0xa0,  0x97,  0x8f,  0x87,  0x7f,  0x78,  0x71,  0x6b,  0x65,  0x5f,  0x5a,  0x55,  0x50,  0x4b,
        0x47,  0x43,  0x3f,  0x3c,  0x38,  0x35,  0x32,  0x2f,  0x2d,  0x2a,  0x28,  0x25,  0x23,  0x21,  0x1f,  0x1e,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x708, 0x6a4, 0x644, 0x5ea, 0x596, 0x546, 0x4f8, 0x4b2,
        0x46e, 0x42e, 0x3f2, 0x3ba, 0x384, 0x352, 0x322, 0x2f5, 0x2cb, 0x2a3, 0x27c, 0x259, 0x237, 0x217, 0x1f9, 0x1dd,
        0x1c2, 0x1a9, 0x191, 0x17b, 0x165, 0x151, 0x13e, 0x12c, 0x11c, 0x10c, 0xfd,  0xee,  0xe1,  0xd4,  0xc8,  0xbd,
        0xb3,  0xa9,  0x9f,  0x96,  0x8e,  0x86,  0x7e,  0x77,  0x70,  0x6a,  0x64,  0x5e,  0x59,  0x54,  0x4f,  0x4b,
        0x47,  0x43,  0x3f,  0x3b,  0x38,  0x35,  0x32,  0x2f,  0x2c,  0x2a,  0x27,  0x25,  0x23,  0x21,  0x1f,  0x1d,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6fc, 0x698, 0x638, 0x5e0, 0x58a, 0x53c, 0x4f0, 0x4aa,
        0x466, 0x428, 0x3ec, 0x3b4, 0x37e, 0x34c, 0x31c, 0x2f0, 0x2c5, 0x29e, 0x278, 0x255, 0x233, 0x214, 0x1f6, 0x1da,
        0x1bf, 0x1a6, 0x18e, 0x178, 0x163, 0x14f, 0x13c, 0x12a, 0x11a, 0x10a, 0xfb,  0xed,  0xdf,  0xd3,  0xc7,  0xbc,
        0xb1,  0xa7,  0x9e,  0x95,  0x8d,  0x85,  0x7d,  0x76,  0x6f,  0x69,  0x63,  0x5e,  0x58,  0x53,  0x4f,  0x4a,
        0x46,  0x42,  0x3e,  0x3b,  0x37,  0x34,  0x31,  0x2f,  0x2c,  0x29,  0x27,  0x25,  0x23,  0x21,  0x1f,  0x1d,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6ee, 0x68c, 0x62e, 0x5d4, 0x580, 0x532, 0x4e8, 0x4a0,
        0x45e, 0x420, 0x3e4, 0x3ac, 0x377, 0x346, 0x317, 0x2ea, 0x2c0, 0x299, 0x274, 0x250, 0x22f, 0x210, 0x1f2, 0x1d6,
        0x1bc, 0x1a3, 0x18b, 0x175, 0x160, 0x14c, 0x13a, 0x128, 0x118, 0x108, 0xf9,  0xeb,  0xde,  0xd1,  0xc6,  0xbb,
        0xb0,  0xa6,  0x9d,  0x94,  0x8c,  0x84,  0x7d,  0x76,  0x6f,  0x68,  0x63,  0x5d,  0x58,  0x53,  0x4e,  0x4a,
        0x46,  0x42,  0x3e,  0x3b,  0x37,  0x34,  0x31,  0x2e,  0x2c,  0x29,  0x27,  0x25,  0x23,  0x21,  0x1f,  0x1d,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6e2, 0x680, 0x622, 0x5ca, 0x576, 0x528, 0x4de, 0x498,
        0x456, 0x418, 0x3dc, 0x3a6, 0x371, 0x340, 0x311, 0x2e5, 0x2bb, 0x294, 0x26f, 0x24c, 0x22b, 0x20c, 0x1ee, 0x1d3,
        0x1b9, 0x1a0, 0x188, 0x172, 0x15e, 0x14a, 0x138, 0x126, 0x116, 0x106, 0xf7,  0xe9,  0xdc,  0xd0,  0xc4,  0xb9,
        0xaf,  0xa5,  0x9c,  0x93,  0x8b,  0x83,  0x7b,  0x75,  0x6e,  0x68,  0x62,  0x5c,  0x57,  0x52,  0x4e,  0x49,
        0x45,  0x41,  0x3d,  0x3a,  0x37,  0x34,  0x31,  0x2e,  0x2b,  0x29,  0x27,  0x24,  0x22,  0x20,  0x1e,  0x1d,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6d6, 0x674, 0x616, 0x5c0, 0x56c, 0x51e, 0x4d6, 0x490,
        0x44e, 0x410, 0x3d6, 0x39e, 0x36b, 0x33a, 0x30b, 0x2e0, 0x2b6, 0x28f, 0x26b, 0x248, 0x227, 0x208, 0x1eb, 0x1cf,
        0x1b5, 0x19d, 0x186, 0x170, 0x15b, 0x148, 0x135, 0x124, 0x114, 0x104, 0xf5,  0xe8,  0xdb,  0xce,  0xc3,  0xb8,
        0xae,  0xa4,  0x9b,  0x92,  0x8a,  0x82,  0x7b,  0x74,  0x6d,  0x67,  0x61,  0x5c,  0x57,  0x52,  0x4d,  0x49,
        0x45,  0x41,  0x3d,  0x3a,  0x36,  0x33,  0x30,  0x2e,  0x2b,  0x29,  0x26,  0x24,  0x22,  0x20,  0x1e,  0x1d,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6c8, 0x668, 0x60c, 0x5b4, 0x562, 0x516, 0x4cc, 0x488,
        0x446, 0x408, 0x3ce, 0x398, 0x364, 0x334, 0x306, 0x2da, 0x2b1, 0x28b, 0x266, 0x244, 0x223, 0x204, 0x1e7, 0x1cc,
        0x1b2, 0x19a, 0x183, 0x16d, 0x159, 0x145, 0x133, 0x122, 0x112, 0x102, 0xf4,  0xe6,  0xd9,  0xcd,  0xc1,  0xb7,
        0xac,  0xa3,  0x9a,  0x91,  0x89,  0x81,  0x7a,  0x73,  0x6c,  0x66,  0x60,  0x5b,  0x56,  0x51,  0x4d,  0x48,
        0x44,  0x40,  0x3d,  0x39,  0x36,  0x33,  0x30,  0x2d,  0x2b,  0x28,  0x26,  0x24,  0x22,  0x20,  0x1e,  0x1c,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6bc, 0x65c, 0x600, 0x5aa, 0x558, 0x50c, 0x4c4, 0x47e,
        0x43e, 0x402, 0x3c8, 0x392, 0x35e, 0x32e, 0x300, 0x2d5, 0x2ac, 0x286, 0x262, 0x23f, 0x21f, 0x201, 0x1e4, 0x1c9,
        0x1af, 0x197, 0x180, 0x16b, 0x156, 0x143, 0x131, 0x120, 0x110, 0x100, 0xf2,  0xe4,  0xd8,  0xcb,  0xc0,  0xb5,
        0xab,  0xa1,  0x98,  0x90,  0x88,  0x80,  0x79,  0x72,  0x6c,  0x65,  0x60,  0x5a,  0x55,  0x50,  0x4c,  0x48,
        0x44,  0x40,  0x3c,  0x39,  0x36,  0x32,  0x30,  0x2d,  0x2a,  0x28,  0x26,  0x24,  0x22,  0x20,  0x1e,  0x1c,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6b0, 0x650, 0x5f4, 0x5a0, 0x54c, 0x500, 0x4b8, 0x474,
        0x434, 0x3f8, 0x3c0, 0x38a, 0x358, 0x328, 0x2fa, 0x2d0, 0x2a6, 0x280, 0x25c, 0x23a, 0x21a, 0x1fc, 0x1e0, 0x1c5,
        0x1ac, 0x194, 0x17d, 0x168, 0x153, 0x140, 0x12e, 0x11d, 0x10d, 0xfe,  0xf0,  0xe2,  0xd6,  0xca,  0xbe,  0xb4,
        0xaa,  0xa0,  0x97,  0x8f,  0x87,  0x7f,  0x78,  0x71,  0x6b,  0x65,  0x5f,  0x5a,  0x55,  0x50,  0x4b,  0x47,
        0x43,  0x3f,  0x3c,  0x38,  0x35,  0x32,  0x2f,  0x2d,  0x2a,  0x28,  0x25,  0x23,  0x21,  0x1f,  0x1e,  0x1c,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x6a4, 0x644, 0x5ea, 0x596, 0x544, 0x4fa, 0x4b2, 0x46e,
        0x42e, 0x3f2, 0x3ba, 0x384, 0x352, 0x322, 0x2f5, 0x2cb, 0x2a2, 0x27d, 0x259, 0x237, 0x217, 0x1f9, 0x1dd, 0x1c2,
        0x1a9, 0x191, 0x17b, 0x165, 0x151, 0x13e, 0x12c, 0x11c, 0x10c, 0xfd,  0xef,  0xe1,  0xd5,  0xc9,  0xbd,  0xb3,
        0xa9,  0x9f,  0x96,  0x8e,  0x86,  0x7e,  0x77,  0x71,  0x6a,  0x64,  0x5e,  0x59,  0x54,  0x4f,  0x4b,  0x47,
        0x43,  0x3f,  0x3b,  0x38,  0x35,  0x32,  0x2f,  0x2c,  0x2a,  0x27,  0x25,  0x23,  0x21,  0x1f,  0x1d,  0x1c,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x698, 0x638, 0x5e0, 0x58a, 0x53c, 0x4f0, 0x4aa, 0x466,
        0x428, 0x3ec, 0x3b4, 0x37e, 0x34c, 0x31c, 0x2f0, 0x2c5, 0x29e, 0x278, 0x255, 0x233, 0x214, 0x1f6, 0x1da, 0x1bf,
        0x1a6, 0x18e, 0x178, 0x163, 0x14f, 0x13c, 0x12a, 0x11a, 0x10a, 0xfb,  0xed,  0xe0,  0xd3,  0xc7,  0xbc,  0xb1,
        0xa7,  0x9e,  0x95,  0x8d,  0x85,  0x7d,  0x76,  0x70,  0x69,  0x63,  0x5e,  0x58,  0x53,  0x4f,  0x4a,  0x46,
        0x42,  0x3e,  0x3b,  0x38,  0x34,  0x31,  0x2f,  0x2c,  0x29,  0x27,  0x25,  0x23,  0x21,  0x1f,  0x1d,  0x1c,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x68c, 0x62e, 0x5d4, 0x580, 0x532, 0x4e8, 0x4a0, 0x45e,
        0x420, 0x3e4, 0x3ac, 0x378, 0x346, 0x317, 0x2ea, 0x2c0, 0x299, 0x274, 0x250, 0x22f, 0x210, 0x1f2, 0x1d6, 0x1bc,
        0x1a3, 0x18b, 0x175, 0x160, 0x14c, 0x13a, 0x128, 0x118, 0x108, 0xf9,  0xeb,  0xde,  0xd1,  0xc6,  0xbb,  0xb0,
        0xa6,  0x9d,  0x94,  0x8c,  0x84,  0x7d,  0x76,  0x6f,  0x68,  0x63,  0x5d,  0x58,  0x53,  0x4e,  0x4a,  0x46,
        0x42,  0x3e,  0x3b,  0x37,  0x34,  0x31,  0x2e,  0x2c,  0x29,  0x27,  0x25,  0x23,  0x21,  0x1f,  0x1d,  0x1b,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x680, 0x622, 0x5ca, 0x576, 0x528, 0x4de, 0x498, 0x456,
        0x418, 0x3de, 0x3a6, 0x372, 0x340, 0x311, 0x2e5, 0x2bb, 0x294, 0x26f, 0x24c, 0x22b, 0x20c, 0x1ef, 0x1d3, 0x1b9,
        0x1a0, 0x188, 0x172, 0x15e, 0x14a, 0x138, 0x126, 0x116, 0x106, 0xf7,  0xe9,  0xdc,  0xd0,  0xc4,  0xb9,  0xaf,
        0xa5,  0x9c,  0x93,  0x8b,  0x83,  0x7c,  0x75,  0x6e,  0x68,  0x62,  0x5c,  0x57,  0x52,  0x4e,  0x49,  0x45,
        0x41,  0x3e,  0x3a,  0x37,  0x34,  0x31,  0x2e,  0x2b,  0x29,  0x27,  0x24,  0x22,  0x20,  0x1f,  0x1d,  0x1b,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x674, 0x616, 0x5c0, 0x56c, 0x51e, 0x4d6, 0x490, 0x44e,
        0x410, 0x3d6, 0x39e, 0x36a, 0x33a, 0x30b, 0x2e0, 0x2b6, 0x28f, 0x26b, 0x248, 0x227, 0x208, 0x1eb, 0x1cf, 0x1b5,
        0x19d, 0x186, 0x170, 0x15b, 0x148, 0x135, 0x124, 0x114, 0x104, 0xf5,  0xe8,  0xdb,  0xce,  0xc3,  0xb8,  0xae,
        0xa4,  0x9b,  0x92,  0x8a,  0x82,  0x7b,  0x74,  0x6d,  0x67,  0x61,  0x5c,  0x57,  0x52,  0x4d,  0x49,  0x45,
        0x41,  0x3d,  0x3a,  0x36,  0x33,  0x30,  0x2e,  0x2b,  0x29,  0x26,  0x24,  0x22,  0x20,  0x1e,  0x1d,  0x1b,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x668, 0x60c, 0x5b4, 0x562, 0x516, 0x4cc, 0x488, 0x446,
        0x408, 0x3ce, 0x398, 0x364, 0x334, 0x306, 0x2da, 0x2b1, 0x28b, 0x266, 0x244, 0x223, 0x204, 0x1e7, 0x1cc, 0x1b2,
        0x19a, 0x183, 0x16d, 0x159, 0x145, 0x133, 0x122, 0x112, 0x102, 0xf4,  0xe6,  0xd9,  0xcd,  0xc1,  0xb7,  0xac,
        0xa3,  0x9a,  0x91,  0x89,  0x81,  0x7a,  0x73,  0x6d,  0x66,  0x60,  0x5b,  0x56,  0x51,  0x4d,  0x48,  0x44,
        0x40,  0x3d,  0x39,  0x36,  0x33,  0x30,  0x2d,  0x2b,  0x28,  0x26,  0x24,  0x22,  0x20,  0x1e,  0x1c,  0x1b,
    },
    {
        0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
        0,     0,     0,     0,     0,     0,     0,     0,     0x65c, 0x600, 0x5aa, 0x558, 0x50c, 0x4c4, 0x47e, 0x43e,
        0x402, 0x3c8, 0x392, 0x35e, 0x32e, 0x300, 0x2d5, 0x2ac, 0x286, 0x262, 0x23f, 0x21f, 0x201, 0x1e4, 0x1c9, 0x1af,
        0x197, 0x180, 0x16b, 0x156, 0x143, 0x131, 0x120, 0x110, 0x100, 0xf2,  0xe4,  0xd8,  0xcc,  0xc0,  0xb5,  0xab,
        0xa1,  0x98,  0x90,  0x88,  0x80,  0x79,  0x72,  0x6c,  0x66,  0x60,  0x5a,  0x55,  0x50,  0x4c,  0x48,  0x44,
        0x40,  0x3c,  0x39,  0x36,  0x33,  0x30,  0x2d,  0x2a,  0x28,  0x26,  0x24,  0x22,  0x20,  0x1e,  0x1c,  0x1b,
    },
};
// Playback rates of the linear note scale: 768 entries per octave.
const u32 gPeriodRates[768] = {
    0x82ac0, 0x828dd, 0x826fa, 0x82518, 0x82337, 0x82156, 0x81f75, 0x81d95, 0x81bb5, 0x819d6, 0x817f7, 0x81618, 0x8143a,
    0x8125c, 0x8107f, 0x80ea2, 0x80cc6, 0x80aea, 0x8090f, 0x80733, 0x80559, 0x8037f, 0x801a5, 0x7ffcc, 0x7fdf3, 0x7fc1a,
    0x7fa42, 0x7f86a, 0x7f693, 0x7f4bc, 0x7f2e6, 0x7f110, 0x7ef3a, 0x7ed65, 0x7eb91, 0x7e9bc, 0x7e7e9, 0x7e615, 0x7e442,
    0x7e270, 0x7e09d, 0x7decc, 0x7dcfa, 0x7db2a, 0x7d959, 0x7d789, 0x7d5b9, 0x7d3ea, 0x7d21b, 0x7d04d, 0x7ce7f, 0x7ccb2,
    0x7cae4, 0x7c918, 0x7c74b, 0x7c580, 0x7c3b4, 0x7c1e9, 0x7c01e, 0x7be54, 0x7bc8a, 0x7bac1, 0x7b8f8, 0x7b72f, 0x7b567,
    0x7b39f, 0x7b1d8, 0x7b011, 0x7ae4b, 0x7ac85, 0x7aabf, 0x7a8fa, 0x7a735, 0x7a570, 0x7a3ac, 0x7a1e9, 0x7a025, 0x79e62,
    0x79ca0, 0x79ade, 0x7991c, 0x7975b, 0x7959a, 0x793da, 0x7921a, 0x7905a, 0x78e9b, 0x78cdc, 0x78b1e, 0x78960, 0x787a2,
    0x785e5, 0x78429, 0x7826c, 0x780b0, 0x77ef5, 0x77d39, 0x77b7f, 0x779c4, 0x7780a, 0x77651, 0x77498, 0x772df, 0x77126,
    0x76f6e, 0x76db7, 0x76c00, 0x76a49, 0x76892, 0x766dc, 0x76527, 0x76372, 0x761bd, 0x76008, 0x75e54, 0x75ca1, 0x75aed,
    0x7593b, 0x75788, 0x755d6, 0x75424, 0x75273, 0x750c2, 0x74f12, 0x74d62, 0x74bb2, 0x74a03, 0x74854, 0x746a5, 0x744f7,
    0x74349, 0x7419c, 0x73fef, 0x73e42, 0x73c96, 0x73aea, 0x7393f, 0x73793, 0x735e9, 0x7343e, 0x73295, 0x730eb, 0x72f42,
    0x72d99, 0x72bf1, 0x72a49, 0x728a1, 0x726fa, 0x72553, 0x723ac, 0x72206, 0x72061, 0x71ebb, 0x71d16, 0x71b72, 0x719ce,
    0x7182a, 0x71686, 0x714e3, 0x71341, 0x7119e, 0x70ffc, 0x70e5b, 0x70cba, 0x70b19, 0x70979, 0x707d8, 0x70639, 0x7049a,
    0x702fb, 0x7015c, 0x6ffbe, 0x6fe20, 0x6fc83, 0x6fae6, 0x6f949, 0x6f7ad, 0x6f611, 0x6f475, 0x6f2da, 0x6f13f, 0x6efa5,
    0x6ee0b, 0x6ec71, 0x6ead8, 0x6e93f, 0x6e7a6, 0x6e60e, 0x6e476, 0x6e2de, 0x6e147, 0x6dfb1, 0x6de1a, 0x6dc84, 0x6daee,
    0x6d959, 0x6d7c4, 0x6d630, 0x6d49c, 0x6d308, 0x6d174, 0x6cfe1, 0x6ce4e, 0x6ccbc, 0x6cb2a, 0x6c998, 0x6c807, 0x6c676,
    0x6c4e6, 0x6c355, 0x6c1c5, 0x6c036, 0x6bea7, 0x6bd18, 0x6bb8a, 0x6b9fc, 0x6b86e, 0x6b6e1, 0x6b554, 0x6b3c7, 0x6b23b,
    0x6b0af, 0x6af23, 0x6ad98, 0x6ac0d, 0x6aa83, 0x6a8f9, 0x6a76f, 0x6a5e6, 0x6a45d, 0x6a2d4, 0x6a14c, 0x69fc4, 0x69e3c,
    0x69cb5, 0x69b2e, 0x699a7, 0x69821, 0x6969b, 0x69516, 0x69390, 0x6920c, 0x69087, 0x68f03, 0x68d7f, 0x68bfc, 0x68a79,
    0x688f6, 0x68774, 0x685f2, 0x68470, 0x682ef, 0x6816e, 0x67fed, 0x67e6d, 0x67ced, 0x67b6d, 0x679ee, 0x6786f, 0x676f1,
    0x67573, 0x673f5, 0x67277, 0x670fa, 0x66f7d, 0x66e01, 0x66c85, 0x66b09, 0x6698d, 0x66812, 0x66697, 0x6651d, 0x663a3,
    0x66229, 0x660b0, 0x65f37, 0x65dbe, 0x65c46, 0x65ace, 0x65956, 0x657df, 0x65668, 0x654f1, 0x6537a, 0x65204, 0x6508f,
    0x64f19, 0x64da4, 0x64c30, 0x64abb, 0x64947, 0x647d4, 0x64660, 0x644ee, 0x6437b, 0x64209, 0x64097, 0x63f25, 0x63db4,
    0x63c43, 0x63ad2, 0x63962, 0x637f2, 0x63682, 0x63513, 0x633a4, 0x63235, 0x630c7, 0x62f59, 0x62deb, 0x62c7e, 0x62b11,
    0x629a4, 0x62838, 0x626cc, 0x62560, 0x623f5, 0x6228a, 0x6211f, 0x61fb5, 0x61e4a, 0x61ce1, 0x61b77, 0x61a0e, 0x618a5,
    0x6173d, 0x615d5, 0x6146d, 0x61306, 0x6119f, 0x61038, 0x60ed1, 0x60d6b, 0x60c05, 0x60aa0, 0x6093b, 0x607d6, 0x60671,
    0x6050d, 0x603a9, 0x60246, 0x600e2, 0x5ff7f, 0x5fe1d, 0x5fcba, 0x5fb58, 0x5f9f7, 0x5f895, 0x5f734, 0x5f5d4, 0x5f473,
    0x5f313, 0x5f1b4, 0x5f054, 0x5eef5, 0x5ed96, 0x5ec38, 0x5eada, 0x5e97c, 0x5e81e, 0x5e6c1, 0x5e564, 0x5e408, 0x5e2ab,
    0x5e14f, 0x5dff4, 0x5de98, 0x5dd3d, 0x5dbe3, 0x5da88, 0x5d92e, 0x5d7d4, 0x5d67b, 0x5d522, 0x5d3c9, 0x5d270, 0x5d118,
    0x5cfc0, 0x5ce69, 0x5cd11, 0x5cbba, 0x5ca64, 0x5c90d, 0x5c7b7, 0x5c662, 0x5c50c, 0x5c3b7, 0x5c262, 0x5c10e, 0x5bfba,
    0x5be66, 0x5bd12, 0x5bbbf, 0x5ba6c, 0x5b919, 0x5b7c7, 0x5b675, 0x5b523, 0x5b3d2, 0x5b281, 0x5b130, 0x5afdf, 0x5ae8f,
    0x5ad3f, 0x5abef, 0x5aaa0, 0x5a951, 0x5a802, 0x5a6b4, 0x5a566, 0x5a418, 0x5a2cb, 0x5a17d, 0x5a030, 0x59ee4, 0x59d97,
    0x59c4b, 0x59b00, 0x599b4, 0x59869, 0x5971e, 0x595d4, 0x5948a, 0x59340, 0x591f6, 0x590ad, 0x58f64, 0x58e1b, 0x58cd3,
    0x58b8b, 0x58a43, 0x588fb, 0x587b4, 0x5866d, 0x58526, 0x583e0, 0x5829a, 0x58154, 0x5800f, 0x57ec9, 0x57d85, 0x57c40,
    0x57afc, 0x579b8, 0x57874, 0x57731, 0x575ed, 0x574ab, 0x57368, 0x57226, 0x570e4, 0x56fa2, 0x56e61, 0x56d20, 0x56bdf,
    0x56a9e, 0x5695e, 0x5681e, 0x566de, 0x5659f, 0x56460, 0x56321, 0x561e3, 0x560a4, 0x55f67, 0x55e29, 0x55cec, 0x55bae,
    0x55a72, 0x55935, 0x557f9, 0x556bd, 0x55581, 0x55446, 0x5530b, 0x551d0, 0x55096, 0x54f5b, 0x54e22, 0x54ce8, 0x54baf,
    0x54a75, 0x5493d, 0x54804, 0x546cc, 0x54594, 0x5445c, 0x54325, 0x541ee, 0x540b7, 0x53f80, 0x53e4a, 0x53d14, 0x53bde,
    0x53aa9, 0x53974, 0x5383f, 0x5370a, 0x535d6, 0x534a2, 0x5336e, 0x5323b, 0x53107, 0x52fd4, 0x52ea2, 0x52d6f, 0x52c3d,
    0x52b0c, 0x529da, 0x528a9, 0x52778, 0x52647, 0x52517, 0x523e6, 0x522b6, 0x52187, 0x52057, 0x51f28, 0x51dfa, 0x51ccb,
    0x51b9d, 0x51a6f, 0x51941, 0x51814, 0x516e6, 0x515ba, 0x5148d, 0x51361, 0x51235, 0x51109, 0x50fdd, 0x50eb2, 0x50d87,
    0x50c5c, 0x50b32, 0x50a07, 0x508de, 0x507b4, 0x5068b, 0x50561, 0x50439, 0x50310, 0x501e8, 0x500c0, 0x4ff98, 0x4fe70,
    0x4fd49, 0x4fc22, 0x4fafb, 0x4f9d5, 0x4f8af, 0x4f789, 0x4f663, 0x4f53e, 0x4f419, 0x4f2f4, 0x4f1cf, 0x4f0ab, 0x4ef87,
    0x4ee63, 0x4ed3f, 0x4ec1c, 0x4eaf9, 0x4e9d6, 0x4e8b4, 0x4e792, 0x4e670, 0x4e54e, 0x4e42d, 0x4e30b, 0x4e1ea, 0x4e0ca,
    0x4dfa9, 0x4de89, 0x4dd69, 0x4dc4a, 0x4db2a, 0x4da0b, 0x4d8ec, 0x4d7ce, 0x4d6af, 0x4d591, 0x4d474, 0x4d356, 0x4d239,
    0x4d11c, 0x4cfff, 0x4cee2, 0x4cdc6, 0x4ccaa, 0x4cb8e, 0x4ca73, 0x4c958, 0x4c83d, 0x4c722, 0x4c607, 0x4c4ed, 0x4c3d3,
    0x4c2ba, 0x4c1a0, 0x4c087, 0x4bf6e, 0x4be55, 0x4bd3d, 0x4bc25, 0x4bb0d, 0x4b9f5, 0x4b8de, 0x4b7c6, 0x4b6af, 0x4b599,
    0x4b482, 0x4b36c, 0x4b256, 0x4b141, 0x4b02b, 0x4af16, 0x4ae01, 0x4acec, 0x4abd8, 0x4aac4, 0x4a9b0, 0x4a89c, 0x4a789,
    0x4a676, 0x4a563, 0x4a450, 0x4a33e, 0x4a22b, 0x4a119, 0x4a008, 0x49ef6, 0x49de5, 0x49cd4, 0x49bc3, 0x49ab3, 0x499a3,
    0x49893, 0x49783, 0x49674, 0x49564, 0x49455, 0x49347, 0x49238, 0x4912a, 0x4901c, 0x48f0e, 0x48e01, 0x48cf3, 0x48be6,
    0x48ad9, 0x489cd, 0x488c1, 0x487b4, 0x486a9, 0x4859d, 0x48492, 0x48387, 0x4827c, 0x48171, 0x48067, 0x47f5d, 0x47e53,
    0x47d49, 0x47c40, 0x47b36, 0x47a2d, 0x47925, 0x4781c, 0x47714, 0x4760c, 0x47504, 0x473fd, 0x472f6, 0x471ef, 0x470e8,
    0x46fe1, 0x46edb, 0x46dd5, 0x46ccf, 0x46bc9, 0x46ac4, 0x469bf, 0x468ba, 0x467b5, 0x466b1, 0x465ad, 0x464a9, 0x463a5,
    0x462a1, 0x4619e, 0x4609b, 0x45f98, 0x45e96, 0x45d93, 0x45c91, 0x45b8f, 0x45a8e, 0x4598c, 0x4588b, 0x4578a, 0x4568a,
    0x45589, 0x45489, 0x45389, 0x45289, 0x45189, 0x4508a, 0x44f8b, 0x44e8c, 0x44d8e, 0x44c8f, 0x44b91, 0x44a93, 0x44995,
    0x44898, 0x4479b, 0x4469e, 0x445a1, 0x444a4, 0x443a8, 0x442ac, 0x441b0, 0x440b4, 0x43fb9, 0x43ebe, 0x43dc3, 0x43cc8,
    0x43bcd, 0x43ad3, 0x439d9, 0x438df, 0x437e5, 0x436ec, 0x435f3, 0x434fa, 0x43401, 0x43309, 0x43210, 0x43118, 0x43021,
    0x42f29, 0x42e32, 0x42d3a, 0x42c44, 0x42b4d, 0x42a56, 0x42960, 0x4286a, 0x42774, 0x4267f, 0x42589, 0x42494, 0x4239f,
    0x422aa, 0x421b6, 0x420c2, 0x41fce, 0x41eda, 0x41de6, 0x41cf3, 0x41c00, 0x41b0d, 0x41a1a, 0x41927, 0x41835, 0x41743,
    0x41651,
};
// Volume levels for the sound command, indexed 0..64.
const s32 gVolumeTable[65] = {
    -723, -361, -301, -266, -241, -221, -206, -192, -181, -170, -161, -153, -145, -138, -132, -126, -120,
    -115, -110, -105, -101, -97,  -93,  -89,  -85,  -82,  -78,  -75,  -72,  -69,  -66,  -63,  -60,  -58,
    -55,  -52,  -50,  -48,  -45,  -43,  -41,  -39,  -37,  -35,  -33,  -31,  -29,  -27,  -25,  -23,  -21,
    -20,  -18,  -16,  -15,  -13,  -12,  -10,  -9,   -7,   -6,   -4,   -3,   -1,   0,
};
// One period of a sine wave, 64 entries with amplitude 255; every channel points at it twice.
const s16 gSineWave[64] = {
    0,    0x18, 0x31, 0x4a, 0x61, 0x78, 0x8d, 0xa1, 0xb4, 0xc5, 0xd4, 0xe0, 0xeb, 0xf4, 0xfa, 0xfd,
    0xff, 0xfd, 0xfa, 0xf4, 0xeb, 0xe0, 0xd4, 0xc5, 0xb4, 0xa1, 0x8d, 0x78, 0x61, 0x4a, 0x31, 0x18,
    0,    -24,  -49,  -74,  -97,  -120, -141, -161, -180, -197, -212, -224, -235, -244, -250, -253,
    -255, -253, -250, -244, -235, -224, -212, -197, -180, -161, -141, -120, -97,  -74,  -49,  -24,
};
// A curve falling from 255 to -255 over 64 entries.
const s16 gFallingWave[64] = {
    0xff, 0xfe, 0xfd, 0xfb, 0xfa, 0xf7, 0xf4, 0xef, 0xeb, 0xe5, 0xe0, 0xda, 0xd4, 0xcc, 0xc5, 0xbc,
    0xb4, 0xaa, 0xa1, 0x97, 0x8d, 0x82, 0x78, 0x6c, 0x61, 0x55, 0x4a, 0x3d, 0x31, 0x24, 0x18, 0xc,
    0,    -12,  -24,  -36,  -49,  -61,  -74,  -85,  -97,  -108, -120, -130, -141, -151, -161, -170,
    -180, -188, -197, -204, -212, -218, -224, -229, -235, -239, -244, -247, -250, -251, -253, -254,
};
// A square wave: 32 entries of -255, then 32 of 255.
const s16 gSquareWave[64] = {
    -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255,
    -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

// Interrupt mask bit of the timer that drives the player.
#define TRACKER_IRQ_TIMER2 0x20

TrackerPlayer gTrackerPlayer;

// Wave shapes the vibrato and tremolo effects select (low three bits of the parameter).
const s16 *gEffectWaves[8] = {gSineWave, gFallingWave, gSquareWave, gSineWave,
                              gSineWave, gFallingWave, gSquareWave, gSineWave};

#pragma thumb off

// Loads the song in file fileId and starts it with volume (at most 0x80; 0 while gSystemState.mUseTrackerVolume is 0).
// The song already loaded, with the global fade at TRACKER_GLOBAL_FADE_NONE and restart 0, only takes the new volume.
// sampleFileBase is the number added to each sample name's number to get the sample's file id (0 keeps the last one);
// startOrder goes to StartPlayback. With fadeFirst == 1 while a song plays the request is stored and the global fade
// started (TRACKER_GLOBAL_FADE_TO_PENDING), for UpdateTracker to play once it is done. A new song's file gives the
// header bytes, the order table, the patterns (a cell byte with bit 7 set says which of the five bytes follow; the
// others become 0xff, 0, 0, 0xff, 0), the instruments with their two envelopes, and their samples, whose data is loaded
// by the number in the sample's name.
void PlayTrack(u16 fileId, u16 sampleFileBase, u8 volume, u16 startOrder, int fadeFirst, int restart) {
    u8 name[0x17];
    u8 *patterns;
    TrackerInstrument *instruments;
    u8 *next;
    u32 size;
    unsigned long i; // unsigned long, not u32: the compiler's output needs it (register choice)
    u8 *src;
    u8 *pattern;
    u32 j;
    TrackerSample *samples;
    u32 left;

    if (volume > 0x80) {
        OS_Terminate();
    }
    if (gSystemState.mUseTrackerVolume == 0) {
        volume = 0;
    }
    if (fileId == gTrackerPlayer.mTrackFileId && gTrackerPlayer.mGlobalFadeState == TRACKER_GLOBAL_FADE_NONE &&
        restart == 0) {
        SetGlobalVolume(volume, 0);
        return;
    }
    gTrackerPlayer.mLoading = 1;
    if (sampleFileBase == 0) {
        if (!(gSystemState.mFlags & 0x1000000)) {
            OS_Terminate();
        }
        sampleFileBase = gTrackerPlayer.mSampleFileBase;
    } else {
        gTrackerPlayer.mSampleFileBase = sampleFileBase;
    }
    if (gTrackerPlayer.mPlaying == 1 && fadeFirst == 1) {
        if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_DONE) {
            gTrackerPlayer.mGlobalFadeState = TRACKER_GLOBAL_FADE_TO_PENDING;
        }
        gTrackerPlayer.mPendingTrackFileId = fileId;
        if (sampleFileBase != 0) {
            gTrackerPlayer.mPendingSampleFileBase = sampleFileBase;
        }
        gTrackerPlayer.mPendingVolume = volume;
        gTrackerPlayer.mPendingStartOrder = startOrder;
        gTrackerPlayer.mPendingRestart = restart;
        gTrackerPlayer.mLoading = 0;
        return;
    }
    gTrackerPlayer.mGlobalFadeState = TRACKER_GLOBAL_FADE_NONE;
    if (fileId != gTrackerPlayer.mTrackFileId) {
        ReleaseTrack(0);
    }
    if (gTrackerPlayer.mPlaying == 1) {
        StopPlayback();
    }
    if (gTrackerPlayer.mTrackFileId == 0) {
        if (fileId == 0) {
            OS_Terminate();
        }
        gTrackerPlayer.mTrackFileId = fileId;
        src = FindOrLoadCachedFile(gTrackerPlayer.mTrackFileId);
        if (src == NULL) {
            OS_Terminate();
        }
        gTrackerPlayer.mOrderCount = (src[0x41] << 8) | src[0x40];
        gTrackerPlayer.mLoopOrder = (src[0x43] << 8) | src[0x42];
        gTrackerPlayer.mChannelCount = (src[0x45] << 8) | src[0x44];
        gTrackerPlayer.mPatternCount = (src[0x47] << 8) | src[0x46];
        gTrackerPlayer.mInstrumentCount = (src[0x49] << 8) | src[0x48];
        gTrackerPlayer.mFlags = (src[0x4b] << 8) | src[0x4a];
        gTrackerPlayer.mInitialTicksPerRow = (src[0x4d] << 8) | src[0x4c];
        gTrackerPlayer.mInitialTickRate = (src[0x4f] << 8) | src[0x4e];
        src += 0x50;
        for (i = 0; i < TRACKER_ORDER_COUNT; i++) {
            gTrackerPlayer.mOrders[i] = *src++;
        }

        patterns = Alloc(gTrackerPlayer.mPatternCount * sizeof(TrackerPattern));
        gTrackerPlayer.mPatternBlock = (TrackerPattern *) patterns;
        if (patterns == NULL) {
            OS_Terminate();
        }
        for (i = 0; i < gTrackerPlayer.mPatternCount; i++) {
            u32 row;
            u32 channel;

            gTrackerPlayer.mPatterns[i] = (TrackerPattern *) patterns;
            patterns += sizeof(TrackerPattern);
            gTrackerPlayer.mPatterns[i]->mUnk_00 = src[4];
            gTrackerPlayer.mPatterns[i]->mRowCount = (src[6] << 8) | src[5];
            size = (src[8] << 8) | src[7];
            src += 9;
            next = src;
            gTrackerPlayer.mPatterns[i]->mCells =
                Alloc(gTrackerPlayer.mChannelCount * gTrackerPlayer.mPatterns[i]->mRowCount * 5);
            if (gTrackerPlayer.mPatterns[i]->mCells == NULL) {
                OS_Terminate();
            }
            pattern = gTrackerPlayer.mPatterns[i]->mCells;
            for (row = 0; row < gTrackerPlayer.mPatterns[i]->mRowCount; row++) {
                for (channel = 0; channel < gTrackerPlayer.mChannelCount; channel++) {
                    u8 flags = *src++;

                    if (flags & 0x80) {
                        if (flags & 1) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0xff;
                        }
                        if (flags & 2) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0;
                        }
                        if (flags & 4) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0;
                        }
                        if (flags & 8) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0xff;
                        }
                        if (flags & 0x10) {
                            *pattern++ = *src++;
                        } else {
                            *pattern++ = 0;
                        }
                    } else {
                        pattern[0] = flags;
                        pattern[1] = src[0];
                        pattern[2] = src[1];
                        pattern[3] = src[2];
                        pattern[4] = src[3];
                        src += 4;
                        pattern += 5;
                    }
                }
            }
            src = next + size;
        }

        instruments = Alloc(gTrackerPlayer.mInstrumentCount * sizeof(TrackerInstrument));
        gTrackerPlayer.mInstrumentBlock = instruments;
        if (instruments == NULL) {
            OS_Terminate();
        }
        left = gTrackerPlayer.mInstrumentCount * 2;
        samples = Alloc(left * sizeof(TrackerSample));
        gTrackerPlayer.mSampleBlock = samples;
        if (samples == NULL) {
            OS_Terminate();
        }
        for (i = 0; i < gTrackerPlayer.mInstrumentCount; i++) {
            u8 *start;
            u32 header;

            start = src;
            gTrackerPlayer.mInstruments[i] = instruments;
            instruments++;
            header = (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
            gTrackerPlayer.mInstruments[i]->mSampleCount = (src[0x1c] << 8) | src[0x1b];
            if (gTrackerPlayer.mInstruments[i]->mSampleCount != 0) {
                src += 0x21;
                for (j = 0; j < 0x60; j++) {
                    gTrackerPlayer.mInstruments[i]->mNoteSamples[j] = *src++;
                }
                for (j = 0; j < 12; j++) {
                    gTrackerPlayer.mInstruments[i]->mVolumeEnvelope.mPointTickPos[j] = (src[1] << 8) | src[0];
                    gTrackerPlayer.mInstruments[i]->mVolumeEnvelope.mPointValues[j] = (src[3] << 8) | src[2];
                    src += 4;
                }
                for (j = 0; j < 12; j++) {
                    gTrackerPlayer.mInstruments[i]->mPanEnvelope.mPointTickPos[j] = (src[1] << 8) | src[0];
                    gTrackerPlayer.mInstruments[i]->mPanEnvelope.mPointValues[j] = (src[3] << 8) | src[2];
                    src += 4;
                }
                gTrackerPlayer.mInstruments[i]->mVolumeEnvelope.mPointCount = src[0];
                gTrackerPlayer.mInstruments[i]->mPanEnvelope.mPointCount = src[1];
                gTrackerPlayer.mInstruments[i]->mVolumeEnvelope.mSustainPoint = src[2];
                gTrackerPlayer.mInstruments[i]->mVolumeEnvelope.mLoopStartPoint = src[3];
                gTrackerPlayer.mInstruments[i]->mVolumeEnvelope.mLoopEndPoint = src[4];
                gTrackerPlayer.mInstruments[i]->mPanEnvelope.mSustainPoint = src[5];
                gTrackerPlayer.mInstruments[i]->mPanEnvelope.mLoopStartPoint = src[6];
                gTrackerPlayer.mInstruments[i]->mPanEnvelope.mLoopEndPoint = src[7];
                gTrackerPlayer.mInstruments[i]->mVolumeEnvelope.mFlags = src[8];
                gTrackerPlayer.mInstruments[i]->mPanEnvelope.mFlags = src[9];
                gTrackerPlayer.mInstruments[i]->mUnk_2cd = src[10];
                gTrackerPlayer.mInstruments[i]->mUnk_2ce = src[11];
                gTrackerPlayer.mInstruments[i]->mUnk_2cf = src[12];
                gTrackerPlayer.mInstruments[i]->mUnk_2d0 = src[13];
                gTrackerPlayer.mInstruments[i]->mFadeOutPerTick = ((src[15] << 8) | src[14]) * 2;
                src += 16;
                src += header - (src - start);
                for (j = 0; j < gTrackerPlayer.mInstruments[i]->mSampleCount; j++) {
                    if (left == 0) {
                        OS_Terminate();
                    }
                    left--;
                    gTrackerPlayer.mInstruments[i]->mSamples[j] = samples;
                    samples++;
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mLength =
                        (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mLoopStart =
                        (src[7] << 24) | (src[6] << 16) | (src[5] << 8) | src[4];
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mLoopLength =
                        (src[11] << 24) | (src[10] << 16) | (src[9] << 8) | src[8];
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mVolume = src[12];
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mFinetune = src[13];
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mFlags = src[14];
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mPan = src[15];
                    gTrackerPlayer.mInstruments[i]->mSamples[j]->mNoteOffset = src[16];
                    if (gTrackerPlayer.mInstruments[i]->mSamples[j]->mLength != 0) {
                        MI_CpuCopy8(src + 0x12, name, 0x16);
                        name[0x16] = 0;
                        gTrackerPlayer.mInstruments[i]->mSamples[j]->mFileId = AddFirstDecimal(name, sampleFileBase);
                        gTrackerPlayer.mInstruments[i]->mSamples[j]->mData =
                            LoadFile(gTrackerPlayer.mInstruments[i]->mSamples[j]->mFileId);
                    }
                    src += 0x28;
                }
            } else {
                src += header - (src - start);
            }
        }
    }
    gTrackerPlayer.mFirstChannel = 16 - gTrackerPlayer.mChannelCount;
    gTrackerPlayer.mFramesSinceIrq = 0;
    gSystemState.mFlags |= 0x1000000;
    gWavePlayer.mRequestedChannelCount = gTrackerPlayer.mFirstChannel;
    RefreshWaveChannelCount();
    StartPlayback(volume, (s16) startOrder);
    gTrackerPlayer.mLoading = 0;
}

// Releases everything the loaded song allocated and resets the player; with fadeOut == 1 while a song plays it only
// starts the global fade (TRACKER_GLOBAL_FADE_TO_RELEASE), except in TRACKER_GLOBAL_FADE_DONE, where it releases at
// once.
void ReleaseTrack(int fadeOut) {
    u32 i;
    u32 j;

    gTrackerPlayer.mReleasing = 1;
    if (gTrackerPlayer.mPlaying == 1 && fadeOut == 1) {
        if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_DONE) {
            gTrackerPlayer.mGlobalFadeState = TRACKER_GLOBAL_FADE_TO_RELEASE;
            gTrackerPlayer.mReleasing = 0;
            return;
        }
    } else {
        gTrackerPlayer.mGlobalFadeState = TRACKER_GLOBAL_FADE_NONE;
    }
    if (gTrackerPlayer.mPlaying == 1) {
        StopPlayback();
    }
    for (i = 0; i < TRACKER_INSTRUMENT_COUNT; i++) {
        if (gTrackerPlayer.mInstruments[i] != 0) {
            for (j = 0; j < gTrackerPlayer.mInstruments[i]->mSampleCount; j++) {
                if (gTrackerPlayer.mInstruments[i]->mSamples[j]->mData != 0) {
                    Free(&gTrackerPlayer.mInstruments[i]->mSamples[j]->mData);
                }
                gTrackerPlayer.mInstruments[i]->mSamples[j] = 0;
            }
            gTrackerPlayer.mInstruments[i] = 0;
        }
    }
    if (gTrackerPlayer.mSampleBlock != 0) {
        Free(&gTrackerPlayer.mSampleBlock);
    }
    if (gTrackerPlayer.mInstrumentBlock != 0) {
        Free(&gTrackerPlayer.mInstrumentBlock);
    }
    for (i = 0; i < TRACKER_PATTERN_COUNT; i++) {
        if (gTrackerPlayer.mPatterns[i] != 0) {
            Free(&gTrackerPlayer.mPatterns[i]->mCells);
            gTrackerPlayer.mPatterns[i] = 0;
        }
    }
    if (gTrackerPlayer.mPatternBlock != 0) {
        Free(&gTrackerPlayer.mPatternBlock);
    }
    gTrackerPlayer.mGlobalFadeState = TRACKER_GLOBAL_FADE_NONE;
    gTrackerPlayer.mTrackFileId = 0;
    gTrackerPlayer.mFirstChannel = 0;
    gTrackerPlayer.mGlobalVolume = -128;
    gTrackerPlayer.mPlaying = 0;
    gTrackerPlayer.mOrderCount = 0;
    gTrackerPlayer.mLoopOrder = 0;
    gTrackerPlayer.mChannelCount = 0;
    gTrackerPlayer.mPatternCount = 0;
    gTrackerPlayer.mInstrumentCount = 0;
    gTrackerPlayer.mFlags = 0;
    gTrackerPlayer.mInitialTicksPerRow = 0;
    gTrackerPlayer.mInitialTickRate = 0;
    for (i = 0; i < TRACKER_ORDER_COUNT; i++) {
        gTrackerPlayer.mOrders[i] = 0;
    }
    gTrackerPlayer.mFramesSinceIrq = 0;
    gSystemState.mFlags &= ~0x1000000;
    gTrackerPlayer.mReleasing = 0;
}

// Returns base plus the first decimal number found in text.
u16 AddFirstDecimal(const u8 *text, int base) {
    int value = 0;
    int start = 0;
    int end;
    int scale;
    u32 c;

    while ((c = text[start]) != 0 && (c < '0' || c > '9')) {
        start++;
    }
    if (c == 0) {
        OS_Terminate();
    }
    end = start + 1;
    while (text[end] != 0 && text[end] >= '0' && text[end] <= '9') {
        end++;
    }
    end--;
    scale = 1;
    while (end >= start) {
        value += scale * (text[end] - '0');
        scale *= 10;
        end--;
    }
    return base + value;
}

// Resets the 32 channel states and the timing state for the loaded song, then arms the timer interrupt
// and calls the player's callback with 0.
void StartPlayback(u8 volume, s16 startOrder) {
    u32 mask = 0;
    u32 i;
    u32 k;

    if (gTrackerPlayer.mChannelCount == 0) {
        OS_Terminate();
    }
    func_020402f0();
    for (i = 0; i < gTrackerPlayer.mChannelCount; i++) {
        mask |= 1 << (gTrackerPlayer.mFirstChannel + i);
    }
    func_02040198(mask, 0);
    gTrackerPlayer.mGlobalVolume = volume;
    for (i = 0; i < TRACKER_CHANNEL_COUNT; i++) {
        TrackerChannel *c = &gTrackerPlayer.mChannels[i];
        c->mNoteOff = 0;
        c->mNote = 255;
        c->mInstrumentHandle = 0;
        c->mVolumeColumnEffect = 255;
        c->mEffect = (u8) c->mVolumeColumnEffect;
        c->mVolumeColumnEffectParam = 0;
        c->mSample = 0;
        c->mPeriod = 0;
        c->mArpeggioTicks = 0;
        c->mWaveIndex = 0;
        c->mTargetPeriod = 0;
        c->mToneSlidePerTick = 0;
        c->mVibratoPerTick = 0;
        c->mVibratoDepth = 0;
        c->mVibratoWave = gSineWave;
        c->mTremoloPerTick = 0;
        c->mTremoloDepth = 0;
        c->mTremoloWave = gSineWave;
        c->mVolume = 0;
        c->mVolumeScale = 128;
        c->mPan = 128;
        c->mFadeOutVolume = 0x10000;
        for (k = 0; k < 14; k++) {
            c->mLastParams[k] = 1;
        }
    }
    gTrackerPlayer.mTicksPerRow = gTrackerPlayer.mInitialTicksPerRow;
    gTrackerPlayer.mTickIndex = gTrackerPlayer.mInitialTicksPerRow - 1;
    gTrackerPlayer.mRowDelayTickCount = 0;
    gTrackerPlayer.mTickRate = gTrackerPlayer.mInitialTickRate;
    gTrackerPlayer.mTickLength = 8192000 / (gTrackerPlayer.mInitialTickRate * 50);
    gTrackerPlayer.mTickTime = 0;
    gTrackerPlayer.mOrderIndex = startOrder - 1;
    gTrackerPlayer.mJumpOrderIndex = -1;
    gTrackerPlayer.mPatternIndex = 255;
    gTrackerPlayer.mLastRow = 0;
    gTrackerPlayer.mRowIndex = 1;
    gTrackerPlayer.mPlaying = 1;
    gTrackerPlayer.mPaused = 0;
    func_02036194();
    StartTrackerTimer();
    if (gTrackerPlayer.mCallback != 0) {
        gTrackerPlayer.mCallback(0);
    }
}

// Stops the timer and the hardware channels, then calls the player's callback with 1.
void StopPlayback(void) {
    StopTrackerTimer();
    StopTrackerChannels();
    gTrackerPlayer.mPlaying = 0;
    if (gTrackerPlayer.mCallback != 0) {
        gTrackerPlayer.mCallback(1);
    }
}

// Sends the stop command to each hardware channel of the song, then sends the combined mask.
void StopTrackerChannels(void) {
    u32 mask = 0;
    u32 i;

    for (i = 0; i < gTrackerPlayer.mChannelCount; i++) {
        func_020400f4(1 << (gTrackerPlayer.mFirstChannel + i), 0, 0, 0);
        while (SND_RecvCommandReply(0) != 0) {
        }
        SND_FlushCommand(2);
    }
    for (i = 0; i < gTrackerPlayer.mChannelCount; i++) {
        mask |= 1 << (gTrackerPlayer.mFirstChannel + i);
    }
    func_020401b8(mask, 0);
}

// Installs the timer interrupt handler and starts the timer at the rate the flags select; with
// TRACKER_TIMER_AUTO_FINE it first sets TRACKER_TIMER_FINE unless the tick rate is 125, and clears it otherwise.
void StartTrackerTimer(void) {
    OS_SetIrqFunction(TRACKER_IRQ_TIMER2, HandleTrackerIrq);
    OS_EnableIrqMask(TRACKER_IRQ_TIMER2);
    if (gTrackerPlayer.mTimerFlags & TRACKER_TIMER_AUTO_FINE) {
        if (gTrackerPlayer.mTickRate != 125) {
            gTrackerPlayer.mTimerFlags |= TRACKER_TIMER_FINE;
        } else {
            gTrackerPlayer.mTimerFlags &= ~TRACKER_TIMER_FINE;
        }
    }
    if (gTrackerPlayer.mTimerFlags & TRACKER_TIMER_FINE) {
        gTrackerPlayer.mTimePerIrq = 0x147;
        func_0203f004(2, 654, 2);
    } else {
        gTrackerPlayer.mTimePerIrq = 0x51e;
        func_0203f004(2, 0xa3a, 2);
    }
}

void StopTrackerTimer(void) {
    func_0203f024(2);
}

// Timer interrupt handler. With the global fade at TRACKER_GLOBAL_FADE_NONE it runs AdvanceTracker; in
// TRACKER_GLOBAL_FADE_TO_PENDING and TRACKER_GLOBAL_FADE_TO_RELEASE it lowers the global volume by 4 and runs
// AdvanceTracker until the volume is below 4, then TO_PENDING becomes TRACKER_GLOBAL_FADE_DONE and TO_RELEASE calls
// the player's callback with 3 and releases the song; either way the timer is left stopped.
void HandleTrackerIrq(void) {
    gTrackerPlayer.mFramesSinceIrq = 0;
    if (gTrackerPlayer.mPaused == 1) {
        *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
        StopTrackerTimer();
        StartTrackerTimer();
        return;
    }
    if (gTrackerPlayer.mPlaying == 1 && gTrackerPlayer.mLoading == 0 && gTrackerPlayer.mReleasing == 0) {
        u8 fadeState = gTrackerPlayer.mGlobalFadeState;
        u8 vol;

        if (fadeState == TRACKER_GLOBAL_FADE_NONE) {
            *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
            StopTrackerTimer();
            AdvanceTracker();
            StartTrackerTimer();
            return;
        }
        // Only TRACKER_GLOBAL_FADE_TO_PENDING and TRACKER_GLOBAL_FADE_TO_RELEASE (fading) go on.
        if ((u8) (fadeState + 255) > 1) {
            return;
        }
        *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
        StopTrackerTimer();
        vol = GetGlobalVolume();
        if (vol >= 4) {
            SetGlobalVolume(vol - 4, 1);
            AdvanceTracker();
            StartTrackerTimer();
            return;
        }
        if (gTrackerPlayer.mGlobalFadeState == TRACKER_GLOBAL_FADE_TO_PENDING) {
            gTrackerPlayer.mGlobalFadeState = TRACKER_GLOBAL_FADE_DONE;
            return;
        }
        if (gTrackerPlayer.mCallback != 0) {
            gTrackerPlayer.mCallback(3);
        }
        ReleaseTrack(0);
    }
}

// Frame update: while a global fade state is set, counts the frames since the last timer interrupt and releases the
// song after more than 8; in TRACKER_GLOBAL_FADE_DONE calls the player's callback with 2 and plays the pending request.
void UpdateTracker(void) {
    if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_NONE) {
        gTrackerPlayer.mFramesSinceIrq++;
        if (gTrackerPlayer.mFramesSinceIrq > 8) {
            ReleaseTrack(0);
        }
    }
    if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_DONE) {
        return;
    }
    if (gTrackerPlayer.mCallback != 0) {
        gTrackerPlayer.mCallback(2);
    }
    PlayTrack(gTrackerPlayer.mPendingTrackFileId, gTrackerPlayer.mPendingSampleFileBase, gTrackerPlayer.mPendingVolume,
              gTrackerPlayer.mPendingStartOrder, 0, gTrackerPlayer.mPendingRestart);
}

void func_02036194(void) {}

// Player tick, run from the timer interrupt. Each time the tick accumulator passes the tick length the tick counter
// advances: on the first tick of a row the row's cells are read (note, instrument, volume column, effect, parameter;
// the volume column is applied as a second effect), the samples of new notes are started and the first-tick effects
// applied; on the other ticks the running effects and envelopes update period, volume and pan. The changes go to the
// sound driver as channel commands. With flag 0x400 of gSystemState.mUnk_2c8 each channel's effect parameter is
// formatted for display.
void AdvanceTracker(void) {
    s8 globalVol;
    char text[3];
    BOOL volChanged;
    u8 *cell;
    s8 nextRow;
    u32 i;
    u8 volume;
    u8 cellEffect;
    BOOL trigger;
    BOOL periodUpd;
    BOOL panUpd;
    BOOL notePresent;
    BOOL instPresent;
    BOOL instChanged;
    BOOL porta;
    u32 sampleOffset;
    BOOL tickPanUpd;
    u32 tickK;
    u8 note;
    u8 inst;
    u8 vol;
    u8 cellParam;
    u16 param;
    u16 hi;
    int lo;
    u16 period;
    BOOL volUpd;
    u8 effect;
    TrackerChannel *c;
    u32 k;

    volChanged = FALSE;
    globalVol = gTrackerPlayer.mGlobalVolume;
    gTrackerPlayer.mTickTime += gTrackerPlayer.mTimePerIrq;
    if (gTrackerPlayer.mTickTime >= gTrackerPlayer.mTickLength) {
        gTrackerPlayer.mTickTime -= gTrackerPlayer.mTickLength;
        nextRow = -1;
        gTrackerPlayer.mTickIndex++;
        if (gTrackerPlayer.mTickIndex >= gTrackerPlayer.mTicksPerRow + gTrackerPlayer.mRowDelayTickCount) {
            gTrackerPlayer.mTickIndex = 0;
        }
        if (gTrackerPlayer.mTickIndex == 0) {
            gTrackerPlayer.mRowDelayTickCount = 0;
            if (gTrackerPlayer.mRowIndex > gTrackerPlayer.mLastRow) {
                gTrackerPlayer.mRowIndex = 0;
                AdvanceOrder();
            }
            // Five bytes per cell, one cell per channel.
            cell = (u8 *) gTrackerPlayer.mPatterns[gTrackerPlayer.mPatternIndex]->mCells +
                   gTrackerPlayer.mRowIndex * gTrackerPlayer.mChannelCount * 5;
            for (i = 0; i < gTrackerPlayer.mChannelCount; i++) {
                c = &gTrackerPlayer.mChannels[i];
                trigger = FALSE;
                c->mStartKind = 0;
                effect = TRACKER_FX_NONE;
                note = cell[0];
                periodUpd = FALSE;
                volUpd = FALSE;
                panUpd = FALSE;
                notePresent = FALSE;
                instPresent = FALSE;
                instChanged = FALSE;
                porta = FALSE;
                sampleOffset = 0;
                if (note != TRACKER_NOTE_OFF) {
                    inst = cell[1];
                    vol = cell[2];
                    cellEffect = cell[3];
                    cellParam = cell[4];
                    effect = cellEffect;
                    param = cellParam;
                    if (cellEffect != TRACKER_FX_NONE) {
                        hi = (param & 0xf0) >> 4;
                        lo = param & 0xf;
                        switch (cellEffect) {
                            case TRACKER_FX_ORDER_JUMP:
                                if (gTrackerPlayer.mCallback != 0 && cellParam <= gTrackerPlayer.mOrderIndex) {
                                    gTrackerPlayer.mCallback(4);
                                }
                                gTrackerPlayer.mRowIndex = gTrackerPlayer.mLastRow;
                                gTrackerPlayer.mOrderIndex = cellParam - 1;
                                break;
                            case TRACKER_FX_ROW_BREAK:
                                param = hi * 10 + lo;
                                if (param < gTrackerPlayer.mLastRow) {
                                    nextRow = param;
                                } else {
                                    nextRow = 0;
                                }
                                break;
                            case TRACKER_FX_SPEED:
                                if (cellParam < 32) {
                                    if (cellParam != 0) {
                                        gTrackerPlayer.mTicksPerRow = cellParam;
                                    }
                                } else {
                                    gTrackerPlayer.mTickRate = cellParam;
                                    gTrackerPlayer.mTickLength = 8192000 / (gTrackerPlayer.mTickRate * 50);
                                }
                                break;
                        }
                    }
                    if (inst != 0) {
                        if (inst > gTrackerPlayer.mInstrumentCount) {
                            OS_Terminate();
                        }
                        if (gTrackerPlayer.mInstruments[inst - 1]->mSampleCount == 0) {
                            OS_Terminate();
                        }
                        instPresent = TRUE;
                        if (c->mInstrumentHandle != inst || c->mNoteOff == 1) {
                            c->mInstrumentHandle = inst;
                            instChanged = TRUE;
                        }
                        c->mWaveIndex = 0;
                        trigger = TRUE;
                    } else {
                        inst = c->mInstrumentHandle;
                    }
                    if (note == TRACKER_NOTE_NONE) {
                        note = c->mNote;
                    } else {
                        c->mNote = note;
                        notePresent = TRUE;
                        c->mWaveIndex = 0;
                        trigger = notePresent;
                    }
                    if (note != TRACKER_NOTE_NONE && inst != 0 && inst <= gTrackerPlayer.mInstrumentCount) {
                        if (cellEffect == TRACKER_FX_EXTENDED && lo == TRACKER_FXE_FINETUNE) {
                            TrackerInstrument *in = gTrackerPlayer.mInstruments[inst - 1];

                            in->mSamples[in->mNoteSamples[note - 1]]->mFinetune = lo;
                        }
                        if (trigger == TRUE) {
                            inst--;
                            note--;
                            if (gTrackerPlayer.mInstruments[inst]->mNoteSamples[note] >=
                                gTrackerPlayer.mInstruments[inst]->mSampleCount) {
                                OS_Terminate();
                            }
                            c->mSample = gTrackerPlayer.mInstruments[inst]
                                             ->mSamples[gTrackerPlayer.mInstruments[inst]->mNoteSamples[note]];
                            if (c->mSample == 0) {
                                OS_Terminate();
                            }
                            if (instPresent == TRUE) {
                                volUpd = TRUE;
                                panUpd = TRUE;
                                c->mVolume = c->mSample->mVolume;
                                c->mPan = c->mSample->mPan;
                            }
                            period = GetNotePeriod((u8) (note + c->mSample->mNoteOffset), c->mSample->mFinetune);
                        } else {
                            period = c->mPeriod;
                        }
                        c->mVolumeColumnEffect = TRACKER_FX_NONE;
                        c->mEffect = TRACKER_FX_NONE;
                        c->mVolumeColumnEffectParam = 0;
                        c->mEffectParam = 0;
                        // Pass 0 applies the volume column, pass 1 the effect column.
                        for (k = 0; k < 2; k++) {
                            effect = TRACKER_FX_NONE;
                            if (k == 0) {
                                if (vol >= 0x10 && vol <= 0x50) {
                                    effect = TRACKER_FX_VOLUME;
                                    param = vol - 0x10;
                                } else if (vol >= 0x60 && vol <= 0x70) {
                                    effect = TRACKER_FX_VOL_SLIDE;
                                    param = vol & 0xf;
                                } else if (vol >= 0x70 && vol <= 0x80) {
                                    effect = TRACKER_FX_VOL_SLIDE;
                                    param = (vol & 0xf) << 4;
                                } else if (vol >= 0x80 && vol <= 0x90) {
                                    effect = TRACKER_FX_EXTENDED;
                                    param = 0xb0 | (vol & 0xf);
                                } else if (vol >= 0x90 && vol <= 0xa0) {
                                    effect = TRACKER_FX_EXTENDED;
                                    param = 0xa0 | (vol & 0xf);
                                } else if (vol >= 0xa0 && vol <= 0xb0) {
                                    c->mVibratoPerTick = vol & 0xf;
                                } else if (vol >= 0xb0 && vol <= 0xc0) {
                                    c->mVibratoDepth = vol & 0xf;
                                } else if (vol >= 0xc0 && vol < 0xd0) {
                                    effect = TRACKER_FX_PAN;
                                    param = (vol & 0xf) << 4;
                                } else if (vol >= 0xd0 && vol <= 0xe0) {
                                    effect = TRACKER_FX_PAN_SLIDE;
                                    param = vol & 0xf;
                                } else if (vol >= 0xe0 && vol <= 0xf0) {
                                    effect = TRACKER_FX_PAN_SLIDE;
                                    param = (vol & 0xf) << 4;
                                } else if (vol >= 0xf0) {
                                    effect = TRACKER_FX_TONE_SLIDE;
                                    param = (vol & 0xf) << 4;
                                }
                            } else {
                                effect = cellEffect;
                                param = cellParam;
                                if (effect == TRACKER_FX_NONE && cellParam != 0) {
                                    effect = TRACKER_FX_ARPEGGIO;
                                }
                            }
                            if (effect == TRACKER_FX_NONE) {
                                continue;
                            }
                            hi = (param & 0xf0) >> 4;
                            lo = param & 0xf;
                            switch (effect) {
                                case TRACKER_FX_ARPEGGIO:
                                    if (param != 0) {
                                        c->mEffect = effect;
                                        c->mEffectParam = param;
                                        c->mArpeggioTicks = 0;
                                    }
                                    break;
                                case TRACKER_FX_SLIDE_UP:
                                    if (param != 0) {
                                        c->mLastParams[0] = param;
                                    } else {
                                        param = c->mLastParams[0];
                                    }
                                    c->mEffect = effect;
                                    c->mEffectParam = param;
                                    if (gTrackerPlayer.mFlags & 1) {
                                        c->mEffectParam <<= 2;
                                    }
                                    break;
                                case TRACKER_FX_SLIDE_DOWN:
                                    if (param != 0) {
                                        c->mLastParams[1] = param;
                                    } else {
                                        param = c->mLastParams[1];
                                    }
                                    c->mEffect = effect;
                                    c->mEffectParam = param;
                                    if (gTrackerPlayer.mFlags & 1) {
                                        c->mEffectParam <<= 2;
                                    }
                                    break;
                                case TRACKER_FX_TONE_SLIDE:
                                case TRACKER_FX_TONE_SLIDE_VOL:
                                    if (param != 0) {
                                        if (effect == TRACKER_FX_TONE_SLIDE) {
                                            c->mLastParams[2] = param;
                                        } else {
                                            c->mLastParams[4] = param;
                                        }
                                    } else {
                                        if (effect == TRACKER_FX_TONE_SLIDE) {
                                            param = c->mLastParams[2];
                                        } else {
                                            param = c->mLastParams[4];
                                        }
                                    }
                                    if (k != 0) {
                                        c->mEffect = effect;
                                        c->mEffectParam = param;
                                    } else {
                                        c->mVolumeColumnEffect = effect;
                                        c->mVolumeColumnEffectParam = param;
                                    }
                                    if (notePresent == TRUE && period != 0) {
                                        c->mTargetPeriod = period;
                                    }
                                    if (param != 0 && effect == TRACKER_FX_TONE_SLIDE) {
                                        c->mToneSlidePerTick = param;
                                        if (gTrackerPlayer.mFlags & 1) {
                                            c->mToneSlidePerTick <<= 2;
                                        }
                                    }
                                    porta = TRUE;
                                    break;
                                case TRACKER_FX_VIBRATO:
                                    if (param != 0) {
                                        c->mLastParams[3] = param;
                                    } else {
                                        param = c->mLastParams[3];
                                        hi = (param & 0xf0) >> 4;
                                        lo = param & 0xf;
                                    }
                                    c->mEffect = effect;
                                    c->mEffectParam = param;
                                    if (hi != 0 && lo != 0) {
                                        c->mVibratoPerTick = hi;
                                        c->mVibratoDepth = lo;
                                    }
                                    break;
                                case TRACKER_FX_PAN:
                                    c->mPan = param;
                                    panUpd = TRUE;
                                    break;
                                case TRACKER_FX_TREMOLO:
                                    if (param != 0) {
                                        c->mLastParams[6] = param;
                                    } else {
                                        param = c->mLastParams[6];
                                        hi = (param & 0xf0) >> 4;
                                        lo = param & 0xf;
                                    }
                                    c->mEffect = effect;
                                    c->mEffectParam = param;
                                    if (hi != 0 && lo != 0) {
                                        c->mTremoloPerTick = hi;
                                        c->mTremoloDepth = lo;
                                    }
                                    break;
                                case TRACKER_FX_SAMPLE_OFFSET:
                                    if (param != 0) {
                                        sampleOffset = param << 8;
                                        trigger = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_VIBRATO_VOL:
                                    if (param != 0) {
                                        c->mLastParams[5] = param;
                                    } else {
                                        param = c->mLastParams[5];
                                    }
                                    c->mEffect = effect;
                                    c->mEffectParam = param;
                                    break;
                                case TRACKER_FX_VOL_SLIDE:
                                    if (param != 0) {
                                        c->mLastParams[7] = param;
                                    } else {
                                        param = c->mLastParams[7];
                                    }
                                    if (k != 0) {
                                        c->mEffect = effect;
                                        c->mEffectParam = param;
                                    } else {
                                        c->mVolumeColumnEffect = effect;
                                        c->mVolumeColumnEffectParam = param;
                                    }
                                    break;
                                case TRACKER_FX_VOLUME:
                                    if (param > 64) {
                                        param = 64;
                                    }
                                    c->mVolume = param;
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_EXTENDED:
                                    c->mEffect = effect;
                                    c->mEffectParam = param;
                                    switch (hi) {
                                        case TRACKER_FXE_VIBRATO_WAVE:
                                            c->mVibratoWave = gEffectWaves[lo & 7];
                                            break;
                                        case TRACKER_FXE_TREMOLO_WAVE:
                                            c->mTremoloWave = gEffectWaves[lo & 7];
                                            break;
                                        case TRACKER_FXE_RETRIGGER:
                                            c->mStartKind = TRACKER_START_RETRIGGER;
                                            c->mStartTicks = 0;
                                            c->mStartTickCount = lo;
                                            break;
                                        case TRACKER_FXE_PAN:
                                            c->mPan = lo << 4;
                                            panUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_NOTE_DELAY:
                                            c->mStartKind = TRACKER_START_DELAYED;
                                            c->mStartTicks = 0;
                                            c->mStartTickCount = lo;
                                            break;
                                        case TRACKER_FXE_ROW_DELAY:
                                            if (lo != 0) {
                                                gTrackerPlayer.mRowDelayTickCount =
                                                    (lo - 1) * gTrackerPlayer.mTicksPerRow;
                                            }
                                            break;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOLUME:
                                    if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_PENDING &&
                                        gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_RELEASE) {
                                        globalVol = param << 1;
                                        if (globalVol > 128) {
                                            globalVol = 128;
                                        }
                                        volChanged = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOL_SLIDE:
                                    if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_PENDING &&
                                        gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_RELEASE) {
                                        if (param != 0) {
                                            c->mLastParams[12] = param;
                                        } else {
                                            param = c->mLastParams[12];
                                        }
                                        c->mEffect = effect;
                                        c->mEffectParam = param;
                                    }
                                    break;
                                case TRACKER_FX_PAN_SLIDE:
                                    if (param != 0) {
                                        c->mLastParams[13] = param;
                                    } else {
                                        param = c->mLastParams[13];
                                    }
                                    if (k != 0) {
                                        c->mEffect = effect;
                                        c->mEffectParam = param;
                                    } else {
                                        c->mVolumeColumnEffect = effect;
                                        c->mVolumeColumnEffectParam = param;
                                    }
                                    break;
                            }
                        }
                        if (porta == FALSE) {
                            c->mPeriod = period;
                        } else {
                            period = c->mPeriod;
                        }
                        volume = c->mVolume;
                        if (porta == TRUE && instChanged == FALSE) {
                            trigger = FALSE;
                        }
                        // First-tick part of the two stored effects.
                        for (k = 0; k < 2; k++) {
                            if (k == 0) {
                                param = c->mVolumeColumnEffectParam;
                                effect = c->mVolumeColumnEffect;
                            } else {
                                param = c->mEffectParam;
                                effect = c->mEffect;
                            }
                            hi = (param & 0xf0) >> 4;
                            lo = param & 0xf;
                            switch (effect) {
                                case TRACKER_FX_ARPEGGIO:
                                    period = c->mPeriod;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_SLIDE_UP:
                                    if (gTrackerPlayer.mFlags & 1) {
                                        c->mPeriod -= param;
                                        if (c->mPeriod < TRACKER_PERIOD_MIN) {
                                            c->mPeriod = TRACKER_PERIOD_MIN;
                                        }
                                    }
                                    period = c->mPeriod;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_SLIDE_DOWN:
                                    if (gTrackerPlayer.mFlags & 1) {
                                        c->mPeriod += param;
                                    }
                                    period = c->mPeriod;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_TONE_SLIDE:
                                case TRACKER_FX_TONE_SLIDE_VOL:
                                    period = c->mPeriod;
                                    periodUpd = TRUE;
                                    if (effect == TRACKER_FX_TONE_SLIDE_VOL) {
                                        ApplyVolumeSlide(c, (s8) hi, (s8) lo);
                                        volume = c->mVolume;
                                        volUpd = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_VIBRATO:
                                case TRACKER_FX_VIBRATO_VOL:
                                    period = ApplyVibrato(c);
                                    periodUpd = TRUE;
                                    if (effect == TRACKER_FX_VIBRATO_VOL) {
                                        ApplyVolumeSlide(c, (s8) hi, (s8) lo);
                                        volume = c->mVolume;
                                        volUpd = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_TREMOLO:
                                    volume = ApplyTremolo(c);
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_VOL_SLIDE:
                                    ApplyVolumeSlide(c, (s8) hi, (s8) lo);
                                    volume = c->mVolume;
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_EXTENDED:
                                    switch (hi) {
                                        case TRACKER_FXE_FINE_SLIDE_UP:
                                            if (lo != 0) {
                                                c->mLastParams[8] = lo;
                                            } else {
                                                lo = c->mLastParams[8];
                                            }
                                            if (gTrackerPlayer.mFlags & 1) {
                                                c->mPeriod -= lo << 2;
                                            } else {
                                                c->mPeriod -= lo;
                                            }
                                            if (c->mPeriod < TRACKER_PERIOD_MIN) {
                                                c->mPeriod = TRACKER_PERIOD_MIN;
                                            }
                                            period = c->mPeriod;
                                            periodUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_FINE_SLIDE_DOWN:
                                            if (lo != 0) {
                                                c->mLastParams[9] = lo;
                                            } else {
                                                lo = c->mLastParams[9];
                                            }
                                            if (gTrackerPlayer.mFlags & 1) {
                                                c->mPeriod += lo << 2;
                                            } else {
                                                c->mPeriod += lo;
                                            }
                                            period = c->mPeriod;
                                            periodUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_FINE_VOL_UP:
                                            if (lo != 0) {
                                                c->mLastParams[10] = lo;
                                            } else {
                                                lo = c->mLastParams[10];
                                            }
                                            if (lo != 0) {
                                                ApplyVolumeSlide(c, (s8) lo, 0);
                                                volume = c->mVolume;
                                                volUpd = TRUE;
                                            }
                                            break;
                                        case TRACKER_FXE_FINE_VOL_DOWN:
                                            if (lo != 0) {
                                                c->mLastParams[11] = lo;
                                            } else {
                                                lo = c->mLastParams[11];
                                            }
                                            if (lo != 0) {
                                                ApplyVolumeSlide(c, 0, (s8) lo);
                                                volume = c->mVolume;
                                                volUpd = TRUE;
                                            }
                                            break;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOL_SLIDE:
                                    if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_PENDING &&
                                        gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_RELEASE) {
                                        ApplyGlobalVolumeSlide(&globalVol, (s8) hi, (s8) lo);
                                        volChanged = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_PAN_SLIDE:
                                    ApplyPanSlide(c, (s8) hi, (s8) lo);
                                    panUpd = TRUE;
                                    break;
                            }
                        }
                        if (trigger == TRUE) {
                            u32 freq;
                            u16 chVol;
                            TrackerSample *s;
                            u32 data;

                            freq = GetPeriodRate(period, (gTrackerPlayer.mFlags & 1) ? TRUE : FALSE);
                            c->mNoteOff = 0;
                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                StartEnvelope(&gTrackerPlayer, c, TRACKER_ENV_VOLUME);
                                StartEnvelope(&gTrackerPlayer, c, TRACKER_ENV_PAN);
                            } else {
                                SetEnvelopePosition(&gTrackerPlayer, c, TRACKER_ENV_VOLUME, cellParam);
                                SetEnvelopePosition(&gTrackerPlayer, c, TRACKER_ENV_PAN, cellParam);
                            }
                            c->mFadeOutVolume = 0x10000;
                            chVol = func_02040b60(gVolumeTable[ComputeVolumeIndex(
                                (s8) volume, c->mVolumeEnvState.mValue, c->mFadeOutVolume, gTrackerPlayer.mGlobalVolume,
                                c->mVolumeScale)]);
                            if (c->mStartKind != TRACKER_START_DELAYED) {
                                func_020400f4(1 << (gTrackerPlayer.mFirstChannel + i), 0, 0, 0);
                            }
                            s = c->mSample;
                            // A sample without a loop plays once from the offset; a looped one plays from its start.
                            if ((s->mFlags & 3) == 0 || s->mLoopLength == 0) {
                                if (sampleOffset >= s->mLength) {
                                    goto next;
                                }
                                data = (u32) s->mData;
                                if (data == 0) {
                                    goto next;
                                }
                                if (c->mStartKind != 0) {
                                    c->mUnk_4f = (s->mFlags & 0x10) ? TRUE : FALSE;
                                    c->mStartData = data + sampleOffset;
                                    c->mUnk_58 = (c->mSample->mLength - sampleOffset) >> 2;
                                    c->mStartLoopLength = 0;
                                    c->mStartVolume = chVol;
                                    c->mUnk_60 = 0xffb0ff / freq;
                                    c->mStartPan = ComputePan(c->mPan, c->mPanEnvState.mValue) >> 1;
                                }
                                if (c->mStartKind == TRACKER_START_DELAYED) {
                                    goto next;
                                }
                                c->mUnk_64 = 0xffb0ff / freq;
                                c->mSentVolume = chVol;
                                func_02040240(gTrackerPlayer.mFirstChannel + i,
                                              (c->mSample->mFlags & 0x10) ? TRUE : FALSE, data + sampleOffset, 2, 0,
                                              (c->mSample->mLength - sampleOffset) >> 2, c->mSentVolume & 0xff,
                                              c->mSentVolume >> 8, c->mUnk_64,
                                              ComputePan(c->mPan, c->mPanEnvState.mValue) >> 1);
                                func_020400cc(1 << (gTrackerPlayer.mFirstChannel + i), 0, 0, 0);
                            } else {
                                data = (u32) s->mData;
                                if (data == 0) {
                                    goto next;
                                }
                                if (c->mStartKind != 0) {
                                    c->mUnk_4f = (s->mFlags & 0x10) ? TRUE : FALSE;
                                    c->mStartData = data;
                                    c->mUnk_58 = c->mSample->mLoopStart >> 2;
                                    c->mStartLoopLength = c->mSample->mLoopLength >> 2;
                                    c->mStartVolume = chVol;
                                    c->mUnk_60 = 0xffb0ff / freq;
                                    c->mStartPan = ComputePan(c->mPan, c->mPanEnvState.mValue) >> 1;
                                }
                                if (c->mStartKind == TRACKER_START_DELAYED) {
                                    goto next;
                                }
                                c->mUnk_64 = 0xffb0ff / freq;
                                c->mSentVolume = chVol;
                                s = c->mSample;
                                func_02040240(gTrackerPlayer.mFirstChannel + i, (s->mFlags & 0x10) ? TRUE : FALSE, data,
                                              1, s->mLoopStart >> 2, s->mLoopLength >> 2, c->mSentVolume & 0xff,
                                              c->mSentVolume >> 8, c->mUnk_64,
                                              ComputePan(c->mPan, c->mPanEnvState.mValue) >> 1);
                                func_020400cc(1 << (gTrackerPlayer.mFirstChannel + i), 0, 0, 0);
                            }
                        } else {
                            BOOL changed;

                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                changed = AdvanceEnvelope(&gTrackerPlayer, c, TRACKER_ENV_VOLUME, FALSE) |
                                          AdvanceFadeOut(&gTrackerPlayer, c);
                            } else {
                                SetEnvelopePosition(&gTrackerPlayer, c, TRACKER_ENV_VOLUME, cellParam);
                                changed = TRUE;
                            }
                            if (changed == TRUE && volUpd == FALSE) {
                                volume = c->mVolume;
                                volUpd = TRUE;
                            }
                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                if (AdvanceEnvelope(&gTrackerPlayer, c, TRACKER_ENV_PAN, FALSE) == TRUE) {
                                    panUpd = TRUE;
                                }
                            } else {
                                SetEnvelopePosition(&gTrackerPlayer, c, TRACKER_ENV_PAN, cellParam);
                                panUpd = TRUE;
                            }
                            if (periodUpd == TRUE) {
                                c->mUnk_64 =
                                    0xffb0ff / GetPeriodRate(period, (gTrackerPlayer.mFlags & 1) ? TRUE : FALSE);
                                func_020401d8(1 << (gTrackerPlayer.mFirstChannel + i), c->mUnk_64);
                            }
                            if (volUpd == TRUE) {
                                c->mSentVolume = func_02040b60(gVolumeTable[ComputeVolumeIndex(
                                    (s8) volume, c->mVolumeEnvState.mValue, c->mFadeOutVolume,
                                    gTrackerPlayer.mGlobalVolume, c->mVolumeScale)]);
                                func_020401f8(1 << (gTrackerPlayer.mFirstChannel + i), c->mSentVolume & 0xff,
                                              c->mSentVolume >> 8);
                            }
                            if (panUpd == TRUE) {
                                func_02040220(1 << (gTrackerPlayer.mFirstChannel + i),
                                              ComputePan(c->mPan, c->mPanEnvState.mValue) >> 1);
                            }
                            if (gSystemState.mUnk_2c8 & 0x400) {
                                // The debug display flag is tested here, but nothing in this branch formats the
                                // parameter: the statement only keeps the test, as in the original.
                                param = (u16) param;
                            }
                        }
                    } else {
                        if (gSystemState.mUnk_2c8 & 0x400) {
                            effect = TRACKER_FX_NONE;
                            param = 0;
                        }
                    }
                } else {
                    BOOL changed;

                    c->mNoteOff = 1;
                    changed = AdvanceEnvelope(&gTrackerPlayer, c, TRACKER_ENV_VOLUME, TRUE) |
                              AdvanceFadeOut(&gTrackerPlayer, c);
                    if (changed == TRUE) {
                        c->mSentVolume = func_02040b60(
                            gVolumeTable[ComputeVolumeIndex(c->mVolume, c->mVolumeEnvState.mValue, c->mFadeOutVolume,
                                                            gTrackerPlayer.mGlobalVolume, c->mVolumeScale)]);
                        func_020401f8(1 << (gTrackerPlayer.mFirstChannel + i), c->mSentVolume & 0xff,
                                      c->mSentVolume >> 8);
                    }
                    if (AdvanceEnvelope(&gTrackerPlayer, c, TRACKER_ENV_PAN, TRUE) == TRUE) {
                        func_02040220(1 << (gTrackerPlayer.mFirstChannel + i),
                                      ComputePan(c->mPan, c->mPanEnvState.mValue) >> 1);
                    }
                }
            next:
                if (gSystemState.mUnk_2c8 & 0x400) {
                    if (effect != TRACKER_FX_NONE) {
                        sprintf(text, "%02x", param);
                    } else {
                        sprintf(text, "--");
                    }
                }
                cell += 5;
            }
            gTrackerPlayer.mRowIndex++;
            if (nextRow != -1) {
                gTrackerPlayer.mRowIndex = nextRow;
                AdvanceOrder();
            }
        } else {
            for (i = 0; i < gTrackerPlayer.mChannelCount; i++) {
                u8 tickEffect;
                u8 tickParam;
                u8 tickHi;
                u8 tickLo;
                u8 tickVolume;
                BOOL tickPeriodUpd;

                c = &gTrackerPlayer.mChannels[i];
                tickPeriodUpd = FALSE;
                volUpd = FALSE;
                tickPanUpd = FALSE;
                // A delayed note starts, or a retriggered one restarts, when its tick count is reached.
                if (c->mStartKind != 0) {
                    if (++c->mStartTicks == c->mStartTickCount) {
                        if (c->mStartKind != TRACKER_START_DELAYED) {
                            c->mStartTicks = 0;
                        } else {
                            c->mStartKind = 0;
                        }
                        func_020400f4(1 << (gTrackerPlayer.mFirstChannel + i), 0, 0, 0);
                        if (c->mStartLoopLength == 0) {
                            func_02040240(gTrackerPlayer.mFirstChannel + i, c->mUnk_4f, c->mStartData, 2, 0, c->mUnk_58,
                                          c->mStartVolume & 0xff, c->mStartVolume >> 8, c->mUnk_60, c->mStartPan);
                        } else {
                            func_02040240(gTrackerPlayer.mFirstChannel + i, c->mUnk_4f, c->mStartData, 1, c->mUnk_58,
                                          c->mStartLoopLength, c->mStartVolume & 0xff, c->mStartVolume >> 8, c->mUnk_60,
                                          c->mStartPan);
                        }
                        func_020400cc(1 << (gTrackerPlayer.mFirstChannel + i), 0, 0, 0);
                    }
                }
                for (tickK = 0; tickK < 2; tickK++) {
                    if (tickK == 0) {
                        tickParam = c->mVolumeColumnEffectParam;
                        tickEffect = c->mVolumeColumnEffect;
                    } else {
                        tickParam = c->mEffectParam;
                        tickEffect = c->mEffect;
                    }
                    tickHi = (tickParam & 0xf0) >> 4;
                    tickLo = tickParam & 0xf;
                    switch (tickEffect) {
                        case TRACKER_FX_ARPEGGIO:
                            note = c->mNote - 1;
                            switch ((u8) (c->mArpeggioTicks % 3)) {
                                case 0:
                                    period =
                                        GetNotePeriod((u8) (note + c->mSample->mNoteOffset), c->mSample->mFinetune);
                                    break;
                                case 1:
                                    period = GetNotePeriod((u8) (note + tickHi + c->mSample->mNoteOffset),
                                                           c->mSample->mFinetune);
                                    break;
                                case 2:
                                    period = GetNotePeriod((u8) (note + tickLo + c->mSample->mNoteOffset),
                                                           c->mSample->mFinetune);
                                    break;
                            }
                            c->mArpeggioTicks++;
                            tickPeriodUpd = TRUE;
                            break;
                        case TRACKER_FX_SLIDE_UP:
                            c->mPeriod -= tickParam;
                            if (c->mPeriod < TRACKER_PERIOD_MIN) {
                                c->mPeriod = TRACKER_PERIOD_MIN;
                            }
                            tickPeriodUpd = TRUE;
                            period = c->mPeriod;
                            break;
                        case TRACKER_FX_SLIDE_DOWN:
                            c->mPeriod += tickParam;
                            tickPeriodUpd = TRUE;
                            period = c->mPeriod;
                            break;
                        case TRACKER_FX_TONE_SLIDE:
                        case TRACKER_FX_TONE_SLIDE_VOL:
                            ApplyToneSlide(c);
                            period = c->mPeriod;
                            tickPeriodUpd = TRUE;
                            if (tickEffect == TRACKER_FX_TONE_SLIDE_VOL) {
                                ApplyVolumeSlide(c, (s8) tickHi, (s8) tickLo);
                                tickVolume = c->mVolume;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_VIBRATO:
                        case TRACKER_FX_VIBRATO_VOL:
                            period = ApplyVibrato(c);
                            tickPeriodUpd = TRUE;
                            if (tickEffect == TRACKER_FX_VIBRATO_VOL) {
                                ApplyVolumeSlide(c, (s8) tickHi, (s8) tickLo);
                                tickVolume = c->mVolume;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_TREMOLO:
                            tickVolume = ApplyTremolo(c);
                            volUpd = TRUE;
                            break;
                        case TRACKER_FX_VOL_SLIDE:
                            ApplyVolumeSlide(c, (s8) tickHi, (s8) tickLo);
                            tickVolume = c->mVolume;
                            volUpd = TRUE;
                            break;
                        case TRACKER_FX_EXTENDED:
                            if (tickHi == TRACKER_FXE_NOTE_CUT && gTrackerPlayer.mTickIndex >= tickLo) {
                                c->mVolume = 0;
                                tickVolume = 0;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_GLOBAL_VOL_SLIDE:
                            if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_PENDING &&
                                gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_TO_RELEASE) {
                                ApplyGlobalVolumeSlide(&globalVol, (s8) tickHi, (s8) tickLo);
                                volChanged = TRUE;
                            }
                            break;
                        case TRACKER_FX_PAN_SLIDE:
                            ApplyPanSlide(c, (s8) tickHi, (s8) tickLo);
                            tickPanUpd = TRUE;
                            break;
                    }
                }
                if ((AdvanceEnvelope(&gTrackerPlayer, c, TRACKER_ENV_VOLUME, FALSE) |
                     AdvanceFadeOut(&gTrackerPlayer, c)) == TRUE &&
                    volUpd == FALSE) {
                    tickVolume = c->mVolume;
                    volUpd = TRUE;
                }
                if (AdvanceEnvelope(&gTrackerPlayer, c, TRACKER_ENV_PAN, FALSE) == TRUE) {
                    tickPanUpd = TRUE;
                }
                if (tickPeriodUpd == TRUE) {
                    c->mUnk_64 = 0xffb0ff / GetPeriodRate(period, (gTrackerPlayer.mFlags & 1) ? TRUE : FALSE);
                    func_020401d8(1 << (gTrackerPlayer.mFirstChannel + i), c->mUnk_64);
                }
                if (volUpd == TRUE) {
                    c->mSentVolume = func_02040b60(
                        gVolumeTable[ComputeVolumeIndex((s8) tickVolume, c->mVolumeEnvState.mValue, c->mFadeOutVolume,
                                                        gTrackerPlayer.mGlobalVolume, c->mVolumeScale)]);
                    func_020401f8(1 << (gTrackerPlayer.mFirstChannel + i), c->mSentVolume & 0xff, c->mSentVolume >> 8);
                }
                if (tickPanUpd == TRUE) {
                    func_02040220(1 << (gTrackerPlayer.mFirstChannel + i),
                                  ComputePan(c->mPan, c->mPanEnvState.mValue) >> 1);
                }
            }
        }
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
    if (volChanged == TRUE) {
        SetGlobalVolume(globalVol, 0);
    }
}

// Advances to the next order position (or the jump target), stops the voices when the song ends or loops and
// loads the pattern's row count.
void AdvanceOrder(void) {
    if (gTrackerPlayer.mJumpOrderIndex != -1) {
        gTrackerPlayer.mOrderIndex = gTrackerPlayer.mJumpOrderIndex;
        gTrackerPlayer.mJumpOrderIndex = -1;
        if (gTrackerPlayer.mStopOnJump == 1) {
            StopTrackerChannels();
        }
    } else {
        gTrackerPlayer.mOrderIndex = gTrackerPlayer.mOrderIndex + 1;
    }
    if (gTrackerPlayer.mOrderIndex >= gTrackerPlayer.mOrderCount) {
        StopTrackerChannels();
        gTrackerPlayer.mOrderIndex = gTrackerPlayer.mLoopOrder;
        if (gTrackerPlayer.mCallback != 0) {
            gTrackerPlayer.mCallback(4);
        }
    }
    gTrackerPlayer.mPatternIndex = gTrackerPlayer.mOrders[gTrackerPlayer.mOrderIndex];
    gTrackerPlayer.mLastRow = gTrackerPlayer.mPatterns[gTrackerPlayer.mPatternIndex]->mRowCount - 1;
}

void ApplyToneSlide(TrackerChannel *c) {
    if (c->mPeriod < c->mTargetPeriod) {
        c->mPeriod = c->mPeriod + c->mToneSlidePerTick;
        if (c->mPeriod > c->mTargetPeriod) {
            c->mPeriod = c->mTargetPeriod;
        }
    }
    if (c->mPeriod > c->mTargetPeriod) {
        c->mPeriod = c->mPeriod - c->mToneSlidePerTick;
        if (c->mPeriod < c->mTargetPeriod) {
            c->mPeriod = c->mTargetPeriod;
        }
    }
}

// Period with the vibrato offset from the channel's wave table added; steps the wave position.
u16 ApplyVibrato(TrackerChannel *c) {
    u16 period = c->mPeriod;

    if (gTrackerPlayer.mFlags & 1) {
        period += (c->mVibratoDepth * c->mVibratoWave[c->mWaveIndex]) >> 5;
    } else {
        period += (c->mVibratoDepth * c->mVibratoWave[c->mWaveIndex]) >> 7;
    }
    c->mWaveIndex += c->mVibratoPerTick;
    c->mWaveIndex &= 0x3f;
    return period;
}

// Volume with the tremolo offset added (capped at 64); steps the wave position.
u8 ApplyTremolo(TrackerChannel *c) {
    u8 v = (u8) c->mVolume + ((c->mTremoloDepth * c->mTremoloWave[c->mWaveIndex]) >> 6);

    if (v > 64) {
        v = 64;
    }
    c->mWaveIndex += c->mTremoloPerTick;
    c->mWaveIndex &= 0x3f;
    return v;
}

// Volume slide: adds up when it is non-zero, subtracts down otherwise; kept within 0..64.
void ApplyVolumeSlide(TrackerChannel *c, int up, int down) {
    if (up != 0) {
        c->mVolume = c->mVolume + up;
    } else {
        c->mVolume = c->mVolume - down;
    }
    if (c->mVolume < 0) {
        c->mVolume = 0;
    }
    if (c->mVolume > 64) {
        c->mVolume = 64;
    }
}

// Panning slide: adds up when it is non-zero, subtracts down otherwise; kept within 0..255.
void ApplyPanSlide(TrackerChannel *c, int up, int down) {
    if (up != 0) {
        c->mPan = c->mPan + up;
    } else {
        c->mPan = c->mPan - down;
    }
    if (c->mPan < 0) {
        c->mPan = 0;
    }
    if (c->mPan > 255) {
        c->mPan = 255;
    }
}

// Master volume slide in steps of two: adds up when it is non-zero, subtracts down otherwise; kept within 0..128.
void ApplyGlobalVolumeSlide(s8 *v, int up, int down) {
    if (up != 0) {
        *v = *v + up * 2;
    } else {
        *v = *v - down * 2;
    }
    if (*v < 0) {
        *v = 0;
    }
    if (*v > 128) {
        *v = 128;
    }
}

// Period of a note at a fine-tune value: linear scale or the period table, by the song's flag.
u16 GetNotePeriod(int note, int finetune) {
    if ((gTrackerPlayer.mFlags & 1) == 0) {
        return gNotePeriods[(finetune >> 4) + 8][note];
    }
    return 7680 - note * 64 - (finetune >> 1);
}

// Playback rate of a period: a clock divided by twice the period, or with the linear flag the table entry
// shifted down by the octave.
u32 GetPeriodRate(int period, int linear) {
    if (linear == 0) {
        return 7093789 / (period * 2);
    }
    return gPeriodRates[period % 768] >> (period / 768);
}

// Starts the volume (type 0) or panning (type 1) envelope of a channel's instrument; without a usable instrument
// the level is set to its default.
void StartEnvelope(TrackerPlayer *p, TrackerChannel *c, int type) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u8 inst = c->mInstrumentHandle;

    if (inst != 0 && inst <= gTrackerPlayer.mInstrumentCount) {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                st = &c->mVolumeEnvState;
                env = &p->mInstruments[inst - 1]->mVolumeEnvelope;
                st->mValue = TRACKER_ENV_VOLUME_LEVEL;
                break;
            case TRACKER_ENV_PAN:
                st = &c->mPanEnvState;
                env = &p->mInstruments[inst - 1]->mPanEnvelope;
                st->mValue = TRACKER_ENV_PAN_LEVEL;
                break;
        }
        if ((env->mFlags & TRACKER_ENV_ON) == 0 || env->mPointCount <= 1) {
            return;
        }
        st->mPointIndex = 0;
        st->mTickPos = env->mPointTickPos[st->mPointIndex];
        if (env->mPointTickPos[st->mPointIndex + 1] - st->mTickPos > 0) {
            st->mValue = env->mPointValues[st->mPointIndex] << 8;
            st->mValuePerTick = (env->mPointValues[st->mPointIndex + 1] << 8) - st->mValue;
            st->mValuePerTick = st->mValuePerTick / (env->mPointTickPos[st->mPointIndex + 1] - st->mTickPos);
        } else {
            st->mTickPos = st->mTickPos - 1;
            st->mValue = env->mPointValues[st->mPointIndex + 1] << 8;
            st->mValuePerTick = 0;
        }
        st->mNoteOff = 0;
    } else {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                c->mVolumeEnvState.mValue = TRACKER_ENV_VOLUME_LEVEL;
                break;
            case TRACKER_ENV_PAN:
                c->mPanEnvState.mValue = TRACKER_ENV_PAN_LEVEL;
                break;
        }
    }
}

// Advances an envelope by one tick (holding at the sustain point until keyOff is 1, looping when the envelope
// loops); returns 1 when the value's whole part changed.
int AdvanceEnvelope(TrackerPlayer *p, TrackerChannel *c, int type, int keyOff) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u32 before;
    u8 inst = c->mInstrumentHandle;

    if (inst != 0 && inst <= gTrackerPlayer.mInstrumentCount) {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                st = &c->mVolumeEnvState;
                env = &p->mInstruments[inst - 1]->mVolumeEnvelope;
                break;
            case TRACKER_ENV_PAN:
                st = &c->mPanEnvState;
                env = &p->mInstruments[inst - 1]->mPanEnvelope;
                break;
        }
        if ((env->mFlags & TRACKER_ENV_ON) && env->mPointCount > 1) {
            if (keyOff == 1) {
                st->mNoteOff = 1;
            }
            if (st->mPointIndex < env->mPointCount - 1) {
                if (st->mNoteOff == 1 || st->mPointIndex != env->mSustainPoint ||
                    (env->mFlags & TRACKER_ENV_SUSTAIN) == 0) {
                    before = st->mValue >> 8;
                    st->mValue = st->mValue + st->mValuePerTick;
                    st->mTickPos = st->mTickPos + 1;
                    if (st->mTickPos == env->mPointTickPos[st->mPointIndex + 1]) {
                        st->mPointIndex++;
                        if (env->mFlags & TRACKER_ENV_LOOP) {
                            if (st->mPointIndex == env->mLoopEndPoint) {
                                st->mPointIndex = env->mLoopStartPoint;
                            }
                        }
                        if (st->mPointIndex < env->mPointCount - 1) {
                            st->mTickPos = env->mPointTickPos[st->mPointIndex];
                            if (env->mPointTickPos[st->mPointIndex + 1] - st->mTickPos > 0) {
                                st->mValue = env->mPointValues[st->mPointIndex] << 8;
                                st->mValuePerTick = (env->mPointValues[st->mPointIndex + 1] << 8) - st->mValue;
                                st->mValuePerTick =
                                    st->mValuePerTick / (env->mPointTickPos[st->mPointIndex + 1] - st->mTickPos);
                            } else {
                                st->mTickPos = st->mTickPos - 1;
                                st->mValue = env->mPointValues[st->mPointIndex + 1] << 8;
                                st->mValuePerTick = 0;
                            }
                        }
                    }
                    return before != (st->mValue >> 8);
                }
                return 0;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

// Places an envelope at tick position pos: finds the segment, sets the value and the per-tick step.
void SetEnvelopePosition(TrackerPlayer *p, TrackerChannel *c, int type, u16 pos) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u8 inst = c->mInstrumentHandle;

    if (inst == 0 || inst > gTrackerPlayer.mInstrumentCount) {
        return;
    }
    switch (type) {
        case TRACKER_ENV_VOLUME:
            st = &c->mVolumeEnvState;
            env = &p->mInstruments[inst - 1]->mVolumeEnvelope;
            break;
        case TRACKER_ENV_PAN:
            st = &c->mPanEnvState;
            env = &p->mInstruments[inst - 1]->mPanEnvelope;
            break;
    }
    if ((env->mFlags & TRACKER_ENV_ON) == 0 || env->mPointCount <= 1) {
        return;
    }
    st->mPointIndex = 0;
    for (;;) {
        if (st->mPointIndex == env->mPointCount - 1) {
            break;
        }
        if (pos < env->mPointTickPos[st->mPointIndex + 1]) {
            break;
        }
        st->mPointIndex = st->mPointIndex + 1;
    }
    if (st->mPointIndex == env->mPointCount - 1) {
        st->mTickPos = env->mPointTickPos[st->mPointIndex];
        st->mValue = env->mPointValues[st->mPointIndex] << 8;
        st->mValuePerTick = 0;
        return;
    }
    if (env->mPointTickPos[st->mPointIndex + 1] - st->mTickPos > 0) {
        st->mValue = env->mPointValues[st->mPointIndex] << 8;
        st->mValuePerTick = (env->mPointValues[st->mPointIndex + 1] << 8) - st->mValue;
        st->mValuePerTick =
            st->mValuePerTick / (env->mPointTickPos[st->mPointIndex + 1] - env->mPointTickPos[st->mPointIndex]);
        st->mTickPos = pos;
        st->mValue = st->mValuePerTick * (st->mTickPos - env->mPointTickPos[st->mPointIndex]) + st->mValue;
    } else {
        st->mValue = env->mPointValues[st->mPointIndex + 1] << 8;
        st->mValuePerTick = 0;
        st->mTickPos = pos - 1;
    }
}

// After key-off, lowers the channel's fade-out level by the instrument's fade step (or clears it without a
// volume envelope); returns 1 when the level changed.
int AdvanceFadeOut(TrackerPlayer *p, TrackerChannel *c) {
    TrackerInstrument *instr;
    s32 before;
    u8 inst;

    if (c->mNoteOff == 1) {
        inst = c->mInstrumentHandle;
        if (inst != 0 && inst <= gTrackerPlayer.mInstrumentCount) {
            instr = p->mInstruments[inst - 1];
            before = c->mFadeOutVolume;
            if (instr->mVolumeEnvelope.mFlags & TRACKER_ENV_ON) {
                c->mFadeOutVolume = before - instr->mFadeOutPerTick;
                if (c->mFadeOutVolume <= 0) {
                    c->mFadeOutVolume = 0;
                }
            } else {
                c->mFadeOutVolume = 0;
            }
            return before != c->mFadeOutVolume;
        }
        return 0;
    }
    return 0;
}

// Table index (0..64) of a channel's output volume: channel volume, volume envelope level, fade-out level and
// master volume multiplied together.
int ComputeVolumeIndex(int vol, u16 env, int fade, u8 master, u8 chanVol) {
    return ((((fade * master) >> 16) * (((vol * chanVol) >> 7 << 3) * env >> 16)) >> 8);
}

// Panning (0..255) after the panning envelope level is applied, weighted toward the middle.
u32 ComputePan(int pan, u16 level) {
    int v = pan + (((level >> 8) - 32) * (128 - abs(pan - 128))) / 32;

    if ((v < 255 ? v : 255) < 0) {
        return 0;
    }
    return v < 255 ? v : 255;
}

// Sets the master volume (0 while gSystemState.mUseTrackerVolume is 0, at most 128) and, while a song plays, sends the
// new volume of each active hardware channel.
void SetGlobalVolume(u8 volume, int mode) {
    u32 mask;
    u32 i;

    if (gSystemState.mUseTrackerVolume == 0) {
        volume = 0;
    }
    if (volume > 128) {
        volume = 128;
    }
    gTrackerPlayer.mGlobalVolume = volume;
    if (gTrackerPlayer.mPlaying != 1) {
        return;
    }
    if (gTrackerPlayer.mGlobalFadeState != TRACKER_GLOBAL_FADE_NONE && mode != 1) {
        return;
    }
    mask = func_02040a90();
    for (i = 0; i < gTrackerPlayer.mChannelCount; i++) {
        if (mask & (1 << (gTrackerPlayer.mFirstChannel + i))) {
            TrackerChannel *c = &gTrackerPlayer.mChannels[i];

            c->mSentVolume =
                func_02040b60(gVolumeTable[ComputeVolumeIndex(c->mVolume, c->mVolumeEnvState.mValue, c->mFadeOutVolume,
                                                              gTrackerPlayer.mGlobalVolume, c->mVolumeScale)]);
            func_020401f8(1 << (gTrackerPlayer.mFirstChannel + i), c->mSentVolume & 0xff, c->mSentVolume >> 8);
        }
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
}

u8 GetGlobalVolume(void) {
    return gTrackerPlayer.mGlobalVolume;
}

#pragma thumb on
