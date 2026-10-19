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
const u16 data_02051844[16][96] = {
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
const u32 data_02050c44[768] = {
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
const s32 data_02050b40[65] = {
    -723, -361, -301, -266, -241, -221, -206, -192, -181, -170, -161, -153, -145, -138, -132, -126, -120,
    -115, -110, -105, -101, -97,  -93,  -89,  -85,  -82,  -78,  -75,  -72,  -69,  -66,  -63,  -60,  -58,
    -55,  -52,  -50,  -48,  -45,  -43,  -41,  -39,  -37,  -35,  -33,  -31,  -29,  -27,  -25,  -23,  -21,
    -20,  -18,  -16,  -15,  -13,  -12,  -10,  -9,   -7,   -6,   -4,   -3,   -1,   0,
};
// One period of a sine wave, 64 entries with amplitude 255; every channel points at it twice.
const s16 data_02050ac0[64] = {
    0,    0x18, 0x31, 0x4a, 0x61, 0x78, 0x8d, 0xa1, 0xb4, 0xc5, 0xd4, 0xe0, 0xeb, 0xf4, 0xfa, 0xfd,
    0xff, 0xfd, 0xfa, 0xf4, 0xeb, 0xe0, 0xd4, 0xc5, 0xb4, 0xa1, 0x8d, 0x78, 0x61, 0x4a, 0x31, 0x18,
    0,    -24,  -49,  -74,  -97,  -120, -141, -161, -180, -197, -212, -224, -235, -244, -250, -253,
    -255, -253, -250, -244, -235, -224, -212, -197, -180, -161, -141, -120, -97,  -74,  -49,  -24,
};
// A curve falling from 255 to -255 over 64 entries.
const s16 data_02050a40[64] = {
    0xff, 0xfe, 0xfd, 0xfb, 0xfa, 0xf7, 0xf4, 0xef, 0xeb, 0xe5, 0xe0, 0xda, 0xd4, 0xcc, 0xc5, 0xbc,
    0xb4, 0xaa, 0xa1, 0x97, 0x8d, 0x82, 0x78, 0x6c, 0x61, 0x55, 0x4a, 0x3d, 0x31, 0x24, 0x18, 0xc,
    0,    -12,  -24,  -36,  -49,  -61,  -74,  -85,  -97,  -108, -120, -130, -141, -151, -161, -170,
    -180, -188, -197, -204, -212, -218, -224, -229, -235, -239, -244, -247, -250, -251, -253, -254,
};
// A square wave: 32 entries of -255, then 32 of 255.
const s16 data_020509c0[64] = {
    -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255,
    -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

// Interrupt mask bit of the timer that drives the player.
#define TRACKER_IRQ_TIMER2 0x20

TrackerPlayer data_020c6b14;

// Wave shapes the vibrato and tremolo effects select (low three bits of the parameter).
const s16 *data_02063c9c[8] = {data_02050ac0, data_02050a40, data_020509c0, data_02050ac0,
                               data_02050ac0, data_02050a40, data_020509c0, data_02050ac0};

#pragma thumb off

// Loads song a (a file id) and starts it with volume c (at most 0x80; 0 while data_020c18bc.mUnk_493 is 0). The song
// already loaded with nothing pending only takes the new volume. b is the number added to each sample name's number to
// get the sample's file id (0 keeps the last one); d goes to func_02035c7c. With e == 1 while a song plays the request
// is stored for func_020360f4 to replay. A new song's file gives the header bytes, the order table, the patterns (a
// cell byte with bit 7 set says which of the five bytes follow; the others become 0xff, 0, 0, 0xff, 0), the instruments
// with their two envelopes, and their samples, whose data is loaded by the number in the sample's name.
void func_02035108(u16 a, u16 b, u8 c, u16 d, int e, int f) {
    u8 name[0x17];
    u8 *patterns;
    TrackerInstrument *instruments;
    u8 *next;
    u32 size;
    unsigned long i;
    u8 *src;
    u8 *pattern;
    u32 j;
    TrackerSample *samples;
    u32 left;

    if (c > 0x80) {
        OS_Terminate();
    }
    if (data_020c18bc.mUnk_493 == 0) {
        c = 0;
    }
    if (a == data_020c6b14.mUnk_172e && data_020c6b14.mUnk_1748 == 0 && f == 0) {
        func_02038578(c, 0);
        return;
    }
    data_020c6b14.mUnk_1745 = 1;
    if (b == 0) {
        if (!(data_020c18bc.mUnk_46c & 0x1000000)) {
            OS_Terminate();
        }
        b = data_020c6b14.mUnk_172c;
    } else {
        data_020c6b14.mUnk_172c = b;
    }
    if (data_020c6b14.mUnk_1724 == 1 && e == 1) {
        if (data_020c6b14.mUnk_1748 != 3) {
            data_020c6b14.mUnk_1748 = 1;
        }
        data_020c6b14.mUnk_1730 = a;
        if (b != 0) {
            data_020c6b14.mUnk_1732 = b;
        }
        data_020c6b14.mUnk_1749 = c;
        data_020c6b14.mUnk_173c = d;
        data_020c6b14.mUnk_1734 = f;
        data_020c6b14.mUnk_1745 = 0;
        return;
    }
    data_020c6b14.mUnk_1748 = 0;
    if (a != data_020c6b14.mUnk_172e) {
        func_020359d0(0);
    }
    if (data_020c6b14.mUnk_1724 == 1) {
        func_02035e40();
    }
    if (data_020c6b14.mUnk_172e == 0) {
        if (a == 0) {
            OS_Terminate();
        }
        data_020c6b14.mUnk_172e = a;
        src = func_0202ded8(data_020c6b14.mUnk_172e);
        if (src == NULL) {
            OS_Terminate();
        }
        data_020c6b14.mUnk_174a = (src[0x41] << 8) | src[0x40];
        data_020c6b14.mUnk_174b = (src[0x43] << 8) | src[0x42];
        data_020c6b14.mUnk_174c = (src[0x45] << 8) | src[0x44];
        data_020c6b14.mUnk_174d = (src[0x47] << 8) | src[0x46];
        data_020c6b14.mUnk_174e = (src[0x49] << 8) | src[0x48];
        data_020c6b14.mUnk_174f = (src[0x4b] << 8) | src[0x4a];
        data_020c6b14.mUnk_1750 = (src[0x4d] << 8) | src[0x4c];
        data_020c6b14.mUnk_1751 = (src[0x4f] << 8) | src[0x4e];
        src += 0x50;
        for (i = 0; i < TRACKER_ORDER_COUNT; i++) {
            data_020c6b14.mUnk_1752[i] = *src++;
        }

        patterns = func_0201c430(data_020c6b14.mUnk_174d * sizeof(TrackerPattern));
        data_020c6b14.mUnk_04 = (TrackerPattern *) patterns;
        if (patterns == NULL) {
            OS_Terminate();
        }
        for (i = 0; i < data_020c6b14.mUnk_174d; i++) {
            u32 row;
            u32 channel;

            data_020c6b14.mUnk_10[i] = (TrackerPattern *) patterns;
            patterns += sizeof(TrackerPattern);
            data_020c6b14.mUnk_10[i]->mUnk_00 = src[4];
            data_020c6b14.mUnk_10[i]->mUnk_01 = (src[6] << 8) | src[5];
            size = (src[8] << 8) | src[7];
            src += 9;
            next = src;
            data_020c6b14.mUnk_10[i]->mUnk_04 =
                func_0201c430(data_020c6b14.mUnk_174c * data_020c6b14.mUnk_10[i]->mUnk_01 * 5);
            if (data_020c6b14.mUnk_10[i]->mUnk_04 == NULL) {
                OS_Terminate();
            }
            pattern = data_020c6b14.mUnk_10[i]->mUnk_04;
            for (row = 0; row < data_020c6b14.mUnk_10[i]->mUnk_01; row++) {
                for (channel = 0; channel < data_020c6b14.mUnk_174c; channel++) {
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

        instruments = func_0201c430(data_020c6b14.mUnk_174e * sizeof(TrackerInstrument));
        data_020c6b14.mUnk_08 = instruments;
        if (instruments == NULL) {
            OS_Terminate();
        }
        left = data_020c6b14.mUnk_174e * 2;
        samples = func_0201c430(left * sizeof(TrackerSample));
        data_020c6b14.mUnk_0c = samples;
        if (samples == NULL) {
            OS_Terminate();
        }
        for (i = 0; i < data_020c6b14.mUnk_174e; i++) {
            u8 *start;
            u32 header;

            start = src;
            data_020c6b14.mUnk_410[i] = instruments;
            instruments++;
            header = (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
            data_020c6b14.mUnk_410[i]->mUnk_26c = (src[0x1c] << 8) | src[0x1b];
            if (data_020c6b14.mUnk_410[i]->mUnk_26c != 0) {
                src += 0x21;
                for (j = 0; j < 0x60; j++) {
                    data_020c6b14.mUnk_410[i]->mUnk_26d[j] = *src++;
                }
                for (j = 0; j < 12; j++) {
                    data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_00[j] = (src[1] << 8) | src[0];
                    data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_18[j] = (src[3] << 8) | src[2];
                    src += 4;
                }
                for (j = 0; j < 12; j++) {
                    data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_00[j] = (src[1] << 8) | src[0];
                    data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_18[j] = (src[3] << 8) | src[2];
                    src += 4;
                }
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_30 = src[0];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_30 = src[1];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_31 = src[2];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_32 = src[3];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_33 = src[4];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_31 = src[5];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_32 = src[6];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_33 = src[7];
                data_020c6b14.mUnk_410[i]->mUnk_200.mUnk_34 = src[8];
                data_020c6b14.mUnk_410[i]->mUnk_236.mUnk_34 = src[9];
                data_020c6b14.mUnk_410[i]->mUnk_2cd = src[10];
                data_020c6b14.mUnk_410[i]->mUnk_2ce = src[11];
                data_020c6b14.mUnk_410[i]->mUnk_2cf = src[12];
                data_020c6b14.mUnk_410[i]->mUnk_2d0 = src[13];
                data_020c6b14.mUnk_410[i]->mUnk_2d2 = ((src[15] << 8) | src[14]) * 2;
                src += 16;
                src += header - (src - start);
                for (j = 0; j < data_020c6b14.mUnk_410[i]->mUnk_26c; j++) {
                    if (left == 0) {
                        OS_Terminate();
                    }
                    left--;
                    data_020c6b14.mUnk_410[i]->mUnk_000[j] = samples;
                    samples++;
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_00 =
                        (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | src[0];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_04 =
                        (src[7] << 24) | (src[6] << 16) | (src[5] << 8) | src[4];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_08 =
                        (src[11] << 24) | (src[10] << 16) | (src[9] << 8) | src[8];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_12 = src[12];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_13 = src[13];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_14 = src[14];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_16 = src[15];
                    data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_15 = src[16];
                    if (data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_00 != 0) {
                        MI_CpuCopy8(src + 0x12, name, 0x16);
                        name[0x16] = 0;
                        data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_10 = func_02035bcc(name, b);
                        data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_0c =
                            func_0202e86c(data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_10);
                    }
                    src += 0x28;
                }
            } else {
                src += header - (src - start);
            }
        }
    }
    data_020c6b14.mUnk_1744 = 16 - data_020c6b14.mUnk_174c;
    data_020c6b14.mUnk_1718 = 0;
    data_020c18bc.mUnk_46c |= 0x1000000;
    data_020c648c.mUnk_181 = data_020c6b14.mUnk_1744;
    func_0202f084();
    func_02035c7c(c, (s16) d);
    data_020c6b14.mUnk_1745 = 0;
}

// Releases everything the loaded song allocated and resets the player; with a == 1 while a song plays it
// only requests a fade-out (mode 2).
void func_020359d0(int a) {
    u32 i;
    u32 j;

    data_020c6b14.mUnk_1746 = 1;
    if (data_020c6b14.mUnk_1724 == 1 && a == 1) {
        if (data_020c6b14.mUnk_1748 != 3) {
            data_020c6b14.mUnk_1748 = 2;
            data_020c6b14.mUnk_1746 = 0;
            return;
        }
    } else {
        data_020c6b14.mUnk_1748 = 0;
    }
    if (data_020c6b14.mUnk_1724 == 1) {
        func_02035e40();
    }
    for (i = 0; i < TRACKER_INSTRUMENT_COUNT; i++) {
        if (data_020c6b14.mUnk_410[i] != 0) {
            for (j = 0; j < data_020c6b14.mUnk_410[i]->mUnk_26c; j++) {
                if (data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_0c != 0) {
                    func_0201c4f0(&data_020c6b14.mUnk_410[i]->mUnk_000[j]->mUnk_0c);
                }
                data_020c6b14.mUnk_410[i]->mUnk_000[j] = 0;
            }
            data_020c6b14.mUnk_410[i] = 0;
        }
    }
    if (data_020c6b14.mUnk_0c != 0) {
        func_0201c4f0(&data_020c6b14.mUnk_0c);
    }
    if (data_020c6b14.mUnk_08 != 0) {
        func_0201c4f0(&data_020c6b14.mUnk_08);
    }
    for (i = 0; i < TRACKER_PATTERN_COUNT; i++) {
        if (data_020c6b14.mUnk_10[i] != 0) {
            func_0201c4f0(&data_020c6b14.mUnk_10[i]->mUnk_04);
            data_020c6b14.mUnk_10[i] = 0;
        }
    }
    if (data_020c6b14.mUnk_04 != 0) {
        func_0201c4f0(&data_020c6b14.mUnk_04);
    }
    data_020c6b14.mUnk_1748 = 0;
    data_020c6b14.mUnk_172e = 0;
    data_020c6b14.mUnk_1744 = 0;
    data_020c6b14.mUnk_1747 = -128;
    data_020c6b14.mUnk_1724 = 0;
    data_020c6b14.mUnk_174a = 0;
    data_020c6b14.mUnk_174b = 0;
    data_020c6b14.mUnk_174c = 0;
    data_020c6b14.mUnk_174d = 0;
    data_020c6b14.mUnk_174e = 0;
    data_020c6b14.mUnk_174f = 0;
    data_020c6b14.mUnk_1750 = 0;
    data_020c6b14.mUnk_1751 = 0;
    for (i = 0; i < TRACKER_ORDER_COUNT; i++) {
        data_020c6b14.mUnk_1752[i] = 0;
    }
    data_020c6b14.mUnk_1718 = 0;
    data_020c18bc.mUnk_46c &= ~0x1000000;
    data_020c6b14.mUnk_1746 = 0;
}

// Returns base plus the first decimal number found in text.
u16 func_02035bcc(const u8 *text, int base) {
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
void func_02035c7c(u8 a, s16 b) {
    u32 mask = 0;
    u32 i;
    u32 k;

    if (data_020c6b14.mUnk_174c == 0) {
        OS_Terminate();
    }
    func_020402f0();
    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        mask |= 1 << (data_020c6b14.mUnk_1744 + i);
    }
    func_02040198(mask, 0);
    data_020c6b14.mUnk_1747 = a;
    for (i = 0; i < TRACKER_CHANNEL_COUNT; i++) {
        TrackerChannel *c = &data_020c6b14.mUnk_610[i];
        c->mUnk_10 = 0;
        c->mUnk_14 = 255;
        c->mUnk_15 = 0;
        c->mUnk_17 = 255;
        c->mUnk_16 = (u8) c->mUnk_17;
        c->mUnk_1a = 0;
        c->mUnk_00 = 0;
        c->mUnk_04 = 0;
        c->mUnk_1c = 0;
        c->mUnk_1d = 0;
        c->mUnk_06 = 0;
        c->mUnk_1e = 0;
        c->mUnk_20 = 0;
        c->mUnk_21 = 0;
        c->mUnk_08 = data_02050ac0;
        c->mUnk_22 = 0;
        c->mUnk_23 = 0;
        c->mUnk_0c = data_02050ac0;
        c->mUnk_24 = 0;
        c->mUnk_25 = 128;
        c->mUnk_26 = 128;
        c->mUnk_48 = 0x10000;
        for (k = 0; k < 14; k++) {
            c->mUnk_6a[k] = 1;
        }
    }
    data_020c6b14.mUnk_1852 = data_020c6b14.mUnk_1750;
    data_020c6b14.mUnk_1856 = data_020c6b14.mUnk_1750 - 1;
    data_020c6b14.mUnk_1858 = 0;
    data_020c6b14.mUnk_1853 = data_020c6b14.mUnk_1751;
    data_020c6b14.mUnk_171c = 8192000 / (data_020c6b14.mUnk_1751 * 50);
    data_020c6b14.mUnk_1720 = 0;
    data_020c6b14.mUnk_1738 = b - 1;
    data_020c6b14.mUnk_173a = -1;
    data_020c6b14.mUnk_1854 = 255;
    data_020c6b14.mUnk_1740 = 0;
    data_020c6b14.mUnk_1742 = 1;
    data_020c6b14.mUnk_1724 = 1;
    data_020c6b14.mUnk_1728 = 0;
    func_02036194();
    func_02035f2c();
    if (data_020c6b14.mUnk_00 != 0) {
        data_020c6b14.mUnk_00(0);
    }
}

// Stops the timer and the hardware channels, then calls the player's callback with 1.
void func_02035e40(void) {
    func_02035fc4();
    func_02035e7c();
    data_020c6b14.mUnk_1724 = 0;
    if (data_020c6b14.mUnk_00 != 0) {
        data_020c6b14.mUnk_00(1);
    }
}

// Sends the stop command to each hardware channel of the song, then sends the combined mask.
void func_02035e7c(void) {
    u32 mask = 0;
    u32 i;

    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        func_020400f4(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
        while (SND_RecvCommandReply(0) != 0) {
        }
        SND_FlushCommand(2);
    }
    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        mask |= 1 << (data_020c6b14.mUnk_1744 + i);
    }
    func_020401b8(mask, 0);
}

// Installs the timer interrupt handler and starts the timer at the rate the flags select.
void func_02035f2c(void) {
    OS_SetIrqFunction(TRACKER_IRQ_TIMER2, func_02035fd4);
    OS_EnableIrqMask(TRACKER_IRQ_TIMER2);
    if (data_020c6b14.mUnk_1710 & 2) {
        if (data_020c6b14.mUnk_1853 != 125) {
            data_020c6b14.mUnk_1710 |= 1;
        } else {
            data_020c6b14.mUnk_1710 &= ~1;
        }
    }
    if (data_020c6b14.mUnk_1710 & 1) {
        data_020c6b14.mUnk_1714 = 0x147;
        func_0203f004(2, 654, 2);
    } else {
        data_020c6b14.mUnk_1714 = 0x51e;
        func_0203f004(2, 0xa3a, 2);
    }
}

void func_02035fc4(void) {
    func_0203f024(2);
}

// Timer interrupt handler: runs one player tick, or steps the fade-out while stopping (modes 1 and 2).
void func_02035fd4(void) {
    data_020c6b14.mUnk_1718 = 0;
    if (data_020c6b14.mUnk_1728 == 1) {
        *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
        func_02035fc4();
        func_02035f2c();
        return;
    }
    if (data_020c6b14.mUnk_1724 == 1 && data_020c6b14.mUnk_1745 == 0 && data_020c6b14.mUnk_1746 == 0) {
        u8 mode = data_020c6b14.mUnk_1748;
        u8 vol;

        if (mode == 0) {
            *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
            func_02035fc4();
            func_02036198();
            func_02035f2c();
            return;
        }
        // Only modes 1 and 2 (fading out) go on.
        if ((u8) (mode + 255) > 1) {
            return;
        }
        *(vu32 *) (HW_DTCM + 0x3ff8) |= TRACKER_IRQ_TIMER2;
        func_02035fc4();
        vol = func_02038694();
        if (vol >= 4) {
            func_02038578(vol - 4, 1);
            func_02036198();
            func_02035f2c();
            return;
        }
        if (data_020c6b14.mUnk_1748 == 1) {
            data_020c6b14.mUnk_1748 = 3;
            return;
        }
        if (data_020c6b14.mUnk_00 != 0) {
            data_020c6b14.mUnk_00(3);
        }
        func_020359d0(0);
    }
}

// Frame update: counts the frames a stop has been pending and, in mode 3, calls the player's callback with 2
// and starts the next song.
void func_020360f4(void) {
    if (data_020c6b14.mUnk_1748 != 0) {
        data_020c6b14.mUnk_1718++;
        if (data_020c6b14.mUnk_1718 > 8) {
            func_020359d0(0);
        }
    }
    if (data_020c6b14.mUnk_1748 != 3) {
        return;
    }
    if (data_020c6b14.mUnk_00 != 0) {
        data_020c6b14.mUnk_00(2);
    }
    func_02035108(data_020c6b14.mUnk_1730, data_020c6b14.mUnk_1732, data_020c6b14.mUnk_1749, data_020c6b14.mUnk_173c, 0,
                  data_020c6b14.mUnk_1734);
}

void func_02036194(void) {}

// Player tick, run from the timer interrupt. Each time the tick accumulator passes the tick length the tick counter
// advances: on the first tick of a row the row's cells are read (note, instrument, volume column, effect, parameter;
// the volume column is applied as a second effect), the samples of new notes are started and the first-tick effects
// applied; on the other ticks the running effects and envelopes update period, volume and pan. The changes go to the
// sound driver as channel commands. With flag 0x400 of data_020c18bc.mUnk_2c8 each channel's effect parameter is
// formatted for display.
void func_02036198(void) {
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
    globalVol = data_020c6b14.mUnk_1747;
    data_020c6b14.mUnk_1720 += data_020c6b14.mUnk_1714;
    if (data_020c6b14.mUnk_1720 >= data_020c6b14.mUnk_171c) {
        data_020c6b14.mUnk_1720 -= data_020c6b14.mUnk_171c;
        nextRow = -1;
        data_020c6b14.mUnk_1856++;
        if (data_020c6b14.mUnk_1856 >= data_020c6b14.mUnk_1852 + data_020c6b14.mUnk_1858) {
            data_020c6b14.mUnk_1856 = 0;
        }
        if (data_020c6b14.mUnk_1856 == 0) {
            data_020c6b14.mUnk_1858 = 0;
            if (data_020c6b14.mUnk_1742 > data_020c6b14.mUnk_1740) {
                data_020c6b14.mUnk_1742 = 0;
                func_02037c10();
            }
            // Five bytes per cell, one cell per channel.
            cell = (u8 *) data_020c6b14.mUnk_10[data_020c6b14.mUnk_1854]->mUnk_04 +
                   data_020c6b14.mUnk_1742 * data_020c6b14.mUnk_174c * 5;
            for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
                c = &data_020c6b14.mUnk_610[i];
                trigger = FALSE;
                c->mUnk_4c = 0;
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
                                if (data_020c6b14.mUnk_00 != 0 && cellParam <= data_020c6b14.mUnk_1738) {
                                    data_020c6b14.mUnk_00(4);
                                }
                                data_020c6b14.mUnk_1742 = data_020c6b14.mUnk_1740;
                                data_020c6b14.mUnk_1738 = cellParam - 1;
                                break;
                            case TRACKER_FX_ROW_BREAK:
                                param = hi * 10 + lo;
                                if (param < data_020c6b14.mUnk_1740) {
                                    nextRow = param;
                                } else {
                                    nextRow = 0;
                                }
                                break;
                            case TRACKER_FX_SPEED:
                                if (cellParam < 32) {
                                    if (cellParam != 0) {
                                        data_020c6b14.mUnk_1852 = cellParam;
                                    }
                                } else {
                                    data_020c6b14.mUnk_1853 = cellParam;
                                    data_020c6b14.mUnk_171c = 8192000 / (data_020c6b14.mUnk_1853 * 50);
                                }
                                break;
                        }
                    }
                    if (inst != 0) {
                        if (inst > data_020c6b14.mUnk_174e) {
                            OS_Terminate();
                        }
                        if (data_020c6b14.mUnk_410[inst - 1]->mUnk_26c == 0) {
                            OS_Terminate();
                        }
                        instPresent = TRUE;
                        if (c->mUnk_15 != inst || c->mUnk_10 == 1) {
                            c->mUnk_15 = inst;
                            instChanged = TRUE;
                        }
                        c->mUnk_1d = 0;
                        trigger = TRUE;
                    } else {
                        inst = c->mUnk_15;
                    }
                    if (note == TRACKER_NOTE_NONE) {
                        note = c->mUnk_14;
                    } else {
                        c->mUnk_14 = note;
                        notePresent = TRUE;
                        c->mUnk_1d = 0;
                        trigger = notePresent;
                    }
                    if (note != TRACKER_NOTE_NONE && inst != 0 && inst <= data_020c6b14.mUnk_174e) {
                        if (cellEffect == TRACKER_FX_EXTENDED && lo == TRACKER_FXE_FINETUNE) {
                            TrackerInstrument *in = data_020c6b14.mUnk_410[inst - 1];

                            in->mUnk_000[in->mUnk_26d[note - 1]]->mUnk_13 = lo;
                        }
                        if (trigger == TRUE) {
                            inst--;
                            note--;
                            if (data_020c6b14.mUnk_410[inst]->mUnk_26d[note] >=
                                data_020c6b14.mUnk_410[inst]->mUnk_26c) {
                                OS_Terminate();
                            }
                            c->mUnk_00 =
                                data_020c6b14.mUnk_410[inst]->mUnk_000[data_020c6b14.mUnk_410[inst]->mUnk_26d[note]];
                            if (c->mUnk_00 == 0) {
                                OS_Terminate();
                            }
                            if (instPresent == TRUE) {
                                volUpd = TRUE;
                                panUpd = TRUE;
                                c->mUnk_24 = c->mUnk_00->mUnk_12;
                                c->mUnk_26 = c->mUnk_00->mUnk_16;
                            }
                            period = func_02037eac((u8) (note + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                        } else {
                            period = c->mUnk_04;
                        }
                        c->mUnk_17 = TRACKER_FX_NONE;
                        c->mUnk_16 = TRACKER_FX_NONE;
                        c->mUnk_1a = 0;
                        c->mUnk_18 = 0;
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
                                    c->mUnk_20 = vol & 0xf;
                                } else if (vol >= 0xb0 && vol <= 0xc0) {
                                    c->mUnk_21 = vol & 0xf;
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
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                        c->mUnk_1c = 0;
                                    }
                                    break;
                                case TRACKER_FX_SLIDE_UP:
                                    if (param != 0) {
                                        c->mUnk_6a[0] = param;
                                    } else {
                                        param = c->mUnk_6a[0];
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_18 <<= 2;
                                    }
                                    break;
                                case TRACKER_FX_SLIDE_DOWN:
                                    if (param != 0) {
                                        c->mUnk_6a[1] = param;
                                    } else {
                                        param = c->mUnk_6a[1];
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_18 <<= 2;
                                    }
                                    break;
                                case TRACKER_FX_TONE_SLIDE:
                                case TRACKER_FX_TONE_SLIDE_VOL:
                                    if (param != 0) {
                                        if (effect == TRACKER_FX_TONE_SLIDE) {
                                            c->mUnk_6a[2] = param;
                                        } else {
                                            c->mUnk_6a[4] = param;
                                        }
                                    } else {
                                        if (effect == TRACKER_FX_TONE_SLIDE) {
                                            param = c->mUnk_6a[2];
                                        } else {
                                            param = c->mUnk_6a[4];
                                        }
                                    }
                                    if (k != 0) {
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    } else {
                                        c->mUnk_17 = effect;
                                        c->mUnk_1a = param;
                                    }
                                    if (notePresent == TRUE && period != 0) {
                                        c->mUnk_06 = period;
                                    }
                                    if (param != 0 && effect == TRACKER_FX_TONE_SLIDE) {
                                        c->mUnk_1e = param;
                                        if (data_020c6b14.mUnk_174f & 1) {
                                            c->mUnk_1e <<= 2;
                                        }
                                    }
                                    porta = TRUE;
                                    break;
                                case TRACKER_FX_VIBRATO:
                                    if (param != 0) {
                                        c->mUnk_6a[3] = param;
                                    } else {
                                        param = c->mUnk_6a[3];
                                        hi = (param & 0xf0) >> 4;
                                        lo = param & 0xf;
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (hi != 0 && lo != 0) {
                                        c->mUnk_20 = hi;
                                        c->mUnk_21 = lo;
                                    }
                                    break;
                                case TRACKER_FX_PAN:
                                    c->mUnk_26 = param;
                                    panUpd = TRUE;
                                    break;
                                case TRACKER_FX_TREMOLO:
                                    if (param != 0) {
                                        c->mUnk_6a[6] = param;
                                    } else {
                                        param = c->mUnk_6a[6];
                                        hi = (param & 0xf0) >> 4;
                                        lo = param & 0xf;
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    if (hi != 0 && lo != 0) {
                                        c->mUnk_22 = hi;
                                        c->mUnk_23 = lo;
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
                                        c->mUnk_6a[5] = param;
                                    } else {
                                        param = c->mUnk_6a[5];
                                    }
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    break;
                                case TRACKER_FX_VOL_SLIDE:
                                    if (param != 0) {
                                        c->mUnk_6a[7] = param;
                                    } else {
                                        param = c->mUnk_6a[7];
                                    }
                                    if (k != 0) {
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    } else {
                                        c->mUnk_17 = effect;
                                        c->mUnk_1a = param;
                                    }
                                    break;
                                case TRACKER_FX_VOLUME:
                                    if (param > 64) {
                                        param = 64;
                                    }
                                    c->mUnk_24 = param;
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_EXTENDED:
                                    c->mUnk_16 = effect;
                                    c->mUnk_18 = param;
                                    switch (hi) {
                                        case TRACKER_FXE_VIBRATO_WAVE:
                                            c->mUnk_08 = data_02063c9c[lo & 7];
                                            break;
                                        case TRACKER_FXE_TREMOLO_WAVE:
                                            c->mUnk_0c = data_02063c9c[lo & 7];
                                            break;
                                        case TRACKER_FXE_RETRIGGER:
                                            c->mUnk_4c = TRACKER_START_RETRIGGER;
                                            c->mUnk_4d = 0;
                                            c->mUnk_4e = lo;
                                            break;
                                        case TRACKER_FXE_PAN:
                                            c->mUnk_26 = lo << 4;
                                            panUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_NOTE_DELAY:
                                            c->mUnk_4c = TRACKER_START_DELAYED;
                                            c->mUnk_4d = 0;
                                            c->mUnk_4e = lo;
                                            break;
                                        case TRACKER_FXE_ROW_DELAY:
                                            if (lo != 0) {
                                                data_020c6b14.mUnk_1858 = (lo - 1) * data_020c6b14.mUnk_1852;
                                            }
                                            break;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOLUME:
                                    if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                        globalVol = param << 1;
                                        if (globalVol > 128) {
                                            globalVol = 128;
                                        }
                                        volChanged = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOL_SLIDE:
                                    if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                        if (param != 0) {
                                            c->mUnk_6a[12] = param;
                                        } else {
                                            param = c->mUnk_6a[12];
                                        }
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    }
                                    break;
                                case TRACKER_FX_PAN_SLIDE:
                                    if (param != 0) {
                                        c->mUnk_6a[13] = param;
                                    } else {
                                        param = c->mUnk_6a[13];
                                    }
                                    if (k != 0) {
                                        c->mUnk_16 = effect;
                                        c->mUnk_18 = param;
                                    } else {
                                        c->mUnk_17 = effect;
                                        c->mUnk_1a = param;
                                    }
                                    break;
                            }
                        }
                        if (porta == FALSE) {
                            c->mUnk_04 = period;
                        } else {
                            period = c->mUnk_04;
                        }
                        volume = c->mUnk_24;
                        if (porta == TRUE && instChanged == FALSE) {
                            trigger = FALSE;
                        }
                        // First-tick part of the two stored effects.
                        for (k = 0; k < 2; k++) {
                            if (k == 0) {
                                param = c->mUnk_1a;
                                effect = c->mUnk_17;
                            } else {
                                param = c->mUnk_18;
                                effect = c->mUnk_16;
                            }
                            hi = (param & 0xf0) >> 4;
                            lo = param & 0xf;
                            switch (effect) {
                                case TRACKER_FX_ARPEGGIO:
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_SLIDE_UP:
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_04 -= param;
                                        if (c->mUnk_04 < TRACKER_PERIOD_MIN) {
                                            c->mUnk_04 = TRACKER_PERIOD_MIN;
                                        }
                                    }
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_SLIDE_DOWN:
                                    if (data_020c6b14.mUnk_174f & 1) {
                                        c->mUnk_04 += param;
                                    }
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    break;
                                case TRACKER_FX_TONE_SLIDE:
                                case TRACKER_FX_TONE_SLIDE_VOL:
                                    period = c->mUnk_04;
                                    periodUpd = TRUE;
                                    if (effect == TRACKER_FX_TONE_SLIDE_VOL) {
                                        func_02037df8(c, (s8) hi, (s8) lo);
                                        volume = c->mUnk_24;
                                        volUpd = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_VIBRATO:
                                case TRACKER_FX_VIBRATO_VOL:
                                    period = func_02037d38(c);
                                    periodUpd = TRUE;
                                    if (effect == TRACKER_FX_VIBRATO_VOL) {
                                        func_02037df8(c, (s8) hi, (s8) lo);
                                        volume = c->mUnk_24;
                                        volUpd = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_TREMOLO:
                                    volume = func_02037dac(c);
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_VOL_SLIDE:
                                    func_02037df8(c, (s8) hi, (s8) lo);
                                    volume = c->mUnk_24;
                                    volUpd = TRUE;
                                    break;
                                case TRACKER_FX_EXTENDED:
                                    switch (hi) {
                                        case TRACKER_FXE_FINE_SLIDE_UP:
                                            if (lo != 0) {
                                                c->mUnk_6a[8] = lo;
                                            } else {
                                                lo = c->mUnk_6a[8];
                                            }
                                            if (data_020c6b14.mUnk_174f & 1) {
                                                c->mUnk_04 -= lo << 2;
                                            } else {
                                                c->mUnk_04 -= lo;
                                            }
                                            if (c->mUnk_04 < TRACKER_PERIOD_MIN) {
                                                c->mUnk_04 = TRACKER_PERIOD_MIN;
                                            }
                                            period = c->mUnk_04;
                                            periodUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_FINE_SLIDE_DOWN:
                                            if (lo != 0) {
                                                c->mUnk_6a[9] = lo;
                                            } else {
                                                lo = c->mUnk_6a[9];
                                            }
                                            if (data_020c6b14.mUnk_174f & 1) {
                                                c->mUnk_04 += lo << 2;
                                            } else {
                                                c->mUnk_04 += lo;
                                            }
                                            period = c->mUnk_04;
                                            periodUpd = TRUE;
                                            break;
                                        case TRACKER_FXE_FINE_VOL_UP:
                                            if (lo != 0) {
                                                c->mUnk_6a[10] = lo;
                                            } else {
                                                lo = c->mUnk_6a[10];
                                            }
                                            if (lo != 0) {
                                                func_02037df8(c, (s8) lo, 0);
                                                volume = c->mUnk_24;
                                                volUpd = TRUE;
                                            }
                                            break;
                                        case TRACKER_FXE_FINE_VOL_DOWN:
                                            if (lo != 0) {
                                                c->mUnk_6a[11] = lo;
                                            } else {
                                                lo = c->mUnk_6a[11];
                                            }
                                            if (lo != 0) {
                                                func_02037df8(c, 0, (s8) lo);
                                                volume = c->mUnk_24;
                                                volUpd = TRUE;
                                            }
                                            break;
                                    }
                                    break;
                                case TRACKER_FX_GLOBAL_VOL_SLIDE:
                                    if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                        func_02037e70(&globalVol, (s8) hi, (s8) lo);
                                        volChanged = TRUE;
                                    }
                                    break;
                                case TRACKER_FX_PAN_SLIDE:
                                    func_02037e34(c, (s8) hi, (s8) lo);
                                    panUpd = TRUE;
                                    break;
                            }
                        }
                        if (trigger == TRUE) {
                            u32 freq;
                            u16 chVol;
                            TrackerSample *s;
                            u32 data;

                            freq = func_02037efc(period, (data_020c6b14.mUnk_174f & 1) ? TRUE : FALSE);
                            c->mUnk_10 = 0;
                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                func_02037f5c(&data_020c6b14, c, TRACKER_ENV_VOLUME);
                                func_02037f5c(&data_020c6b14, c, TRACKER_ENV_PAN);
                            } else {
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_VOLUME, cellParam);
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_PAN, cellParam);
                            }
                            c->mUnk_48 = 0x10000;
                            chVol = func_02040b60(data_02050b40[func_020384f8(
                                (s8) volume, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                            if (c->mUnk_4c != TRACKER_START_DELAYED) {
                                func_020400f4(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                            }
                            s = c->mUnk_00;
                            // A sample without a loop plays once from the offset; a looped one plays from its start.
                            if ((s->mUnk_14 & 3) == 0 || s->mUnk_08 == 0) {
                                if (sampleOffset >= s->mUnk_00) {
                                    goto next;
                                }
                                data = (u32) s->mUnk_0c;
                                if (data == 0) {
                                    goto next;
                                }
                                if (c->mUnk_4c != 0) {
                                    c->mUnk_4f = (s->mUnk_14 & 0x10) ? TRUE : FALSE;
                                    c->mUnk_54 = data + sampleOffset;
                                    c->mUnk_58 = (c->mUnk_00->mUnk_00 - sampleOffset) >> 2;
                                    c->mUnk_5c = 0;
                                    c->mUnk_52 = chVol;
                                    c->mUnk_60 = 0xffb0ff / freq;
                                    c->mUnk_50 = func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1;
                                }
                                if (c->mUnk_4c == TRACKER_START_DELAYED) {
                                    goto next;
                                }
                                c->mUnk_64 = 0xffb0ff / freq;
                                c->mUnk_68 = chVol;
                                func_02040240(data_020c6b14.mUnk_1744 + i, (c->mUnk_00->mUnk_14 & 0x10) ? TRUE : FALSE,
                                              data + sampleOffset, 2, 0, (c->mUnk_00->mUnk_00 - sampleOffset) >> 2,
                                              c->mUnk_68 & 0xff, c->mUnk_68 >> 8, c->mUnk_64,
                                              func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                                func_020400cc(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                            } else {
                                data = (u32) s->mUnk_0c;
                                if (data == 0) {
                                    goto next;
                                }
                                if (c->mUnk_4c != 0) {
                                    c->mUnk_4f = (s->mUnk_14 & 0x10) ? TRUE : FALSE;
                                    c->mUnk_54 = data;
                                    c->mUnk_58 = c->mUnk_00->mUnk_04 >> 2;
                                    c->mUnk_5c = c->mUnk_00->mUnk_08 >> 2;
                                    c->mUnk_52 = chVol;
                                    c->mUnk_60 = 0xffb0ff / freq;
                                    c->mUnk_50 = func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1;
                                }
                                if (c->mUnk_4c == TRACKER_START_DELAYED) {
                                    goto next;
                                }
                                c->mUnk_64 = 0xffb0ff / freq;
                                c->mUnk_68 = chVol;
                                s = c->mUnk_00;
                                func_02040240(data_020c6b14.mUnk_1744 + i, (s->mUnk_14 & 0x10) ? TRUE : FALSE, data, 1,
                                              s->mUnk_04 >> 2, s->mUnk_08 >> 2, c->mUnk_68 & 0xff, c->mUnk_68 >> 8,
                                              c->mUnk_64, func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                                func_020400cc(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                            }
                        } else {
                            BOOL changed;

                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                changed = func_020380b8(&data_020c6b14, c, TRACKER_ENV_VOLUME, FALSE) |
                                          func_02038464(&data_020c6b14, c);
                            } else {
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_VOLUME, cellParam);
                                changed = TRUE;
                            }
                            if (changed == TRUE && volUpd == FALSE) {
                                volume = c->mUnk_24;
                                volUpd = TRUE;
                            }
                            if (cellEffect != TRACKER_FX_ENV_POS) {
                                if (func_020380b8(&data_020c6b14, c, TRACKER_ENV_PAN, FALSE) == TRUE) {
                                    panUpd = TRUE;
                                }
                            } else {
                                func_020382c0(&data_020c6b14, c, TRACKER_ENV_PAN, cellParam);
                                panUpd = TRUE;
                            }
                            if (periodUpd == TRUE) {
                                c->mUnk_64 =
                                    0xffb0ff / func_02037efc(period, (data_020c6b14.mUnk_174f & 1) ? TRUE : FALSE);
                                func_020401d8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_64);
                            }
                            if (volUpd == TRUE) {
                                c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(
                                    (s8) volume, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                                func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
                            }
                            if (panUpd == TRUE) {
                                func_02040220(1 << (data_020c6b14.mUnk_1744 + i),
                                              func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                            }
                            if (data_020c18bc.mUnk_2c8 & 0x400) {
                                // The debug display flag is tested here, but nothing in this branch formats the
                                // parameter: the statement only keeps the test, as in the original.
                                param = (u16) param;
                            }
                        }
                    } else {
                        if (data_020c18bc.mUnk_2c8 & 0x400) {
                            effect = TRACKER_FX_NONE;
                            param = 0;
                        }
                    }
                } else {
                    BOOL changed;

                    c->mUnk_10 = 1;
                    changed =
                        func_020380b8(&data_020c6b14, c, TRACKER_ENV_VOLUME, TRUE) | func_02038464(&data_020c6b14, c);
                    if (changed == TRUE) {
                        c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(
                            c->mUnk_24, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                        func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
                    }
                    if (func_020380b8(&data_020c6b14, c, TRACKER_ENV_PAN, TRUE) == TRUE) {
                        func_02040220(1 << (data_020c6b14.mUnk_1744 + i),
                                      func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                    }
                }
            next:
                if (data_020c18bc.mUnk_2c8 & 0x400) {
                    if (effect != TRACKER_FX_NONE) {
                        sprintf(text, "%02x", param);
                    } else {
                        sprintf(text, "--");
                    }
                }
                cell += 5;
            }
            data_020c6b14.mUnk_1742++;
            if (nextRow != -1) {
                data_020c6b14.mUnk_1742 = nextRow;
                func_02037c10();
            }
        } else {
            for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
                u8 tickEffect;
                u8 tickParam;
                u8 tickHi;
                u8 tickLo;
                u8 tickVolume;
                BOOL tickPeriodUpd;

                c = &data_020c6b14.mUnk_610[i];
                tickPeriodUpd = FALSE;
                volUpd = FALSE;
                tickPanUpd = FALSE;
                // A delayed note starts, or a retriggered one restarts, when its tick count is reached.
                if (c->mUnk_4c != 0) {
                    if (++c->mUnk_4d == c->mUnk_4e) {
                        if (c->mUnk_4c != TRACKER_START_DELAYED) {
                            c->mUnk_4d = 0;
                        } else {
                            c->mUnk_4c = 0;
                        }
                        func_020400f4(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                        if (c->mUnk_5c == 0) {
                            func_02040240(data_020c6b14.mUnk_1744 + i, c->mUnk_4f, c->mUnk_54, 2, 0, c->mUnk_58,
                                          c->mUnk_52 & 0xff, c->mUnk_52 >> 8, c->mUnk_60, c->mUnk_50);
                        } else {
                            func_02040240(data_020c6b14.mUnk_1744 + i, c->mUnk_4f, c->mUnk_54, 1, c->mUnk_58,
                                          c->mUnk_5c, c->mUnk_52 & 0xff, c->mUnk_52 >> 8, c->mUnk_60, c->mUnk_50);
                        }
                        func_020400cc(1 << (data_020c6b14.mUnk_1744 + i), 0, 0, 0);
                    }
                }
                for (tickK = 0; tickK < 2; tickK++) {
                    if (tickK == 0) {
                        tickParam = c->mUnk_1a;
                        tickEffect = c->mUnk_17;
                    } else {
                        tickParam = c->mUnk_18;
                        tickEffect = c->mUnk_16;
                    }
                    tickHi = (tickParam & 0xf0) >> 4;
                    tickLo = tickParam & 0xf;
                    switch (tickEffect) {
                        case TRACKER_FX_ARPEGGIO:
                            note = c->mUnk_14 - 1;
                            switch ((u8) (c->mUnk_1c % 3)) {
                                case 0:
                                    period = func_02037eac((u8) (note + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                                    break;
                                case 1:
                                    period =
                                        func_02037eac((u8) (note + tickHi + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                                    break;
                                case 2:
                                    period =
                                        func_02037eac((u8) (note + tickLo + c->mUnk_00->mUnk_15), c->mUnk_00->mUnk_13);
                                    break;
                            }
                            c->mUnk_1c++;
                            tickPeriodUpd = TRUE;
                            break;
                        case TRACKER_FX_SLIDE_UP:
                            c->mUnk_04 -= tickParam;
                            if (c->mUnk_04 < TRACKER_PERIOD_MIN) {
                                c->mUnk_04 = TRACKER_PERIOD_MIN;
                            }
                            tickPeriodUpd = TRUE;
                            period = c->mUnk_04;
                            break;
                        case TRACKER_FX_SLIDE_DOWN:
                            c->mUnk_04 += tickParam;
                            tickPeriodUpd = TRUE;
                            period = c->mUnk_04;
                            break;
                        case TRACKER_FX_TONE_SLIDE:
                        case TRACKER_FX_TONE_SLIDE_VOL:
                            func_02037cdc(c);
                            period = c->mUnk_04;
                            tickPeriodUpd = TRUE;
                            if (tickEffect == TRACKER_FX_TONE_SLIDE_VOL) {
                                func_02037df8(c, (s8) tickHi, (s8) tickLo);
                                tickVolume = c->mUnk_24;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_VIBRATO:
                        case TRACKER_FX_VIBRATO_VOL:
                            period = func_02037d38(c);
                            tickPeriodUpd = TRUE;
                            if (tickEffect == TRACKER_FX_VIBRATO_VOL) {
                                func_02037df8(c, (s8) tickHi, (s8) tickLo);
                                tickVolume = c->mUnk_24;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_TREMOLO:
                            tickVolume = func_02037dac(c);
                            volUpd = TRUE;
                            break;
                        case TRACKER_FX_VOL_SLIDE:
                            func_02037df8(c, (s8) tickHi, (s8) tickLo);
                            tickVolume = c->mUnk_24;
                            volUpd = TRUE;
                            break;
                        case TRACKER_FX_EXTENDED:
                            if (tickHi == TRACKER_FXE_NOTE_CUT && data_020c6b14.mUnk_1856 >= tickLo) {
                                c->mUnk_24 = 0;
                                tickVolume = 0;
                                volUpd = TRUE;
                            }
                            break;
                        case TRACKER_FX_GLOBAL_VOL_SLIDE:
                            if (data_020c6b14.mUnk_1748 != 1 && data_020c6b14.mUnk_1748 != 2) {
                                func_02037e70(&globalVol, (s8) tickHi, (s8) tickLo);
                                volChanged = TRUE;
                            }
                            break;
                        case TRACKER_FX_PAN_SLIDE:
                            func_02037e34(c, (s8) tickHi, (s8) tickLo);
                            tickPanUpd = TRUE;
                            break;
                    }
                }
                if ((func_020380b8(&data_020c6b14, c, TRACKER_ENV_VOLUME, FALSE) | func_02038464(&data_020c6b14, c)) ==
                        TRUE &&
                    volUpd == FALSE) {
                    tickVolume = c->mUnk_24;
                    volUpd = TRUE;
                }
                if (func_020380b8(&data_020c6b14, c, TRACKER_ENV_PAN, FALSE) == TRUE) {
                    tickPanUpd = TRUE;
                }
                if (tickPeriodUpd == TRUE) {
                    c->mUnk_64 = 0xffb0ff / func_02037efc(period, (data_020c6b14.mUnk_174f & 1) ? TRUE : FALSE);
                    func_020401d8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_64);
                }
                if (volUpd == TRUE) {
                    c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(
                        (s8) tickVolume, c->mUnk_28.mUnk_04, c->mUnk_48, data_020c6b14.mUnk_1747, c->mUnk_25)]);
                    func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
                }
                if (tickPanUpd == TRUE) {
                    func_02040220(1 << (data_020c6b14.mUnk_1744 + i),
                                  func_02038524(c->mUnk_26, c->mUnk_38.mUnk_04) >> 1);
                }
            }
        }
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
    if (volChanged == TRUE) {
        func_02038578(globalVol, 0);
    }
}

// Advances to the next order position (or the jump target), stops the voices when the song ends or loops and
// loads the pattern's row count.
void func_02037c10(void) {
    if (data_020c6b14.mUnk_173a != -1) {
        data_020c6b14.mUnk_1738 = data_020c6b14.mUnk_173a;
        data_020c6b14.mUnk_173a = -1;
        if (data_020c6b14.mUnk_173e == 1) {
            func_02035e7c();
        }
    } else {
        data_020c6b14.mUnk_1738 = data_020c6b14.mUnk_1738 + 1;
    }
    if (data_020c6b14.mUnk_1738 >= data_020c6b14.mUnk_174a) {
        func_02035e7c();
        data_020c6b14.mUnk_1738 = data_020c6b14.mUnk_174b;
        if (data_020c6b14.mUnk_00 != 0) {
            data_020c6b14.mUnk_00(4);
        }
    }
    data_020c6b14.mUnk_1854 = data_020c6b14.mUnk_1752[data_020c6b14.mUnk_1738];
    data_020c6b14.mUnk_1740 = data_020c6b14.mUnk_10[data_020c6b14.mUnk_1854]->mUnk_01 - 1;
}

// Moves the channel's period toward its target by the slide speed.
void func_02037cdc(TrackerChannel *c) {
    if (c->mUnk_04 < c->mUnk_06) {
        c->mUnk_04 = c->mUnk_04 + c->mUnk_1e;
        if (c->mUnk_04 > c->mUnk_06) {
            c->mUnk_04 = c->mUnk_06;
        }
    }
    if (c->mUnk_04 > c->mUnk_06) {
        c->mUnk_04 = c->mUnk_04 - c->mUnk_1e;
        if (c->mUnk_04 < c->mUnk_06) {
            c->mUnk_04 = c->mUnk_06;
        }
    }
}

// Period with the vibrato offset from the channel's wave table added; steps the wave position.
u16 func_02037d38(TrackerChannel *c) {
    u16 period = c->mUnk_04;

    if (data_020c6b14.mUnk_174f & 1) {
        period += (c->mUnk_21 * c->mUnk_08[c->mUnk_1d]) >> 5;
    } else {
        period += (c->mUnk_21 * c->mUnk_08[c->mUnk_1d]) >> 7;
    }
    c->mUnk_1d += c->mUnk_20;
    c->mUnk_1d &= 0x3f;
    return period;
}

// Volume with the tremolo offset added (capped at 64); steps the wave position.
u8 func_02037dac(TrackerChannel *c) {
    u8 v = (u8) c->mUnk_24 + ((c->mUnk_23 * c->mUnk_0c[c->mUnk_1d]) >> 6);

    if (v > 64) {
        v = 64;
    }
    c->mUnk_1d += c->mUnk_22;
    c->mUnk_1d &= 0x3f;
    return v;
}

// Volume slide: adds up when it is non-zero, subtracts down otherwise; kept within 0..64.
void func_02037df8(TrackerChannel *c, int up, int down) {
    if (up != 0) {
        c->mUnk_24 = c->mUnk_24 + up;
    } else {
        c->mUnk_24 = c->mUnk_24 - down;
    }
    if (c->mUnk_24 < 0) {
        c->mUnk_24 = 0;
    }
    if (c->mUnk_24 > 64) {
        c->mUnk_24 = 64;
    }
}

// Panning slide: adds up when it is non-zero, subtracts down otherwise; kept within 0..255.
void func_02037e34(TrackerChannel *c, int up, int down) {
    if (up != 0) {
        c->mUnk_26 = c->mUnk_26 + up;
    } else {
        c->mUnk_26 = c->mUnk_26 - down;
    }
    if (c->mUnk_26 < 0) {
        c->mUnk_26 = 0;
    }
    if (c->mUnk_26 > 255) {
        c->mUnk_26 = 255;
    }
}

// Master volume slide in steps of two: adds up when it is non-zero, subtracts down otherwise; kept within 0..128.
void func_02037e70(s8 *v, int up, int down) {
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
u16 func_02037eac(int note, int finetune) {
    if ((data_020c6b14.mUnk_174f & 1) == 0) {
        return data_02051844[(finetune >> 4) + 8][note];
    }
    return 7680 - note * 64 - (finetune >> 1);
}

// Playback rate of a period: a clock divided by twice the period, or with the linear flag the table entry
// shifted down by the octave.
u32 func_02037efc(int period, int linear) {
    if (linear == 0) {
        return 7093789 / (period * 2);
    }
    return data_02050c44[period % 768] >> (period / 768);
}

// Starts the volume (type 0) or panning (type 1) envelope of a channel's instrument; without a usable instrument
// the level is set to its default.
void func_02037f5c(TrackerPlayer *p, TrackerChannel *c, int type) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u8 inst = c->mUnk_15;

    if (inst != 0 && inst <= data_020c6b14.mUnk_174e) {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                st = &c->mUnk_28;
                env = &p->mUnk_410[inst - 1]->mUnk_200;
                st->mUnk_04 = TRACKER_ENV_VOLUME_LEVEL;
                break;
            case TRACKER_ENV_PAN:
                st = &c->mUnk_38;
                env = &p->mUnk_410[inst - 1]->mUnk_236;
                st->mUnk_04 = TRACKER_ENV_PAN_LEVEL;
                break;
        }
        if ((env->mUnk_34 & TRACKER_ENV_ON) == 0 || env->mUnk_30 <= 1) {
            return;
        }
        st->mUnk_00 = 0;
        st->mUnk_02 = env->mUnk_00[st->mUnk_00];
        if (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02 > 0) {
            st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
            st->mUnk_08 = (env->mUnk_18[st->mUnk_00 + 1] << 8) - st->mUnk_04;
            st->mUnk_08 = st->mUnk_08 / (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02);
        } else {
            st->mUnk_02 = st->mUnk_02 - 1;
            st->mUnk_04 = env->mUnk_18[st->mUnk_00 + 1] << 8;
            st->mUnk_08 = 0;
        }
        st->mUnk_0c = 0;
    } else {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                c->mUnk_28.mUnk_04 = TRACKER_ENV_VOLUME_LEVEL;
                break;
            case TRACKER_ENV_PAN:
                c->mUnk_38.mUnk_04 = TRACKER_ENV_PAN_LEVEL;
                break;
        }
    }
}

// Advances an envelope by one frame (holding at the sustain point until keyOff is 1, looping when the envelope
// loops); returns 1 when the level's whole part changed.
int func_020380b8(TrackerPlayer *p, TrackerChannel *c, int type, int keyOff) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u32 before;
    u8 inst = c->mUnk_15;

    if (inst != 0 && inst <= data_020c6b14.mUnk_174e) {
        switch (type) {
            case TRACKER_ENV_VOLUME:
                st = &c->mUnk_28;
                env = &p->mUnk_410[inst - 1]->mUnk_200;
                break;
            case TRACKER_ENV_PAN:
                st = &c->mUnk_38;
                env = &p->mUnk_410[inst - 1]->mUnk_236;
                break;
        }
        if ((env->mUnk_34 & TRACKER_ENV_ON) && env->mUnk_30 > 1) {
            if (keyOff == 1) {
                st->mUnk_0c = 1;
            }
            if (st->mUnk_00 < env->mUnk_30 - 1) {
                if (st->mUnk_0c == 1 || st->mUnk_00 != env->mUnk_31 || (env->mUnk_34 & TRACKER_ENV_SUSTAIN) == 0) {
                    before = st->mUnk_04 >> 8;
                    st->mUnk_04 = st->mUnk_04 + st->mUnk_08;
                    st->mUnk_02 = st->mUnk_02 + 1;
                    if (st->mUnk_02 == env->mUnk_00[st->mUnk_00 + 1]) {
                        st->mUnk_00++;
                        if (env->mUnk_34 & TRACKER_ENV_LOOP) {
                            if (st->mUnk_00 == env->mUnk_33) {
                                st->mUnk_00 = env->mUnk_32;
                            }
                        }
                        if (st->mUnk_00 < env->mUnk_30 - 1) {
                            st->mUnk_02 = env->mUnk_00[st->mUnk_00];
                            if (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02 > 0) {
                                st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
                                st->mUnk_08 = (env->mUnk_18[st->mUnk_00 + 1] << 8) - st->mUnk_04;
                                st->mUnk_08 = st->mUnk_08 / (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02);
                            } else {
                                st->mUnk_02 = st->mUnk_02 - 1;
                                st->mUnk_04 = env->mUnk_18[st->mUnk_00 + 1] << 8;
                                st->mUnk_08 = 0;
                            }
                        }
                    }
                    return before != (st->mUnk_04 >> 8);
                }
                return 0;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

// Places an envelope at frame pos: finds the segment, sets the level and the per-frame step.
void func_020382c0(TrackerPlayer *p, TrackerChannel *c, int type, u16 pos) {
    TrackerEnvelope *env;
    TrackerEnvState *st;
    u8 inst = c->mUnk_15;

    if (inst == 0 || inst > data_020c6b14.mUnk_174e) {
        return;
    }
    switch (type) {
        case TRACKER_ENV_VOLUME:
            st = &c->mUnk_28;
            env = &p->mUnk_410[inst - 1]->mUnk_200;
            break;
        case TRACKER_ENV_PAN:
            st = &c->mUnk_38;
            env = &p->mUnk_410[inst - 1]->mUnk_236;
            break;
    }
    if ((env->mUnk_34 & TRACKER_ENV_ON) == 0 || env->mUnk_30 <= 1) {
        return;
    }
    st->mUnk_00 = 0;
    for (;;) {
        if (st->mUnk_00 == env->mUnk_30 - 1) {
            break;
        }
        if (pos < env->mUnk_00[st->mUnk_00 + 1]) {
            break;
        }
        st->mUnk_00 = st->mUnk_00 + 1;
    }
    if (st->mUnk_00 == env->mUnk_30 - 1) {
        st->mUnk_02 = env->mUnk_00[st->mUnk_00];
        st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
        st->mUnk_08 = 0;
        return;
    }
    if (env->mUnk_00[st->mUnk_00 + 1] - st->mUnk_02 > 0) {
        st->mUnk_04 = env->mUnk_18[st->mUnk_00] << 8;
        st->mUnk_08 = (env->mUnk_18[st->mUnk_00 + 1] << 8) - st->mUnk_04;
        st->mUnk_08 = st->mUnk_08 / (env->mUnk_00[st->mUnk_00 + 1] - env->mUnk_00[st->mUnk_00]);
        st->mUnk_02 = pos;
        st->mUnk_04 = st->mUnk_08 * (st->mUnk_02 - env->mUnk_00[st->mUnk_00]) + st->mUnk_04;
    } else {
        st->mUnk_04 = env->mUnk_18[st->mUnk_00 + 1] << 8;
        st->mUnk_08 = 0;
        st->mUnk_02 = pos - 1;
    }
}

// After key-off, lowers the channel's fade-out level by the instrument's fade step (or clears it without a
// volume envelope); returns 1 when the level changed.
int func_02038464(TrackerPlayer *p, TrackerChannel *c) {
    TrackerInstrument *instr;
    s32 before;
    u8 inst;

    if (c->mUnk_10 == 1) {
        inst = c->mUnk_15;
        if (inst != 0 && inst <= data_020c6b14.mUnk_174e) {
            instr = p->mUnk_410[inst - 1];
            before = c->mUnk_48;
            if (instr->mUnk_200.mUnk_34 & TRACKER_ENV_ON) {
                c->mUnk_48 = before - instr->mUnk_2d2;
                if (c->mUnk_48 <= 0) {
                    c->mUnk_48 = 0;
                }
            } else {
                c->mUnk_48 = 0;
            }
            return before != c->mUnk_48;
        }
        return 0;
    }
    return 0;
}

// Table index (0..64) of a channel's output volume: channel volume, volume envelope level, fade-out level and
// master volume multiplied together.
int func_020384f8(int vol, u16 env, int fade, u8 master, u8 chanVol) {
    return ((((fade * master) >> 16) * (((vol * chanVol) >> 7 << 3) * env >> 16)) >> 8);
}

// Panning (0..255) after the panning envelope level is applied, weighted toward the middle.
u32 func_02038524(int pan, u16 level) {
    int v = pan + (((level >> 8) - 32) * (128 - abs(pan - 128))) / 32;

    if ((v < 255 ? v : 255) < 0) {
        return 0;
    }
    return v < 255 ? v : 255;
}

// Sets the master volume (0 unless the system flag allows sound, at most 128) and, while a song plays, sends the
// new volume of each active hardware channel.
void func_02038578(u8 volume, int mode) {
    u32 mask;
    u32 i;

    if (data_020c18bc.mUnk_493 == 0) {
        volume = 0;
    }
    if (volume > 128) {
        volume = 128;
    }
    data_020c6b14.mUnk_1747 = volume;
    if (data_020c6b14.mUnk_1724 != 1) {
        return;
    }
    if (data_020c6b14.mUnk_1748 != 0 && mode != 1) {
        return;
    }
    mask = func_02040a90();
    for (i = 0; i < data_020c6b14.mUnk_174c; i++) {
        if (mask & (1 << (data_020c6b14.mUnk_1744 + i))) {
            TrackerChannel *c = &data_020c6b14.mUnk_610[i];

            c->mUnk_68 = func_02040b60(data_02050b40[func_020384f8(c->mUnk_24, c->mUnk_28.mUnk_04, c->mUnk_48,
                                                                   data_020c6b14.mUnk_1747, c->mUnk_25)]);
            func_020401f8(1 << (data_020c6b14.mUnk_1744 + i), c->mUnk_68 & 0xff, c->mUnk_68 >> 8);
        }
    }
    while (SND_RecvCommandReply(0) != 0) {
    }
    SND_FlushCommand(2);
}

u8 func_02038694(void) {
    return data_020c6b14.mUnk_1747;
}

#pragma thumb on
