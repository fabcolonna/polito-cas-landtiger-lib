#ifndef __GLCD_CONFIG_H
#define __GLCD_CONFIG_H

/// @brief This macro controls the timing between the low level operations with
///        the GLCD controller. Since they may introduce overhead, they're turned off. If you
///        have rendering problems, reintroduce this define by uncommenting it.
/// #define GLCD_REQUIRES_DELAY

/// @brief Setting a limit to the number of components that a given object can have.
#define GLCD_MAX_COMPS_PER_OBJECT 12

/// @brief Maximum number of fonts that can be loaded into the LCD.
#define GLCD_MAX_FONTS 16

#endif