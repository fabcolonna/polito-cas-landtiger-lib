# 1 "Source/Libs/GLCD/glcd.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 397 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "Source/Libs/GLCD/glcd.c" 2
# 1 "Source/Libs/GLCD\\glcd.h" 1



# 1 "Source/Libs/GLCD\\glcd_macros.h" 1



# 1 "Source/Libs/GLCD\\glcd_types.h" 1



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
# 5 "Source/Libs/GLCD\\glcd_types.h" 2
# 1 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdbool.h" 1 3
# 6 "Source/Libs/GLCD\\glcd_types.h" 2

typedef enum
{
    LCD_ORIENT_VER = 0,
    LCD_ORIENT_HOR = 90,
    LCD_ORIENT_VER_INV = 180,
    LCD_ORIENT_HOR_INV = 270
} LCD_Orientation;


typedef enum
{
    LCD_ALIGN_TOP_LEFT,
    LCD_ALIGN_TOP_CENTER,
    LCD_ALIGN_TOP_RIGHT,
    LCD_ALIGN_CENTER_LEFT,
    LCD_ALIGN_CENTER,
    LCD_ALIGN_CENTER_RIGHT,
    LCD_ALIGN_BOTTOM_LEFT,
    LCD_ALIGN_BOTTOM_CENTER,
    LCD_ALIGN_BOTTOM_RIGHT
} LCD_Alignment;



typedef enum
{
    LCD_COL_WHITE = 0xFFFF,
    LCD_COL_BLACK = 0x0000,
    LCD_COL_GREY = 0xF7DE,
    LCD_COL_BLUE = 0x001F,
    LCD_COL_BLUE2 = 0x051F,
    LCD_COL_RED = 0xF800,
    LCD_COL_MAGENTA = 0xF81F,
    LCD_COL_GREEN = 0x07E0,
    LCD_COL_CYAN = 0x7FFF,
    LCD_COL_YELLOW = 0xFFE0,
    LCD_COL_NONE = 0x10000
} LCD_ColorPalette;





typedef u32 LCD_Color;

typedef struct
{
    u16 x, y;
} LCD_Coordinate;



typedef enum
{
    LCD_DEF_FONT_MSGOTHIC = 0,
    LCD_DEF_FONT_SYSTEM = 1
} LCD_DefaultFont;

typedef struct
{
    const u32 *data;
    u16 data_size, max_char_width, char_height;



    const u16 *char_widths;
} LCD_Font;

typedef i8 LCD_FontID;






typedef struct
{
    LCD_Coordinate from, to;
    LCD_Color color;
} LCD_Line;

typedef struct
{
    u16 width, height;
    LCD_Color edge_color, fill_color;
} LCD_Rect;

typedef struct
{
    LCD_Coordinate center;
    u16 radius;
    LCD_Color edge_color, fill_color;
} LCD_Circle;

typedef struct
{
    const u32 *pixels;
    u16 width, height;
    _Bool has_alpha;
} LCD_Image;

typedef struct
{
    char *text;
    LCD_Color text_color, bg_color;
    LCD_FontID font;
} LCD_Text;


typedef enum
{
    LCD_COMP_LINE,
    LCD_COMP_RECT,
    LCD_COMP_CIRCLE,

    LCD_COMP_IMAGE,
    LCD_COMP_TEXT
} LCD_ComponentType;


typedef struct
{
    LCD_ComponentType type;
    LCD_Coordinate pos;
    union {
        LCD_Line line;
        LCD_Rect rect;
        LCD_Circle circle;
        LCD_Image image;
        LCD_Text text;
    } object;
} LCD_Component;





typedef struct
{
    LCD_Component *comps;
    u8 comps_size;
} LCD_Obj;

typedef i16 LCD_ObjID;
# 5 "Source/Libs/GLCD\\glcd_macros.h" 2

# 1 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 1 3
# 71 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
    typedef unsigned int size_t;
# 91 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
    typedef unsigned short wchar_t;




typedef struct div_t { int quot, rem; } div_t;

typedef struct ldiv_t { long int quot, rem; } ldiv_t;


typedef struct lldiv_t { long long quot, rem; } lldiv_t;
# 139 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) int __aeabi_MB_CUR_MAX(void);
# 158 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) double atof(const char * ) __attribute__((__nonnull__(1)));





extern __attribute__((__nothrow__)) int atoi(const char * ) __attribute__((__nonnull__(1)));





extern __attribute__((__nothrow__)) long int atol(const char * ) __attribute__((__nonnull__(1)));






extern __attribute__((__nothrow__)) long long atoll(const char * ) __attribute__((__nonnull__(1)));







extern __attribute__((__nothrow__)) double strtod(const char * __restrict , char ** __restrict ) __attribute__((__nonnull__(1)));
# 206 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) float strtof(const char * __restrict , char ** __restrict ) __attribute__((__nonnull__(1)));
extern __attribute__((__nothrow__)) long double strtold(const char * __restrict , char ** __restrict ) __attribute__((__nonnull__(1)));




extern __attribute__((__nothrow__)) long int strtol(const char * __restrict ,
                        char ** __restrict , int ) __attribute__((__nonnull__(1)));
# 243 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) unsigned long int strtoul(const char * __restrict ,
                                       char ** __restrict , int ) __attribute__((__nonnull__(1)));
# 275 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) long long strtoll(const char * __restrict ,
                                  char ** __restrict , int )
                          __attribute__((__nonnull__(1)));






extern __attribute__((__nothrow__)) unsigned long long strtoull(const char * __restrict ,
                                            char ** __restrict , int )
                                   __attribute__((__nonnull__(1)));






extern __attribute__((__nothrow__)) int rand(void);
# 303 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) void srand(unsigned int );
# 313 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
struct _rand_state { int __x[57]; };
extern __attribute__((__nothrow__)) int _rand_r(struct _rand_state *);
extern __attribute__((__nothrow__)) void _srand_r(struct _rand_state *, unsigned int);
struct _ANSI_rand_state { int __x[1]; };
extern __attribute__((__nothrow__)) int _ANSI_rand_r(struct _ANSI_rand_state *);
extern __attribute__((__nothrow__)) void _ANSI_srand_r(struct _ANSI_rand_state *, unsigned int);





extern __attribute__((__nothrow__)) void *calloc(size_t , size_t );





extern __attribute__((__nothrow__)) void free(void * );







extern __attribute__((__nothrow__)) void *malloc(size_t );





extern __attribute__((__nothrow__)) void *realloc(void * , size_t );
# 374 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
typedef int (*__heapprt)(void *, char const *, ...);
extern __attribute__((__nothrow__)) void __heapstats(int (* )(void * ,
                                           char const * , ...),
                        void * ) __attribute__((__nonnull__(1)));
# 390 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) int __heapvalid(int (* )(void * ,
                                           char const * , ...),
                       void * , int ) __attribute__((__nonnull__(1)));
# 411 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__noreturn__)) void abort(void);
# 422 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) int atexit(void (* )(void)) __attribute__((__nonnull__(1)));
# 444 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__noreturn__)) void exit(int );
# 460 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__noreturn__)) void _Exit(int );
# 471 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) char *getenv(const char * ) __attribute__((__nonnull__(1)));
# 484 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) int system(const char * );
# 497 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern void *bsearch(const void * , const void * ,
              size_t , size_t ,
              int (* )(const void *, const void *)) __attribute__((__nonnull__(1,2,5)));
# 532 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern void qsort(void * , size_t , size_t ,
           int (* )(const void *, const void *)) __attribute__((__nonnull__(1,4)));
# 560 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__const__)) int abs(int );






extern __attribute__((__nothrow__)) __attribute__((__const__)) div_t div(int , int );
# 579 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__const__)) long int labs(long int );
# 589 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__const__)) ldiv_t ldiv(long int , long int );
# 610 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__const__)) long long llabs(long long );
# 620 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) __attribute__((__const__)) lldiv_t lldiv(long long , long long );
# 644 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
typedef struct __sdiv32by16 { long quot, rem; } __sdiv32by16;
typedef struct __udiv32by16 { unsigned long quot, rem; } __udiv32by16;

typedef struct __sdiv64by32 { long rem, quot; } __sdiv64by32;

__attribute__((__value_in_regs__)) extern __attribute__((__nothrow__)) __attribute__((__const__)) __sdiv32by16 __rt_sdiv32by16(
     int ,
     short int );



__attribute__((__value_in_regs__)) extern __attribute__((__nothrow__)) __attribute__((__const__)) __udiv32by16 __rt_udiv32by16(
     unsigned int ,
     unsigned short );



__attribute__((__value_in_regs__)) extern __attribute__((__nothrow__)) __attribute__((__const__)) __sdiv64by32 __rt_sdiv64by32(
     int , unsigned int ,
     int );







extern __attribute__((__nothrow__)) unsigned int __fp_status(unsigned int , unsigned int );
# 705 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) int mblen(const char * , size_t );
# 720 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) int mbtowc(wchar_t * __restrict ,
                   const char * __restrict , size_t );
# 739 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) int wctomb(char * , wchar_t );
# 761 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) size_t mbstowcs(wchar_t * __restrict ,
                      const char * __restrict , size_t ) __attribute__((__nonnull__(2)));
# 779 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) size_t wcstombs(char * __restrict ,
                      const wchar_t * __restrict , size_t ) __attribute__((__nonnull__(2)));
# 798 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\stdlib.h" 3
extern __attribute__((__nothrow__)) void __use_realtime_heap(void);
extern __attribute__((__nothrow__)) void __use_realtime_division(void);
extern __attribute__((__nothrow__)) void __use_two_region_memory(void);
extern __attribute__((__nothrow__)) void __use_no_heap(void);
extern __attribute__((__nothrow__)) void __use_no_heap_region(void);

extern __attribute__((__nothrow__)) char const *__C_library_version_string(void);
extern __attribute__((__nothrow__)) int __C_library_version_number(void);
# 7 "Source/Libs/GLCD\\glcd_macros.h" 2
# 1 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 1 3
# 58 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) void *memcpy(void * __restrict ,
                    const void * __restrict , size_t ) __attribute__((__nonnull__(1,2)));






extern __attribute__((__nothrow__)) void *memmove(void * ,
                    const void * , size_t ) __attribute__((__nonnull__(1,2)));
# 77 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) char *strcpy(char * __restrict , const char * __restrict ) __attribute__((__nonnull__(1,2)));






extern __attribute__((__nothrow__)) char *strncpy(char * __restrict , const char * __restrict , size_t ) __attribute__((__nonnull__(1,2)));
# 93 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) char *strcat(char * __restrict , const char * __restrict ) __attribute__((__nonnull__(1,2)));






extern __attribute__((__nothrow__)) char *strncat(char * __restrict , const char * __restrict , size_t ) __attribute__((__nonnull__(1,2)));
# 117 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) int memcmp(const void * , const void * , size_t ) __attribute__((__nonnull__(1,2)));







extern __attribute__((__nothrow__)) int strcmp(const char * , const char * ) __attribute__((__nonnull__(1,2)));






extern __attribute__((__nothrow__)) int strncmp(const char * , const char * , size_t ) __attribute__((__nonnull__(1,2)));
# 141 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) int strcasecmp(const char * , const char * ) __attribute__((__nonnull__(1,2)));







extern __attribute__((__nothrow__)) int strncasecmp(const char * , const char * , size_t ) __attribute__((__nonnull__(1,2)));
# 158 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) int strcoll(const char * , const char * ) __attribute__((__nonnull__(1,2)));
# 169 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) size_t strxfrm(char * __restrict , const char * __restrict , size_t ) __attribute__((__nonnull__(2)));
# 193 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) void *memchr(const void * , int , size_t ) __attribute__((__nonnull__(1)));
# 209 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) char *strchr(const char * , int ) __attribute__((__nonnull__(1)));
# 218 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) size_t strcspn(const char * , const char * ) __attribute__((__nonnull__(1,2)));
# 232 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) char *strpbrk(const char * , const char * ) __attribute__((__nonnull__(1,2)));
# 247 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) char *strrchr(const char * , int ) __attribute__((__nonnull__(1)));
# 257 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) size_t strspn(const char * , const char * ) __attribute__((__nonnull__(1,2)));
# 270 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) char *strstr(const char * , const char * ) __attribute__((__nonnull__(1,2)));
# 280 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) char *strtok(char * __restrict , const char * __restrict ) __attribute__((__nonnull__(2)));
extern __attribute__((__nothrow__)) char *_strtok_r(char * , const char * , char ** ) __attribute__((__nonnull__(2,3)));
# 321 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) void *memset(void * , int , size_t ) __attribute__((__nonnull__(1)));





extern __attribute__((__nothrow__)) char *strerror(int );







extern __attribute__((__nothrow__)) size_t strlen(const char * ) __attribute__((__nonnull__(1)));





extern __attribute__((__nothrow__)) size_t strnlen(const char * , size_t ) __attribute__((__nonnull__(1)));







extern __attribute__((__nothrow__)) size_t strlcpy(char * , const char * , size_t ) __attribute__((__nonnull__(1,2)));
# 369 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) size_t strlcat(char * , const char * , size_t ) __attribute__((__nonnull__(1,2)));
# 395 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\string.h" 3
extern __attribute__((__nothrow__)) void _membitcpybl(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitcpybb(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitcpyhl(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitcpyhb(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitcpywl(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitcpywb(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitmovebl(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitmovebb(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitmovehl(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitmovehb(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitmovewl(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
extern __attribute__((__nothrow__)) void _membitmovewb(void * , const void * , int , int , size_t ) __attribute__((__nonnull__(1,2)));
# 8 "Source/Libs/GLCD\\glcd_macros.h" 2
# 5 "Source/Libs/GLCD\\glcd.h" 2






__attribute__((always_inline)) inline u16 rgb888_to_rgb565(u32 rgb888)
{
    const u8 r = (rgb888 >> 16) & 0xFF;
    const u8 g = (rgb888 >> 8) & 0xFF;
    const u8 b = rgb888 & 0xFF;
    return (u16)((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3));
}





void LCD_Init(LCD_Orientation orientation);



_Bool LCD_IsInitialized(void);


u16 LCD_GetWidth(void);


u16 LCD_GetHeight(void);


LCD_Coordinate LCD_GetSize(void);


LCD_Coordinate LCD_GetCenter(void);
# 50 "Source/Libs/GLCD\\glcd.h"
LCD_Color LCD_GetPointColor(LCD_Coordinate point);




void LCD_SetPointColor(LCD_Color color, LCD_Coordinate point);





void LCD_SetBackgroundColor(LCD_Color color);




LCD_ObjID LCD_RQAddObject(const LCD_Obj *const obj);



void LCD_RQRender(void);




void LCD_RQRemoveObject(LCD_ObjID id, _Bool redraw_screen);





void LCD_RQSetObjectVisibility(LCD_ObjID id, _Bool visible, _Bool redraw_screen);


void LCD_RQClear(void);






LCD_FontID LCD_FMAddFont(LCD_Font font);


void LCD_FMRemoveFont(LCD_FontID id);
# 2 "Source/Libs/GLCD/glcd.c" 2

# 1 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\LPC17xx.h" 1
# 41 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\LPC17xx.h"
typedef enum IRQn
{

  Reset_IRQn = -15,
  NonMaskableInt_IRQn = -14,
  HardFault_IRQn = -13,
  MemoryManagement_IRQn = -12,
  BusFault_IRQn = -11,
  UsageFault_IRQn = -10,
  SVCall_IRQn = -5,
  DebugMonitor_IRQn = -4,
  PendSV_IRQn = -2,
  SysTick_IRQn = -1,


  WDT_IRQn = 0,
  TIMER0_IRQn = 1,
  TIMER1_IRQn = 2,
  TIMER2_IRQn = 3,
  TIMER3_IRQn = 4,
  UART0_IRQn = 5,
  UART1_IRQn = 6,
  UART2_IRQn = 7,
  UART3_IRQn = 8,
  PWM1_IRQn = 9,
  I2C0_IRQn = 10,
  I2C1_IRQn = 11,
  I2C2_IRQn = 12,
  SPI_IRQn = 13,
  SSP0_IRQn = 14,
  SSP1_IRQn = 15,
  PLL0_IRQn = 16,
  RTC_IRQn = 17,
  EINT0_IRQn = 18,
  EINT1_IRQn = 19,
  EINT2_IRQn = 20,
  EINT3_IRQn = 21,
  ADC_IRQn = 22,
  BOD_IRQn = 23,
  USB_IRQn = 24,
  CAN_IRQn = 25,
  DMA_IRQn = 26,
  I2S_IRQn = 27,
  ENET_IRQn = 28,
  RIT_IRQn = 29,
  MCPWM_IRQn = 30,
  QEI_IRQn = 31,
  PLL1_IRQn = 32,
  USBActivity_IRQn = 33,
  CANActivity_IRQn = 34,
} IRQn_Type;
# 106 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\LPC17xx.h"
# 1 "./Source/CMSIS_core\\core_cm3.h" 1
# 29 "./Source/CMSIS_core\\core_cm3.h" 3
# 63 "./Source/CMSIS_core\\core_cm3.h" 3
# 1 "./Source/CMSIS_core\\cmsis_version.h" 1 3
# 29 "./Source/CMSIS_core\\cmsis_version.h" 3
# 64 "./Source/CMSIS_core\\core_cm3.h" 2 3
# 115 "./Source/CMSIS_core\\core_cm3.h" 3
# 1 "./Source/CMSIS_core\\cmsis_compiler.h" 1 3
# 47 "./Source/CMSIS_core\\cmsis_compiler.h" 3
# 1 "./Source/CMSIS_core\\cmsis_armclang.h" 1 3
# 31 "./Source/CMSIS_core\\cmsis_armclang.h" 3
# 64 "./Source/CMSIS_core\\cmsis_armclang.h" 3
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed)) T_UINT32 { uint32_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT16_WRITE { uint16_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT16_READ { uint16_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT32_WRITE { uint32_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

  struct __attribute__((packed, aligned(1))) T_UINT32_READ { uint32_t v; };
#pragma clang diagnostic pop
# 260 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  op2 %= 32U;
  if (op2 == 0U)
  {
    return op1;
  }
  return (op1 >> op2) | (op1 << (32U - op2));
}
# 295 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint8_t __CLZ(uint32_t value)
{
# 306 "./Source/CMSIS_core\\cmsis_armclang.h" 3
  if (value == 0U)
  {
    return 32U;
  }
  return __builtin_clz(value);
}
# 425 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __RRX(uint32_t value)
{
  uint32_t result;

  __asm volatile ("rrx %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}
# 440 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint8_t __LDRBT(volatile uint8_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrbt %0, %1" : "=r" (result) : "Q" (*ptr) );
  return ((uint8_t) result);
}
# 455 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint16_t __LDRHT(volatile uint16_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrht %0, %1" : "=r" (result) : "Q" (*ptr) );
  return ((uint16_t) result);
}
# 470 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __LDRT(volatile uint32_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrt %0, %1" : "=r" (result) : "Q" (*ptr) );
  return(result);
}
# 485 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRBT(uint8_t value, volatile uint8_t *ptr)
{
  __asm volatile ("strbt %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}
# 497 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRHT(uint16_t value, volatile uint16_t *ptr)
{
  __asm volatile ("strht %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}
# 509 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRT(uint32_t value, volatile uint32_t *ptr)
{
  __asm volatile ("strt %1, %0" : "=Q" (*ptr) : "r" (value) );
}
# 737 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __enable_irq(void)
{
  __asm volatile ("cpsie i" : : : "memory");
}
# 750 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __disable_irq(void)
{
  __asm volatile ("cpsid i" : : : "memory");
}
# 762 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_CONTROL(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}
# 792 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_CONTROL(uint32_t control)
{
  __asm volatile ("MSR control, %0" : : "r" (control) : "memory");
  __builtin_arm_isb(0xF);
}
# 818 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_IPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_APSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_xPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_PSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, psp" : "=r" (result) );
  return(result);
}
# 890 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_PSP(uint32_t topOfProcStack)
{
  __asm volatile ("MSR psp, %0" : : "r" (topOfProcStack) : );
}
# 914 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_MSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, msp" : "=r" (result) );
  return(result);
}
# 944 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_MSP(uint32_t topOfMainStack)
{
  __asm volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}
# 995 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_PRIMASK(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}
# 1025 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_PRIMASK(uint32_t priMask)
{
  __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}
# 1053 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __enable_fault_irq(void)
{
  __asm volatile ("cpsie f" : : : "memory");
}







__attribute__((always_inline)) static __inline void __disable_fault_irq(void)
{
  __asm volatile ("cpsid f" : : : "memory");
}







__attribute__((always_inline)) static __inline uint32_t __get_BASEPRI(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, basepri" : "=r" (result) );
  return(result);
}
# 1105 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_BASEPRI(uint32_t basePri)
{
  __asm volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}
# 1130 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_BASEPRI_MAX(uint32_t basePri)
{
  __asm volatile ("MSR basepri_max, %0" : : "r" (basePri) : "memory");
}







__attribute__((always_inline)) static __inline uint32_t __get_FAULTMASK(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, faultmask" : "=r" (result) );
  return(result);
}
# 1171 "./Source/CMSIS_core\\cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_FAULTMASK(uint32_t faultMask)
{
  __asm volatile ("MSR faultmask, %0" : : "r" (faultMask) : "memory");
}
# 48 "./Source/CMSIS_core\\cmsis_compiler.h" 2 3
# 116 "./Source/CMSIS_core\\core_cm3.h" 2 3
# 211 "./Source/CMSIS_core\\core_cm3.h" 3
typedef union
{
  struct
  {
    uint32_t _reserved0:27;
    uint32_t Q:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
  } b;
  uint32_t w;
} APSR_Type;
# 245 "./Source/CMSIS_core\\core_cm3.h" 3
typedef union
{
  struct
  {
    uint32_t ISR:9;
    uint32_t _reserved0:23;
  } b;
  uint32_t w;
} IPSR_Type;
# 263 "./Source/CMSIS_core\\core_cm3.h" 3
typedef union
{
  struct
  {
    uint32_t ISR:9;
    uint32_t _reserved0:1;
    uint32_t ICI_IT_1:6;
    uint32_t _reserved1:8;
    uint32_t T:1;
    uint32_t ICI_IT_2:2;
    uint32_t Q:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
  } b;
  uint32_t w;
} xPSR_Type;
# 314 "./Source/CMSIS_core\\core_cm3.h" 3
typedef union
{
  struct
  {
    uint32_t nPRIV:1;
    uint32_t SPSEL:1;
    uint32_t _reserved1:30;
  } b;
  uint32_t w;
} CONTROL_Type;
# 345 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile uint32_t ISER[8U];
        uint32_t RESERVED0[24U];
  volatile uint32_t ICER[8U];
        uint32_t RESERVED1[24U];
  volatile uint32_t ISPR[8U];
        uint32_t RESERVED2[24U];
  volatile uint32_t ICPR[8U];
        uint32_t RESERVED3[24U];
  volatile uint32_t IABR[8U];
        uint32_t RESERVED4[56U];
  volatile uint8_t IP[240U];
        uint32_t RESERVED5[644U];
  volatile uint32_t STIR;
} NVIC_Type;
# 379 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile const uint32_t CPUID;
  volatile uint32_t ICSR;
  volatile uint32_t VTOR;
  volatile uint32_t AIRCR;
  volatile uint32_t SCR;
  volatile uint32_t CCR;
  volatile uint8_t SHP[12U];
  volatile uint32_t SHCSR;
  volatile uint32_t CFSR;
  volatile uint32_t HFSR;
  volatile uint32_t DFSR;
  volatile uint32_t MMFAR;
  volatile uint32_t BFAR;
  volatile uint32_t AFSR;
  volatile const uint32_t PFR[2U];
  volatile const uint32_t DFR;
  volatile const uint32_t ADR;
  volatile const uint32_t MMFR[4U];
  volatile const uint32_t ISAR[5U];
        uint32_t RESERVED0[5U];
  volatile uint32_t CPACR;
} SCB_Type;
# 660 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile const uint32_t ICTR;



        uint32_t RESERVED1[1U];

} SCnSCB_Type;
# 706 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile uint32_t CTRL;
  volatile uint32_t LOAD;
  volatile uint32_t VAL;
  volatile const uint32_t CALIB;
} SysTick_Type;
# 758 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile union
  {
    volatile uint8_t u8;
    volatile uint16_t u16;
    volatile uint32_t u32;
  } PORT [32U];
        uint32_t RESERVED0[864U];
  volatile uint32_t TER;
        uint32_t RESERVED1[15U];
  volatile uint32_t TPR;
        uint32_t RESERVED2[15U];
  volatile uint32_t TCR;
        uint32_t RESERVED3[32U];
        uint32_t RESERVED4[43U];
  volatile uint32_t LAR;
  volatile const uint32_t LSR;
        uint32_t RESERVED5[6U];
  volatile const uint32_t PID4;
  volatile const uint32_t PID5;
  volatile const uint32_t PID6;
  volatile const uint32_t PID7;
  volatile const uint32_t PID0;
  volatile const uint32_t PID1;
  volatile const uint32_t PID2;
  volatile const uint32_t PID3;
  volatile const uint32_t CID0;
  volatile const uint32_t CID1;
  volatile const uint32_t CID2;
  volatile const uint32_t CID3;
} ITM_Type;
# 846 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile uint32_t CTRL;
  volatile uint32_t CYCCNT;
  volatile uint32_t CPICNT;
  volatile uint32_t EXCCNT;
  volatile uint32_t SLEEPCNT;
  volatile uint32_t LSUCNT;
  volatile uint32_t FOLDCNT;
  volatile const uint32_t PCSR;
  volatile uint32_t COMP0;
  volatile uint32_t MASK0;
  volatile uint32_t FUNCTION0;
        uint32_t RESERVED0[1U];
  volatile uint32_t COMP1;
  volatile uint32_t MASK1;
  volatile uint32_t FUNCTION1;
        uint32_t RESERVED1[1U];
  volatile uint32_t COMP2;
  volatile uint32_t MASK2;
  volatile uint32_t FUNCTION2;
        uint32_t RESERVED2[1U];
  volatile uint32_t COMP3;
  volatile uint32_t MASK3;
  volatile uint32_t FUNCTION3;
} DWT_Type;
# 993 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile const uint32_t SSPSR;
  volatile uint32_t CSPSR;
        uint32_t RESERVED0[2U];
  volatile uint32_t ACPR;
        uint32_t RESERVED1[55U];
  volatile uint32_t SPPR;
        uint32_t RESERVED2[131U];
  volatile const uint32_t FFSR;
  volatile uint32_t FFCR;
  volatile const uint32_t FSCR;
        uint32_t RESERVED3[759U];
  volatile const uint32_t TRIGGER;
  volatile const uint32_t FIFO0;
  volatile const uint32_t ITATBCTR2;
        uint32_t RESERVED4[1U];
  volatile const uint32_t ITATBCTR0;
  volatile const uint32_t FIFO1;
  volatile uint32_t ITCTRL;
        uint32_t RESERVED5[39U];
  volatile uint32_t CLAIMSET;
  volatile uint32_t CLAIMCLR;
        uint32_t RESERVED7[8U];
  volatile const uint32_t DEVID;
  volatile const uint32_t DEVTYPE;
} TPI_Type;
# 1155 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile const uint32_t TYPE;
  volatile uint32_t CTRL;
  volatile uint32_t RNR;
  volatile uint32_t RBAR;
  volatile uint32_t RASR;
  volatile uint32_t RBAR_A1;
  volatile uint32_t RASR_A1;
  volatile uint32_t RBAR_A2;
  volatile uint32_t RASR_A2;
  volatile uint32_t RBAR_A3;
  volatile uint32_t RASR_A3;
} MPU_Type;
# 1251 "./Source/CMSIS_core\\core_cm3.h" 3
typedef struct
{
  volatile uint32_t DHCSR;
  volatile uint32_t DCRSR;
  volatile uint32_t DCRDR;
  volatile uint32_t DEMCR;
} CoreDebug_Type;
# 1477 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);

  reg_value = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;
  reg_value &= ~((uint32_t)((0xFFFFUL << 16U) | (7UL << 8U)));
  reg_value = (reg_value |
                ((uint32_t)0x5FAUL << 16U) |
                (PriorityGroupTmp << 8U) );
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = reg_value;
}







static __inline uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) >> 8U));
}
# 1508 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    __asm volatile("":::"memory");
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __asm volatile("":::"memory");
  }
}
# 1527 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1546 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __builtin_arm_dsb(0xF);
    __builtin_arm_isb(0xF);
  }
}
# 1565 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1584 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}
# 1599 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}
# 1616 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1638 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)] = (uint8_t)((priority << (8U - 5)) & (uint32_t)0xFFUL);
  }
  else
  {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - 5)) & (uint32_t)0xFFUL);
  }
}
# 1660 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)] >> (8U - 5)));
  }
  else
  {
    return(((uint32_t)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - 5)));
  }
}
# 1685 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(5)) ? (uint32_t)(5) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits = ((PriorityGroupTmp + (uint32_t)(5)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(5));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority & (uint32_t)((1UL << (SubPriorityBits )) - 1UL)))
         );
}
# 1712 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(5)) ? (uint32_t)(5) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits = ((PriorityGroupTmp + (uint32_t)(5)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(5));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority = (Priority ) & (uint32_t)((1UL << (SubPriorityBits )) - 1UL);
}
# 1735 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  vectors[(int32_t)IRQn + 16] = vector;

}
# 1751 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  return vectors[(int32_t)IRQn + 16];
}






__attribute__((__noreturn__)) static __inline void __NVIC_SystemReset(void)
{
  __builtin_arm_dsb(0xF);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = (uint32_t)((0x5FAUL << 16U) |
                           (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) |
                            (1UL << 2U) );
  __builtin_arm_dsb(0xF);

  for(;;)
  {
    __builtin_arm_nop();
  }
}
# 1784 "./Source/CMSIS_core\\core_cm3.h" 3
# 1 "./Source/CMSIS_core\\mpu_armv7.h" 1 3
# 29 "./Source/CMSIS_core\\mpu_armv7.h" 3
# 183 "./Source/CMSIS_core\\mpu_armv7.h" 3
typedef struct {
  uint32_t RBAR;
  uint32_t RASR;
} ARM_MPU_Region_t;




static __inline void ARM_MPU_Enable(uint32_t MPU_Control)
{
  __builtin_arm_dmb(0xF);
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = MPU_Control | (1UL );

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= (1UL << 16U);

  __builtin_arm_dsb(0xF);
  __builtin_arm_isb(0xF);
}



static __inline void ARM_MPU_Disable(void)
{
  __builtin_arm_dmb(0xF);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1UL << 16U);

  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL &= ~(1UL );
  __builtin_arm_dsb(0xF);
  __builtin_arm_isb(0xF);
}




static __inline void ARM_MPU_ClrRegion(uint32_t rnr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = 0U;
}





static __inline void ARM_MPU_SetRegion(uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}






static __inline void ARM_MPU_SetRegionEx(uint32_t rnr, uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}






static __inline void ARM_MPU_OrderedMemcpy(volatile uint32_t* dst, const uint32_t* __restrict src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i)
  {
    dst[i] = src[i];
  }
}





static __inline void ARM_MPU_Load(ARM_MPU_Region_t const* table, uint32_t cnt)
{
  const uint32_t rowWordSize = sizeof(ARM_MPU_Region_t)/4U;
  while (cnt > 4U) {
    ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), 4U*rowWordSize);
    table += 4U;
    cnt -= 4U;
  }
  ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), cnt*rowWordSize);
}
# 1785 "./Source/CMSIS_core\\core_cm3.h" 2 3
# 1805 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t SCB_GetFPUType(void)
{
    return 0U;
}
# 1836 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > (0xFFFFFFUL ))
  {
    return (1UL);
  }

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD = (uint32_t)(ticks - 1UL);
  __NVIC_SetPriority (SysTick_IRQn, (1UL << 5) - 1UL);
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL = 0UL;
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL = (1UL << 2U) |
                   (1UL << 1U) |
                   (1UL );
  return (0UL);
}
# 1866 "./Source/CMSIS_core\\core_cm3.h" 3
extern volatile int32_t ITM_RxBuffer;
# 1878 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline uint32_t ITM_SendChar (uint32_t ch)
{
  if (((((ITM_Type *) (0xE0000000UL) )->TCR & (1UL )) != 0UL) &&
      ((((ITM_Type *) (0xE0000000UL) )->TER & 1UL ) != 0UL) )
  {
    while (((ITM_Type *) (0xE0000000UL) )->PORT[0U].u32 == 0UL)
    {
      __builtin_arm_nop();
    }
    ((ITM_Type *) (0xE0000000UL) )->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}
# 1899 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline int32_t ITM_ReceiveChar (void)
{
  int32_t ch = -1;

  if (ITM_RxBuffer != ((int32_t)0x5AA55AA5U))
  {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ((int32_t)0x5AA55AA5U);
  }

  return (ch);
}
# 1919 "./Source/CMSIS_core\\core_cm3.h" 3
static __inline int32_t ITM_CheckChar (void)
{

  if (ITM_RxBuffer == ((int32_t)0x5AA55AA5U))
  {
    return (0);
  }
  else
  {
    return (1);
  }
}
# 107 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\LPC17xx.h" 2
# 1 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\system_LPC17xx.h" 1
# 49 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\system_LPC17xx.h"
extern uint32_t SystemCoreClock;






extern void SystemInit (void);







extern void SystemCoreClockUpdate (void);
# 108 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\LPC17xx.h" 2
# 120 "C:/Program Files/Arm/Packs/Keil/LPC1700_DFP/2.7.2/Device/Include\\LPC17xx.h"
typedef struct
{
  volatile uint32_t FLASHCFG;
       uint32_t RESERVED0[31];
  volatile uint32_t PLL0CON;
  volatile uint32_t PLL0CFG;
  volatile const uint32_t PLL0STAT;
  volatile uint32_t PLL0FEED;
       uint32_t RESERVED1[4];
  volatile uint32_t PLL1CON;
  volatile uint32_t PLL1CFG;
  volatile const uint32_t PLL1STAT;
  volatile uint32_t PLL1FEED;
       uint32_t RESERVED2[4];
  volatile uint32_t PCON;
  volatile uint32_t PCONP;
       uint32_t RESERVED3[15];
  volatile uint32_t CCLKCFG;
  volatile uint32_t USBCLKCFG;
  volatile uint32_t CLKSRCSEL;
  volatile uint32_t CANSLEEPCLR;
  volatile uint32_t CANWAKEFLAGS;
       uint32_t RESERVED4[10];
  volatile uint32_t EXTINT;
       uint32_t RESERVED5;
  volatile uint32_t EXTMODE;
  volatile uint32_t EXTPOLAR;
       uint32_t RESERVED6[12];
  volatile uint32_t RSID;
       uint32_t RESERVED7[7];
  volatile uint32_t SCS;
  volatile uint32_t IRCTRIM;
  volatile uint32_t PCLKSEL0;
  volatile uint32_t PCLKSEL1;
       uint32_t RESERVED8[4];
  volatile uint32_t USBIntSt;
  volatile uint32_t DMAREQSEL;
  volatile uint32_t CLKOUTCFG;
 } LPC_SC_TypeDef;



typedef struct
{
  volatile uint32_t PINSEL0;
  volatile uint32_t PINSEL1;
  volatile uint32_t PINSEL2;
  volatile uint32_t PINSEL3;
  volatile uint32_t PINSEL4;
  volatile uint32_t PINSEL5;
  volatile uint32_t PINSEL6;
  volatile uint32_t PINSEL7;
  volatile uint32_t PINSEL8;
  volatile uint32_t PINSEL9;
  volatile uint32_t PINSEL10;
       uint32_t RESERVED0[5];
  volatile uint32_t PINMODE0;
  volatile uint32_t PINMODE1;
  volatile uint32_t PINMODE2;
  volatile uint32_t PINMODE3;
  volatile uint32_t PINMODE4;
  volatile uint32_t PINMODE5;
  volatile uint32_t PINMODE6;
  volatile uint32_t PINMODE7;
  volatile uint32_t PINMODE8;
  volatile uint32_t PINMODE9;
  volatile uint32_t PINMODE_OD0;
  volatile uint32_t PINMODE_OD1;
  volatile uint32_t PINMODE_OD2;
  volatile uint32_t PINMODE_OD3;
  volatile uint32_t PINMODE_OD4;
  volatile uint32_t I2CPADCFG;
} LPC_PINCON_TypeDef;



typedef struct
{
  union {
    volatile uint32_t FIODIR;
    struct {
      volatile uint16_t FIODIRL;
      volatile uint16_t FIODIRH;
    };
    struct {
      volatile uint8_t FIODIR0;
      volatile uint8_t FIODIR1;
      volatile uint8_t FIODIR2;
      volatile uint8_t FIODIR3;
    };
  };
  uint32_t RESERVED0[3];
  union {
    volatile uint32_t FIOMASK;
    struct {
      volatile uint16_t FIOMASKL;
      volatile uint16_t FIOMASKH;
    };
    struct {
      volatile uint8_t FIOMASK0;
      volatile uint8_t FIOMASK1;
      volatile uint8_t FIOMASK2;
      volatile uint8_t FIOMASK3;
    };
  };
  union {
    volatile uint32_t FIOPIN;
    struct {
      volatile uint16_t FIOPINL;
      volatile uint16_t FIOPINH;
    };
    struct {
      volatile uint8_t FIOPIN0;
      volatile uint8_t FIOPIN1;
      volatile uint8_t FIOPIN2;
      volatile uint8_t FIOPIN3;
    };
  };
  union {
    volatile uint32_t FIOSET;
    struct {
      volatile uint16_t FIOSETL;
      volatile uint16_t FIOSETH;
    };
    struct {
      volatile uint8_t FIOSET0;
      volatile uint8_t FIOSET1;
      volatile uint8_t FIOSET2;
      volatile uint8_t FIOSET3;
    };
  };
  union {
    volatile uint32_t FIOCLR;
    struct {
      volatile uint16_t FIOCLRL;
      volatile uint16_t FIOCLRH;
    };
    struct {
      volatile uint8_t FIOCLR0;
      volatile uint8_t FIOCLR1;
      volatile uint8_t FIOCLR2;
      volatile uint8_t FIOCLR3;
    };
  };
} LPC_GPIO_TypeDef;


typedef struct
{
  volatile const uint32_t IntStatus;
  volatile const uint32_t IO0IntStatR;
  volatile const uint32_t IO0IntStatF;
  volatile uint32_t IO0IntClr;
  volatile uint32_t IO0IntEnR;
  volatile uint32_t IO0IntEnF;
       uint32_t RESERVED0[3];
  volatile const uint32_t IO2IntStatR;
  volatile const uint32_t IO2IntStatF;
  volatile uint32_t IO2IntClr;
  volatile uint32_t IO2IntEnR;
  volatile uint32_t IO2IntEnF;
} LPC_GPIOINT_TypeDef;



typedef struct
{
  volatile uint32_t IR;
  volatile uint32_t TCR;
  volatile uint32_t TC;
  volatile uint32_t PR;
  volatile uint32_t PC;
  volatile uint32_t MCR;
  volatile uint32_t MR0;
  volatile uint32_t MR1;
  volatile uint32_t MR2;
  volatile uint32_t MR3;
  volatile uint32_t CCR;
  volatile const uint32_t CR0;
  volatile const uint32_t CR1;
       uint32_t RESERVED0[2];
  volatile uint32_t EMR;
       uint32_t RESERVED1[12];
  volatile uint32_t CTCR;
} LPC_TIM_TypeDef;



typedef struct
{
  volatile uint32_t IR;
  volatile uint32_t TCR;
  volatile uint32_t TC;
  volatile uint32_t PR;
  volatile uint32_t PC;
  volatile uint32_t MCR;
  volatile uint32_t MR0;
  volatile uint32_t MR1;
  volatile uint32_t MR2;
  volatile uint32_t MR3;
  volatile uint32_t CCR;
  volatile const uint32_t CR0;
  volatile const uint32_t CR1;
  volatile const uint32_t CR2;
  volatile const uint32_t CR3;
       uint32_t RESERVED0;
  volatile uint32_t MR4;
  volatile uint32_t MR5;
  volatile uint32_t MR6;
  volatile uint32_t PCR;
  volatile uint32_t LER;
       uint32_t RESERVED1[7];
  volatile uint32_t CTCR;
} LPC_PWM_TypeDef;



typedef struct
{
  union {
  volatile const uint8_t RBR;
  volatile uint8_t THR;
  volatile uint8_t DLL;
       uint32_t RESERVED0;
  };
  union {
  volatile uint8_t DLM;
  volatile uint32_t IER;
  };
  union {
  volatile const uint32_t IIR;
  volatile uint8_t FCR;
  };
  volatile uint8_t LCR;
       uint8_t RESERVED1[7];
  volatile const uint8_t LSR;
       uint8_t RESERVED2[7];
  volatile uint8_t SCR;
       uint8_t RESERVED3[3];
  volatile uint32_t ACR;
  volatile uint8_t ICR;
       uint8_t RESERVED4[3];
  volatile uint8_t FDR;
       uint8_t RESERVED5[7];
  volatile uint8_t TER;
       uint8_t RESERVED6[3];
} LPC_UART_TypeDef;


typedef struct
{
  union {
  volatile const uint8_t RBR;
  volatile uint8_t THR;
  volatile uint8_t DLL;
       uint32_t RESERVED0;
  };
  union {
  volatile uint8_t DLM;
       uint8_t RESERVED1[3];
  volatile uint32_t IER;
  };
  union {
  volatile const uint32_t IIR;
  volatile uint8_t FCR;
  };
  volatile uint8_t LCR;
       uint8_t RESERVED2[3];
  volatile uint8_t MCR;
       uint8_t RESERVED3[3];
  volatile const uint8_t LSR;
       uint8_t RESERVED4[3];
  volatile const uint8_t MSR;
       uint8_t RESERVED5[3];
  volatile uint8_t SCR;
       uint8_t RESERVED6[3];
  volatile uint32_t ACR;
       uint32_t RESERVED7;
  volatile uint32_t FDR;
       uint32_t RESERVED8;
  volatile uint8_t TER;
       uint8_t RESERVED9[27];
  volatile uint8_t RS485CTRL;
       uint8_t RESERVED10[3];
  volatile uint8_t ADRMATCH;
       uint8_t RESERVED11[3];
  volatile uint8_t RS485DLY;
       uint8_t RESERVED12[3];
} LPC_UART1_TypeDef;



typedef struct
{
  volatile uint32_t SPCR;
  volatile const uint32_t SPSR;
  volatile uint32_t SPDR;
  volatile uint32_t SPCCR;
       uint32_t RESERVED0[3];
  volatile uint32_t SPINT;
} LPC_SPI_TypeDef;



typedef struct
{
  volatile uint32_t CR0;
  volatile uint32_t CR1;
  volatile uint32_t DR;
  volatile const uint32_t SR;
  volatile uint32_t CPSR;
  volatile uint32_t IMSC;
  volatile uint32_t RIS;
  volatile uint32_t MIS;
  volatile uint32_t ICR;
  volatile uint32_t DMACR;
} LPC_SSP_TypeDef;



typedef struct
{
  volatile uint32_t I2CONSET;
  volatile const uint32_t I2STAT;
  volatile uint32_t I2DAT;
  volatile uint32_t I2ADR0;
  volatile uint32_t I2SCLH;
  volatile uint32_t I2SCLL;
  volatile uint32_t I2CONCLR;
  volatile uint32_t MMCTRL;
  volatile uint32_t I2ADR1;
  volatile uint32_t I2ADR2;
  volatile uint32_t I2ADR3;
  volatile const uint32_t I2DATA_BUFFER;
  volatile uint32_t I2MASK0;
  volatile uint32_t I2MASK1;
  volatile uint32_t I2MASK2;
  volatile uint32_t I2MASK3;
} LPC_I2C_TypeDef;



typedef struct
{
  volatile uint32_t I2SDAO;
  volatile uint32_t I2SDAI;
  volatile uint32_t I2STXFIFO;
  volatile const uint32_t I2SRXFIFO;
  volatile const uint32_t I2SSTATE;
  volatile uint32_t I2SDMA1;
  volatile uint32_t I2SDMA2;
  volatile uint32_t I2SIRQ;
  volatile uint32_t I2STXRATE;
  volatile uint32_t I2SRXRATE;
  volatile uint32_t I2STXBITRATE;
  volatile uint32_t I2SRXBITRATE;
  volatile uint32_t I2STXMODE;
  volatile uint32_t I2SRXMODE;
} LPC_I2S_TypeDef;



typedef struct
{
  volatile uint32_t RICOMPVAL;
  volatile uint32_t RIMASK;
  volatile uint8_t RICTRL;
       uint8_t RESERVED0[3];
  volatile uint32_t RICOUNTER;
} LPC_RIT_TypeDef;



typedef struct
{
  volatile uint8_t ILR;
       uint8_t RESERVED0[7];
  volatile uint8_t CCR;
       uint8_t RESERVED1[3];
  volatile uint8_t CIIR;
       uint8_t RESERVED2[3];
  volatile uint8_t AMR;
       uint8_t RESERVED3[3];
  volatile const uint32_t CTIME0;
  volatile const uint32_t CTIME1;
  volatile const uint32_t CTIME2;
  volatile uint8_t SEC;
       uint8_t RESERVED4[3];
  volatile uint8_t MIN;
       uint8_t RESERVED5[3];
  volatile uint8_t HOUR;
       uint8_t RESERVED6[3];
  volatile uint8_t DOM;
       uint8_t RESERVED7[3];
  volatile uint8_t DOW;
       uint8_t RESERVED8[3];
  volatile uint16_t DOY;
       uint16_t RESERVED9;
  volatile uint8_t MONTH;
       uint8_t RESERVED10[3];
  volatile uint16_t YEAR;
       uint16_t RESERVED11;
  volatile uint32_t CALIBRATION;
  volatile uint32_t GPREG0;
  volatile uint32_t GPREG1;
  volatile uint32_t GPREG2;
  volatile uint32_t GPREG3;
  volatile uint32_t GPREG4;
  volatile uint8_t RTC_AUXEN;
       uint8_t RESERVED12[3];
  volatile uint8_t RTC_AUX;
       uint8_t RESERVED13[3];
  volatile uint8_t ALSEC;
       uint8_t RESERVED14[3];
  volatile uint8_t ALMIN;
       uint8_t RESERVED15[3];
  volatile uint8_t ALHOUR;
       uint8_t RESERVED16[3];
  volatile uint8_t ALDOM;
       uint8_t RESERVED17[3];
  volatile uint8_t ALDOW;
       uint8_t RESERVED18[3];
  volatile uint16_t ALDOY;
       uint16_t RESERVED19;
  volatile uint8_t ALMON;
       uint8_t RESERVED20[3];
  volatile uint16_t ALYEAR;
       uint16_t RESERVED21;
} LPC_RTC_TypeDef;



typedef struct
{
  volatile uint8_t WDMOD;
       uint8_t RESERVED0[3];
  volatile uint32_t WDTC;
  volatile uint8_t WDFEED;
       uint8_t RESERVED1[3];
  volatile const uint32_t WDTV;
  volatile uint32_t WDCLKSEL;
} LPC_WDT_TypeDef;



typedef struct
{
  volatile uint32_t ADCR;
  volatile uint32_t ADGDR;
       uint32_t RESERVED0;
  volatile uint32_t ADINTEN;
  volatile const uint32_t ADDR0;
  volatile const uint32_t ADDR1;
  volatile const uint32_t ADDR2;
  volatile const uint32_t ADDR3;
  volatile const uint32_t ADDR4;
  volatile const uint32_t ADDR5;
  volatile const uint32_t ADDR6;
  volatile const uint32_t ADDR7;
  volatile const uint32_t ADSTAT;
  volatile uint32_t ADTRM;
} LPC_ADC_TypeDef;



typedef struct
{
  volatile uint32_t DACR;
  volatile uint32_t DACCTRL;
  volatile uint16_t DACCNTVAL;
       uint16_t RESERVED;
} LPC_DAC_TypeDef;



typedef struct
{
  volatile const uint32_t MCCON;
  volatile uint32_t MCCON_SET;
  volatile uint32_t MCCON_CLR;
  volatile const uint32_t MCCAPCON;
  volatile uint32_t MCCAPCON_SET;
  volatile uint32_t MCCAPCON_CLR;
  volatile uint32_t MCTIM0;
  volatile uint32_t MCTIM1;
  volatile uint32_t MCTIM2;
  volatile uint32_t MCPER0;
  volatile uint32_t MCPER1;
  volatile uint32_t MCPER2;
  volatile uint32_t MCPW0;
  volatile uint32_t MCPW1;
  volatile uint32_t MCPW2;
  volatile uint32_t MCDEADTIME;
  volatile uint32_t MCCCP;
  volatile uint32_t MCCR0;
  volatile uint32_t MCCR1;
  volatile uint32_t MCCR2;
  volatile const uint32_t MCINTEN;
  volatile uint32_t MCINTEN_SET;
  volatile uint32_t MCINTEN_CLR;
  volatile const uint32_t MCCNTCON;
  volatile uint32_t MCCNTCON_SET;
  volatile uint32_t MCCNTCON_CLR;
  volatile const uint32_t MCINTFLAG;
  volatile uint32_t MCINTFLAG_SET;
  volatile uint32_t MCINTFLAG_CLR;
  volatile uint32_t MCCAP_CLR;
} LPC_MCPWM_TypeDef;



typedef struct
{
  volatile uint32_t QEICON;
  volatile const uint32_t QEISTAT;
  volatile uint32_t QEICONF;
  volatile const uint32_t QEIPOS;
  volatile uint32_t QEIMAXPOS;
  volatile uint32_t CMPOS0;
  volatile uint32_t CMPOS1;
  volatile uint32_t CMPOS2;
  volatile const uint32_t INXCNT;
  volatile uint32_t INXCMP;
  volatile uint32_t QEILOAD;
  volatile const uint32_t QEITIME;
  volatile const uint32_t QEIVEL;
  volatile const uint32_t QEICAP;
  volatile uint32_t VELCOMP;
  volatile uint32_t FILTER;
       uint32_t RESERVED0[998];
  volatile uint32_t QEIIEC;
  volatile uint32_t QEIIES;
  volatile const uint32_t QEIINTSTAT;
  volatile const uint32_t QEIIE;
  volatile uint32_t QEICLR;
  volatile uint32_t QEISET;
} LPC_QEI_TypeDef;



typedef struct
{
  volatile uint32_t mask[512];
} LPC_CANAF_RAM_TypeDef;


typedef struct
{
  volatile uint32_t AFMR;
  volatile uint32_t SFF_sa;
  volatile uint32_t SFF_GRP_sa;
  volatile uint32_t EFF_sa;
  volatile uint32_t EFF_GRP_sa;
  volatile uint32_t ENDofTable;
  volatile const uint32_t LUTerrAd;
  volatile const uint32_t LUTerr;
  volatile uint32_t FCANIE;
  volatile uint32_t FCANIC0;
  volatile uint32_t FCANIC1;
} LPC_CANAF_TypeDef;


typedef struct
{
  volatile const uint32_t CANTxSR;
  volatile const uint32_t CANRxSR;
  volatile const uint32_t CANMSR;
} LPC_CANCR_TypeDef;


typedef struct
{
  volatile uint32_t MOD;
  volatile uint32_t CMR;
  volatile uint32_t GSR;
  volatile const uint32_t ICR;
  volatile uint32_t IER;
  volatile uint32_t BTR;
  volatile uint32_t EWL;
  volatile const uint32_t SR;
  volatile uint32_t RFS;
  volatile uint32_t RID;
  volatile uint32_t RDA;
  volatile uint32_t RDB;
  volatile uint32_t TFI1;
  volatile uint32_t TID1;
  volatile uint32_t TDA1;
  volatile uint32_t TDB1;
  volatile uint32_t TFI2;
  volatile uint32_t TID2;
  volatile uint32_t TDA2;
  volatile uint32_t TDB2;
  volatile uint32_t TFI3;
  volatile uint32_t TID3;
  volatile uint32_t TDA3;
  volatile uint32_t TDB3;
} LPC_CAN_TypeDef;



typedef struct
{
  volatile const uint32_t DMACIntStat;
  volatile const uint32_t DMACIntTCStat;
  volatile uint32_t DMACIntTCClear;
  volatile const uint32_t DMACIntErrStat;
  volatile uint32_t DMACIntErrClr;
  volatile const uint32_t DMACRawIntTCStat;
  volatile const uint32_t DMACRawIntErrStat;
  volatile const uint32_t DMACEnbldChns;
  volatile uint32_t DMACSoftBReq;
  volatile uint32_t DMACSoftSReq;
  volatile uint32_t DMACSoftLBReq;
  volatile uint32_t DMACSoftLSReq;
  volatile uint32_t DMACConfig;
  volatile uint32_t DMACSync;
} LPC_GPDMA_TypeDef;


typedef struct
{
  volatile uint32_t DMACCSrcAddr;
  volatile uint32_t DMACCDestAddr;
  volatile uint32_t DMACCLLI;
  volatile uint32_t DMACCControl;
  volatile uint32_t DMACCConfig;
} LPC_GPDMACH_TypeDef;



typedef struct
{
  volatile const uint32_t HcRevision;
  volatile uint32_t HcControl;
  volatile uint32_t HcCommandStatus;
  volatile uint32_t HcInterruptStatus;
  volatile uint32_t HcInterruptEnable;
  volatile uint32_t HcInterruptDisable;
  volatile uint32_t HcHCCA;
  volatile const uint32_t HcPeriodCurrentED;
  volatile uint32_t HcControlHeadED;
  volatile uint32_t HcControlCurrentED;
  volatile uint32_t HcBulkHeadED;
  volatile uint32_t HcBulkCurrentED;
  volatile const uint32_t HcDoneHead;
  volatile uint32_t HcFmInterval;
  volatile const uint32_t HcFmRemaining;
  volatile const uint32_t HcFmNumber;
  volatile uint32_t HcPeriodicStart;
  volatile uint32_t HcLSTreshold;
  volatile uint32_t HcRhDescriptorA;
  volatile uint32_t HcRhDescriptorB;
  volatile uint32_t HcRhStatus;
  volatile uint32_t HcRhPortStatus1;
  volatile uint32_t HcRhPortStatus2;
       uint32_t RESERVED0[40];
  volatile const uint32_t Module_ID;

  volatile const uint32_t OTGIntSt;
  volatile uint32_t OTGIntEn;
  volatile uint32_t OTGIntSet;
  volatile uint32_t OTGIntClr;
  volatile uint32_t OTGStCtrl;
  volatile uint32_t OTGTmr;
       uint32_t RESERVED1[58];

  volatile const uint32_t USBDevIntSt;
  volatile uint32_t USBDevIntEn;
  volatile uint32_t USBDevIntClr;
  volatile uint32_t USBDevIntSet;

  volatile uint32_t USBCmdCode;
  volatile const uint32_t USBCmdData;

  volatile const uint32_t USBRxData;
  volatile uint32_t USBTxData;
  volatile const uint32_t USBRxPLen;
  volatile uint32_t USBTxPLen;
  volatile uint32_t USBCtrl;
  volatile uint32_t USBDevIntPri;

  volatile const uint32_t USBEpIntSt;
  volatile uint32_t USBEpIntEn;
  volatile uint32_t USBEpIntClr;
  volatile uint32_t USBEpIntSet;
  volatile uint32_t USBEpIntPri;

  volatile uint32_t USBReEp;
  volatile uint32_t USBEpInd;
  volatile uint32_t USBMaxPSize;

  volatile const uint32_t USBDMARSt;
  volatile uint32_t USBDMARClr;
  volatile uint32_t USBDMARSet;
       uint32_t RESERVED2[9];
  volatile uint32_t USBUDCAH;
  volatile const uint32_t USBEpDMASt;
  volatile uint32_t USBEpDMAEn;
  volatile uint32_t USBEpDMADis;
  volatile const uint32_t USBDMAIntSt;
  volatile uint32_t USBDMAIntEn;
       uint32_t RESERVED3[2];
  volatile const uint32_t USBEoTIntSt;
  volatile uint32_t USBEoTIntClr;
  volatile uint32_t USBEoTIntSet;
  volatile const uint32_t USBNDDRIntSt;
  volatile uint32_t USBNDDRIntClr;
  volatile uint32_t USBNDDRIntSet;
  volatile const uint32_t USBSysErrIntSt;
  volatile uint32_t USBSysErrIntClr;
  volatile uint32_t USBSysErrIntSet;
       uint32_t RESERVED4[15];

  union {
  volatile const uint32_t I2C_RX;
  volatile uint32_t I2C_TX;
  };
  volatile const uint32_t I2C_STS;
  volatile uint32_t I2C_CTL;
  volatile uint32_t I2C_CLKHI;
  volatile uint32_t I2C_CLKLO;
       uint32_t RESERVED5[824];

  union {
  volatile uint32_t USBClkCtrl;
  volatile uint32_t OTGClkCtrl;
  };
  union {
  volatile const uint32_t USBClkSt;
  volatile const uint32_t OTGClkSt;
  };
} LPC_USB_TypeDef;



typedef struct
{
  volatile uint32_t MAC1;
  volatile uint32_t MAC2;
  volatile uint32_t IPGT;
  volatile uint32_t IPGR;
  volatile uint32_t CLRT;
  volatile uint32_t MAXF;
  volatile uint32_t SUPP;
  volatile uint32_t TEST;
  volatile uint32_t MCFG;
  volatile uint32_t MCMD;
  volatile uint32_t MADR;
  volatile uint32_t MWTD;
  volatile const uint32_t MRDD;
  volatile const uint32_t MIND;
       uint32_t RESERVED0[2];
  volatile uint32_t SA0;
  volatile uint32_t SA1;
  volatile uint32_t SA2;
       uint32_t RESERVED1[45];
  volatile uint32_t Command;
  volatile const uint32_t Status;
  volatile uint32_t RxDescriptor;
  volatile uint32_t RxStatus;
  volatile uint32_t RxDescriptorNumber;
  volatile const uint32_t RxProduceIndex;
  volatile uint32_t RxConsumeIndex;
  volatile uint32_t TxDescriptor;
  volatile uint32_t TxStatus;
  volatile uint32_t TxDescriptorNumber;
  volatile uint32_t TxProduceIndex;
  volatile const uint32_t TxConsumeIndex;
       uint32_t RESERVED2[10];
  volatile const uint32_t TSV0;
  volatile const uint32_t TSV1;
  volatile const uint32_t RSV;
       uint32_t RESERVED3[3];
  volatile uint32_t FlowControlCounter;
  volatile const uint32_t FlowControlStatus;
       uint32_t RESERVED4[34];
  volatile uint32_t RxFilterCtrl;
  volatile uint32_t RxFilterWoLStatus;
  volatile uint32_t RxFilterWoLClear;
       uint32_t RESERVED5;
  volatile uint32_t HashFilterL;
  volatile uint32_t HashFilterH;
       uint32_t RESERVED6[882];
  volatile const uint32_t IntStatus;
  volatile uint32_t IntEnable;
  volatile uint32_t IntClear;
  volatile uint32_t IntSet;
       uint32_t RESERVED7;
  volatile uint32_t PowerDown;
       uint32_t RESERVED8;
  volatile uint32_t Module_ID;
} LPC_EMAC_TypeDef;
# 4 "Source/Libs/GLCD/glcd.c" 2
# 1 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\alloca.h" 1 3
# 5 "Source/Libs/GLCD/glcd.c" 2
# 1 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 1 3
# 157 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern __attribute__((__pcs__("aapcs"))) unsigned __ARM_dcmp4(double , double );
extern __attribute__((__pcs__("aapcs"))) unsigned __ARM_fcmp4(float , float );







extern __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_fpclassifyf(float );
extern __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_fpclassify(double );



static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isfinitef(float __x)
{
    return (((*(unsigned *)&(__x)) >> 23) & 0xff) != 0xff;
}
static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isfinite(double __x)
{
    return (((*(1 + (unsigned *)&(__x))) >> 20) & 0x7ff) != 0x7ff;
}



static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isinff(float __x)
{
    return ((*(unsigned *)&(__x)) << 1) == 0xff000000;
}
static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isinf(double __x)
{
    return (((*(1 + (unsigned *)&(__x))) << 1) == 0xffe00000) && ((*(unsigned *)&(__x)) == 0);
}



static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_islessgreaterf(float __x, float __y)
{
    unsigned __f = __ARM_fcmp4(__x, __y) >> 28;
    return (__f == 8) || (__f == 2);
}
static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_islessgreater(double __x, double __y)
{
    unsigned __f = __ARM_dcmp4(__x, __y) >> 28;
    return (__f == 8) || (__f == 2);
}





static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isnanf(float __x)
{
    return (0x7f800000 - ((*(unsigned *)&(__x)) & 0x7fffffff)) >> 31;
}
static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isnan(double __x)
{
    unsigned __xf = (*(1 + (unsigned *)&(__x))) | (((*(unsigned *)&(__x)) == 0) ? 0 : 1);
    return (0x7ff00000 - (__xf & 0x7fffffff)) >> 31;
}



static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isnormalf(float __x)
{
    unsigned __xe = ((*(unsigned *)&(__x)) >> 23) & 0xff;
    return (__xe != 0xff) && (__xe != 0);
}
static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_isnormal(double __x)
{
    unsigned __xe = ((*(1 + (unsigned *)&(__x))) >> 20) & 0x7ff;
    return (__xe != 0x7ff) && (__xe != 0);
}



static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_signbitf(float __x)
{
    return (*(unsigned *)&(__x)) >> 31;
}
static __inline __attribute__((__nothrow__)) __attribute__((__pcs__("aapcs"))) int __ARM_signbit(double __x)
{
    return (*(1 + (unsigned *)&(__x))) >> 31;
}
# 266 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
  typedef float float_t;
  typedef double double_t;
# 282 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern const int math_errhandling;







extern __attribute__((__nothrow__)) double acos(double );



extern __attribute__((__nothrow__)) double asin(double );





extern __attribute__((__nothrow__)) __attribute__((__const__)) double atan(double );



extern __attribute__((__nothrow__)) double atan2(double , double );





extern __attribute__((__nothrow__)) double cos(double );




extern __attribute__((__nothrow__)) double sin(double );





extern void __use_accurate_range_reduction(void);



extern __attribute__((__nothrow__)) double tan(double );





extern __attribute__((__nothrow__)) double cosh(double );




extern __attribute__((__nothrow__)) double sinh(double );






extern __attribute__((__nothrow__)) __attribute__((__const__)) double tanh(double );



extern __attribute__((__nothrow__)) double exp(double );






extern __attribute__((__nothrow__)) double frexp(double , int * ) __attribute__((__nonnull__(2)));







extern __attribute__((__nothrow__)) double ldexp(double , int );




extern __attribute__((__nothrow__)) double log(double );





extern __attribute__((__nothrow__)) double log10(double );



extern __attribute__((__nothrow__)) double modf(double , double * ) __attribute__((__nonnull__(2)));





extern __attribute__((__nothrow__)) double pow(double , double );






extern __attribute__((__nothrow__)) double sqrt(double );
# 410 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
    static __inline double _sqrt(double __x) { return sqrt(__x); }
# 427 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
    static __inline float _sqrtf(float __x) { return (float)sqrt(__x); }







extern __attribute__((__nothrow__)) __attribute__((__const__)) double ceil(double );


extern __attribute__((__nothrow__)) __attribute__((__const__)) double fabs(double );



extern __attribute__((__nothrow__)) __attribute__((__const__)) double floor(double );



extern __attribute__((__nothrow__)) double fmod(double , double );
# 467 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern __attribute__((__nothrow__)) double acosh(double );



extern __attribute__((__nothrow__)) double asinh(double );



extern __attribute__((__nothrow__)) double atanh(double );



extern __attribute__((__nothrow__)) double cbrt(double );



static __inline __attribute__((__nothrow__)) __attribute__((__const__)) double copysign(double __x, double __y)



{
    (*(1 + (unsigned *)&(__x))) = ((*(1 + (unsigned *)&(__x))) & 0x7fffffff) | ((*(1 + (unsigned *)&(__y))) & 0x80000000);
    return __x;
}
static __inline __attribute__((__nothrow__)) __attribute__((__const__)) float copysignf(float __x, float __y)



{
    (*(unsigned *)&(__x)) = ((*(unsigned *)&(__x)) & 0x7fffffff) | ((*(unsigned *)&(__y)) & 0x80000000);
    return __x;
}
extern __attribute__((__nothrow__)) double erf(double );



extern __attribute__((__nothrow__)) double erfc(double );



extern __attribute__((__nothrow__)) double expm1(double );
# 533 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern __attribute__((__nothrow__)) double hypot(double , double );






extern __attribute__((__nothrow__)) int ilogb(double );



extern __attribute__((__nothrow__)) int ilogbf(float );



extern __attribute__((__nothrow__)) int ilogbl(long double );
# 646 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern __attribute__((__nothrow__)) double lgamma (double );




extern __attribute__((__nothrow__)) double log1p(double );



extern __attribute__((__nothrow__)) double logb(double );



extern __attribute__((__nothrow__)) float logbf(float );



extern __attribute__((__nothrow__)) long double logbl(long double );



extern __attribute__((__nothrow__)) double nextafter(double , double );




extern __attribute__((__nothrow__)) float nextafterf(float , float );




extern __attribute__((__nothrow__)) long double nextafterl(long double , long double );




extern __attribute__((__nothrow__)) double nexttoward(double , long double );




extern __attribute__((__nothrow__)) float nexttowardf(float , long double );




extern __attribute__((__nothrow__)) long double nexttowardl(long double , long double );




extern __attribute__((__nothrow__)) double remainder(double , double );



extern __attribute__((__nothrow__)) __attribute__((__const__)) double rint(double );



extern __attribute__((__nothrow__)) double scalbln(double , long int );



extern __attribute__((__nothrow__)) float scalblnf(float , long int );



extern __attribute__((__nothrow__)) long double scalblnl(long double , long int );



extern __attribute__((__nothrow__)) double scalbn(double , int );



extern __attribute__((__nothrow__)) float scalbnf(float , int );



extern __attribute__((__nothrow__)) long double scalbnl(long double , int );
# 740 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern __attribute__((__nothrow__)) __attribute__((__const__)) float fabsf(float);
static __inline __attribute__((__nothrow__)) __attribute__((__const__)) float _fabsf(float __f) { return fabsf(__f); }
extern __attribute__((__nothrow__)) float sinf(float );
extern __attribute__((__nothrow__)) float cosf(float );
extern __attribute__((__nothrow__)) float tanf(float );
extern __attribute__((__nothrow__)) float acosf(float );
extern __attribute__((__nothrow__)) float asinf(float );
extern __attribute__((__nothrow__)) float atanf(float );
extern __attribute__((__nothrow__)) float atan2f(float , float );
extern __attribute__((__nothrow__)) float sinhf(float );
extern __attribute__((__nothrow__)) float coshf(float );
extern __attribute__((__nothrow__)) float tanhf(float );
extern __attribute__((__nothrow__)) float expf(float );
extern __attribute__((__nothrow__)) float logf(float );
extern __attribute__((__nothrow__)) float log10f(float );
extern __attribute__((__nothrow__)) float powf(float , float );
extern __attribute__((__nothrow__)) float sqrtf(float );
extern __attribute__((__nothrow__)) float ldexpf(float , int );
extern __attribute__((__nothrow__)) float frexpf(float , int * ) __attribute__((__nonnull__(2)));
extern __attribute__((__nothrow__)) __attribute__((__const__)) float ceilf(float );
extern __attribute__((__nothrow__)) __attribute__((__const__)) float floorf(float );
extern __attribute__((__nothrow__)) float fmodf(float , float );
extern __attribute__((__nothrow__)) float modff(float , float * ) __attribute__((__nonnull__(2)));
# 780 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
__attribute__((__nothrow__)) long double acosl(long double );
__attribute__((__nothrow__)) long double asinl(long double );
__attribute__((__nothrow__)) long double atanl(long double );
__attribute__((__nothrow__)) long double atan2l(long double , long double );
__attribute__((__nothrow__)) long double ceill(long double );
__attribute__((__nothrow__)) long double cosl(long double );
__attribute__((__nothrow__)) long double coshl(long double );
__attribute__((__nothrow__)) long double expl(long double );
__attribute__((__nothrow__)) long double fabsl(long double );
__attribute__((__nothrow__)) long double floorl(long double );
__attribute__((__nothrow__)) long double fmodl(long double , long double );
__attribute__((__nothrow__)) long double frexpl(long double , int* ) __attribute__((__nonnull__(2)));
__attribute__((__nothrow__)) long double ldexpl(long double , int );
__attribute__((__nothrow__)) long double logl(long double );
__attribute__((__nothrow__)) long double log10l(long double );
__attribute__((__nothrow__)) long double modfl(long double , long double * ) __attribute__((__nonnull__(2)));
__attribute__((__nothrow__)) long double powl(long double , long double );
__attribute__((__nothrow__)) long double sinl(long double );
__attribute__((__nothrow__)) long double sinhl(long double );
__attribute__((__nothrow__)) long double sqrtl(long double );
__attribute__((__nothrow__)) long double tanl(long double );
__attribute__((__nothrow__)) long double tanhl(long double );






extern __attribute__((__nothrow__)) float acoshf(float );
__attribute__((__nothrow__)) long double acoshl(long double );
extern __attribute__((__nothrow__)) float asinhf(float );
__attribute__((__nothrow__)) long double asinhl(long double );
extern __attribute__((__nothrow__)) float atanhf(float );
__attribute__((__nothrow__)) long double atanhl(long double );
__attribute__((__nothrow__)) long double copysignl(long double , long double );
extern __attribute__((__nothrow__)) float cbrtf(float );
__attribute__((__nothrow__)) long double cbrtl(long double );
extern __attribute__((__nothrow__)) float erff(float );
__attribute__((__nothrow__)) long double erfl(long double );
extern __attribute__((__nothrow__)) float erfcf(float );
__attribute__((__nothrow__)) long double erfcl(long double );
extern __attribute__((__nothrow__)) float expm1f(float );
__attribute__((__nothrow__)) long double expm1l(long double );
extern __attribute__((__nothrow__)) float log1pf(float );
__attribute__((__nothrow__)) long double log1pl(long double );
extern __attribute__((__nothrow__)) float hypotf(float , float );
__attribute__((__nothrow__)) long double hypotl(long double , long double );
extern __attribute__((__nothrow__)) float lgammaf(float );
__attribute__((__nothrow__)) long double lgammal(long double );
extern __attribute__((__nothrow__)) float remainderf(float , float );
__attribute__((__nothrow__)) long double remainderl(long double , long double );
extern __attribute__((__nothrow__)) float rintf(float );
__attribute__((__nothrow__)) long double rintl(long double );







extern __attribute__((__nothrow__)) double exp2(double );
extern __attribute__((__nothrow__)) float exp2f(float );
__attribute__((__nothrow__)) long double exp2l(long double );
extern __attribute__((__nothrow__)) double fdim(double , double );
extern __attribute__((__nothrow__)) float fdimf(float , float );
__attribute__((__nothrow__)) long double fdiml(long double , long double );
# 855 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern __attribute__((__nothrow__)) double fma(double , double , double );
extern __attribute__((__nothrow__)) float fmaf(float , float , float );

static __inline __attribute__((__nothrow__)) long double fmal(long double __x, long double __y, long double __z) { return (long double)fma((double)__x, (double)__y, (double)__z); }


extern __attribute__((__nothrow__)) __attribute__((__const__)) double fmax(double , double );
extern __attribute__((__nothrow__)) __attribute__((__const__)) float fmaxf(float , float );
__attribute__((__nothrow__)) long double fmaxl(long double , long double );
extern __attribute__((__nothrow__)) __attribute__((__const__)) double fmin(double , double );
extern __attribute__((__nothrow__)) __attribute__((__const__)) float fminf(float , float );
__attribute__((__nothrow__)) long double fminl(long double , long double );
extern __attribute__((__nothrow__)) double log2(double );
extern __attribute__((__nothrow__)) float log2f(float );
__attribute__((__nothrow__)) long double log2l(long double );
extern __attribute__((__nothrow__)) long lrint(double );
extern __attribute__((__nothrow__)) long lrintf(float );

static __inline __attribute__((__nothrow__)) long lrintl(long double __x) { return lrint((double)__x); }


extern __attribute__((__nothrow__)) long long llrint(double );
extern __attribute__((__nothrow__)) long long llrintf(float );

static __inline __attribute__((__nothrow__)) long long llrintl(long double __x) { return llrint((double)__x); }


extern __attribute__((__nothrow__)) long lround(double );
extern __attribute__((__nothrow__)) long lroundf(float );

static __inline __attribute__((__nothrow__)) long lroundl(long double __x) { return lround((double)__x); }


extern __attribute__((__nothrow__)) long long llround(double );
extern __attribute__((__nothrow__)) long long llroundf(float );

static __inline __attribute__((__nothrow__)) long long llroundl(long double __x) { return llround((double)__x); }


extern __attribute__((__nothrow__)) __attribute__((__const__)) double nan(const char * );
extern __attribute__((__nothrow__)) __attribute__((__const__)) float nanf(const char * );

static __inline __attribute__((__nothrow__)) __attribute__((__const__)) long double nanl(const char *__t) { return (long double)nan(__t); }
# 908 "C:\\Program Files\\Keil_v5\\ARM\\ARMCLANG\\bin\\..\\include\\math.h" 3
extern __attribute__((__nothrow__)) __attribute__((__const__)) double nearbyint(double );
extern __attribute__((__nothrow__)) __attribute__((__const__)) float nearbyintf(float );
__attribute__((__nothrow__)) long double nearbyintl(long double );
extern __attribute__((__nothrow__)) double remquo(double , double , int * );
extern __attribute__((__nothrow__)) float remquof(float , float , int * );

static __inline long double remquol(long double __x, long double __y, int *__q) { return (long double)remquo((double)__x, (double)__y, __q); }


extern __attribute__((__nothrow__)) __attribute__((__const__)) double round(double );
extern __attribute__((__nothrow__)) __attribute__((__const__)) float roundf(float );
__attribute__((__nothrow__)) long double roundl(long double );
extern __attribute__((__nothrow__)) double tgamma(double );
extern __attribute__((__nothrow__)) float tgammaf(float );
__attribute__((__nothrow__)) long double tgammal(long double );
extern __attribute__((__nothrow__)) __attribute__((__const__)) double trunc(double );
extern __attribute__((__nothrow__)) __attribute__((__const__)) float truncf(float );
__attribute__((__nothrow__)) long double truncl(long double );
# 6 "Source/Libs/GLCD/glcd.c" 2
# 51 "Source/Libs/GLCD/glcd.c"
typedef enum
{
    ILI9320,
    ILI9325,
    ILI9328,
    ILI9331,
    SSD1298,
    SSD1289,
    ST7781,
    LGDP4531,
    SPFD5408B,
    R61505U,
    HX8346A,
    HX8347D,
    HX8347A,
    LGDP4535,
    SSD2119
} LCD_Model;




static u8 model_code;
static _Bool initialized = 0;


static u16 lcd_orientation;
static u16 MAX_X, MAX_Y;


static LCD_Color current_bg_color = LCD_COL_BLACK;



typedef struct
{
    LCD_ObjID id;
    LCD_Obj *obj;
    _Bool visible, rendered;
} LCD_RQItem;
# 99 "Source/Libs/GLCD/glcd.c"
static LCD_RQItem render_queue[1024] = {0};
static u32 render_queue_free_list[1024] = {0};



static u32 render_queue_free_list_count;
static u32 render_queue_size;




static LCD_Font font_list[16] = {0};
static u8 font_list_size = 0;
# 121 "Source/Libs/GLCD/glcd.c"
static inline void send(u16 halfw)
{
    ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00040) )->FIODIR |= 0xFF;
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 21)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 21)));
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 19)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 19)));
    ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00040) )->FIOPIN = halfw;
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 20)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 20)));
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 20)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 20)));
    ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00040) )->FIOPIN = halfw >> 8;
}



static inline u16 recv(void)
{
    u16 value;

    ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00040) )->FIODIR &= ~(0xFF);
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 21)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 21)));
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 19)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 19)));
    for (u16 i = 30; i > 0; i--);
    value = ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00040) )->FIOPIN0;
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 19)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 19)));
    for (u16 i = 30; i > 0; i--);
    value = (value << 8) | ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00040) )->FIOPIN0;
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 21)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 21)));

    return value;
}




static void init_rw_operation_at(u16 index)
{
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 22)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 22)));
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 23)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 23)));
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 25)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 25)));
    send(index);
    for (u16 i = 22; i > 0; i--);
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 24)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 24)));
    for (u16 i = 1; i > 0; i--);
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 24)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 24)));
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 22)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 22)));
}







static void do_write(u16 data)
{
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 22)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 22)));
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 23)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 23)));
    send(data);
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 24)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 24)));
    for (u16 i = 1; i > 0; i--);
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 24)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 24)));
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 22)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 22)));
}







static u16 do_read(void)
{
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 22)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 22)));
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 23)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 23)));
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 24)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 24)));
    ((0) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 25)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 25)));
    const u16 value = recv();
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 25)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 25)));
    ((1) ? (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = (1 << 22)) : (((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOCLR = (1 << 22)));
    return value;
}




static void write_to(u16 reg, u16 data)
{
    init_rw_operation_at(reg);
    do_write(data);
}




static u16 read_from(u16 reg)
{
    init_rw_operation_at(reg);
    return do_read();
}





static void set_gram_cursor(u16 x, u16 y)
{
    if (lcd_orientation == LCD_ORIENT_HOR || lcd_orientation == LCD_ORIENT_HOR_INV)
    {

        u16 tmp = x;
        x = y;
        y = (MAX_X - 1) - tmp;
    }

    switch (model_code)
    {
    default:
        write_to(0x0020, x);
        write_to(0x0021, y);
        break;

    case SSD1298:
    case SSD1289:
        write_to(0x004e, x);
        write_to(0x004f, y);
        break;

    case HX8346A:
    case HX8347A:
    case HX8347D:
        write_to(0x02, x >> 8);
        write_to(0x03, x);

        write_to(0x06, y >> 8);
        write_to(0x07, y);
        break;

    case SSD2119:
        break;
    }
}





static void draw_line(const LCD_Line *const line)
{
    if (!line)
        return;


    LCD_Coordinate from = line->from, to = line->to;

    if (from.x > to.x)
        { u16 tmp = to.x; to.x = from.x; from.x = tmp; }
    if (from.y > to.y)
        { u16 tmp = to.y; to.y = from.y; from.y = tmp; }

    u16 tmp, dx = to.x - from.x, dy = to.y - from.y;
    if (dx == 0)
    {
        while (from.y <= to.y)
            LCD_SetPointColor(line->color, (LCD_Coordinate){from.x, from.y++});
    }
    else if (dy == 0)
    {
        while (from.x <= to.x)
            LCD_SetPointColor(line->color, (LCD_Coordinate){from.x++, from.y});
    }
    else if (dx > dy)
    {
        tmp = 2 * dy - dx;
        while (from.x != to.x)
        {
            LCD_SetPointColor(line->color, (LCD_Coordinate){from.x++, from.y});
            if (tmp > 0)
            {
                from.y++;
                tmp += 2 * dy - 2 * dx;
            }
            else
                tmp += 2 * dy;
        }
        LCD_SetPointColor(line->color, (LCD_Coordinate){from.x, from.y});
    }
    else
    {
        tmp = 2 * dx - dy;
        while (from.y != to.y)
        {
            LCD_SetPointColor(line->color, (LCD_Coordinate){from.x, from.y++});
            if (tmp > 0)
            {
                from.x++;
                tmp += 2 * dy - 2 * dx;
            }
            else
                tmp += 2 * dy;
        }
        LCD_SetPointColor(line->color, (LCD_Coordinate){from.x, from.y});
    }
}

static void draw_rect(const LCD_Rect *const rect, LCD_Coordinate pos)
{
    if (!rect)
        return;




    if (rect->width == 0 || rect->height == 0)
        return;

    LCD_Coordinate from = pos;
    LCD_Coordinate to = {from.x + rect->width, from.y + rect->height};
    if (from.x >= MAX_X || from.y >= MAX_Y)
        return;


    if (to.x >= MAX_X)
        to.x = MAX_X - 1;
    if (to.y >= MAX_Y)
        to.y = MAX_Y - 1;


    u16 tmp;
    if (from.x > to.x)
        { u16 tmp = to.x; to.x = from.x; from.x = tmp; }
    if (from.y > to.y)
        { u16 tmp = to.y; to.y = from.y; from.y = tmp; }


    LCD_Line line = {.color = rect->edge_color};

    line.from = from;
    line.to = (LCD_Coordinate){to.x, from.y};
    draw_line(&line);
    line.to = (LCD_Coordinate){from.x, to.y};
    draw_line(&line);

    line.from = to;
    line.to = (LCD_Coordinate){to.x, from.y};
    draw_line(&line);
    line.to = (LCD_Coordinate){from.x, to.y};
    draw_line(&line);

    if (rect->fill_color != LCD_COL_NONE)
    {

        for (u16 i = from.x + 1; i < to.x; i++)
            for (u16 j = from.y + 1; j < to.y; j++)
                LCD_SetPointColor(rect->fill_color, (LCD_Coordinate){i, j});
    }
}

static void draw_circle(const LCD_Circle *const circle)
{
    const LCD_Coordinate center = circle->center;
    const u16 radius = circle->radius;

    if (radius == 0)
        return;




    LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){center.x + radius, center.y});
    LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){center.x - radius, center.y});
    LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){center.x, center.y + radius});
    LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){center.x, center.y - radius});

    u16 x = radius, y = 0;
    int p = 1 - radius;




    while (x > y++)
    {





        if (p <= 0)
            p += (2 * y + 1);
        else
        {
            x--;
            p += (2 * (y - x) + 1);
        }

        if (y >= x)
            break;


        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){x + center.x, y + center.y});
        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){-x + center.x, y + center.y});
        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){x + center.x, -y + center.y});
        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){-x + center.x, -y + center.y});
        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){y + center.x, x + center.y});
        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){-y + center.x, x + center.y});
        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){y + center.x, -x + center.y});
        LCD_SetPointColor(circle->edge_color, (LCD_Coordinate){-y + center.x, -x + center.y});
    }

    if (circle->fill_color != LCD_COL_NONE)
    {
        for (u16 i = center.x - radius + 1; i < center.x + radius; i++)
            for (u16 j = center.y - radius + 1; j < center.y + radius; j++)
                if (pow((i - center.x), 2) + pow((j - center.y), 2) <= pow(radius, 2))
                    LCD_SetPointColor(circle->fill_color, (LCD_Coordinate){i, j});
    }
}

static void draw_img_rle(const LCD_Image *const img, LCD_Coordinate pos)
{
    if (!img || !img->pixels)
        return;

    u16 width = img->width, height = img->height;
    if (width == 0 || height == 0)
        return;


    if (pos.x >= MAX_X || pos.y >= MAX_Y)
        return;


    if (pos.x + width > MAX_X)
        width = MAX_X - pos.x;
    if (pos.y + height > MAX_Y)
        height = MAX_Y - pos.y;

    const u32 *pixels_copy = img->pixels;
    u32 pixel_data, rgb888;

    u8 alpha_data;
    u16 current_x, pixels_left, count;
    for (u16 i = 0; i < height; i++)
    {
        current_x = pos.x;
        pixels_left = width;

        while (pixels_left > 0)
        {
            count = *pixels_copy++;
            pixel_data = *pixels_copy++;


            for (u16 j = 0; j < count && pixels_left > 0; j++)
            {
                if (img->has_alpha)
                {

                    alpha_data = (u8)(pixel_data >> 24);
                    rgb888 = (u32)(pixel_data & 0x00FFFFFF);

                    if (alpha_data)
                        LCD_SetPointColor(rgb888_to_rgb565(rgb888), (LCD_Coordinate){current_x, pos.y + i});

                    current_x++;
                }
                else
                {

                    rgb888 = (u32)(pixel_data & 0x00FFFFFF);
                    LCD_SetPointColor(rgb888_to_rgb565(rgb888), (LCD_Coordinate){current_x++, pos.y + i});
                }

                pixels_left--;
            }
        }
    }
}




static i16 print_char(u8 chr, const LCD_Font *const font, const LCD_Coordinate *const where, LCD_Color txt_col,
                        LCD_Color bg_col)
{

    if (!font || !where || chr < 32 || chr > 126)
        return -1;

    if (where->x >= MAX_X || where->y >= MAX_Y)
        return -1;







    const u32 *font_data_copy = font->data;
    font_data_copy += (chr - 32) * font->char_height;

    u32 *const chr_data = (u32 *)(__builtin_alloca(font->char_height * sizeof(u32)));
    memcpy(chr_data, font_data_copy, font->char_height * sizeof(u32));

    u32 value;
    for (u16 i = 0; i < font->char_height; i++)
    {
        for (u16 j = 0; j < font->max_char_width; j++)
        {



            value = chr_data[i] & (0xFFFFFFFF >> (32 - font->max_char_width));


            if ((value >> (font->max_char_width - 1 - j)) & 0x1)
                LCD_SetPointColor(txt_col, (LCD_Coordinate){where->x + j, where->y + i});
            else
                LCD_SetPointColor(bg_col, (LCD_Coordinate){where->x + j, where->y + i});
        }
    }







    return (font->char_widths) ? font->char_widths[chr - 32] : font->max_char_width;
}

static void print_text(const LCD_Text *const text, LCD_Coordinate pos)
{
    if (!text || !text->text)
        return;

    const LCD_FontID font_id = text->font;
    if (font_id >= font_list_size)
        return;

    const LCD_Font font = font_list[font_id];
    if (!font.data)
        return;

    if (pos.x >= MAX_X || pos.y >= MAX_Y)
        return;

    u8 chr, *str = (u8 *)(text->text);

    u16 max_width = 0;
    u16 start_x = pos.x;
    u16 total_height = font.char_height;

    i16 current_char_width;
    _Bool no_more_space = 0;
    while ((chr = *str++) && !no_more_space)
    {
        if ((current_char_width = print_char(chr, &font, &pos, text->text_color, text->bg_color)) < 0)
            return;


        if (pos.x + current_char_width < MAX_X)
            pos.x += current_char_width;
        else if (pos.y + font.char_height < MAX_Y)
        {
            pos.x = 0;
            pos.y += font.char_height;
            total_height += font.char_height;
        }
        else
            no_more_space = 1;


        u16 current_width = pos.x - start_x;
        if (current_width > max_width)
            max_width = current_width;
    }


    if (max_width == 0)
        max_width = pos.x - start_x;
}



static void render_item(LCD_RQItem *const item)
{
    if (!item || !item->obj || item->id < 0 || !item->obj->comps || item->obj->comps_size == 0)
        return;


    if (item->rendered || !item->visible)
        return;

    LCD_Component *comp;
    for (u16 i = 0; i < item->obj->comps_size; i++)
    {
        comp = &item->obj->comps[i];
        switch (comp->type)
        {
        case LCD_COMP_LINE:
            draw_line(&comp->object.line);
            break;
        case LCD_COMP_RECT:
            draw_rect(&comp->object.rect, comp->pos);
            break;
        case LCD_COMP_CIRCLE:
            draw_circle(&comp->object.circle);
            break;
        case LCD_COMP_IMAGE:
            draw_img_rle(&comp->object.image, comp->pos);
            break;
        case LCD_COMP_TEXT:
            print_text(&comp->object.text, comp->pos);
            break;
        }
    }

    item->rendered = 1;
}

static void unrender_item(LCD_RQItem *const item)
{
    if (!item || !item->obj || item->id < 0 || !item->obj->comps || item->obj->comps_size == 0)
        return;


    if (!item->rendered)
        return;

    LCD_Component *comp;
    for (u16 i = 0; i < item->obj->comps_size; i++)
    {
        comp = &item->obj->comps[i];
        if (comp->type == LCD_COMP_LINE)
        {
            LCD_Line line = comp->object.line;
            line.color = current_bg_color;
            draw_line(&line);
        }
        else if (comp->type == LCD_COMP_RECT)
        {
            LCD_Rect rect = comp->object.rect;
            rect.edge_color = current_bg_color;
            if (rect.fill_color != LCD_COL_NONE)
                rect.fill_color = current_bg_color;

            draw_rect(&rect, comp->pos);
        }
        else if (comp->type == LCD_COMP_CIRCLE)
        {
            LCD_Circle circle = comp->object.circle;
            circle.edge_color = current_bg_color;
            if (circle.fill_color != LCD_COL_NONE)
                circle.fill_color = current_bg_color;

            draw_circle(&circle);
        }
        else if (comp->type == LCD_COMP_IMAGE)
        {
            const LCD_Rect rect = {
                .width = comp->object.image.width,
                .height = comp->object.image.height,
                .fill_color = current_bg_color,
                .edge_color = current_bg_color,
            };

            draw_rect(&rect, comp->pos);
        }
        else if (comp->type == LCD_COMP_TEXT)
        {
            LCD_Text text = comp->object.text;
            text.text_color = current_bg_color;
            text.bg_color = current_bg_color;

            print_text(&text, comp->pos);
        }
    }

    item->rendered = 0;
}



# 1 "./Source/Libs/GLCD/Fonts\\font_msgothic.h" 1
# 25 "./Source/Libs/GLCD/Fonts\\font_msgothic.h"
const u32 Font_MSGothicData[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
    0x36, 0x24, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x24, 0x24, 0x24, 0x24, 0xFE, 0x48, 0x48, 0x48, 0x48, 0xFC, 0x48, 0x48, 0x48, 0x48, 0x00,
    0x10, 0x38, 0x54, 0x92, 0x92, 0x50, 0x30, 0x18, 0x14, 0x12, 0x92, 0x92, 0x54, 0x38, 0x10, 0x00,
    0x00, 0x62, 0x92, 0x94, 0x94, 0x68, 0x08, 0x10, 0x20, 0x2C, 0x52, 0x52, 0x92, 0x8C, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x48, 0x48, 0x48, 0x30, 0x20, 0x54, 0x94, 0x88, 0x88, 0x94, 0x62, 0x00, 0x00,
    0x30, 0x30, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x08, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x08, 0x04, 0x00,
    0x40, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x40, 0x00,
    0x00, 0x00, 0x00, 0x10, 0x92, 0x54, 0x38, 0x10, 0x38, 0x54, 0x92, 0x10, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x10, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00,
    0x00, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x48, 0x30, 0x00, 0x00,
    0x00, 0x10, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x84, 0x84, 0x04, 0x08, 0x08, 0x10, 0x20, 0x20, 0x40, 0x80, 0xFC, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x84, 0x84, 0x04, 0x08, 0x30, 0x08, 0x04, 0x84, 0x84, 0x48, 0x30, 0x00, 0x00,
    0x00, 0x08, 0x08, 0x18, 0x18, 0x28, 0x28, 0x48, 0x48, 0x88, 0xFC, 0x08, 0x08, 0x08, 0x00, 0x00,
    0x00, 0xFC, 0x80, 0x80, 0x80, 0xB0, 0xC8, 0x84, 0x04, 0x04, 0x04, 0x84, 0x48, 0x30, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x84, 0x84, 0x80, 0xB0, 0xC8, 0x84, 0x84, 0x84, 0x84, 0x48, 0x30, 0x00, 0x00,
    0x00, 0xFC, 0x04, 0x04, 0x08, 0x08, 0x08, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x84, 0x84, 0x84, 0x48, 0x30, 0x48, 0x84, 0x84, 0x84, 0x48, 0x30, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x84, 0x84, 0x84, 0x84, 0x4C, 0x34, 0x04, 0x84, 0x84, 0x48, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x10, 0x20, 0x00,
    0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x48, 0x84, 0x84, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00,
    0x00, 0x38, 0x44, 0x82, 0x9A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x9C, 0x80, 0x42, 0x3C, 0x00, 0x00,
    0x00, 0x10, 0x10, 0x28, 0x28, 0x28, 0x28, 0x44, 0x44, 0x44, 0x7C, 0x82, 0x82, 0x82, 0x00, 0x00,
    0x00, 0xF8, 0x84, 0x82, 0x82, 0x82, 0x84, 0xF8, 0x84, 0x82, 0x82, 0x82, 0x84, 0xF8, 0x00, 0x00,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x80, 0x80, 0x80, 0x80, 0x80, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00,
    0x00, 0xF8, 0x84, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x84, 0xF8, 0x00, 0x00,
    0x00, 0xFE, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFC, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFE, 0x00, 0x00,
    0x00, 0xFE, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFC, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x80, 0x80, 0x80, 0x8E, 0x82, 0x82, 0x82, 0x46, 0x3A, 0x00, 0x00,
    0x00, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0xFE, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00,
    0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00,
    0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x84, 0x84, 0x48, 0x30, 0x00, 0x00,
    0x00, 0x82, 0x84, 0x84, 0x88, 0x90, 0x90, 0xA0, 0xD0, 0x88, 0x88, 0x84, 0x82, 0x82, 0x00, 0x00,
    0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFE, 0x00, 0x00,
    0x00, 0x82, 0x82, 0xC6, 0xC6, 0xC6, 0xC6, 0xAA, 0xAA, 0xAA, 0xAA, 0x92, 0x92, 0x92, 0x00, 0x00,
    0x00, 0x82, 0x82, 0xC2, 0xC2, 0xA2, 0xA2, 0x92, 0x92, 0x8A, 0x8A, 0x86, 0x86, 0x82, 0x00, 0x00,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00,
    0x00, 0xF8, 0x84, 0x82, 0x82, 0x82, 0x84, 0xF8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x92, 0x8A, 0x44, 0x3A, 0x00, 0x00,
    0x00, 0xF8, 0x84, 0x82, 0x82, 0x82, 0x84, 0xF8, 0x88, 0x88, 0x84, 0x84, 0x82, 0x82, 0x00, 0x00,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x80, 0x60, 0x18, 0x04, 0x02, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00,
    0x00, 0xFE, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00,
    0x00, 0x82, 0x82, 0x82, 0x44, 0x44, 0x44, 0x44, 0x28, 0x28, 0x28, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x92, 0x92, 0x92, 0x92, 0xAA, 0xAA, 0xAA, 0xAA, 0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x00,
    0x00, 0x82, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x28, 0x28, 0x44, 0x44, 0x82, 0x82, 0x00, 0x00,
    0x00, 0x82, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0xFE, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0xFE, 0x00, 0x00,
    0x7C, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x00,
    0x00, 0x82, 0x82, 0x44, 0x44, 0x28, 0x28, 0x7C, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x7C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x7C, 0x00,
    0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0x30, 0x30, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x84, 0x04, 0x3C, 0x44, 0x84, 0x8C, 0x76, 0x00, 0x00,
    0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xB8, 0xC4, 0x82, 0x82, 0x82, 0x82, 0xC4, 0xB8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x80, 0x80, 0x80, 0x80, 0x42, 0x3C, 0x00, 0x00,
    0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x3A, 0x46, 0x82, 0x82, 0x82, 0x82, 0x46, 0x3A, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0xFE, 0x80, 0x80, 0x42, 0x3C, 0x00, 0x00,
    0x00, 0x18, 0x20, 0x20, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x44, 0x44, 0x38, 0x40, 0x7C, 0x82, 0x82, 0x7C, 0x00,
    0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xB8, 0xC4, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x60, 0x00,
    0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x84, 0x88, 0x90, 0xA0, 0xD0, 0x88, 0x84, 0x82, 0x00, 0x00,
    0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 0xD2, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB8, 0xC4, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB8, 0xC4, 0x82, 0x82, 0x82, 0xC4, 0xB8, 0x80, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x46, 0x82, 0x82, 0x82, 0x46, 0x3A, 0x02, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x82, 0x80, 0x60, 0x1C, 0x02, 0x82, 0x7C, 0x00, 0x00,
    0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x46, 0x3A, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x92, 0x92, 0xAA, 0xAA, 0x44, 0x44, 0x44, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x44, 0x28, 0x10, 0x10, 0x28, 0x44, 0x82, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10, 0x20, 0xC0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xFE, 0x00, 0x00,
    0x1C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1C, 0x00,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x70, 0x00,
    0x64, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};



const LCD_Font Font_MSGothic = {
    .data = Font_MSGothicData,
    .data_size = 95,
    .char_widths = 0,
    .max_char_width = 8,
    .char_height = 16,
};
# 704 "Source/Libs/GLCD/glcd.c" 2
# 1 "./Source/Libs/GLCD/Fonts\\font_system.h" 1
# 17 "./Source/Libs/GLCD/Fonts\\font_system.h"
const u32 Font_SystemData[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3C, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x36, 0x36, 0x7F, 0x36, 0x36, 0x36, 0x7F, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x18, 0x3C, 0x66, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x66, 0x3C, 0x18, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x70, 0xD8, 0xDA, 0x76, 0x0C, 0x18, 0x30, 0x6E, 0x5B, 0x1B, 0x0E, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x38, 0x6C, 0x6C, 0x38, 0x60, 0x6F, 0x66, 0x66, 0x3B, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0C, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x18, 0x0C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x18, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x18, 0x18, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x1C, 0x7F, 0x1C, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x0C, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x06, 0x0C, 0x0C, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1E, 0x33, 0x37, 0x37, 0x33, 0x3B, 0x3B, 0x33, 0x1E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0C, 0x1C, 0x7C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x30, 0x36, 0x36, 0x36, 0x66, 0x7F, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7E, 0x60, 0x60, 0x60, 0x7C, 0x06, 0x06, 0x0C, 0x78, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1C, 0x18, 0x30, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7E, 0x06, 0x0C, 0x0C, 0x18, 0x18, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x76, 0x3C, 0x6E, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x0C, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x1C, 0x1C, 0x0C, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7E, 0xC3, 0xC3, 0xCF, 0xDB, 0xDB, 0xCF, 0xC0, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3C, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x60, 0x60, 0x60, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x78, 0x6C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7E, 0x60, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7E, 0x60, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x60, 0x60, 0x6E, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x6C, 0x6C, 0x78, 0x6C, 0x6C, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x63, 0x63, 0x77, 0x6B, 0x6B, 0x6B, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x63, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0C, 0x06, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x66, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x63, 0x63, 0x63, 0x6B, 0x6B, 0x6B, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x34, 0x18, 0x18, 0x2C, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7E, 0x06, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0C, 0x0C, 0x06, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00,
    0x00, 0x18, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x7E, 0x60, 0x60, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1E, 0x30, 0x30, 0x30, 0x7E, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x7C, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0C, 0x0C, 0x00, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x78, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x60, 0x66, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x6B, 0x6B, 0x6B, 0x6B, 0x6B, 0x63, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x06, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x6E, 0x70, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x60, 0x60, 0x3C, 0x06, 0x06, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x30, 0x7E, 0x30, 0x30, 0x30, 0x30, 0x30, 0x1E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x6B, 0x6B, 0x6B, 0x6B, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x0C, 0x18, 0xF0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0C, 0x18, 0x18, 0x18, 0x30, 0x60, 0x30, 0x18, 0x18, 0x18, 0x0C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x71, 0xDB, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};



const LCD_Font Font_System = {
    .data = Font_SystemData,
    .data_size = 95,
    .char_widths = 0,
    .max_char_width = 8,
    .char_height = 16,
};
# 705 "Source/Libs/GLCD/glcd.c" 2

void LCD_Init(LCD_Orientation orientation)
{


    ((LPC_PINCON_TypeDef *) ((0x40000000UL) + 0x2C000) )->PINSEL1 &= ~(0x03FFFFC0);



    ((LPC_PINCON_TypeDef *) ((0x40000000UL) + 0x2C000) )->PINSEL4 &= ~(0x0000FFFF);



    ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIODIR |= 0x03f80000;
    ((LPC_GPIO_TypeDef *) ((0x2009C000UL) + 0x00000) )->FIOSET = 0x03f80000;

    lcd_orientation = orientation;
    if (orientation == LCD_ORIENT_VER || orientation == LCD_ORIENT_VER_INV)
    {
        MAX_X = 240;
        MAX_Y = 320;
    }
    else
    {
        MAX_X = 320;
        MAX_Y = 240;
    }

    for (u16 i = 0; i < 100; i++) for (u16 j = 0; j < 1141; j++);
    const u16 code = read_from(0x0000);
    if (code == 0x9325 || code == 0x9328)
    {
        model_code = ILI9325;
        write_to(0x00e7, 0x0010);
        write_to(0x0000, 0x0001);
        write_to(0x0001, 0x0100);

        write_to(0x0002, 0x0700);
        write_to(0x0003, (1 << 12) | (1 << 5) | (1 << 4) | (0 << 3));
        write_to(0x0004, 0x0000);
        write_to(0x0008, 0x0207);
        write_to(0x0009, 0x0000);
        write_to(0x000a, 0x0000);
        write_to(0x000c, 0x0001);
        write_to(0x000d, 0x0000);
        write_to(0x000f, 0x0000);


        write_to(0x0010, 0x0000);
        write_to(0x0011, 0x0007);
        write_to(0x0012, 0x0000);
        write_to(0x0013, 0x0000);
        for (u16 i = 0; i < 50; i++) for (u16 j = 0; j < 1141; j++);
        write_to(0x0010, 0x1590);
        write_to(0x0011, 0x0227);
        for (u16 i = 0; i < 50; i++) for (u16 j = 0; j < 1141; j++);
        write_to(0x0012, 0x009c);
        for (u16 i = 0; i < 50; i++) for (u16 j = 0; j < 1141; j++);
        write_to(0x0013, 0x1900);
        write_to(0x0029, 0x0023);

        write_to(0x002b, 0x000e);
        for (u16 i = 0; i < 50; i++) for (u16 j = 0; j < 1141; j++);
        write_to(0x0020, 0x0000);
        write_to(0x0021, 0x0000);

        for (u16 i = 0; i < 50; i++) for (u16 j = 0; j < 1141; j++);
        write_to(0x0030, 0x0007);
        write_to(0x0031, 0x0707);
        write_to(0x0032, 0x0006);
        write_to(0x0035, 0x0704);
        write_to(0x0036, 0x1f04);
        write_to(0x0037, 0x0004);
        write_to(0x0038, 0x0000);
        write_to(0x0039, 0x0706);
        write_to(0x003c, 0x0701);
        write_to(0x003d, 0x000f);
        for (u16 i = 0; i < 50; i++) for (u16 j = 0; j < 1141; j++);
        write_to(0x0050, 0x0000);
        write_to(0x0051, 0x00ef);
        write_to(0x0052, 0x0000);
        write_to(0x0053, 0x013f);
        write_to(0x0060, 0xa700);
        write_to(0x0061, 0x0001);
        write_to(0x006a, 0x0000);
        write_to(0x0080, 0x0000);
        write_to(0x0081, 0x0000);
        write_to(0x0082, 0x0000);
        write_to(0x0083, 0x0000);
        write_to(0x0084, 0x0000);
        write_to(0x0085, 0x0000);
        write_to(0x0090, 0x0010);
        write_to(0x0092, 0x0000);
        write_to(0x0093, 0x0003);
        write_to(0x0095, 0x0110);
        write_to(0x0097, 0x0000);
        write_to(0x0098, 0x0000);


        write_to(0x0007, 0x0133);
        write_to(0x0020, 0x0000);
        write_to(0x0021, 0x0000);
    }

    for (u16 i = 0; i < 50; i++) for (u16 j = 0; j < 1141; j++);


    font_list[0] = Font_MSGothic;
    font_list[1] = Font_System;
    font_list_size = 2;


    render_queue_free_list_count = 1024;
    render_queue_size = 0;
# 827 "Source/Libs/GLCD/glcd.c"
    for (u32 i = 0; i < 1024; i++)
        render_queue_free_list[i] = 1024 - 1 - i;

    initialized = 1;
}

_Bool LCD_IsInitialized(void)
{
    return initialized;
}

u16 LCD_GetWidth(void)
{
    return MAX_X;
}

u16 LCD_GetHeight(void)
{
    return MAX_Y;
}

LCD_Coordinate LCD_GetSize(void)
{
    return (LCD_Coordinate){MAX_X, MAX_Y};
}

LCD_Coordinate LCD_GetCenter(void)
{
    return (LCD_Coordinate){MAX_X / 2, MAX_Y / 2};
}

LCD_Color LCD_GetPointColor(LCD_Coordinate point)
{
    LCD_Color dummy;

    set_gram_cursor(point.x, point.y);
    init_rw_operation_at(0x0022);

    switch (model_code)
    {
    case ST7781:
    case LGDP4531:
    case LGDP4535:
    case SSD1289:
    case SSD1298:
        dummy = do_read();
        dummy = do_read();
        return dummy;
    case HX8347A:
    case HX8347D: {
        dummy = do_read();

        const u8 red = do_read() >> 3;
        const u8 green = do_read() >> 2;
        const u8 blue = do_read() >> 3;
        dummy = (u16)((red << 11) | (green << 5) | blue);
        return dummy;
    }
    default:
        dummy = do_read();
        dummy = do_read();
        return (u16)(((dummy >> 11) & 0x1F) | ((dummy >> 5) & 0x3F) | (dummy & 0x1F));
    }
}

void LCD_SetPointColor(LCD_Color color, LCD_Coordinate point)
{
    if (point.x >= MAX_X || point.y >= MAX_Y)
        return;

    set_gram_cursor(point.x, point.y);
    if (color != LCD_COL_NONE)
        write_to(0x0022, color & 0xFFFF);
}

void LCD_SetBackgroundColor(LCD_Color color)
{
    if (color == LCD_COL_NONE)
        return;

    current_bg_color = color;


    if (model_code == HX8347D || model_code == HX8347A)
    {
        write_to(0x02, 0x00);
        write_to(0x03, 0x00);
        write_to(0x04, 0x00);
        write_to(0x05, 0xEF);
        write_to(0x06, 0x00);
        write_to(0x07, 0x00);
        write_to(0x08, 0x01);
        write_to(0x09, 0x3F);
    }
    else
        set_gram_cursor(0, 0);

    init_rw_operation_at(0x0022);
    for (u32 index = 0; index < MAX_X * MAX_Y; index++)
        do_write(color & 0xFFFF);




    LCD_RQItem *item;
    for (u32 i = 0, count = 0; i < 1024 && count != render_queue_size; i++)
    {
        item = &render_queue[i];
        if (item->id == -1 && !item->obj)
            continue;

        count++;
        item->rendered = 0;
    }


    LCD_RQRender();
}



LCD_ObjID LCD_RQAddObject(const LCD_Obj *const obj)
{
    if (!obj || !obj->comps || obj->comps_size <= 0)
        return -1;


    if (render_queue_free_list_count <= 0)
        return -1;

    const u32 rq_slot = render_queue_free_list[--render_queue_free_list_count];
    LCD_RQItem *item = &render_queue[rq_slot];

    item->id = rq_slot;
    item->obj = (LCD_Obj *)obj;
    item->visible = 1;
    item->rendered = 0;

    render_queue_size++;
    return rq_slot;
}

void LCD_RQRender(void)
{







    LCD_RQItem *item;
    for (u32 i = 0, count = 0; i < 1024 && count != render_queue_size; i++)
    {
        item = &render_queue[i];
        if (item->id == -1 && !item->obj)
            continue;

        count++;
        if (item->visible && !item->rendered)
            render_item(item);
    }
}

void LCD_RQRemoveObject(LCD_ObjID id, _Bool redraw_screen)
{
    if (id < 0 || id >= 1024)
        return;


    LCD_RQItem *const item = &render_queue[id];
    if (!item || item->id == -1 || !item->obj)
        return;

    item->visible = 0;
    unrender_item(item);


    item->id = -1;
    item->obj = 0;


    render_queue_free_list[render_queue_free_list_count++] = id;
    render_queue_size--;

    if (redraw_screen)
        LCD_RQRender();
}

void LCD_RQSetObjectVisibility(LCD_ObjID id, _Bool visible, _Bool redraw_screen)
{
    if (id < 0 || id >= 1024)
        return;

    LCD_RQItem *const item = &render_queue[id];
    if (!item || item->id == -1 || !item->obj)
        return;

    item->visible = visible;
    if (visible && !item->rendered)
        render_item(item);
    else if (!visible && item->rendered)
    {
        unrender_item(item);
        if (redraw_screen)
            LCD_RQRender();
    }
}

void LCD_RQClear(void)
{



    LCD_RQItem *item;
    for (u32 i = 0, count = 0; i < 1024 && count != render_queue_size; i++)
    {
        item = &render_queue[i];
        if (item->id == -1 || !item->obj)
            continue;

        item->id = -1;
        item->obj = 0;
        item->rendered = 0;
        item->visible = 0;

        count++;
        render_queue_free_list[render_queue_free_list_count++] = i;
    }

    render_queue_size = 0;
    LCD_SetBackgroundColor(current_bg_color);
}

LCD_FontID LCD_FMAddFont(LCD_Font font)
{
    if (font_list_size >= 16)
        return -1;

    LCD_FontID id = font_list_size;
    font_list[id] = font;
    font_list_size++;
    return id;
}

void LCD_FMRemoveFont(LCD_FontID id)
{
    if (id >= font_list_size || id <= 1)
        return;

    for (u32 i = id; i < font_list_size - 1; i++)
        font_list[i] = font_list[i + 1];
    font_list_size--;
}
