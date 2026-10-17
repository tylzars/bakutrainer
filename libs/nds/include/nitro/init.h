#pragma once

#include "types.h"

// Start-up code.
void Entry(void);
void func_02000954(u32 value, u32 *dst, u32 size);
void func_02000970(void *end);
void func_02000a1c(void);
void AutoloadCallback(void);
void func_02000ab0(void);
void func_02000b98(void);
void func_02000b9c(void);

// Parameter block read by the start-up code: autoload list start and end, first autoload source,
// BSS start and end, end of the compressed part, SDK version word, markers and library strings.
extern char BuildInfo[];

// The game's entry point, called at the end of start-up.
void main(void);
