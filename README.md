# Custom Peripherals Library for the LandTiger LPC1768 board

![screenshot](./Assets/README/screenshot.png)

## What's this?

This is a sample project for the LandTiger, an ARM-based LPC1768 board, built for the ARM IDE Keil uVision 5. It includes a **custom C interface for every peripheral** studied in the course of **Computer Architectures**, in the MSc Computer Engineering program at Politecnico di Torino. Why did I do this? Mainly because I wanted to learn how things like drawing on the screen, and playing audio worked under the hood, but also to experiment by introducing lots of additional functionality while also providing a more abstract and powerful interface.

## File Structure

I used Visual Studio Code for developing everything, and Keil to test. The project is structured as follows:

- `Keil/`: contains the Keil project;
  - `Source`: contains the source code and the library itself;
    - `Libs`: the library, with a folder for each peripheral;
    - `Program`: contains the `main()` function and other program-specific files;
    - `Startup`: contains the startup code for the LPC1768;
    - `CMSIS_core`: CMSIS core files;
- `Scripts/`: contains the Python scripts useful to generate data readable by the library's functions.

Every folder, except for the `Scripts/` one - which is meant to be used externally - is already included in the `.uvprojx` file.

### Program directory

This is where your program will reside. It also includes additional directories for storing fonts and assets (`Fonts/` and `Assets/`, respectively). Additional information on what goes inside these folder is provided in the next sections of this file.

> **Note**: While the `Program/` directory is already included in the Keil Source Group, source files stored inside are not. You'll need to add them manually while developing your application.

As soon as you open the project in Keil, you may notice that they're not visible in the Keil Project Explorer: that's because I did not add them in the Source Group, but are only present in the *include path* of the C/C++ compiler. The reason for this is that the content of the sources belonging to these directories is usually generated, and not meant to be changed manually.

### Scripts

I developed two Python scripts to overcome the limitations caused by the absence of a file system in the current implementation of the drivers. This limitation is particularly evident when dealing with **images**, **fonts**, and **music** files. The culprit peripherals are the GLCD (screen) and the DAC (Digital to Analog Converter).

In order to use the scripts, you need to have Python installed on your machine. You can download it from the [official website](https://www.python.org/downloads/), or by using a package manager like `brew` on macOS or `winget` on Windows. Once you have everything setup, setup a virtual environment wherever you want, by running:

```bash
cd <wherever-you-want>
python3 -m venv .venv
```

Then, activate it using:

```bash
source .venv/bin/activate # macOS/Linux
.venv\Scripts\activate # Windows
```

Finally, install the required packages by running:

```bash
pip3 install pillow
```

You're now good to go. Usage instructions for each script can be obtained by running them with the `-h` flag.

## Status

The interface currently supports the following peripherals:

- [X] Buttons (with optional debouncing)
- [X] Timers
- [X] RIT
- [X] SysTick Timer control (not refined)
- [X] Power Management
- [X] LEDs
- [X] Joystick
- [X] ADC for potentiometer support
- [X] DAC Tone Generator with buzzer
- [X] GLCD & TouchScreen for the ADS7843 controller
- [ ] CAN Bus

Below is a more detailed description of some interesting features itroduced for the more interesting peripherals, the GLCD and the DAC. Bear in mind that the library is still in development (and I'm alone in this!), hence you may encounter bugs or unexpected behavior.

## GLCD Features

### Object Rendering

I put *a lot* of effort into the GLCD interface, as it's certainly one of the most complex peripherals to interface with. In fact, it must be flexible enough to support different actions that the user could find useful (like drawing images, text, etc.), but also simple enough to be used without too much hassle.

The current status of the interface is already quite enough: it supports basic components, like text, images and shapes, and provides a **declarative** syntax to use them to create more complex objects, made of more than one component. The rendering is performed using a render queue, in which items are added and removed by the user, through a very intuitive interface. As of now, the render queue is statically allocated, hence its size is fixed to **1024** components. I opted for this solution to keep the overhead low (which would have been surely much higher with dynamic memory allocation). I plan to implement a more sophisticated system in the future though, with the goal of removing this limitation.

#### How to render stuff

First of all, you need to initialize the GLCD by calling the `LCD_Init(LCD_Orientation)` function in the `main()` function. The LCD must be initialized before doing anything else (even the touch controller requires it, as it uses the LCD to draw the calibration interface!).

To display objects on the GLCD, you can use the various functions and macros provided. Here are some of the key functions and macros that you'll use:

**Basic functions**:

- `LCD_SetPointColor(LCD_Color color, LCD_Coordinate point)`: Sets the color of a specific pixel;
- `LCD_SetBackgroundColor(LCD_Color color)`: Tells the interface the color of the background that will be rendered *upon* clearing the screen, then clears the screen and redraws the object with the new background color.

**Rendering management functions**:

- `LCD_RQAddObject(const LCD_Obj *const obj)`: Adds an object to the render queue and returns its ID. If you use the `LCD_BEGIN_DRAWING` and `LCD_END_DRAWING` macros when building drawable components, you don't need to call this function manually;
- `LCD_RQRemoveObject(LCD_ObjID id, bool redraw_screen)`: Removes an object from the render queue using its ID;
- `LCD_RQSetObjectVisibility(LCD_ObjID id, bool visible, bool redraw_screen)`: Shows or hides an object.

> **Note:** Refer to the Doxygen documentation for more information on the functions and macros provided by the library.

**Drawing components**:

Components are the basic building blocks of objects. While the library allows you to declare component directly using the type `LCD_Component`, I highly recommend using the macros described below, that hide the complexity of the structure, making the code more readable.

- `LCD_LINE(...)`: Defines a line component;
- `LCD_RECT(x, y, ...)`: Defines a rectangle component with the top-left corner at `(x, y)`;
- `LCD_CIRCLE(...)`: Defines a circle component;
- `LCD_IMAGE(x, y, image_struct)`: Defines an image component with the top-left corner at `(x, y)`, using the `image_struct` structure generated by the Python script `img2c.py`;
- `LCD_TEXT(x, y, ...)`: Defines a text component with the top-left corner at `(x, y)`.

**Declarative object definition**:

A component cannot be rendered on the screen by itself. It must be part of an **object**, which is a collection of components (or only one). Declaring an object is not trivial, as it requires the user to define the various components, and to manage the data structures that store them (without the risk of having dangling pointers all over the place). To avoid problems of these kind, and maintain a clean code, I introduced a set of macros that simplify the process of defining, and rendering them:

- `LCD_OBJECT(name, num_comps, ...)`: Declares an object, made up of `num_comps` components, ready to be rendered on the screen. At the end, it adds the object to the render queue, and returns its ID;

- `LCD_BEGIN_DRAWING` and `LCD_END_DRAWING`: These macros are used to wrap the drawing code. The `LCD_END_DRAWING` macro automatically triggers a screen update, effectively rendering the object(s) that they contain.

#### A final example of usage

Here's an example of how to use the library to draw a simple cross on the screen:

  ```c
#include "includes.h"

int main(void)
{
    SystemInit();
    LCD_Init(LCD_ORIENT_VER);
    LCD_SetBackgroundColor(LCD_COL_BLACK);

    LCD_ObjID cross;
    u16 x = LCD_GetWidth() / 2;
    u16 y = LCD_GetHeight() / 2;

    LCD_BEGIN_DRAWING;
    cross = LCD_OBJECT(calib_cross, 3, {
        LCD_RECT(x - 15, y - 15, {
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
    LCD_END_DRAWING;

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
  ```

This produces the following output:

![Calibration Cross](./Assets/README/cross.png)

### Custom Fonts

The GLCD has 2 built-in fonts that can be used to render text, `LCD_DEF_FONT_SYSTEM` and `LCD_DEF_FONT_MSGOTHIC`. The problem with these fonts is that the user has no control over the characters' size, and there might be cases where it's necessary to draw smaller or bigger text. That's why I introduced the possibility to load **custom fonts**, with **custom sizes**!

Before diving into the details, let's talk about limitation. As of now, you can load up to **16** different font in the GLCD, and each font can only have **ASCII characters**. This may change in the future, but for now, this is how it is.

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

- Moving the output file to the `Program/Fonts/` directory;
- Including it in the `main.c` file;
- Calling the `LCD_FMAddFont(<LCD_Font>)` with the name of the structure as argument.

The function returns the font's ID, which you can use to render text with that font (the `.font` field of the `LCD_Text` structure), and to unload it when you don't need it anymore, with the `LCD_FMRemoveFont(<font_id>)` function.

#### A final example of usage

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

    LCD_BEGIN_DRAWING;
    LCD_OBJECT(text_obj, 2, {
        LCD_TEXT(center.x - 70, center.y - 15, {
            .text = "This is Pixellari 16!",
            .text_color = LCD_COL_RED,
            .bg_color = LCD_COL_NONE,
            .font = pix16_fid,
        }),
        LCD_TEXT(center.x - 60, center.y + 15, {
            .text = "This is System!",
            .text_color = LCD_COL_GREEN,
            .bg_color = LCD_COL_NONE,
            .font = LCD_DEF_FONT_SYSTEM,
        }),
    });
    LCD_END_DRAWING;

    LCD_FMRemoveFont(pix16_fid);
    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
```

This produces the following output:

![Pixellari 16](./Assets/README/fonts.png)

### Images

Just like fonts, the GLCD interface also supports the drawing of images, in the form of C buffers generated by the Python script `img2c.py`. The script accepts any kind of image as input, and generates a C buffer of RLE-encoded 32 bit values for RGB(A) information, alongside a structure of type `LCD_Image`. This structure is the only thing that the user needs to use explicitly in the project, and contains image metadata, like the width and height of the image, and a pointer to the array containing the pixel data.

The scripts outputs data in either RGB (24-bit) or ARGB (32-bit) format, depending on the input image. In case of the latter, the alpha channel is used to choose whether to draw the pixel or not. As of now, alpha blending is not supported: the alpha channel is used only to decide whether to draw the pixel or not (so, it's either **fully opaque or fully transparent**). I plan to implement a more sophisticated system in the future though, with the goal of supporting background & foreground blending.

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

- Moving the output file to the `Program/Assets/` directory;
- Including it in the `main.c` file;
- Creating an object with an `LCD_IMAGE` component inside (and something else, if you want), just like we saw in the Rendering section:

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

    LCD_BEGIN_DRAWING;
    LCD_OBJECT(hello_img, 2, {
        LCD_IMAGE(x, y, Image_Hello),
    });
    LCD_END_DRAWING;

    POWER_Init(POWR_CFG_SLEEP_ON_EXIT);
    POWER_PowerDownOnWFI();
    POWER_WaitForInterrupts();
}
```

This produces the following output:

![Hello Image](./Assets/README/hello-emu.png)

Amazing, isn't it? 😁 Well, as you can see there are some artifacts due to unoptimal alpha handling, but I look forward to improving it very soon!

### What's next?

The GLCD interface is still in development, and there are a lot of features that I'd like to implement, eventually. Here's a list of what I have in mind:

- [X] Drawing RGB and RGBA images from C buffers;
- [ ] Full support for alpha blending;
- [X] Implement a rendering queue and the concept of object;
- [X] Support for custom fonts loaded as C buffers;
- [ ] Support for object alignment (e.g. centering text, images), to avoid having to calculate the position manually;
- [ ] Memory arena for the render queue, to let the user decide how much memory to allocate for the GLCD based on the application's needs and the available memory, thus removing the static limitation of 1024 components and the need to statically allocated memory for the objects and components to avoid dangling pointers;

## DAC Features

Nothing too fancy here, for now! TTYL when I'll figure out how to play music with this thing. 😁

## Roadmap

Here's a list of features that I'd like to implement in the future, in order of priority:

**GLCD**:

- [ ] GLCD stuff (see its roadmap in the dedicated section above)

**DAC**:

- [ ] A way to play music through the DAC, like PCM data from a WAV file, instead of just pure sinusoidal tones;
- [ ] Offloading the DAC timing to other entities other than the standard timers, so that they can be used for other purposes by the user;

**General**:

- [X] RIT that accepts tasks through callback functions, possibly with different timings;
- [ ] A more general-purpose interface for ADC (support for other channels, etc.);
- [ ] Support for file systems and SD cards, for reading actual files, instead of C buffers, like fonts, images or audio;
- [ ] Support for prolonged joystick actions;

**Not immediately planned**:

- [ ] Ethernet support for network communication;
- [ ] A more comprehensive support for SysTick;

## Contributing

I'm open to contributions, since I'm currenty the only one working on it. I'll define a set of guidelines for contributing in the future, if the project gains traction. For now, if you want to help me, plase open an issue and we can discuss the feature you want to implement, and how to do it.

## License

This project is licensed under the MIT License. For more information, please refer to the LICENSE file.
