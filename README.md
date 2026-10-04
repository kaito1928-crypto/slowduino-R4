# Slowduino R4

## Slowduino ported to the Arduino UNO R4 / Nano R4

Slowduino R4 is a port of the Slowduino EFI firmware adapted to run on
Arduino R4-class hardware.

This project is based on Slowduino and brings its lightweight,
Speeduino-compatible ECU implementation to the Arduino UNO R4 and Nano R4.

The goal of this fork is to preserve the simplicity and TunerStudio-compatible
workflow of Slowduino while adding support for the newer R4 platform.

## About Slowduino

Slowduino is a lightweight EFI/ECU firmware inspired by Speeduino.
It provides fuel injection and ignition control, sensor support,
engine protection features, and TunerStudio compatibility.

The original Slowduino project primarily targets Arduino Uno/Nano-class
ATmega328P hardware.

## What this fork changes

- Ported Slowduino to Arduino R4-class hardware
- Added support for Arduino UNO R4
- Added support for Nano R4
- Adapted hardware-specific code for the R4 platform
- Retains the Slowduino/Speeduino-style tuning workflow
- Retains TunerStudio compatibility

## Project status

This is an experimental community port of Slowduino for Arduino R4 hardware.

Testing and development are ongoing. Use on an actual engine at your own risk.

## Credits

This project is based on **Slowduino**.

Many thanks to the original Slowduino project and its contributors for the
firmware this R4 port is based on.

Slowduino itself is inspired by and designed to work with the ecosystem and
tooling established by **Speeduino**.

## License

This project follows the license of the original Slowduino project.
See `LICENSE` for details.
