#pragma once

#include "types.h"
#include <nitro/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

// One 16-byte record of the stage data file's position chunk: the key, flags, group and a position kept as three
// 32-bit values split into halfwords.
typedef struct MapEntry {
    u16 mUnk_00;
    u8 mUnk_02;
    u8 mUnk_03;
    u16 mUnk_04[2];
    u16 mUnk_08[2];
    u16 mUnk_0c[2];
} MapEntry;

#define MAP_RECORDS 20

// One object of a stage's object list (chunk entries of 4 + 2 * n bytes): the index of its entry in chunk 0x5513, a
// group byte, the number n of halfwords and the halfwords.
typedef struct MapObjDef {
    u16 mUnk_00;
    u8 mUnk_02;
    u8 mUnk_03;
    u16 mUnk_04[1];
} MapObjDef;

// The state of one placed object (0x190 bytes): its definition, a model/animation object at 4, the animation shown
// and the flags at 0x176.
typedef struct MapRecord {
    MapObjDef *mUnk_00;
    u8 mUnk_04[0x150 - 4];
    u16 mUnk_150;
    u16 mUnk_152;
    u8 mUnk_154[0x174 - 0x154];
    u16 mUnk_174;
    u16 mUnk_176_0 : 4;
    u16 mUnk_176_4 : 1;
    u16 mUnk_176_5 : 2;
    u16 mUnk_176_7 : 1;
    u16 mUnk_176_8 : 1;
    u16 mUnk_176_9 : 1;
    u16 mUnk_176_10 : 1;
    u16 mUnk_176_11 : 1;
    u16 mUnk_176_12 : 1;
    u16 mUnk_176_13 : 3;
    u8 mUnk_178[4];
    VecFx32 mUnk_17c;
    u16 mUnk_188;
    u8 mUnk_18a;
    u8 mUnk_18b;
    u8 mUnk_18c;
    u8 mUnk_18d[3];
} MapRecord;

// An animation track of the camera object: a timestamp at 0xc, the model id, frame and frame count, and flags.
typedef struct CameraTrack {
    u8 mUnk_00[0xc];
    u32 mUnk_0c;
    u16 mUnk_10;
    u16 mUnk_12;
    u16 mUnk_14;
    u8 mUnk_16_0 : 1;
    u8 mUnk_16_1 : 1;
    u8 mUnk_16_2 : 1;
    u8 mUnk_16_3 : 2;
    u8 mUnk_16_5 : 1;
    u8 mUnk_16_6 : 2;
} CameraTrack;

// The chunk of the stage data file whose key is id; 0 when the file has no such chunk.
u16 *func_02031eb8(u32 id);

// func_02031824's result: MAP_RING_HIT when a segment was found (the low bits hold its ring << 8 | segment), with
// MAP_RING_SEVERAL (more than one segment in range), MAP_RING_AT_END (a nearest point was a segment end) and
// MAP_RING_TIE (another segment was as near as the nearest).
#define MAP_RING_HIT 0x80000000
#define MAP_RING_SEVERAL 0x40000000
#define MAP_RING_AT_END 0x20000000
#define MAP_RING_TIE 0x10000000

u32 func_02031824(VecFx32 *pos, int radius, int top, int bottom, u16 any, u16 none);
void *func_02031c70(void *p);
int func_02031c98(u16 *entry, u16 mask, u16 bits);
int func_02031d0c(VecFx32 *pos, int a, int b, int c);
int func_02031dc4(s32 x, s32 z);
s32 func_02031f98(MapEntry *entry);
s32 func_02031fa4(MapEntry *entry);
s32 func_02031fb0(MapEntry *entry);
u16 func_02031fbc(MapEntry *entry);
void func_02031fc4(u32 id, VecFx32 *out);
void func_02032018(u32 index, VecFx32 *pos, u8 *flags);
MapEntry *func_0203204c(u32 a, u8 *count);
int func_020320c0(int a, int b, VecFx32 *pos);

// Defined further on in this file.
// The cell table of chunk 0x5510 (terminates when the chunk is missing).
u16 *func_02032898(void);
// The vertex index list of one polygon of chunk 0x5516; *count receives the vertex count.
u16 *func_020321ac(int a, int b, u32 *count);
// Stores up to `max` polygon keys matching (a, b) in out; returns how many.
u32 func_02032244(int a, int b, u16 *out, u32 max);
// Allocates the table of count pairs and clears its fill count.
void func_020322b8(u16 count);
// The distance from the camera's map position to the rectangle (x, z, w, h) in cells, 0 when it lies inside.
u32 func_020322f4(int x, int z, int w, int h);
// Classifies how far the rectangle is from the camera: the low bits of flags plus a limit class.
u32 func_02032368(u32 flags, int x, int z, int w, int h);
// Maps key through the sorted pair table (sorting it first when needed); key itself when it has no entry.
u32 func_0203249c(u32 key);
void func_020325b0(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f);
void func_020325d0(u16 a, u16 b, u16 c, u16 count);
// Frees the map buffers and clears the records.
void func_02032850(void);
// Clears the 20 records of data_020c3e44 (and the camera's halfwords at 0x24/0x26 when a == 1).
void func_02032b88(int a);
// Runs the actions of one placed object's definition and starts the animation they pick.
void func_020328b0(MapRecord *rec);
// Places (or finds) the object of each definition of a list and draws it.
void func_020329d0(u8 *objs);
// Marks the grid cell next to (x0, z0) on the way to (x1, z1); 0 when no cell on the way is marked.
int func_02032bb8(int x0, int z0, int w, int x1, int z1);
// Collects the visible cells around the grid cell of (px, pz).
void func_02032d50(u32 px, u32 pz);
// Collects the cell blocks of chunk 0x5512 in view of the camera, starting from the cell under (x, z).
void func_0203315c(int x, int z);
// Moves the camera a step towards its goal and sets up the projection.
void func_02033488(void);
// The camera's field of view angle.
u16 func_020334ec(void);
// Sets up the projection with the far distance scaled by 8 when a is 1.
void func_020334f8(int a);
// Sets up the projection and view matrices from the camera.
void func_02033520(void);
// Rebuilds the visible cells, objects and markers for a camera at (x, z) and draws the markers.
void func_02033614(s32 x, s32 z);
// Turns the view angle towards the look-at point.
void func_02033b04(void);
// One step of the view angle blend.
void func_02033b8c(void);
// Moves the look-at point towards pos.
void func_0203393c(VecFx32 *pos);
// Moves the camera position towards pos.
void func_02033c0c(VecFx32 *pos);
// Starts the view angle blend over a degrees.
void func_02033df4(int a);
// Sets the goal of the position or look-at blend.
void func_02033e38(int a, int b, int c);
// Advances an animation track.
void func_02033f38(CameraTrack *t, s32 *whole, s32 *next, s32 *rest);

#ifdef __cplusplus
}
#endif
