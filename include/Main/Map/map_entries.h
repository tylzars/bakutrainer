#pragma once

#include "types.h"
#include <nitro/fx.h>
#include <Main/Unknown/move_object.h>
#include <Main/Unknown/game_util.h>

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

// An entry of the sorted table at MapState.mUnk_1c: a key and the value it maps to.
typedef struct MapPair {
    u16 mUnk_0;
    u16 mUnk_2;
} MapPair;

// One cell of the grid (mUnk_30 in MapState, 4 bytes per cell): a direction to the target and, in mUnk_3, how the cell
// was reached (0 = not visited).
typedef struct MapCell {
    s8 mUnk_0;
    s8 mUnk_1;
    s8 mUnk_2;
    u8 mUnk_3;
} MapCell;

// A visible cell found by func_02032d50: the key of its object and its position.
typedef struct MapMarker {
    u16 mUnk_00;
    u16 mUnk_02;
    s32 mUnk_04;
    s32 mUnk_08;
} MapMarker;

// A point of a ring of the stage data file's chunk 0x5540 as func_020312b8 unpacks it: x and z, a bottom height
// (mUnk_08) and a top height (mUnk_0a, 0 when the ring has none).
typedef struct MapRingPoint {
    s32 x;
    s32 z;
    s16 mUnk_08;
    s16 mUnk_0a;
} MapRingPoint;

// An object drawn with a model: the model and its draw parameters.
typedef struct CameraTarget {
    Model mModel;
    DrawParams mDraw;
} CameraTarget;

// An animation track of the camera: a base position, a timestamp at 0xc, the model id, frame and frame
// count, and flags.
typedef struct CameraTrack {
    VecFx32 mUnk_00;
    u32 mUnk_0c;
    u16 mUnk_10;
    u16 mUnk_12;
    u16 mUnk_14;
    u8 mUnk_16_0 : 1;
    u8 mUnk_16_1 : 1;
    u8 mUnk_16_2 : 1;
    u8 mUnk_16_3 : 1;
    u8 mUnk_16_4 : 1;
    u8 mUnk_16_5 : 1;
    u8 mUnk_16_6 : 2;
} CameraTrack;

// The camera's block at 0x9c. The camera code passes its address to func_0202733c; the scene code keeps a
// pointer to it and moves the two camera positions at 0x204 and 0x228.
typedef struct Unk_020c5f04 {
    Model mModel;
    DrawParams mDraw;
    CameraTrack mUnk_1bc;
    CameraTrack mUnk_1d4;
    CameraTrack mUnk_1ec;
    VecFx32 mUnk_204;
    u8 mUnk_210[0x21c - 0x210];
    VecFx32 mUnk_21c;
    VecFx32 mUnk_228;
    VecFx32 mUnk_234;
} Unk_020c5f04;

// Camera view settings at 0x2dc of the camera: 0x14/0x16/0x18 are angles, 0x30 and 0x38/0x3c are flags, 0x44
// a register value, 0x4a the projection set up last, 0x4b/0x4c the polygon alpha and id the model code last wrote.
typedef struct Unk_020c6144 {
    VecFx32 mUnk_00;
    u8 mUnk_0c;
    u8 mUnk_0d;
    u8 mUnk_0e;
    u8 mUnk_0f;
    u8 mUnk_10;
    u8 mUnk_11[3];
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u8 mUnk_1a[2];
    CameraTarget *mUnk_1c;
    CameraTarget *mUnk_20;
    s32 mUnk_24;
    s32 mUnk_28;
    s32 mUnk_2c;
    u16 mUnk_30;
    u8 mUnk_32;
    u8 mUnk_33;
    u8 mUnk_34[0x38 - 0x34];
    s32 mUnk_38;
    s32 mUnk_3c;
    u16 mUnk_40;
    u8 mUnk_42[2];
    s32 mUnk_44;
    u8 mUnk_48[2];
    u8 mUnk_4a;
    u8 mUnk_4b;
    u8 mUnk_4c;
    u8 mUnk_4d;
    u8 mUnk_4e;
    u8 mUnk_4f;
    u16 *mUnk_50;
    u16 mUnk_54;
    u16 mUnk_56;
    u8 mUnk_58[0x5a - 0x58];
    u8 mUnk_5a;
    u8 mUnk_5b;
    u8 mUnk_5c;
    u8 mUnk_5d[3];
    s32 mUnk_60;
    const u16 *mUnk_64;
} Unk_020c6144;

// The camera (0x344 bytes at 0x2024 of the map object data_020c3e44): positions, targets, angles and flags, the
// block at 0x9c and the view settings at 0x2dc. The ARM camera and model code reaches its bytes through the map
// object + 0x2000 (0x2310 and 0x234a) and its halfwords through the map object + 0x2300 (0x2314..0x2318 and
// 0x2340..0x2356).
typedef struct CameraState {
    u16 mUnk_00;
    u16 mUnk_02;
    void (*mUnk_04)();
    u8 **mUnk_08;
    u8 mUnk_0c;
    u8 mUnk_0d[3];
    u16 *mUnk_10;
    MapRingPoint mUnk_14;
    MapRingPoint mUnk_20;
    MapRingPoint mUnk_2c;
    u16 mUnk_38;
    u16 mUnk_3a;
    u8 mUnk_3c;
    u8 mUnk_3d;
    u8 mUnk_3e[2];
    u32 mUnk_40;
    u32 mUnk_44;
    u16 *mUnk_48;
    u16 *mUnk_4c;
    u16 mUnk_50;
    u16 mUnk_52;
    u16 mUnk_54;
    u16 mUnk_56;
    u16 mUnk_58;
    u8 mUnk_5a;
    u8 mUnk_5b_0 : 1;
    u8 mUnk_5b_1 : 1;
    u8 mUnk_5b_2 : 1;
    u8 mUnk_5b_3 : 1;
    u8 mUnk_5b_4 : 1;
    u8 mUnk_5b_5 : 3;
    VecFx32 mUnk_5c;
    s32 mUnk_68;
    s32 mUnk_6c;
    s32 mUnk_70;
    VecFx32 mUnk_74;
    VecFx32 mUnk_80;
    VecFx32 mUnk_8c;
    u8 mUnk_98;
    u8 mUnk_99[3];
    Unk_020c5f04 mUnk_9c;
    Unk_020c6144 mUnk_2dc;
} CameraState;

// The map object (0x2368 bytes of .bss, up to data_020c61ac): two bit sets, indexed by cell number (mUnk_24) and by
// position in a rectangle (mUnk_28), the squared distance limits, the sorted pair table with its search step, the
// buffers the map code allocates, the view matrices (0x6c computed from 0x3c, with its translation at 0x90, and the
// projection at 0x9c), the placed objects and, right after the 20 records, the camera. Every file reaches the
// camera as a part of this object: the compiler then counts each camera access as lying past the declared end
// (pool anchor offset plus member offset exceeds the object's size), which keeps those loads and stores in the
// original order against other stores.
typedef struct MapState {
    s32 mUnk_00;
    u32 mUnk_04[3];
    u8 mUnk_10;
    u8 mUnk_11[3];
    u16 mUnk_14;
    u16 mUnk_16;
    u16 mUnk_18;
    u8 mUnk_1a[2];
    MapPair *mUnk_1c;
    u8 mUnk_20;
    u8 mUnk_21;
    u8 mUnk_22[2];
    u32 *mUnk_24;
    u32 *mUnk_28;
    MapMarker *mUnk_2c;
    MapCell *mUnk_30;
    u16 mUnk_34;
    u16 mUnk_36;
    u16 mUnk_38;
    u8 mUnk_3a[2];
    MtxFx43 mUnk_3c;
    MtxFx43 mUnk_6c;
    MtxFx44 mUnk_9c;
    s32 mUnk_dc;
    s32 mUnk_e0;
    MapRecord mUnk_e4[MAP_RECORDS];
    CameraState mUnk_2024;
} MapState;

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
// Clears the 20 records of data_020c3e44 (and the camera's halfwords mUnk_00/mUnk_02 when a == 1).
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
// Moves the camera by its position and look-at modes, then sets up the view.
void func_02033fc8(void);
// Sets the fog with the default density table.
void func_02034768(int a, int b, int c);
// Sets the fog (off when a and b are 0), its colour and the clear colour, and loads a density table.
void func_02034778(int a, int b, int c, const u16 *table);
// Sets light 0's colour and a -z direction and marks it on.
void func_020348ac(int r, int g, int b, int z);
// Marks all lights off.
void func_02034904(void);

#ifdef __cplusplus
}
#endif
