#ifndef __DAC_TYPES_H
#define __DAC_TYPES_H

#include "types.h"

/// @brief Note frequencies based on the equal temperament scale.
/// @note Octave 4 is used as the reference, other octaves can be
///       obtained by multiplying/dividing the frequencies by 2.
typedef enum
{
    DAC_NOTE_C = 262,
    DAC_NOTE_C_SHARP = 277,
    DAC_NOTE_D = 294,
    DAC_NOTE_D_SHARP = 311,
    DAC_NOTE_E = 330,
    DAC_NOTE_F = 349,
    DAC_NOTE_F_SHARP = 370,
    DAC_NOTE_G = 392,
    DAC_NOTE_G_SHARP = 415,
    DAC_NOTE_A = 440,
    DAC_NOTE_A_SHARP = 466,
    DAC_NOTE_B = 494,
    DAC_NOTE_PAUSE = 0,
} DAC_Note;

/// @brief Octave values.
typedef enum
{
    DAC_OCT_1 = 1,
    DAC_OCT_2,
    DAC_OCT_3,
    DAC_OCT_4,
    DAC_OCT_5,
    DAC_OCT_6,
    DAC_OCT_7,
} DAC_Octave;

typedef enum
{
    DAC_NOTE_WHOLE = 4,
    DAC_NOTE_HALF = 2,
    DAC_NOTE_QUARTER = 1
} DAC_NoteType;

// TONE

typedef struct
{
    u16 note;
    u8 octave;
    u8 type;
} DAC_Tone;

#define DAC_BPM_DEFAULT 60
#define DAC_VOL_DEFAULT 5

#endif