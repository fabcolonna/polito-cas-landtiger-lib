#ifndef __GLCD_CONFIG_H
#define __GLCD_CONFIG_H

/// @brief This macro controls the timing between the low level operations with
///        the GLCD controller. Since they may introduce overhead, they're turned off. If you
///        have rendering problems, reintroduce this define by uncommenting it. This also
///        stands true for the GLCD_INLINE_AMAP (AMAP = As Much As Possible) define.
/// #define GLCD_REQUIRES_DELAY
/// #define GLCD_INLINE_AMAP

/// @brief Maximum number of components that can be rendered on the screen.
/// @note  Items in the render list RQ only contain references to the objects that are
///        stored in memory, hence the memory footprint is not huge. We can afford to have a larger number.
#define GLCD_MAX_RENDERABLE_ITEMS 512

/// @brief Setting a limit to the number of components that a given object can have.
#define GLCD_MAX_COMPS_PER_OBJECT 12

/// @brief Maximum number of fonts that can be loaded into the LCD.
#define GLCD_MAX_FONTS 16

#endif