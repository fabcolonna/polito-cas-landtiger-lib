#ifndef __TOUCH_H
#define __TOUCH_H

#include "glcd_types.h"
#include "utils.h"

// TYPES

typedef struct
{
    u16 x, y;
} TP_Coordinate;

typedef struct
{
    LCD_Coordinate pos;
    u16 width, height;
} TP_ButtonArea;

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
const LCD_Coordinate *TP_GetLCDCoordinateFor(const TP_Coordinate *const tp_point);

// TOUCH BUTTON FUNCTIONS

/// @brief Assigns a button area to the given button.
/// @param button The button to assign the area to.
/// @param pos The position of the button.
/// @return The button area.
TP_ButtonArea TP_AssignButtonArea(LCD_Button button, LCD_Coordinate pos);

/// @brief Blocks until the given button area is pressed.
/// @param button The button to wait for.
void TP_WaitForButtonPress(TP_ButtonArea button);

/// @brief Checks if the given button area has been pressed during the touch event which
///        occurred at the given touch point.
/// @param button The button to check.
/// @param touch_point The touch point to check.
/// @return True if the button has been pressed, false otherwise.
bool TP_HasButtonBeenPressed(TP_ButtonArea button, const TP_Coordinate *const touch_point);

#endif