# Capacitive Keyboard Business Card

A functional USB/BLE keyboard in the shape of a business card.

This project is a credit-card-sized PCB with a capacitive touch keyboard, USB-C edge connector, BLE HID support, optional coin-cell power, and a PCB trace antenna.
It is a buisnesscard that has both flair and function. 

Designed by **Aster-Praxidike**.

---

## What is it?

This board is a small capacitive-touch keyboard built around the **WCH CH582M** microcontroller.

It is intended to work as:

- a physical portfolio / hardware business card
- a USB HID keyboard over USB-C
- a BLE HID keyboard over Bluetooth Low Energy


The front of the card contains the keyboard matrix and visual identity.  
The back contains optional battery, programming header, and personal contact information.

---

## Features

- 4×12 capacitive touch keyboard matrix
- CH582M BLE/USB microcontroller
- USB-C PCB edge connector
- USB HID keyboard mode
- BLE HID keyboard mode
- PCB trace antenna for 2.4 GHz
- Optional 20 mm coin-cell footprint
- Programming/debug pads

---

## Hardware Overview

### MCU

The board uses a **WCH CH582M**, a low-cost BLE-capable microcontroller with USB support and analog/touch-capable pins.

### Keyboard

The keyboard is a 48-key capacitive matrix.

The keys use interdigitated copper pads covered by soldermask. They are not mechanical switches. The firmware will csan the matrix by driving row electrodes and measuring capacitive changes on column electrodes.

### USB

The USB-C connector is implemented as a PCB edge connector.

The board is intended to enumerate as a USB HID keyboard when connected over USB.

### BLE

The CH582M provides Bluetooth Low Energy support. The intended wireless behavior is BLE HID keyboard mode.

### Battery

The board has a footprint for a 20 mm coin-cell holder.

Important:

- Battery is not included.
- The board does **not** have charging circuitry for the battery
- USB power is recommended.
- Observe polarity if installing the battery
- if using rechargable cells, **ONLY** use protected cells to prevent dammage to the cells. 
