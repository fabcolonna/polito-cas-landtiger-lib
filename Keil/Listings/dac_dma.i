# 1 "Source/Libs/DAC/dac_dma.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 397 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "Source/Libs/DAC/dac_dma.c" 2
# 1 "Source/Libs/DAC\\dac.h" 1



# 1 "Source/Libs/DAC\\dac_types.h" 1



# 1 "./Source/Libs/Utils\\utils.h" 1



# 1 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdint.h" 1 3
# 56 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdint.h" 3
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;


typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;





typedef signed char int_least8_t;
typedef signed short int int_least16_t;
typedef signed int int_least32_t;
typedef signed long long int int_least64_t;


typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long int uint_least64_t;




typedef signed int int_fast8_t;
typedef signed int int_fast16_t;
typedef signed int int_fast32_t;
typedef signed long long int int_fast64_t;


typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long int uint_fast64_t;






typedef signed int intptr_t;
typedef unsigned int uintptr_t;



typedef signed long long intmax_t;
typedef unsigned long long uintmax_t;
# 5 "./Source/Libs/Utils\\utils.h" 2

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef enum
{
    INT_PRIO_MAX = 0,
    INT_PRIO_MIN = 15,
    INT_PRIO_DEF = -1
} INT_Priority;
# 5 "Source/Libs/DAC\\dac_types.h" 2




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



typedef struct
{
    u16 note;
    u8 octave;
    u8 type;
} DAC_Tone;
# 5 "Source/Libs/DAC\\dac.h" 2


# 1 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdbool.h" 1 3
# 8 "Source/Libs/DAC\\dac.h" 2
# 33 "Source/Libs/DAC\\dac.h"
void DAC_BUZInit(u8 timer_a, u8 timer_b, u8 int_priority);



void DAC_BUZDeinit(void);




void DAC_BUZPlay(DAC_Tone tone, u16 bpm);





void DAC_BUZSetVolume(u8 volume);

_Bool DAC_BUZIsPlaying(void);

void DAC_BUZStop(void);
# 2 "Source/Libs/DAC/dac_dma.c" 2

