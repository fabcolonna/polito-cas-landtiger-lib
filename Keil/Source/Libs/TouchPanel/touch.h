#ifndef __TOUCH_H
#define __TOUCH_H

#include "glcd_types.h"
#include "utils.h"

// TYPES

typedef struct
{
    u16 x, y;
} TP_Coordinate;

// FUNCTIONS

/// @brief Initializes the touch panel.
/// @param skip_calibration If true, calibration is skipped.
/// @note If you skip calibration, the TP_WaitForTouch() could return incorrect values.
void TP_Init(bool skip_calibration);

bool TP_IsInitialized(void);

bool TP_IsCalibrated(void);

/// @brief Blocks until calibration is completed successfully.
/// @note This function is automatically called if skip_calibration is false in TP_Init.
void TP_Calibrate(void);

/// @brief Blocks until a touch event is detected.
/// @return The X, Y coordinates of the touched point, or NULL if the touch panel is not initialized.
/// @note If you call this function without first calibrating the panel, the returned value may be incorrect.
const TP_Coordinate *TP_WaitForTouch(void);

/// @brief Converts the touch panel coordinates to LCD coordinates.
/// @param tp_point The touch panel coordinates.
/// @return The LCD coordinates, or NULL if either the touch panel is not initialized or tp_point = NULL.
const LCD_Coordinate *TP_GetLCDCoordinateFor(TP_Coordinate *const tp_point);

#endif