# Custom Peripherals Library for the LandTiger LPC1768 board

## What's this?

This is a sample project for the LandTiger, an ARM-based LPC1768 board, built for the ARM IDE Keil uVision 5. It includes a custom C interface for peripherals written for scratch by myself, mainly because I wanted to learn how things work under the hood, but also to experiment by introducin functionalities and introduce a more abstract interface.

## File Structure

- `Keil/`: contains the Keil project;
  - `Source`: contains the source code and the library itself;
    - `Libs`: the library, with a folder for each peripheral;
    - `Program`: contains the `main()` function and other program-specific files;
    - `Startup`: contains the startup code for the LPC1768;
    - `CMSIS_core`: CMSIS core files;
- `Scripts/`: contains the Python scripts useful to generate data readable by the library's functions.

### Scripts

The Python scripts are used to generate data that can be read by the library's functions. One example are the scripts that generate RGB data from images: since, as of now, the board has no FS from which to read files, the images are converted to C buffers of RGB8565 or RGB565 data, which can then be read by the `LCD_DrawImage16` and `LCD_DrawImage32` functions, respectively.

> **Note**: **RGB8565** is a 24-bit color format that I made up out of necessity. It's basically an RGB565 format with an extra byte for the alpha channel (at the MSB, hence the 8). This is introduced in order to support transparency in images. As of now, the alpha channel is simply used as a *flag* to indicate whether a pixel is transparent or not. In future versions, though, I might implement a more sophisticated system that allows for proper alpha blending, that's why I decided to keep the extra byte, instead of just one bit.

In order to use the scripts, you need to have Python installed on your machine. You can download it from the [official website](https://www.python.org/downloads/), or by using a package manager like `brew` on macOS or `winget` on Windows.

Once you have Python installed, setup a virtual environment in the `Scripts/` folder by running:

```bash
python3 -m venv .venv
```

Then, activate it using:

```bash
source .venv/bin/activate # macOS/Linux
.venv\Scripts\activate # Windows
```

Finally, install the required packages by running:

```bash
pip install -r requirements.txt
```

You're now good to go. Usage instructions for each script can be obtained by running them with the `-h` flag.

## Status

The interface currently supports the following peripherals:

- [X] Buttons (with optional debouncing)
- [X] Timers
- [X] RIT
- [X] SysTick Timer control
- [X] Power Management
- [X] LEDs
- [X] Joystick
- [X] ADC for potentiometer support
- [X] DAC Tone Generator with buzzer
- [X] GLCD & TouchScreen for the ADS7843 controller
- [ ] CAN Bus

## Maybe in the future

- [X] RIT that accepts tasks through callback functions, possibly with different timings;
- [ ] A more general-purpose interface for ADC (support for other channels, etc.);
- [ ] A way to play music through the DAC, like PCM data from a WAV file, instead of just pure sinusoidal tones;
- [ ] Support for file systems and SD cards, for reading actual files, instead of C buffers, like images or audio;
- [ ] Support for prolonged joystick actions;
- [ ] Offloading the DAC timing to other entities other than the standard timers, so that they can be used for other purposes by the user;
- [ ] Ethernet support for network communication;
- [ ] A more comprehensive support for SysTick;

## License

This project is licensed under the MIT License. For more information, please refer to the LICENSE file.

## Contact me

Feel free to reach me out at my [personal](mailto:fabcolonna@icloud.com) or [institutional](mailto:fabio.colonna@studenti.polito.it) email for any questions or suggestions about this repo.
