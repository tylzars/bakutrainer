#include <nitro/os.h>
#include <Main/System/state_flow.h>
#include <Main/Input/touch_buttons.h>
#include <Main/Save/save_blocks.h>
#include <Main/Input/mic_input.h>
#include <Main/System/random_angle.h>
#include <Main/System/screen_fade.h>
#include <Main/System/bg_image.h>
#include <Main/System/system_init.h>
#include <Main/System/sound_control.h>
#include <Main/Graphics/sprite_slots.h>
#include <Main/Unknown/game_util.h>
#include <Main/System/prompt_screen.h>
#include <Main/System/save_data.h>
#include <Main/Scene/stage_progress.h>
#include <Main/Scene/stage_info_panel.h>
#include <Main/Scene/language_menu.h>
#include <Main/Unknown/move_object.h>
#include <Main/System/save_slot_menu.h>
#include <Main/Graphics/text_draw.h>
#include <Main/Graphics/model.h>
#include <Main/Graphics/texture_blocks.h>
#include <Main/Sound/wave_player.h>
#include <Main/Sound/tracker_player.h>
#include <Main/Map/map_entries.h>
#include <Main/Graphics/script_data.h>
#include <Main/Graphics/gx_pool.h>
#include <Main/clib.h>
#include <Main/Minigame/falling_blocks.h>
#include <Main/Scene/result_screen.h>
#include <Main/Scene/mode_select.h>
#include <Main/Scene/field_objects.h>
#include <Main/Scene/page_viewer.h>
#include <Main/Scene/battle_screen.h>
#include <Main/Minigame/stage_minigame.h>
#include <Main/Scene/stage_scene.h>
#include <Main/Scene/slide_transition.h>
#include <Main/Minigame/three_in_row.h>
#include <Main/Scene/stage_camera.h>
#include <Main/Scene/stage_map_menu.h>

// The prompts (this file's copy).
static const Unk_0204e96c data_0204e96c[0x49] = {
    {0x6c, -1, 0, -1},      {0x71, -1, 0, -1},  {0x96, -1, 0, 0x2f},    {0xb1, -1, 0, -1},      {0xc2, -1, 0, -1},
    {0xef, -1, 0, -1},      {0x10b, -1, 0, -1}, {0x148, -1, 0, -1},     {0x16f, -1, 0, -1},     {0x174, -1, 0, -1},
    {0x17d, -1, 0, -1},     {0x186, -1, 0, -1}, {0x1b3, -1, 0, -1},     {0x1f0, -1, 0, -1},     {0x22d, -1, 0, -1},
    {0x264, -1, 0, -1},     {0x27a, -1, 0, -1}, {0x283, -1, 0, -1},     {0x28c, -1, 0, -1},     {0x2a1, -1, 0, -1},
    {0x2de, -1, 0, -1},     {0x2fa, -1, 0, -1}, {0x311, -1, 0, -1},     {0x31d, -1, 0, -1},     {0x326, -1, 0, -1},
    {0x32f, -1, 0, -1},     {0x34a, -1, 0, -1}, {0x375, -1, 0, -1},     {0x392, -1, 0, -1},     {0x3b6, -1, 0, -1},
    {0x6c, -1, 0, -1},      {0x3d4, -1, 0, -1}, {0x422, -1, 0, -1},     {0x456, -1, 0, -1},     {0x48f, -1, 0, -1},
    {0x4d2, -1, 0, -1},     {0x4e3, -1, 0, -1}, {0x4ea, -1, 0, -1},     {0x509, -1, 0, -1},     {0x517, -1, 0, -1},
    {0x535, -1, 0, -1},     {0x541, -1, 0, -1}, {0x551, -1, 0, -1},     {0x55d, -1, 0, -1},     {0x569, -1, 0, -1},
    {0x573, -1, 0, -1},     {0x57f, -1, 0, -1}, {0x587, 0x1e, 0, 0x3e}, {0x58d, 0x1f, 0, 0x31}, {0x593, 0x1c, 1, 0x32},
    {0x599, 0x1d, 1, 0x33}, {0x59f, -1, 0, -1}, {0x5c2, -1, 0, -1},     {0x5ca, -1, 0, -1},     {0x5d2, -1, 0, -1},
    {0x5da, -1, 0, -1},     {0x5e2, -1, 0, -1}, {0x5ea, -1, 0, -1},     {0x5f2, -1, 0, -1},     {0x5fa, -1, 0, -1},
    {0x602, -1, 0, -1},     {0x60a, -1, 0, -1}, {0x612, 0x1b, 0, 0x30}, {0x618, -1, 0, -1},     {0x620, -1, 0, -1},
    {0x628, -1, 0, -1},     {0x630, -1, 0, -1}, {0x638, -1, 0, -1},     {0x640, -1, 0, -1},     {0x648, -1, 0, -1},
    {0x650, -1, 0, -1},     {0x658, -1, 0, -1}, {0x660, -1, 0, -1},
};

// Objects of this file: state menu, settings, sound control, prompt screen, main loop and model viewer state.
// State of the state menu (game state 2): lists of entries by depth whose leaves push game states, beside the Save and
// Load buttons. mDepth is the number of levels entered; mCardPage 1 shows the Card page instead, 0 the Save and Load
// page (key bit 9 toggles it).
typedef struct StateMenu {
    s32 mUnk_00;
    s32 mDepth;
    s32 mCardPage;
} StateMenu;

// The model viewer's state: the model, the index of the model shown, its rotation and a toggle.
typedef struct SphereModel {
    Model mModel;
    s32 mSphereIndex;
    u16 mAngle;
    u8 mUnk_17e[2];
    s32 mUnk_180;
} SphereModel;

StateMenu gStateMenu;
Unk_020b1084 data_020b1084;
s32 gStateMenuSelection[8];
SoundCtl gSoundCtl;
StageInfo data_020b10d4;
Unk_020b10fc data_020b10fc;
PromptScreen gPromptScreen;
GameFlow gGameFlow;
SphereModel gSphereModel;

// Per game state: whether the main loop runs RunPromptScreen after the state's per-frame handler when key bit 3 is
// pressed.
const u8 gStatePromptScreenEnabled[STATE_COUNT] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0,
                                                   0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0};
const StateFunc gStateLeaveFuncs[STATE_COUNT] = {
    NULL,          func_0200f360, func_0201133c, func_0200f580, func_020186cc, func_02015d34, func_0201a3d4,
    func_0201a600, NULL,          func_02013a2c, func_02019438, NULL,          func_0200f750, func_020059ac,
    func_0200cd1c, func_0200b884, func_02006fac, func_02004930, func_0200548c, func_02007750, func_0200a3f4,
    func_02014184, func_0201438c, func_02011510, func_0200719c,
};
const StateFunc gStateEnterFuncs[STATE_COUNT] = {
    NULL,          func_0200e4c4, SetupStateMenu,   func_0200f374, func_020182cc, func_02015cd4, func_02019df8,
    func_0201a4b0, NULL,          func_0201353c,    func_0201896c, NULL,          func_0200f660, func_02005548,
    func_0200ca14, func_0200b278, func_020067ac,    func_0200445c, func_02004964, func_020074ac, func_0200a384,
    func_020140d4, func_02014190, SetupSphereModel, func_02006fc0,
};
const StateFunc gStateUpdateFuncs[STATE_COUNT] = {
    NULL,          func_0200e5b0, UpdateStateMenu,   func_0200f4d4, func_02018558, func_02015d2c, func_02019fa0,
    func_0201a50c, NULL,          func_0201364c,     func_02018cd4, NULL,          func_0200f758, func_02005898,
    func_0200cbe8, func_0200b610, func_02006b68,     func_02004768, func_02005194, func_02007704, func_0200a3fc,
    func_02014124, func_02014260, UpdateSphereModel, func_020070c4,
};

#define SE_COUNT 0x78

// The play functions below keep each entry's gap counters (mCurrentGap, mCallsSincePlay) in the table itself. Every
// entry has one non-zero id, no gap, no random gap and mUnk_05 0, so in this build the random pick always takes
// mIds[0], every call plays (both counters stay 0) and PlayObjectSe never takes its func_0202f110 path.
const SeEntry gSeEntries[SE_COUNT] = {
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x167, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x2d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x29, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x10f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x110, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x111, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x112, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x110, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x111, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x112, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x113, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x114, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x115, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x113, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x114, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x115, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x113, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x114, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x115, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x116, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x117, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x118, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x116, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x117, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x118, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x119, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11a, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11b, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x119, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11a, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11b, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x119, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11a, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11b, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11c, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x11f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x120, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x121, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x122, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x123, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x124, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x125, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x126, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x127, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x128, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x129, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x12a, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x12b, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x12c, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x12d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x12e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x12f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x130, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x131, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x132, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x133, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x134, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x135, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x136, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x137, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x138, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x139, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x13a, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x13b, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x13c, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x13d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x13e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x13f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x140, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x141, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x142, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x143, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x144, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x145, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x146, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x147, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x148, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x149, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x14a, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x14b, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x14c, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x14d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x14e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x14f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x150, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x151, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x152, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x153, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x154, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x155, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x156, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x157, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x158, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x159, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x15a, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x15b, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x15c, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x15d, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x15e, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x15f, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x160, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x161, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x162, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x163, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x164, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x165, 0, 0, 0, 0, 0}},
    {1, 0x64, 0x40, 0, {0, 0}, 0, 0, 0, 0, {0x166, 0, 0, 0, 0, 0}},
};

u16 LookupLanguageFileId(int index) {
    u16 table[7] = {587, 590, 593, 595, 597, 599, 601};
    return table[index];
}

const char *GetText(int id) {
    if (id != 0) {
        return (const char *) func_02021e18(LookupLanguageFileId(data_020b1084.mLanguage), id);
    }
    return 0;
}

const char *GetLanguageText(int id, int index) {
    return (const char *) func_02021e18(LookupLanguageFileId(index), id);
}

void InitGameFlow(void) {
    ClearStateQueueAndFlag();
    gGameFlow.mState = 0;
    gGameFlow.mPrevState = 0;
    gGameFlow.mReenterRequested = 0;
}

void ClearStateQueueAndFlag(void) {
    gGameFlow.mQueueCount = 0;
    gGameFlow.mStateFlag = 0;
}

void ReenterState(void) {
    GameFlow *flow = &gGameFlow;
    StateFunc f;

    f = gStateLeaveFuncs[flow->mState];
    if (f != NULL) {
        f();
    }
    f = gStateEnterFuncs[flow->mState];
    if (f != NULL) {
        f();
    }
}

void PushState(int state) {
    GameFlow *flow = &gGameFlow;
    u32 i;

    if (flow->mQueueCount >= 25) {
        OS_Terminate();
    }
    for (i = flow->mQueueCount; i != 0; i--) {
        flow->mQueue[i] = flow->mQueue[i - 1];
    }
    flow->mQueue[0] = state;
    flow->mQueueCount++;
}

void PopState(void) {
    PopStateEx(0);
}

void PopStateEx(u32 flag) {
    GameFlow *flow = &gGameFlow;
    u32 i;

    if (flow->mQueueCount == 0) {
        OS_Terminate();
    }
    flow->mNextState = flow->mQueue[0];
    flow->mStateFlag = 0;
    if (flow->mQueueCount > 1) {
        for (i = 0; i < flow->mQueueCount - 1; i++) {
            flow->mQueue[i] = flow->mQueue[i + 1];
        }
    }
    flow->mQueueCount--;
    flow->mReenterRequested = flag;
}

u32 GetState(void) {
    return gGameFlow.mState;
}

u32 GetNextState(void) {
    return gGameFlow.mNextState;
}

void SetFlowValue(u32 index, u32 value) {
    GameFlow *flow = &gGameFlow;

    if (index >= HANDLER_COUNT) {
        OS_Terminate();
    }
    flow->mHandlers[index] = value;
}

u32 GetFlowValue(u32 index) {
    GameFlow *flow = &gGameFlow;

    if (index >= HANDLER_COUNT) {
        OS_Terminate();
    }
    return flow->mHandlers[index];
}

void SetStateFlag(s32 value) {
    gGameFlow.mStateFlag = value;
}

s32 GetStateFlag(void) {
    return gGameFlow.mStateFlag;
}

// Flag of SystemState.mFlags: the per-frame code samples the touch screen while it is set.
#define TOUCH_SAMPLING 0x400000

// Saves the display setup and button flags, then starts the prompt screen: returns 0 when there is nothing to show.
int OpenPromptScreen(int *a, int *b) {
    PromptScreen *st;
    u32 i;

    if (gSystemState.mImageIds[0] != 0xffff || (gSystemState.mFlags & 0x80)) {
        st = &gPromptScreen;
        SetFaded(3);
        if (!(gSystemState.mFlags & TOUCH_SAMPLING)) {
            st->mTouchSamplingWasOn = 0;
            st->mSavedTouchEnabled = gSystemState.mTouchEnabled;
            st->mSavedTouchDown = gSystemState.mTouchDown;
            st->mSavedTouchReleased = gSystemState.mTouchReleased;
            st->mSavedTouchPressed = gSystemState.mTouchPressed;
            st->mSavedTouchReleaseTimer = gSystemState.mTouchReleaseTimer;
            st->mSavedUseTouchReleaseTimer = gSystemState.mUseTouchReleaseTimer;
            gSystemState.mTouchEnabled = gSystemState.mTouchDown = gSystemState.mTouchReleased =
                gSystemState.mTouchPressed = 0;
            gSystemState.mTouchReleaseTimer = 0;
            gSystemState.mUseTouchReleaseTimer = 1;
            gSystemState.mFlags |= TOUCH_SAMPLING;
        } else {
            st->mTouchSamplingWasOn = 1;
        }
        st->mSavedWindowBits = gSystemState.mWindowBits;
        st->mSavedSubWindowBits = gSystemState.mSubWindowBits;
        st->mSavedSubColor0 = *(vu16 *) 0x05000400;
        st->mSavedSubColor1 = *(vu16 *) 0x05000402;
        gSystemState.mSubWindowBits = 0;
        gSystemState.mWindowBits = gSystemState.mSubWindowBits;
        st->mSavedBg3X = gSystemState.mBg3X;
        st->mSavedBg3Y = gSystemState.mBg3Y;
        st->mSavedSubBg3X = gSystemState.mSubBg3X;
        st->mSavedSubBg3Y = gSystemState.mSubBg3Y;
        gSystemState.mSubBg3Y = 0;
        gSystemState.mSubBg3X = 0;
        gSystemState.mBg3Y = 0;
        gSystemState.mBg3X = 0;
        st->mUnk_6c = *(vu16 *) 0x04001050;
        st->mUnk_6e = *(vu16 *) 0x04001052;
        for (i = 0; i < 2; i++) {
            st->mSavedImageIds[i] = gSystemState.mImageIds[i];
        }
        if (st->mSavedImageIds[0] != 0xffff || st->mSavedImageIds[1] != 0xffff) {
            if (st->mSavedImageIds[0] != 0xffff) {
                u16 r = GetLayerWidth(1);
                if (r == 0x80) {
                    OS_Terminate();
                }
                LoadImageEx(r == 0x100 ? 0x10d3 : 0x10d4, 1, 0, 2);
            }
            if (st->mSavedImageIds[1] != 0xffff) {
                u16 r = GetLayerWidth(2);
                if (r == 0x80) {
                    OS_Terminate();
                }
                LoadImageEx(r == 0x100 ? 0x10d3 : 0x10d4, 2, 0, 2);
            }
        }
        func_02034768(0, 0, 0);
        if (IsLcdSwapped() == 0) {
            if (a != 0) {
                *a = 2;
            }
            if (b != 0) {
                *b = 1;
            }
        } else {
            if (a != 0) {
                *a = 1;
            }
            if (b != 0) {
                *b = 2;
            }
        }
        st->mReleasedButtonId = 0;
        st->mState = PROMPT_SCREEN_STATE_SLIDE_IN;
        st->mFirstButtonX = 0x18000;
        st->mSecondButtonX = 0x20000;
        st->mTextY = -0x8000;
        *(vu16 *) 0x04001050 = 0;
        *(vu16 *) 0x04001052 = 0;
        data_020b6ee0.mUnk_a5d0 = 1;
        return 1;
    }
    return 0;
}

// Restores what OpenPromptScreen saved.
void ClosePromptScreen(void) {
    PromptScreen *st = &gPromptScreen;

    SetFaded(3);
    if (st->mSavedImageIds[0] != 0xffff || st->mSavedImageIds[1] != 0xffff) {
        if (st->mSavedImageIds[0] == st->mSavedImageIds[1]) {
            LoadImageEx(st->mSavedImageIds[0], 3, 0, 2);
        } else {
            if (st->mSavedImageIds[0] != 0xffff) {
                LoadImageEx(st->mSavedImageIds[0], 1, 0, 2);
            }
            if (st->mSavedImageIds[1] != 0xffff) {
                LoadImageEx(st->mSavedImageIds[1], 2, 0, 2);
            }
        }
    }
    gSystemState.mBg3X = st->mSavedBg3X;
    gSystemState.mBg3Y = st->mSavedBg3Y;
    gSystemState.mSubBg3X = st->mSavedSubBg3X;
    gSystemState.mSubBg3Y = st->mSavedSubBg3Y;
    gSystemState.mWindowBits = st->mSavedWindowBits;
    gSystemState.mSubWindowBits = st->mSavedSubWindowBits;
    *(vu16 *) 0x05000400 = st->mSavedSubColor0;
    *(vu16 *) 0x05000402 = st->mSavedSubColor1;
    *(vu16 *) 0x04001050 = st->mUnk_6c;
    *(vu16 *) 0x04001052 = st->mUnk_6e;
    if (st->mTouchSamplingWasOn == 0) {
        gSystemState.mTouchEnabled = st->mSavedTouchEnabled;
        gSystemState.mTouchDown = st->mSavedTouchDown;
        gSystemState.mTouchReleased = st->mSavedTouchReleased;
        gSystemState.mTouchPressed = st->mSavedTouchPressed;
        gSystemState.mTouchReleaseTimer = st->mSavedTouchReleaseTimer;
        gSystemState.mUseTouchReleaseTimer = st->mSavedUseTouchReleaseTimer;
        gSystemState.mFlags &= ~TOUCH_SAMPLING;
    }
}

// Draws the prompt screen: the backdrop tiles, the text and the two buttons. While mState is
// PROMPT_SCREEN_STATE_IN_PLACE it also adds the two buttons to the frame's touch buttons.
void DrawPromptScreen(void) {
    int i;
    int flag;
    int mode;

    if (gPromptScreen.mState == PROMPT_SCREEN_STATE_IN_PLACE) {
        flag = 1;
    } else {
        flag = 0;
    }
    if (gSystemState.mImageIds[0] == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x8011ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 1);
        }
    }
    if (gPromptScreen.mSavedImageIds[1] == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x11ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 2);
        }
    }
    mode = IsLcdSwapped() != 0 ? 1 : 2;
    func_020215fc(128, 96, data_0201a64e, GetText(36), 2, 200, 16, mode);
    func_0202cf08(0xe22, 47, (gPromptScreen.mTextY >> 8) + 17, 140, IsLcdSwapped() != 0 ? 2 : 1);
    mode = IsLcdSwapped() != 0 ? 2 : 1;
    func_020215fc(47, (gPromptScreen.mTextY >> 8) + 17, data_0201a61c, GetText(36), 0, 200, 16, mode);
    func_0200bc50((gPromptScreen.mFirstButtonX >> 8) + 128, 82, 0, 0, 140, PROMPT_SCREEN_BUTTON_FIRST, 0xdda,
                  GetText(37), data_0201a61c, 2, 0, 0, flag, 0, 0, 0);
    func_0200bc50((gPromptScreen.mSecondButtonX >> 8) + 128, 122, 0, 0, 140, PROMPT_SCREEN_BUTTON_SECOND, 0xdda,
                  GetText(38), data_0201a61c, 2, 0, 0, flag, 0, 0, 0);
}

// Runs the prompt screen: slides the buttons in, waits for a button, slides them out and restores the display; the
// track volume fades down meanwhile and back up after. The second button also clears data_020b6ee0.mUnk_a538,
// releases the microphone, runs func_02014430(0) and requests state 1. Key bit 3 ends it without a button once both
// screens' fade states are FADE_STATE_CLEAR (and data_020c6b14.mUnk_1748 is 0).
void RunPromptScreen(void) {
    int result;
    int done = 0;
    int running = 1;

    if (OpenPromptScreen(&result, 0) == 1) {
        func_0202eeec();
        StartVolumeFade(VOLUME_FADE_DOWN);
        do {
            if (IsFadeState(3, FADE_STATE_CLEAR) == 1 && data_020c6b14.mUnk_1748 == 0 &&
                (gSystemState.mKeysPressed & 8)) {
                running = 0;
            }
            switch (gPromptScreen.mState) {
                case PROMPT_SCREEN_STATE_SLIDE_IN: {
                    s32 v = gPromptScreen.mFirstButtonX - 0x1e00;
                    if (v < 0) {
                        v = 0;
                    }
                    gPromptScreen.mFirstButtonX = v;
                    v = gPromptScreen.mSecondButtonX - 0x1e00;
                    if (v < 0) {
                        v = 0;
                    }
                    gPromptScreen.mSecondButtonX = v;
                    v = gPromptScreen.mTextY + 0x780;
                    if (v >= 0) {
                        v = 0;
                    }
                    gPromptScreen.mTextY = v;
                    if (gPromptScreen.mFirstButtonX == 0 && gPromptScreen.mSecondButtonX == 0 && v == 0) {
                        gPromptScreen.mState = PROMPT_SCREEN_STATE_IN_PLACE;
                    }
                    break;
                }
                case PROMPT_SCREEN_STATE_SLIDE_OUT: {
                    s32 v = gPromptScreen.mFirstButtonX - 0x1e00;
                    s32 w;
                    if (v < -0x18000) {
                        v = -0x18000;
                    }
                    gPromptScreen.mFirstButtonX = v;
                    if (v <= -0x8000) {
                        v = gPromptScreen.mSecondButtonX - 0x1e00;
                        if (v < -0x18000) {
                            v = -0x18000;
                        }
                        gPromptScreen.mSecondButtonX = v;
                    }
                    w = gPromptScreen.mTextY - 0x780;
                    if (w < -0x4000) {
                        w = -0x4000;
                    }
                    gPromptScreen.mTextY = w;
                    if (gPromptScreen.mFirstButtonX == -0x18000 && gPromptScreen.mSecondButtonX == -0x18000 &&
                        w == -0x4000) {
                        done = 1;
                    }
                    break;
                }
            }
            DrawPromptScreen();
            if (func_0200bd80(0x20000000, PROMPT_SCREEN_BUTTON_FIRST)) {
                gPromptScreen.mState = PROMPT_SCREEN_STATE_SLIDE_OUT;
                gPromptScreen.mReleasedButtonId = PROMPT_SCREEN_BUTTON_FIRST;
            }
            if (func_0200bd80(0x20000000, PROMPT_SCREEN_BUTTON_SECOND)) {
                gPromptScreen.mState = PROMPT_SCREEN_STATE_SLIDE_OUT;
                gPromptScreen.mReleasedButtonId = PROMPT_SCREEN_BUTTON_SECOND;
            }
            if (gPromptScreen.mReleasedButtonId != 0 && done != 0) {
                switch (gPromptScreen.mReleasedButtonId) {
                    case PROMPT_SCREEN_BUTTON_FIRST:
                        running = 0;
                        break;
                    case PROMPT_SCREEN_BUTTON_SECOND:
                        running = 0;
                        data_020b6ee0.mUnk_a538 = running;
                        ReleaseMic();
                        func_02014430(running);
                        PushState(1);
                        PopState();
                        break;
                }
            }
            UpdateSoundCtl();
            UpdateSystemAndWait();
        } while (running == 1);
        StartVolumeFade(VOLUME_FADE_UP);
        ClosePromptScreen();
    }
}

void func_020101d0(void) {
    int i;
    int mode;
    int id = gSystemState.mSaveLastField + 4 + data_020b1084.mLanguage * 10;

    if (gSystemState.mImageIds[0] == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x8011ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 1);
        }
    }
    if (gPromptScreen.mSavedImageIds[1] == 0xffff) {
        for (i = 0; (u32) i < 12; i++) {
            func_0202cf08(0x11ea + i, (i & 3) << 6, (u32) i / 4 << 6, 250, 2);
        }
    }
    mode = IsLcdSwapped() != 0 ? 2 : 1;
    func_020215fc(128, 90, data_0201a64e, (const char *) func_02021e18(SAVE_BLOCK_ID, id + 6), 2, 200, 16, mode);
}

void func_02010294(int a, int b, int c) {
    SetFaded(3);
    UpdateSystemAndWait();
    ResetKeys();
    ResetVramBanks();
    func_0202c1c4();
    SetLcdSwap(c);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, b, 1);
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
    if (a != 0) {
        func_02021fb0(0x1c7, 0x1c6, a, 16);
        func_02024208();
    }
}

void func_020102f8(void) {
    SetFaded(3);
    UpdateSystemAndWait();
    *(vu16 *) 0x04001050 = 0;
    *(vu16 *) 0x04000050 = 0;
    func_0200d6c8();
    func_0200dacc();
    if (gSystemState.mFlags & 0x80) {
        func_02022370();
    }
    data_020b6ee0.mUnk_a5d0 = 1;
}

void func_0201033c(int a, int b) {
    if (a >= 0x49) {
        OS_Terminate();
    }
    SetFlowValue(0x14, a);
    func_02010364(data_0204e96c[a].mUnk_00, b);
}

void func_02010364(int a, int b) {
    SetFlowValue(0x17, a);
    SetFlowValue(0x16, b);
    PushState(3);
    PopState();
}

void InitSoundCtl(u32 track, s32 volume) {
    SoundCtl *ctl = &gSoundCtl;

    if (track >= 9) {
        OS_Terminate();
    }
    data_020c6b14.mUnk_1710 |= 2;
    ctl->mNextTrack = 0;
    ctl->mTrack = 0;
    ctl->mNextVolume = volume;
    ctl->mTargetVolume = volume;
    ctl->mVolumeFadeState = VOLUME_FADE_NONE;
    ctl->mDelayTimer = 0;
    ctl->mNextTrack = track;
    UpdateSoundCtl();
}

void UpdateSoundCtl(void) {
    SoundCtl *ctl = &gSoundCtl;
    u8 volume;

    if (data_020c6b14.mUnk_1748 != 0) {
        return;
    }
    volume = data_020c6b14.mUnk_1724 == 1 ? func_02038694() : ctl->mTargetVolume;
    if (data_020c6b14.mUnk_1724 == 0 && ctl->mTrack != 0) {
        ctl->mTrack = 0;
    }
    switch (ctl->mVolumeFadeState) {
        case VOLUME_FADE_DOWN:
            if (volume != 0) {
                func_02038578(volume - 1, 0);
            } else {
                ctl->mVolumeFadeState = VOLUME_FADE_NONE;
            }
            break;
        case VOLUME_FADE_DOWN_TO_TARGET:
            if (volume > ctl->mTargetVolume) {
                func_02038578(volume - 1, 0);
            } else {
                ctl->mVolumeFadeState = VOLUME_FADE_NONE;
            }
            break;
        case VOLUME_FADE_UP:
            if (volume < ctl->mTargetVolume) {
                func_02038578(volume + 1, 0);
            } else {
                ctl->mVolumeFadeState = VOLUME_FADE_NONE;
            }
            break;
    }
    if (ctl->mDelayTimer != 0) {
        ctl->mDelayTimer--;
        if (ctl->mDelayTimer == 0) {
            RequestTrack(ctl->mDelayedTrack, ctl->mDelayedVolume);
        }
    }
    if (ctl->mNextTrack != ctl->mTrack) {
        ctl->mVolumeFadeState = VOLUME_FADE_NONE;
        ctl->mTargetVolume = ctl->mNextVolume;
        ctl->mTrack = ctl->mNextTrack;
        if (ctl->mTrack >= 9) {
            OS_Terminate();
        }
        if (ctl->mTrack == 0) {
            func_020359d0(1);
            return;
        }
        {
            static const u16 ids[9] = {0, 362, 49, 364, 363, 216, 92, 228, 227};
            func_02035108(ids[ctl->mTrack], 0x10d7, ctl->mTargetVolume, 0, 1, 0);
        }
    }
}

void RequestTrack(u32 track, int volume) {
    SoundCtl *ctl = &gSoundCtl;

    if (track >= 9) {
        OS_Terminate();
    }
    ctl->mDelayTimer = 0;
    ctl->mNextTrack = track;
    ctl->mNextVolume = volume;
}

void RequestTrackDelayed(u32 track, int volume, int delay) {
    SoundCtl *ctl = &gSoundCtl;

    if (track >= 9) {
        OS_Terminate();
    }
    ctl->mDelayTimer = delay;
    ctl->mDelayedTrack = track;
    ctl->mDelayedVolume = volume;
}

void StartVolumeFade(u32 fade) {
    SoundCtl *ctl = &gSoundCtl;

    if (fade >= 4) {
        OS_Terminate();
    }
    ctl->mVolumeFadeState = fade;
    if (fade == VOLUME_FADE_DOWN_TO_TARGET) {
        ctl->mTargetVolume = 0x40;
    }
}

void PlaySeEx(u32 index, u16 volume, int b, int c, u32 d) {
    SeEntry *e;
    u32 pick;
    u16 id;

    if (index == 0 || index >= SE_COUNT) {
        OS_Terminate();
    }
    e = (SeEntry *) &gSeEntries[index];
    pick = NextRandom() % e->mIdCount;
    if (pick >= 6) {
        OS_Terminate();
    }
    id = e->mIds[pick];
    if (id != 0 && e->mCallsSincePlay >= e->mCurrentGap) {
        func_0202eb64(id, volume, 0, e->mPan, 0xffff, 0);
        if (b != 0x100) {
            func_0202f240(id, b);
        }
        e->mCallsSincePlay = 0;
        e->mCurrentGap = c;
        if (e->mGapRandom != 0) {
            e->mCurrentGap += NextRandom() % d;
        }
    } else {
        e->mCallsSincePlay++;
    }
}

void PlaySe(u32 index) {
    SeEntry *e;

    if (index == 0 || index >= SE_COUNT) {
        OS_Terminate();
    }
    e = (SeEntry *) &gSeEntries[index];
    PlaySeEx(index, e->mVolume, 0x100, e->mGap, e->mGapRandom);
}

void PlayObjectSe(void *obj) {
    SeOwner *owner = obj;
    u32 index = owner->mSeIndex;
    SeEntry *e;

    if (index != 0) {
        if (index == 0 || index >= SE_COUNT) {
            OS_Terminate();
        }
        e = (SeEntry *) &gSeEntries[index];
        if (e->mUnk_05 == 1) {
            func_0202f110(e->mIds[0], e->mVolume, 1, e->mPan, 0xffff, 0);
            return;
        }
        PlaySe(index);
        owner->mSeIndex = 0;
    }
}

#define STAGE_UNIT 51200

void func_02010648(u32 a) {
    u32 state;
    u32 lang10;
    u32 base;
    u32 id;

    if (a > gSystemState.mUnk_32e) {
        func_02021e44(SAVE_BLOCK_ID, 0x1c9);
        state = a;
        lang10 = data_020b1084.mLanguage * 10;
        id = gSystemState.mSaveLastField + 4 + lang10;
        base = gSystemState.mSaveLastField + 9;
        func_0201fed4(0);
        if (a != 2) {
            gSystemState.mUnk_324 = gSystemState.mElapsedFrames;
            SetFaded(3);
            SetLcdSwap(1);
            func_0202c1c4();
            func_02022370();
            FreeVramBanks(6);
            FreeVramBanks(2);
            LoadImage(0xdaa, 3);
            func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 2, 2);
        }
        switch (a) {
            case 1:
            case 2:
                id += 1;
                state = 1;
                break;
            case 5:
                gSystemState.mUnk_450 |= 2;
                gSystemState.mUnk_457 = 0x78;
                id += 4;
                break;
            case 3:
                id += 2;
                break;
            case 4:
                id += 3;
                break;
            case 6:
                id += 6;
                break;
            case 10:
                break;
        }
        func_020215fc(0x80, 0x60, data_0201a6c6, (const char *) func_02021e18(SAVE_BLOCK_ID, id), 2, 0xfa, 0x10, 2);
        if (a == 2) {
            func_020215fc(0x80, 0x88, data_0201a6c6, (const char *) func_02021e18(SAVE_BLOCK_ID, base + lang10), 2,
                          0xfa, 0x10, 2);
        }
        UpdateSystemAndWait();
        gSystemState.mFlags |= 0x80000;
        FadeClearAndWait(3);
        gSystemState.mUnk_32e = state;
    }
}

int ReadSaveSlot(u32 slot) {
    int ok = 0;
    SaveRecord *rec = AllocSaveRecord();
    Unk_020b6ee0 *work = &data_020b6ee0;
    int i;
    int j;
    int v;
    StageRec *stage;

    ReadSaveBlock(slot, rec, SAVE_BLOCK_ID);
    if (rec->mWritten == 1) {
        ok = 1;
        work->mUnk_a530 = rec->mUnk_14;
        work->mUnk_a560 = rec->mUnk_18;
        work->mUnk_a564 = rec->mUnk_28;
        work->mUnk_a56c = rec->mUnk_2c;
        work->mUnk_a56d = rec->mUnk_30;
        work->mUnk_a56e = rec->mUnk_34;
        work->mUnk_a56f = rec->mUnk_38;
        v = rec->mUnk_3c;
        if (v == 99) {
            v = -1;
        }
        work->mUnk_a120 = v;
        v = rec->mUnk_40;
        if (v == 999) {
            v = -1;
        }
        work->mUnk_a128 = v;
        func_02011bcc(work->mUnk_a128, work->mUnk_a120, 480);
        data_020b10fc.mUnk_14[0] = rec->mUnk_44;
        data_020b10fc.mUnk_14[1] = rec->mUnk_48;
        data_020b10fc.mUnk_14[2] = rec->mUnk_4c;
        data_020b10fc.mUnk_14[3] = rec->mUnk_50;
        data_020b10fc.mUnk_14[4] = rec->mUnk_54;
        data_020b6ee0.mUnk_a8ee = 0;
        for (i = 0; i < STAGE_MAX; i++) {
            if (rec->mStageWritten[i] == 1) {
                stage = &data_020b1990[func_02014de4(0, i)];
                stage->mUnk_1e4 = (rec->mUnk_f8[i] - 0x100) * STAGE_UNIT;
                stage->mUnk_1e8 = 0;
                stage->mUnk_1ec = (rec->mUnk_194[i] - 0x100) * STAGE_UNIT;
                stage->mInfo.mUnk_12 = rec->mUnk_230[i];
                stage->mInfo.mUnk_04 = rec->mUnk_2cc[i];
                stage->mInfo.mUnk_11 = rec->mUnk_368[i];
                stage->mInfo.mUnk_1c[0] = rec->mUnk_404[i] == 0 ? -1 : func_020144e0(rec->mUnk_404[i] - 1, 2);
                stage->mInfo.mUnk_1c[1] = rec->mUnk_4a0[i] == 0 ? -1 : func_020144e0(rec->mUnk_4a0[i] - 1, 2);
                stage->mInfo.mUnk_1c[2] = rec->mUnk_53c[i] == 0 ? -1 : func_020144e0(rec->mUnk_53c[i] - 1, 2);
                stage->mInfo.mUnk_1c[3] = rec->mUnk_5d8[i] == 0 ? -1 : func_020144e0(rec->mUnk_5d8[i] - 1, 2);
                stage->mInfo.mUnk_1c[4] = rec->mUnk_674[i] == 0 ? -1 : func_020144e0(rec->mUnk_674[i] - 1, 2);
            }
        }
        for (i = 0; i < 3; i++) {
            u8 slotStage = rec->mUnk_1c[i];
            if (slotStage != 0) {
                u8 s = slotStage - 1;
                if (s >= STAGE_MAX) {
                    OS_Terminate();
                }
                j = func_02014f14(0, s);
                if (j == -1) {
                    OS_Terminate();
                }
                func_02015114(i, j);
            } else {
                func_02015114(i, -1);
            }
        }
    }
    FreeSaveRecord(&rec);
    return ok;
}

void WriteSaveSlot(u32 slot) {
    SaveRecord *rec = AllocSaveRecord();
    Unk_020b6ee0 *work = &data_020b6ee0;
    int i;
    int v;
    StageRec *stage;

    func_02010d4c(rec);
    rec->mWritten = 1;
    rec->mUnk_14 = data_020b6ee0.mUnk_a530;
    rec->mUnk_18 = data_020b6ee0.mUnk_a560;
    rec->mUnk_28 = data_020b6ee0.mUnk_a564;
    rec->mUnk_2c = data_020b6ee0.mUnk_a56c;
    rec->mUnk_30 = data_020b6ee0.mUnk_a56d;
    rec->mUnk_34 = data_020b6ee0.mUnk_a56e;
    rec->mUnk_38 = data_020b6ee0.mUnk_a56f;
    if (data_020b6ee0.mUnk_a128 == -1) {
        v = work->mUnk_a12c;
        if (v == -1) {
            v = 999;
        }
        rec->mUnk_40 = v;
        v = work->mUnk_a124;
        if (v == -1) {
            v = 99;
        }
        rec->mUnk_3c = v;
    } else {
        v = work->mUnk_a128;
        if (v == -1) {
            v = 999;
        }
        rec->mUnk_40 = v;
        v = work->mUnk_a120;
        if (v == -1) {
            v = 99;
        }
        rec->mUnk_3c = v;
    }
    rec->mUnk_44 = data_020b10fc.mUnk_14[0];
    rec->mUnk_48 = data_020b10fc.mUnk_14[1];
    rec->mUnk_4c = data_020b10fc.mUnk_14[2];
    rec->mUnk_50 = data_020b10fc.mUnk_14[3];
    rec->mUnk_54 = data_020b10fc.mUnk_14[4];
    for (i = 0; i < 3; i++) {
        u8 slotStage = 0;
        stage = func_02015190(i);
        if (stage != NULL) {
            slotStage = stage->mInfo.mUnk_13;
            if (slotStage >= STAGE_MAX) {
                OS_Terminate();
            }
            slotStage = slotStage + 1;
        }
        rec->mUnk_1c[i] = slotStage;
    }
    for (i = 0; i < STAGE_MAX; i++) {
        rec->mStageWritten[i] = 0;
    }
    rec->mStageCount = data_020b6ee0.mUnk_a8ee;
    for (i = 0; i < data_020b6ee0.mUnk_a8ee; i++) {
        u8 id;
        stage = &data_020b1990[i];
        id = stage->mInfo.mUnk_13;
        if (id >= STAGE_MAX) {
            OS_Terminate();
        }
        if (rec->mStageWritten[id] == 1) {
            OS_Terminate();
        }
        rec->mStageWritten[id] = 1;
        v = stage->mUnk_1e4 / STAGE_UNIT;
        if (v <= -0x100 || v >= 0x100) {
            OS_Terminate();
        }
        rec->mUnk_f8[id] = v + 0x100;
        v = stage->mUnk_1ec / STAGE_UNIT;
        if (v <= -0x100 || v >= 0x100) {
            OS_Terminate();
        }
        rec->mUnk_194[id] = v + 0x100;
        rec->mUnk_230[id] = stage->mInfo.mUnk_12;
        rec->mUnk_2cc[id] = stage->mInfo.mUnk_04;
        rec->mUnk_368[id] = stage->mInfo.mUnk_11;
        rec->mUnk_404[id] = func_020144a4(stage->mInfo.mUnk_1c[0]) + 1;
        rec->mUnk_4a0[id] = func_020144a4(stage->mInfo.mUnk_1c[1]) + 1;
        rec->mUnk_53c[id] = func_020144a4(stage->mInfo.mUnk_1c[2]) + 1;
        rec->mUnk_5d8[id] = func_020144a4(stage->mInfo.mUnk_1c[3]) + 1;
        rec->mUnk_674[id] = func_020144a4(stage->mInfo.mUnk_1c[4]) + 1;
    }
    WriteSaveBlock(slot, rec, SAVE_BLOCK_ID);
    FreeSaveRecord(&rec);
}

void func_02010c94(void) {
    SaveRecord *buf = AllocSaveRecord();
    int i;
    int usedDefaults = 0;
    for (i = 0; (u32) i < SAVE_SLOT_COUNT; i++) {
        ReadSaveBlock(i, buf, SAVE_BLOCK_ID);
        if (buf->mUnk_00[0] == 1) {
            data_020b1084.mLanguage = buf->mUnk_00[1];
            data_020b1084.mUnk_04 = buf->mUnk_00[2];
            data_020b1084.mUnk_08 = buf->mUnk_00[3];
            break;
        }
    }
    if (i == SAVE_SLOT_COUNT) {
        func_02010d34();
        usedDefaults = 1;
    }
    FreeSaveRecord(&buf);
    if (usedDefaults == 1) {
        gSystemState.mSaveCallback(6);
        func_02010cfc();
    }
}

void func_02010cfc(void) {
    SaveRecord *buf = AllocSaveRecord();
    int i;
    for (i = 0; (u32) i < SAVE_SLOT_COUNT; i++) {
        ReadSaveBlock(i, buf, SAVE_BLOCK_ID);
        func_02010d4c(buf);
        WriteSaveBlock(i, buf, SAVE_BLOCK_ID);
    }
    FreeSaveRecord(&buf);
}

void func_02010d34(void) {
    data_020b1084.mLanguage = func_0200f9b8();
    data_020b1084.mUnk_04 = 1;
    data_020b1084.mUnk_08 = 3;
}

void func_02010d4c(SaveRecord *rec) {
    rec->mUnk_00[0] = 1;
    rec->mUnk_00[1] = data_020b1084.mLanguage;
    rec->mUnk_00[2] = data_020b1084.mUnk_04;
    rec->mUnk_00[3] = data_020b1084.mUnk_08;
}

#define FONT_STRIDE 0xa
#define ENTRY_BLOCK 0x236
#define ENTRY_STRIDE 0x25

// The grade (data_020b10d4.mUnk_11, 0x6d bytes on from gStateMenu) is written through gStateMenu's address,
// past that object's end: an out-of-bounds store is ordered after every earlier store, which gives the instruction
// order here.
void SetupStateMenu(void) {
    ResetKeys();
    ResetVramBanks();
    func_0202c1c4();
    func_0202bdd8(0x10cb, 0x10ca, 0x10cc, 0, 2);
    LoadImage(0xdaa, 1);
    LoadImage(0xde7, 2);
    SetLcdSwap(1);
    RequestTrack(0, 0x80);
    gStateMenu.mUnk_00 = 0;
    gStateMenu.mDepth = 0;
    ((u8 *) &gStateMenu)[0x6d] = 1;
    gStateMenu.mCardPage = 0;
}

// Search loops (pad bits 0x40 and 0x80): the index is read through a pointer cast ahead of the step, so it is loaded
// again there instead of reusing the value the loop condition loaded.
void UpdateStateMenu(void) {
    int start;
    int i;
    int rows;
    int last;
    int k;
    int j;
    int n;
    u32 pad;
    int grade;
    if (gSystemState.mKeysPressed & 0x200) {
        gStateMenu.mCardPage = 1 - gStateMenu.mCardPage;
    }
    if (gStateMenu.mCardPage == 1) {
        func_0200bc50(0xe9, 0x30, 0, 0, 0x8c, 0x202, 0xd7, "Card", 0, 2, 0, 0, 1, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x202)) {
            memset(data_020b10d4.mUnk_1c, -1, 10);
            func_02014cf8(&data_020b10d4);
        }
        pad = gSystemState.mKeysPressed;
        if (pad & 0x10) {
            grade = data_020b10d4.mUnk_11;
            if (grade != 12) {
                grade++;
            }
            data_020b10d4.mUnk_11 = grade;
        }
        if (pad & 0x20) {
            grade = data_020b10d4.mUnk_11;
            if (grade != 1) {
                grade--;
            }
            data_020b10d4.mUnk_11 = grade;
        }
        func_0200b898(0x10, 0x10, "grade: %i", data_020b10d4.mUnk_11);
        for (k = 0; k < 5; k++) {
            func_0200b898(0x10, 0x1a + k * 10, "%s", GetText(func_02014518(data_020b10d4.mUnk_1c[k], 3) + 0x8a));
        }
        for (k = 0; k < 5; k++) {
            func_0200b898(0x88, 0x1a + k * 10, "%i %i", func_02014518(data_020b10d4.mUnk_1c[k], 4),
                          func_02014518(data_020b10d4.mUnk_1c[k], 5));
        }
    } else {
        func_0200bc50(0xe9, 0x30, 0, 0, 0x8c, 0x200, 0xd7, "Save", 0, 2, 0, 0, 1, 0, 0, 0);
        func_0200bc50(0xe9, 0x4e, 0, 0, 0x8c, 0x201, 0xd7, "Load", 0, 2, 0, 0, 1, 0, 0, 0);
        if (func_0200bd80(0x20000000, 0x200)) {
            func_02014430(0);
        }
        if (func_0200bd80(0x20000000, 0x201)) {
            func_0201440c();
        }
        rows = 0;
        for (i = 0; i < gStateMenu.mDepth; i++) {
            func_020215fc(0x14 + i * 0x10, 0x1e + rows * 10, data_0201a6c6 + 2 * FONT_STRIDE,
                          (const char *) func_02021e18(ENTRY_BLOCK, i * ENTRY_STRIDE + 2 + gStateMenuSelection[i]), 0,
                          0xf0, 0xa, 2);
            rows++;
        }
        if (gStateMenu.mDepth > 0) {
            start = *(gStateMenuSelection + i - 1);
        } else {
            start = 0;
        }
        for (j = start; j < ENTRY_STRIDE; j++) {
            if (j != start) {
                for (k = 0; k < gStateMenu.mDepth; k++) {
                    if (func_02021e08(ENTRY_BLOCK, j + (k * ENTRY_STRIDE + 2)) != 0) {
                        break;
                    }
                }
                if (k != gStateMenu.mDepth) {
                    break;
                }
            }
            n = gStateMenu.mDepth * ENTRY_STRIDE + 2;
            if (func_02021e08(ENTRY_BLOCK, j + n) != 0) {
                func_020215fc(0x14 + gStateMenu.mDepth * 0x10, 0x1e + rows * 10,
                              j != gStateMenuSelection[gStateMenu.mDepth] ? data_0201a6c6 + FONT_STRIDE
                                                                          : data_0201a6c6 + 2 * FONT_STRIDE,
                              (const char *) func_02021e18(ENTRY_BLOCK, j + n), 0, 0xf0, 0xa, 2);
                last = j;
                rows++;
            }
        }
        if (gSystemState.mKeysPressed & 0x40) {
            if (gStateMenuSelection[gStateMenu.mDepth] > start) {
                do {
                    s32 *p = &gStateMenuSelection[gStateMenu.mDepth];
                    s32 idx = *(s32 *) &gStateMenu.mDepth;
                    (*p)--;
                    n = idx * ENTRY_STRIDE + 2;
                    if (func_02021e08(ENTRY_BLOCK, n + *p) != 0) {
                        break;
                    }
                } while (gStateMenuSelection[gStateMenu.mDepth] > start);
            }
        } else if (gSystemState.mKeysPressed & 0x80) {
            if (gStateMenuSelection[gStateMenu.mDepth] < last) {
                do {
                    s32 *p = &gStateMenuSelection[gStateMenu.mDepth];
                    s32 idx = *(s32 *) &gStateMenu.mDepth;
                    (*p)++;
                    n = idx * ENTRY_STRIDE + 2;
                    if (func_02021e08(ENTRY_BLOCK, n + *p) != 0) {
                        break;
                    }
                } while (gStateMenuSelection[gStateMenu.mDepth] < last);
            }
        }
        if (gSystemState.mKeysPressed & 1) {
            gStateMenu.mDepth++;
            gStateMenuSelection[gStateMenu.mDepth] = *(gStateMenuSelection + gStateMenu.mDepth - 1);
            n = gStateMenu.mDepth * ENTRY_STRIDE + 2 + gStateMenuSelection[gStateMenu.mDepth];
            if ((u32) (func_02021e08(ENTRY_BLOCK, n) - 2) <= 1) {
                int v = func_02021e44(ENTRY_BLOCK, n);
                if (v < 100) {
                    data_020b6ee0.mUnk_a8f2 = v - 1;
                    data_020b6ee0.mUnk_a8e4 = -1;
                    data_020b10fc.mUnk_04 = 4;
                    PushState(4);
                    PopState();
                } else if (v < 200) {
                    PushState(5);
                    PopState();
                } else if (v < 300) {
                    PushState(6);
                    PopState();
                } else if (v < 400) {
                    gStateMenu.mDepth = 0;
                } else if (v < 500) {
                    PushState(10);
                    PopState();
                } else if (v < 600) {
                    gStateMenu.mDepth = 0;
                } else if (v < 700) {
                    PushState(12);
                    PopState();
                } else if (v < 800) {
                    data_020b10fc.mUnk_04 = 0;
                    PushState(14);
                    PopState();
                } else if (v < 900) {
                    data_020b10fc.mUnk_04 = 1;
                    PushState(15);
                    PopState();
                } else if (v < 1000) {
                    PushState(19);
                    PopState();
                } else if (v < 1100) {
                    data_020b10fc.mUnk_04 = 2;
                    PushState(16);
                    PopState();
                } else if (v < 1200) {
                    PushState(1);
                    PopState();
                } else if (v < 1300) {
                    data_020b10fc.mUnk_04 = 3;
                    PushState(17);
                    PopState();
                } else if (v < 1400) {
                    PushState(21);
                    PopState();
                } else if (v < 1500) {
                    PushState(22);
                    PopState();
                } else if (v < 1600) {
                    PushState(23);
                    PopState();
                }
            }
        } else if (gSystemState.mKeysPressed & 2) {
            if (gStateMenu.mDepth > 0) {
                gStateMenu.mDepth--;
            }
        }
    }
}

void func_0201133c(void) {
    func_020102f8();
    ResetVramBanks();
}

void SetupSphereModel(void) {
    func_0200c5d8(0, 0xdaa, 0xdaa, 0x100, 0x10, 0x82, 2);
    func_02024208();
    gSphereModel.mSphereIndex = 0;
    gSphereModel.mAngle = 0x8000;
    gSphereModel.mUnk_180 = 0;
    func_020271ec(&gSphereModel.mModel, 0, 0, 0, data_0201ab04[gSphereModel.mSphereIndex]);
}

void UpdateSphereModel(void) {
    int idx;
    func_0200b8dc(8, 0xb2, "sphere_%.2d.imd", gSphereModel.mSphereIndex);
    if (gSystemState.mKeysDown & 8) {
        gSphereModel.mModel.mUnk_170 = gSphereModel.mSphereIndex * 6 + 0xfdb;
    } else {
        gSphereModel.mModel.mUnk_170 = gSphereModel.mSphereIndex * 6 + 0xfd8;
    }
    func_02024368(&gSphereModel.mModel, 0x80, 0x60, 0x80 << 10, gSphereModel.mAngle, 0);
    PlayObjectSe(&gSphereModel.mModel);
    if (gSystemState.mKeysDown & 0x20) {
        gSphereModel.mAngle -= 0x100;
    } else if (gSystemState.mKeysDown & 0x10) {
        gSphereModel.mAngle += 0x100;
    }
    if ((gSystemState.mKeysPressed & 1) || (gSystemState.mKeysPressed & 2) || (gSystemState.mKeysPressed & 0x400) ||
        (gSystemState.mKeysPressed & 0x800)) {
        gSphereModel.mModel.mUnk_134 |= gSphereModel.mUnk_180 != 0 ? 2 : 1;
        gSphereModel.mUnk_180 = gSphereModel.mUnk_180 == 0 ? 1 : 0;
    }
    if (gSystemState.mKeysPressed & 0x200) {
        idx = gSphereModel.mSphereIndex - 1;
        if (idx < 0) {
            idx = 0;
        }
        gSphereModel.mSphereIndex = idx;
        gSphereModel.mAngle = 0x8000;
        gSphereModel.mUnk_180 = 0;
        func_020271ec(&gSphereModel.mModel, 0, 0, 0, data_0201ab04[idx]);
    } else if (gSystemState.mKeysPressed & 0x100) {
        idx = gSphereModel.mSphereIndex + 1;
        if (idx >= 0x26) {
            idx = 0x26;
        }
        gSphereModel.mSphereIndex = idx;
        gSphereModel.mAngle = 0x8000;
        gSphereModel.mUnk_180 = 0;
        func_020271ec(&gSphereModel.mModel, 0, 0, 0, data_0201ab04[idx]);
    }
}

void func_02011510(void) {
    func_020102f8();
}
