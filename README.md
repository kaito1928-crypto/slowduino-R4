# Slowduino R4

## Slowduino port for Arduino UNO R4 / Nano R4 with CDI ignition control

Slowduino R4 is a fork of Slowduino adapted to run on Arduino R4-class
hardware.

This project brings the lightweight, Speeduino-compatible ECU functionality
of Slowduino to the Arduino UNO R4 and Nano R4, while adding CDI
(Capacitor Discharge Ignition) control for motorcycles that already use
CDI-based ignition systems.

The goal of this fork is to preserve the simplicity and TunerStudio-compatible
workflow of Slowduino while adding support for the newer R4 platform and
making it easier to convert existing CDI-equipped motorcycles to electronic
fuel injection (EFI).

## Key Features

- Ported Slowduino to Arduino R4-class hardware
- Arduino UNO R4 support
- Nano R4 support
- CDI ignition control
- CDI control selectable from the trigger settings in TunerStudio
- Designed for EFI conversion of motorcycles with existing CDI ignition systems
- Fuel injection control
- Ignition timing control
- TunerStudio compatibility
- Based on the original Slowduino project

## CDI Ignition Support

Slowduino R4 adds a CDI (Capacitor Discharge Ignition) control option to the
trigger settings in TunerStudio.

This feature was added primarily to make it easier to convert motorcycles
that already have a CDI ignition system to electronic fuel injection (EFI).

Instead of requiring the motorcycle's existing CDI ignition hardware to be
completely replaced, Slowduino R4 can be configured for CDI control directly
from the trigger settings in TunerStudio.

This allows the existing CDI-based ignition system to be retained while
Slowduino R4 provides fuel injection and ignition timing control.

The CDI functionality is therefore intended especially for EFI conversions
of motorcycles that already have a working CDI ignition system.

## About Slowduino

Slowduino is a lightweight EFI/ECU firmware inspired by Speeduino.

It provides fuel injection and ignition control, sensor support, engine
protection features, and TunerStudio compatibility.

The original Slowduino project primarily targets Arduino Uno/Nano-class
ATmega328P hardware.

## About This Fork

Slowduino R4 adapts Slowduino for the newer Arduino R4 platform and extends
the original firmware with CDI ignition control.

The main goals of this fork are:

- Support Arduino UNO R4 / Nano R4 hardware
- Preserve the lightweight design of Slowduino
- Maintain the familiar TunerStudio tuning workflow
- Add CDI control through the TunerStudio trigger settings
- Make EFI conversion easier for motorcycles already equipped with CDI ignition

## Project Status

This is an experimental community port of Slowduino for Arduino R4 hardware.

Testing and development are ongoing.

Use on an actual engine at your own risk.

## Credits

Slowduino R4 is based on **Slowduino**.

Many thanks to the original Slowduino project and its contributors for the
firmware this R4 port is based on.

Slowduino itself is inspired by and designed to work with the ecosystem and
tooling established by **Speeduino**.

This repository contains modifications for Arduino R4 hardware support and
additional CDI ignition control functionality.

## License

This project follows the license of the original Slowduino project.
See `LICENSE` for details.
