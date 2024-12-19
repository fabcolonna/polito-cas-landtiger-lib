# Sample Keil Project for LPC1768 with Custom C interface for Peripherals

Fabio Colonna
MSc in Computer Engineering @ Politecnico di Torino
AA 2024/25

## What's this?

This is a sample project for the ARM-based LPC1768 board, developed in Keil uVision 5. It is a simple project that uses the custom C interface for peripherals developed in the `lib` folder, already imported as a Source Group in the
.uvprojx file.

## Status

This library is currently under development. These are the peripherals that will eventually be supported:

- [X] Buttons
- [X] Timers
- [X] RIT & Debouncer
- [X] SYSTick
- [X] Power Management
- [X] LEDs
- [X] Joystick (does NOT support prolonged press)
- [X] ADC for potentiometer support
- [ ] DAC Sound System & MIDI Interface
- [ ] Screen (finna be tough)

## Maybe in the future

- [ ] RIT that accepts tasks from the outside, possibly with different timings
- [ ] A more general-purpose interface for ADC
