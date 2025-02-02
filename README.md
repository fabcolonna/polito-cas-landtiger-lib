# Custom Peripherals Library for the LandTiger LPC1768 Board

[![Documentation](https://img.shields.io/badge/docs-GitHub%20Pages-blue)](https://fabcolonna.github.io/polito-cas-landtiger-lib/)

![screenshot](./Assets/README/screenshot.png)

## What's this?

This is a Keil uVision 5 project for developing a **custom C interface for every peripheral of the LandTiger LPC1768** board, introduced during the course of **Computer Architectures**, taught in the MSc Computer Engineering program at Politecnico di Torino. Alongside the peripherals' drivers, the library also provides an implementation of a simple **memory allocator** based on a user-allocated memory pool, in which the library allocates memory required for its functionalities. This is done to let the user decide *how much* memory to allocate, and *where* to allocate it (i.e. on the heap, in the ZI space, or really anywhere you can declare an array of bytes), based on the application's needs and the available memory.

 Why did I create this? Mainly because I wanted to deeply understand how complex things that we take for granted, such as drawing on the screen or playing audio, work at the bare metal level. At the same time, I wanted to provide a more abstract and powerful interface for the user, so that they can focus on the application logic, rather than on the low-level details.

The interface is distributed as a **library** and it's available in the *Releases* section of this repository. It's meant to be included in your Keil projects (see below for instructions on how to do this).

I'd like to emphasize that this project is still in development, and I'm currently alone, so you most probably will encounter bugs or unexpected behavior. I'm doing my best to maintain it, but I'm also a student, so I have to balance this with my other commitments. If you find a bug, please **open an issue**!

### Repo structure

- `Keil/`: contains the Keil project;
  - `Include`: contains the header files accessible by the user;
  - `Source`: contains the source code:
    - `CMSIS_Core`: CMSIS (Cortex Microcontroller Software Interface Standard) core files;
    - `Lib`: contains the library's source code:
      - `Peripherals`: contains the drivers for the peripherals, like the GLCD, the DAC, etc.;
      - `Allocator`: contains the source code for the memory allocator;
      - `Collections`: contains the source code for some data structures used by the library, which work in conjunction with the memory allocator. They've been used to implement the lower level details of some drivers, but I decided to keep them accessible in case the user needs them;
      - `PRNG`: contains the source code for a pseudo-random number generator, which again can be used by the user if needed.
    - `System`: contains the system startup code for the LPC1768 board;
- `Scripts/`: contains the Python scripts used to generate bitmaps from images and fonts (more on this later).

### Keil project targets

The `lib.uvprojx` file is the Keil project file, and it defines three targets:

- `library`: compiles the library without the testing folders, and produces the library in the `BigLib/` folder in the root directory of the repo;
- `test-sim`: produces an executable from every source file in the Source Group, and runs it using the simulator;
- `test-dev`: produces an executable from every source file in the Source Group, and runs it on the physical board.

The last two targets are meant for testing and debugging.

## Current status of the project

The library currently implements drivers for the following peripherals, with varying degrees of completeness:

- [X] Buttons (with optional debouncing);
- [X] Timers;
- [X] RIT, with variable timed jobs;
- [X] Power Management;
- [X] LEDs;
- [X] Joystick, with optional RIT-based polling, but no support for prolonged actions;
- [X] ADC, for potentiometer support (only one channel);
- [X] DAC Tone Generator with buzzer (very basic, only sinusoidal tones);
- [X] GLCD & TouchScreen for the ADS7843 controller;
- [ ] CAN Bus.

### Memory

The library uses a **memory allocator** to manage the memory required for its functionalities. I suggest you to allocate some memory for it as the first thing in you `main()` function. The following code shows how to allocate memory from the static space:

```c
#include "includes.h"

#define MEM_POOL_SIZE 4096
static _MEM_POOL_ALIGN4(pool, MEM_POOL_SIZE);

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER, NULL);
    LCD_SetBackgroundColor(LCD_COL_BLACK);

    // Allocate memory for the library
    MEM_Allocator *alloc = MEM_Init(mem_pool, MEM_POOL_SIZE);

    // Your code here...

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
```

The `MEM_POOL_ALIGN4` macro is used to align the memory pool to a 4-byte boundary, as the allocator **requires** it. The `MEM_Init` function initializes the allocator, and returns a pointer to the `MEM_Allocator` structure, which is used to allocate and deallocate memory.

Once you have the allocator, you can use it to allocate space for the RIT jobs list, the GLCD render list, the collections, and so on. **Everything related to memory** in this library depends on this allocator, so it's crucial to initialize it.

### GLCD

I put *a lot* of effort into the GLCD drivier, as it's certainly one of the most complex. In fact, it must be flexible enough to support different actions that the user could find useful (like drawing images, text, etc.), but also simple enough to be used without too much hassle.

The current status of the druver supports **components**, like text, images and shapes, **object** rendering and manipulation (an object is a set of components that are treated like a single, big, component). These stuff can be declared using a custom-made syntax that tries to be declarative and intuitive.

The driver uses a render list to keep track of the objects that are currently being rendered on the screen, and it provides a set of functions to manipulate it.

#### How to render stuff

First of all, you need to initialize the GLCD. Note that the LCD must be initialized before doing anything display-oriented (even the touch controller requires it, as it uses the LCD to draw the calibration interface!).

To display objects on the GLCD, you can use the various functions and macros provided by the interface. Head over to the documentation to see the full list of functionalities.

**Components** are the basic building blocks of objects. While the library allows you to declare component directly using the type `LCD_Component`, I highly recommend using the provided macros - declared in the `glcd_macros.h` file, that hide the complexity of the structure, making the code **way more readable**.

A component cannot be rendered on the screen by itself. It must be part of an **object**, which is a collection of components (or only one). Declaring an object is not trivial, as it requires the user to define the various components in it, etc. To avoid problems of these kind, and maintain a clean code, I introduced a set of macros that simplify the process of defining, and rendering them:

- `LCD_OBJECT_DEFINE(...)`: Declares an object and returns its `LCD_Obj` structure, without adding it to the render list;
- `LCD_OBJECT(id, ...)`: Adds an object to the render list, returns its id in the provided variable, and returns the error code of type `LCD_Error` (if any). Objects added like these are **immediately visible**, hence they will be rendered on the screen as soon as the rendering function is called;
- `LCD_INVISIBLE_OBJECT(id, ...)`: Same as `LCD_OBJECT`, but the object is **invisible** by default, and it must be made visible using the `LCD_RMSetVisibility` function;

#### `main()` example: drawing a cross

This is the code for drawing a simple cross on the screen:

  ```c
#include "includes.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER, NULL);
    LCD_SetBackgroundColor(LCD_COL_BLACK, false);

    LCD_ObjID cross_id;
    LCD_Error error;

    const u16 x = LCD_GetWidth() / 2;
    const u16 y = LCD_GetHeight() / 2;

    error = LCD_OBJECT(cross, {
        LCD_RECT2(x - 15, y - 15, {
            .width = 30,
            .height = 30,
            .edge_color = LCD_COL_WHITE,
            .fill_color = LCD_COL_NONE,
        }),
        LCD_LINE({
            .from = {x, y - 7},
            .to = {x, y + 7},
            .color = LCD_COL_WHITE,
        }),
        LCD_LINE({
            .from = {x - 7, y},
            .to = {x + 7, y},
            .color = LCD_COL_WHITE
        }),
    });

    if (error != LCD_ERR_NONE)
    {
        // Handle the error
    }

    LCD_RMRender(); // Renders the cross object

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
  ```

This produces the following output:

![Calibration Cross](./Assets/README/cross.png)

>**Note**: If you need to render objects that are not meant to be removed from the screen selectively, you can avoid the overhead caused by the render list logic by simply drawing them directly on the screen using the `LCD_RENDER_TMP` macro, which does not allocate anything and does not return an `LCD_ObjID`. Objects of this type cannot be managed by any render list function. If you wish to delete them, you need to redraw the screen by calling the `LCD_SetBackgroundColor(LCD_Color color, true)` function. The `true` argument tells the function to redraw the screen, effectively deleting all the objects that are part of the render list (i.e. only the objects that have been added using the `LCD_OBJECT` or `LCD_INVISIBLE_OBJECT` macros). This rendering method is useful if you don't need to control the object during its lifetime, you just want to draw it and forget about it.

### GLCD in depth: custom fonts

The GLCD has 2 built-in fonts that can be used to render text, `LCD_DEF_FONT_SYSTEM` and `LCD_DEF_FONT_MSGOTHIC`. The problem with these fonts is that the user has no control over the characters' size, and there might be cases where it's necessary to draw smaller or bigger text. That's why I introduced the possibility to load **custom fonts**, with **custom sizes**.

Before diving into the details, let's talk about limitation. As of now, you can load up to **16** different font in the GLCD, and each font can only have **ASCII characters**. This may change in the future, but for now, this is how it is. This parameter is user-configurable, though, and can be changed in the `glcd_config.h` file.

#### How to load custom fonts

That's not as straightforward as it may seem, and that's because, like I said before, the current implementation **does not have support for file-system**, and this prevents us from reading font files directly. The only ability we have (for now!) is to load stuff as C buffers, and that's what we're going to do.

As the name suggests, the Python script `ttf2c.py` converts a TrueType font file (`.ttf`) into a C buffer that can be included in the project and used by the library's functions. It accepts a size, so multiple arrays can be generated for different sizes of the same font. After running it, you'll get a output file containing the following:

- An array containing the font data (in terms of pixels to be drawn for each character);
- An array containing the individual characters' width, that is used to correctly space the single chars while rendering a string;
- A structure of type `LCD_Font`, which contains the font's metadata, like the width and height of the characters, and a pointer to the arrays described above. This is **the only thing that the user needs** to use explicitly in the project.

Here's an example of the structure described above, for the Pixellari font (size 16):

```c
const LCD_Font Font_Pixellari16 = {
    .data = Font_Pixellari16Data,
    .data_size = 95,
    .char_widths = Font_Pixellari16CharWidths,
    .max_char_width = 14,
    .char_height = 14,
};
```

You don't need to worry about any of this stuff, because loading the font is as simple as:

- Moving the output file to an included directory (by included I mean a directory that is tracked by the compiler);
- Including it in the source file where you want to use it;
- Calling the `LCD_FMAddFont` function, with the name of the generated structure as argument.

This function returns the font's ID, which you can use to render text with that font (the `.font` field of the `LCD_Text` structure), and to unload it when you don't need it anymore, with the `LCD_FMRemoveFont` function.

#### `main()` example: `pixellari.ttf`

Below is an example of how to load a bitmap font (Pixellari) downloaded from [DaFont](https://www.dafont.com/) and use it to display a string:

```c
#include "includes.h"
#include "font_pixellari16.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    LCD_SetBackgroundColor(LCD_COL_BLACK);

    const LCD_Coordinate center = LCD_GetCenter();
    const LCD_FontID pix16_fid = LCD_FMAddFont(Font_Pixellari16);

    LCD_OBJECT(text_obj, {
        LCD_TEXT2(center.x - 70, center.y - 15, {
            .text = "This is Pixellari 16!",
            .text_color = LCD_COL_RED,
            .bg_color = LCD_COL_NONE,
            .font = pix16_fid,
        }),
        LCD_TEXT2(center.x - 60, center.y + 15, {
            .text = "This is System!",
            .text_color = LCD_COL_GREEN,
            .bg_color = LCD_COL_NONE,
            .font = LCD_DEF_FONT_SYSTEM,
        }),
    });

    LCD_RMRender();                 // Renders the text object
    LCD_FMRemoveFont(pix16_fid);    // Unloads the Pixellari 16 font

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
```

This produces the following output:

![Pixellari 16](./Assets/README/fonts.png)

### GLCD in depth: image rendering

Just like fonts, the GLCD interface also supports drawing images, in the form of **bitmaps** (C buffers) generated by the Python script `img2c.py`. The script accepts any kind of image as input, and generates:

- C buffer of **RLE-encoded** 32 bit values for (A)RGB information (A is optional, it's included only if the image has *meaningful* alpha data, i.e. it's not constant at `0xFF`);
- a structure of type `LCD_Image`, which is the only thing that the user needs to use explicitly in the project, and contains image metadata, like the width and height of the image, and a pointer to the array containing the pixel data.

In the current implementation, the alpha channel is only used to choose whether to draw the pixel or not. In other words, alpha blending is not supported: any given pixel is either **fully opaque or fully transparent**. I plan to implement a more sophisticated system in the future though, with the goal of supporting background & foreground blending (I don't know how actually useful this could be, but it'd be nice nonetheless 😁).

> **Note**: Keep in mind that the GLCD driver can only handle **16 bits per pixel**. This means that the image will be **downsampled** from 24/32 bits to 16 bits. This can potentially lead to a loss of color information.

#### How to display images

Here's an example of structure generated by the script, for the following image that's been resized to 200x60 pixels (yeah, the script can do that too! 😁):

<html>
    <p align="center">
        <img src="./Assets/README/hello-res.png" alt="Hello Image">
    </p>
</html>

```c
const LCD_Image Image_Hello = {
    .pixels = Image_HelloData,
    .width = 200,
    .height = 60,
    .has_alpha = true
};
```

You can then display the image by:

- Moving the output file to an included directory;
- Including it in the source file where you want to use it;
- Creating an object with an `LCD_IMAGE` component inside (and something else, if you want).

Just like any other component, you'll get an ID that you can use to remove the image from the render list, to move it around, change its visibility, etc.

#### `main()` example: displaying the *Hello* image

```c
#include "includes.h"
#include "image_hello.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    LCD_SetBackgroundColor(LCD_COL_BLACK);

    const LCD_Coordinate center = LCD_GetCenter();
    u16 x = center.x - Image_Hello.width / 2;
    u16 y = center.y - Image_Hello.height / 2;

    LCD_OBJECT(hello_img, {
        LCD_IMAGE2(x, y, Image_Hello),
    });
    
    LCD_RMRender(); // Renders the image object

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
```

This produces the following output:

![Hello Image](./Assets/README/hello-emu.png)

Amazing, isn't it? 😁 Well, as you can see there are some artifacts due to unoptimal alpha handling, but I look forward to improving it very soon!

### GLCD roadmap

The GLCD interface is still in development, and there are a lot of features that I'd like to implement, eventually. Here's a list of what I have in mind:

- [X] Drawing RGB and RGBA images from bitmaps;
- [ ] Full support for alpha blending;
- [X] Implement a rendering list and the concept of object;
- [X] Support for custom fonts loaded as bitmaps;
- [ ] Support for object alignment (e.g. centering text, images), to avoid having to calculate the position manually;
- [X] Bounding box-based overlap detection for objects drawn on top of each other, in order to re-draw lower level objects when a higher level one is removed (**still in testing**);
- [X] Memory arena for the render list, to let the user decide how much memory to allocate for the GLCD based on the application's needs and the available memory, thus removing the static limitation of 1024 components and the need to statically allocated memory for the objects and components to avoid dangling pointers;
- [ ] Event driven rendering (maybe using the RIT?);
- [ ] Improve efficiency in rendering;
- [ ] Minimize memory usage as much as possible.

## General roadmap

In addition to the GLCD and DAC roadmaps, I have some other features in mind that I'd like to implement in the future:

- [ ] A way to play music through the DAC, like PCM data from a WAV file, instead of just pure sinusoidal tones;
- [ ] Offloading the DAC timing to other entities other than the standard timers, so that they can be used for other purposes by the user;
- [ ] Implement GPDMA for the DAC, to offload the CPU from the task of feeding the DAC with data;
- [ ] RIT-based DAC timing, to avoid using one of the standard timers;
- [X] RIT that accepts tasks through callback functions, possibly with different timings;
- [X] Dynamic RIT job list;
- [ ] A more general-purpose interface for ADC (support for other channels, etc.);
- [ ] Support for file systems and SD cards, for reading actual files, instead of bitmaps represented as C buffers, like fonts, images or audio;
- [ ] Support for prolonged joystick actions;
- [ ] Asynchronous touch button support;

**Not immediately planned**:

- [ ] Ethernet support for network communication;
- [ ] CAN bus.

---

## How to use the library in your projects

This branch is meant to be used for library development and testing. If you want to use the library in your own projects, you should download the **sample project** that I created from the `sample` branch of this repository.

It contains a fully configured Keil project that uses the library, and includes two targets: one for the simulator, and one for the physical board. Both require the library to be included in the `Keil/Libs/BigLib` directory in the project's root.

Follow the `README.md` in the `sample` branch to get started with the sample project.

### Deinitializing git

If you plan to use Git for version control in your project, you should first remove the `.git` directory from the library's directory, to avoid conflicts with this repo. To do this, simply run

```bash
rm -rf .git
```

and initialize a brand new repository with

```bash
git init
```

**Happy coding! 😁**

---

## Contributing to the library

I'm open to contributions, since I'm currenty the only one working on it. I'll define a set of guidelines for contributing in the future, if the project gains traction. For now, if you want to help me, plase open an issue and we can discuss the feature you want to implement, and how to do it.

## License

This project is licensed under the MIT License. For more information, please refer to the LICENSE file.
