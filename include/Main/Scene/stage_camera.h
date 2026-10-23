#pragma once

#include "types.h"
#include <Main/Scene/move_object.h>

#ifdef __cplusplus
extern "C" {
#endif

// Scene objects: each starts with a model (the ones in the first array are 0x188 bytes) or, in the third kind, has
// the position first and the model after it.
typedef struct Unk_020b6ee8 {
    Model mModel;
    // Map position divided by 512.
    s32 mPosX;
    s32 mPosZ;
    void *mUnk_180;
    // Map entry type: 0xa or 0x2d.
    u8 mKind;
    u8 mUnk_185;
    s8 mUnk_186;
    u8 mUnk_187;
} Unk_020b6ee8;

typedef struct Unk_020b7ef4 {
    Model mModel;
    // Map position divided by 512.
    s32 mPosX;
    s32 mPosZ;
    u8 mUnk_180[2];
    // Map entry type: 0xf, 0x14, 0x19, 0x23 or 0x28.
    u8 mKind;
    u8 mUnk_183;
    // Set only for types 0x14 and 0x23 (the entry's flags >> 6). The draws of types 0xf, 0x14 and 0x28, and the
    // direction a type-0x23 object gives a stage, add one quarter turn to it. The type-0x23 draw uses it as it is.
    u8 mQuarterTurns;
    // Type 0x28: mId of the type-0x28 object that a stage meeting this one is moved to (the first one found).
    u8 mTargetId;
    // Type 0x28 only; compared with mTargetId.
    u8 mId;
    u8 mUnk_187;
    // Type 0x14: frames the stage has stayed within |dx| < 400 and |dz| < 800; reset to 0 when it passes
    // (mUnk_187 + 1) * 6 and when the stage is farther.
    u8 mNearFrames;
    u8 mUnk_189[3];
} Unk_020b7ef4;

typedef struct Unk_020bc938 {
    // Map position divided by 512.
    s32 mPosX;
    s32 mPosZ;
    Model mModel;
    // Map entry type; only 0x1e is stored, so the tests for 0x1e always pass in this build.
    u8 mKind;
    u8 mUnk_181[2];
    u8 mPresent;
} Unk_020bc938;

// Float position used by the followers.
typedef struct Vec3Float {
    float x;
    float y;
    float z;
} Vec3Float;

// One follower, moved by MoveFollowers. The element at index mFollowerCount of FollowerGroup.mFollowers is the point
// the others chase and is not moved.
typedef struct Follower {
    // Map position divided by 512.
    Vec3Float mPos;
    Vec3Float mPrevPos;
    // Distance moved along mAngle per MoveFollowers call. UpdateFollower scales mMaxDistancePerFrame by the distance
    // to the follower found ahead (negative, so backwards, when nearer than the two half-diameters) or eases it back
    // toward mMaxDistancePerFrame.
    float mDistancePerFrame;
    // 32.0 for a follower from an entry, 0 for the chased stage (not written for the fixed point). MoveFollowers runs
    // UpdateFollower only when it is not 0, which every moved follower is in this build.
    float mMaxDistancePerFrame;
    // 300 for a follower from an entry, 900 for the chased stage, 0 for the fixed point. The ring test and the spacing
    // of two followers use half of it.
    u16 mDiameter;
    u16 mTargetAngle;
    u16 mAngle;
    // Per 0x2000-wide angle sector (centred on multiples of 0x2000), the distance of the nearest other follower
    // within mNearDistance that spans it; 30000 = none. Filled by UpdateFollower.
    u16 mSectorDistances[8];
    u8 mFollowerEntryIndex;
    // 1 when a follower within mNearDistance lies in the sector around mAngle, set together with the slow-down of
    // mDistancePerFrame; else 0 (UpdateFollower).
    u8 mNeighbourAhead;
} Follower;

typedef struct FollowerGroup {
    // Element [mFollowerCount] is the chased point (the selected stage, or the fixed point while mFixedTargetTimer
    // runs) and is not moved.
    Follower mFollowers[17];
    u8 mUnk_3b8[0x3f0 - 0x3b8];
    // Followers moved this frame (the entries in range); also the index of the chased point.
    s16 mFollowerCount;
    // Distance past the two half-diameters over which a follower's speed rises from 0 to full; always 600 in this
    // build.
    s16 mSlowRange;
    // Only followers nearer than this fill the sector distances and slow a follower; always 0x640 in this build.
    s16 mNearDistance;
    // UpdateFollower runs the ring test (FindNearestRingSegment) only while 1; always 1 in this build.
    u8 mUseRingTest;
    u8 mTargetFixed;
} FollowerGroup;
extern FollowerGroup gFollowerGroup;

// One record per map entry of type 5 (SetupMapObjects); UpdateFollowers copies the present ones in range into
// gFollowerGroup and writes their positions back.
typedef struct FollowerEntry {
    VecFx32 mPos;
    s32 mScreenPos[2];
    u8 mUnk_14[4];
    // 0: drawn as sprite 0xe24; a ring-test hit while the fixed target runs (and its object's mUnk_186 is above 0)
    // counts that down and removes the entry. 1: drawn as sprite 0xe23; removed, with AddMapObjectTimeOrScore(rec, 5,
    // -1), when it nearly stops right behind the chased point while that is not the fixed point.
    u8 mKind;
    // 1 when within |dx| + |dz| < 0x4b0000 of the selected stage in the last UpdateFollowers; only recomputed while
    // mPresent is set, and cleared when the entry is removed.
    u8 mInRange;
    u8 mPresent;
    u8 mUnk_1b;
} FollowerEntry;

void ClearPanelState(void);
void SetStageCameraFar(int stage);
void SetStageCameraNear(int stage);
void UpdateStageCamera(int stage);
void func_02015f34(void);
void func_02016008(int stage);
void func_020164e0(int stage);

void SetupMapObjects(void);
void UpdateFollowers(int stage);
void UpdateFollower(FollowerGroup *followers, int index, u16 angle);
void MoveFollowers(FollowerGroup *followers);
int SelectStageOnTap(int stage, int *out);
void UpdateStageMotion(int index, int *sel, int flags);
int func_020182b8(int row, int column);
void func_020182cc(void);
void func_02018558(void);
void func_020186cc(void);
int func_020186f0(u8 tableRow);
void func_020186fc(int stage);
void AddStageAmountObjSlots(StageRec *rec, int kind, int amount);
void AddMapObjectTimeOrScore(StageRec *rec, int kind, int index);
void func_0201896c(void);
int CompareKeysDescending(const u16 *a, const u16 *b);
void func_02018cd4(void);
void UpdateStageInput(void);
void func_02019438(void);
void StoreStageOffsets(void);
void FindStagePolygonGroup(int stage);
void UpdateRandomStage(void);
void func_020196b4(void);
int func_020196e0(int value);
void func_02019750(u32 index);
void func_02019788(void);
// The points the map menu's camera eases toward (map entry ids 1 and 0 at set-up), moved with the cursor. The look-at
// eases toward gMapMenuLookAt; the position toward gMapMenuCameraPos blended toward gMapMenuLookAt by mBlend
// (0x200 = gMapMenuCameraPos itself). Neither is the camera's own position or look-at.
extern VecFx32 gMapMenuCameraPos;
extern VecFx32 gMapMenuLookAt;
int func_02019874(VecFx32 *pos);
void StartStageMoveObj(StageRec *rec, int x, int y, int c, int d, int mode);
void func_02019b38(void);
void UpdateMapCursorAndCamera(VecFx32 *lookAt, VecFx32 *cameraPos);
void SetupMapMenu(void);
void func_02019800(StageRec *rec);
void func_020198f0(void);
void DrawStageSprite(StageRec *rec, int index, int sprite, int mulX, int mulY, int prio);
void StopStageMoveObjAndClearObjSlots(void);
void UpdateStageMoveObj(void);
#ifdef __cplusplus
}
#endif
