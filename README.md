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
The back contains optional battery and accompanying circuitry, programming header, and personal contact information.

---
## design philosiphy 

As buisness cards are handed out for free this board first and formost needed to be cheap to manufacture. 

this is intended to be a showpiece of sorts for my capabilities as a designe. 

---
## Features

- 4×12 capacitive touch keyboard matrix
- CH582M BLE/USB microcontroller
- USB-C PCB edge connector
- USB HID keyboard mode
- BLE HID keyboard mode
- PCB trace antenna for 2.4 GHz
- Optional 20 mm coin-cell footprint and BMS circuitry
- Programming/debug pads

---

## Hardware Overview

### MCU

The board uses a **WCH CH582M**, because it is both low cost, usb capable, has enough ADC inputs for my needs (12), and has great optional features like bluetooth. 

### Keyboard

The keyboard is a 48-key capacitive matrix.

The keys use interdigitated copper pads covered by solder mask. They are not mechanical switches as that would add cost and complexity. The firmware will scan the matrix by driving row electrodes and measuring voltage on the ADC which is connected to every column. 

### USB

The USB-C connector is implemented as a PCB edge connector.as such, the board **MUST** be manufactured in 0.6 or 0.8mm thickness. 

The board is intended to enumerate as a USB HID keyboard when connected over USB.

### BLE

The CH582M provides Bluetooth Low Energy support. The intended wireless behavior is BLE HID keyboard mode.

### Battery, charging, and protection

The board accepts one 20 mm coin cell. A battery is not included.

Default CR2032 configuration

Revision 1.1 ships configured for a standard, non-rechargeable CR2032:

USB power is recommended.
The onboard charging path is disabled by default.
The low-voltage cutoff is bypassed by default.
Observe the marked battery polarity.
Never attempt to charge a CR2032.

The board can operate from USB without a battery installed.

Optional rechargeable configuration

The board includes footprints for an optional 1-cell lithium charging circuit and low-voltage cutoff. Enabling this configuration requires cutting the designated configuration traces and populating all required charging and protection components.


Use only a compatible 1S 4.2 V rechargeable lithium cell, such as an LIR2032. it is intended to be used with a LIR2032 but accepts other single cell lithium batteries. 
Only use a protected rechargeable cell.
Charging current must not exceed 10 mA.
No charge-status LED is provided.
Never install a non-rechargeable CR2032 while the charging circuit is enabled.
An unmodified board will not charge an installed rechargeable cell.
---

### Version 1.1

version 1.1 is an in progress revision while the functionality is tested. 
the goal is a focus on the battery and Bluetooth as i have learned more about batteries. 

version 1.1 features 3 optional configurations: 

-default "cr2032" state
  this uses a cr2032 or other disposable 3v battery source
-low voltage cutoff
  this adds minimal components to add a low volt cutoff so you can use a rechargeable battery 
-charger IC and low volt cutoff
  with a low volt cutoff, this allows you to charge the battery from the USB connector. 

the battery remains completely optional and only exists for Bluetooth connectivity. they will not be handed out with these components to save cost and space in a wallet (batteries are thick okay? this way the cards are much slimmer, but the options and documentation is there should it be wanted to be installed)

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
The back contains optional battery and accompanying circuitry, programming header, and personal contact information.

---
## design philosiphy 

As buisness cards are handed out for free this board first and formost needed to be cheap to manufacture. 

this is intended to be a showpiece of sorts for my capabilities as a designe. 

---
## Features

- 4×12 capacitive touch keyboard matrix
- CH582M BLE/USB microcontroller
- USB-C PCB edge connector
- USB HID keyboard mode
- BLE HID keyboard mode
- PCB trace antenna for 2.4 GHz
- Optional 20 mm coin-cell footprint and BMS circuitry
- Programming/debug pads

---

## Hardware Overview

### MCU

The board uses a **WCH CH582M**, because it is both low cost, usb capable, has enough ADC inputs for my needs (12), and has great optional features like bluetooth. 

### Keyboard

The keyboard is a 48-key capacitive matrix.

The keys use interdigitated copper pads covered by solder mask. They are not mechanical switches as that would add cost and complexity. The firmware will scan the matrix by driving row electrodes and measuring voltage on the ADC which is connected to every column. 

### USB

The USB-C connector is implemented as a PCB edge connector.as such, the board **MUST** be manufactured in 0.6 or 0.8mm thickness. 

The board is intended to enumerate as a USB HID keyboard when connected over USB.

### BLE

The CH582M provides Bluetooth Low Energy support. The intended wireless behavior is BLE HID keyboard mode.

### Battery, charging, and protection

The board accepts one 20 mm coin cell. A battery is not included.

Default CR2032 configuration

Revision 1.1 ships configured for a standard, non-rechargeable CR2032:

USB power is recommended.
The onboard charging path is disabled by default.
The low-voltage cutoff is bypassed by default.
Observe the marked battery polarity.
Never attempt to charge a CR2032.

The board can operate from USB without a battery installed.

Optional rechargeable configuration

The board includes footprints for an optional 1-cell lithium charging circuit and low-voltage cutoff. Enabling this configuration requires cutting the designated configuration traces and populating all required charging and protection components.


Use only a compatible 1S 4.2 V rechargeable coin cell, such as an LIR2032.
Only use a protected rechargeable cell.
Charging current must not exceed 10 mA.
No charge-status LED is provided.
No charge circuitry is provided.
Never install a non-rechargeable CR2032 while the charging circuit is enabled.
An unmodified board will not charge an installed rechargeable cell.

---

### Version 1.1

version 1.1 is an in progress revision while the functionality is tested. 
the goal is a focus on the battery and Bluetooth as i have learned more about batteries. 

version 1.1 features 3 optional configurations: 

-default "cr2032" state
  this uses a cr2032 or other disposable 3v battery source
-low voltage cutoff
  this adds minimal components to add a low volt cutoff so you can use a rechargeable battery 
-charger IC and low volt cutoff
  with a low volt cutoff, this allows you to charge the battery from the USB connector. 

the battery remains completely optional and only exists for Bluetooth connectivity. they will not be handed out with these components to save cost and space in a wallet (batteries are thick okay? this way the cards are much slimmer, but the options and documentation is there should the end user want to install one)

  

  
