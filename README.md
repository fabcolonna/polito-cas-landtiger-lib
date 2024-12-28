# Keil + Custom Peripherals Interface for the LandTiger board

## What's this?

This is a sample project for the LandTiger, an ARM-based LPC1768 board, built for the ARM IDE Keil uVision 5. It includes a custom C interface for peripherals written for scratch by myself, mainly because I wanted to learn how things work under the hood, but also to experiment by introducin functionalities and introduce a more abstract interface.

## File Structure

- `keil/`: contains the Keil project file and the source code;
  - `lib`: contains the custom C library for each peripheral
  - `program`: contains the main program and other program-specific files
  - `startup`: contains the startup code for the LPC1768
  - `CMSIS_core`: contains the CMSIS core files
- `scripts/`: contains the Python scripts useful to generate data readable by the library's functions.

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

## Who am I?

My name is **Fabio Colonna**, and I am currently an MSc student in Computer Engineering at Politecnico di Torino. I developed a deep passion for embedded systems and low-level programming, and that's what's driving me to develop this project. I hope you find it useful and that it can help you in your projects!

Feel free to reach me out at my [personal](mailto:fabcolonna@icloud.com) or [institutional](mailto:fabio.colonna@studenti.polito.it) email for any questions or suggestions about this repo.
