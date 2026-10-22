#pragma once

#include "types.h"
#include <Main/System/save_data.h>

#ifdef __cplusplus
extern "C" {
#endif

void SetupStateMenu(void);
void UpdateStateMenu(void);
void LeaveStateMenu(void);
void SetupSphereModel(void);
void UpdateSphereModel(void);
void LeaveSphereModel(void);

#ifdef __cplusplus
}
#endif
