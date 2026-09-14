/* =====================================================================
 * wch_arduino_compat.h
 * =====================================================================
 * Minimal Arduino-API-shaped shim for the CH582M, since no real Arduino
 * core exists for the CH58x family (confirmed: openwch/arduino_core_ch32
 * issue #69 explicitly requests one and states it doesn't exist yet).
 *
 * This lets main.cpp keep its original digitalWrite()/analogRead()/
 * delayMicroseconds()/Serial.print() shape, backed by real calls.
 *
 * VERIFIED vs UNVERIFIED:
 *   - GPIO pin encoding + digitalWrite()/pinMode() are built directly on
 *     the R32_Px_DIR/OUT/CLR/PU/PD_DRV registers documented in the
 *     CH583/CH582/CH581 datasheet V1.6, section 7.4. These are accurate.
 *   - analogRead() calls adc_channel_read(), which is still a stub -
 *     same caveat as before, fill in from WCH's EVT ADC example.
 *   - Serial.print()/println() route over USB CDC via usb_cdc_send_string(),
 *     also still a stub - same caveat as before.
 * ===================================================================== */
#ifndef WCH_ARDUINO_COMPAT_H
#define WCH_ARDUINO_COMPAT_H

#include <stdint.h>
#include <stdbool.h>

/* ---------------------------------------------------------------------
 * Pin encoding: bit5 = port (0=A, 1=B), bits[4:0] = bit number
 * ------------------------------------------------------------------- */
#define GPIO_PORT_A 0
#define GPIO_PORT_B 1
#define MAKE_PIN(port, bit) ((uint8_t)(((port) << 5) | (bit)))

/* Row pins used in this project (digital output) */
#define PB9  MAKE_PIN(GPIO_PORT_B, 9)
#define PB8  MAKE_PIN(GPIO_PORT_B, 8)
#define PB17 MAKE_PIN(GPIO_PORT_B, 17)
#define PB16 MAKE_PIN(GPIO_PORT_B, 16)

/* Column pins used as digital outputs briefly (discharge step only) */
#define PA4  MAKE_PIN(GPIO_PORT_A, 4)
#define PA5  MAKE_PIN(GPIO_PORT_A, 5)
#define PA6  MAKE_PIN(GPIO_PORT_A, 6)
#define PA0  MAKE_PIN(GPIO_PORT_A, 0)
#define PA1  MAKE_PIN(GPIO_PORT_A, 1)
#define PA2  MAKE_PIN(GPIO_PORT_A, 2)
#define PA3  MAKE_PIN(GPIO_PORT_A, 3)
#define PA15 MAKE_PIN(GPIO_PORT_A, 15)
#define PA14 MAKE_PIN(GPIO_PORT_A, 14)
#define PA13 MAKE_PIN(GPIO_PORT_A, 13)
#define PA12 MAKE_PIN(GPIO_PORT_A, 12)
#define PA7  MAKE_PIN(GPIO_PORT_A, 7)

/* ADC channel numbers - these are separate from the digital pin codes
 * above; analogRead() takes one of these, not a MAKE_PIN() value */
#define AIN0  0
#define AIN1  1
#define AIN2  2
#define AIN3  3
#define AIN4  4
#define AIN5  5
#define AIN6  6
#define AIN7  7
#define AIN8  8
#define AIN9  9
#define AIN10 10
#define AIN11 11

/* Arduino-style constants */
#define HIGH   1
#define LOW    0
#define OUTPUT 1
#define INPUT  0

/* ---------------------------------------------------------------------
 * Arduino-style function shims
 * ------------------------------------------------------------------- */
void     pinMode(uint8_t pin, uint8_t mode);
void     digitalWrite(uint8_t pin, uint8_t value);
void     setFloatingInput(uint8_t pin); /* not in Arduino API - needed for
                                          * the discharge->sense switch;
                                          * plain pinMode(pin, INPUT) also
                                          * works if you don't need to be
                                          * explicit about pull config */
uint16_t analogRead(uint8_t adcChannel);
void     delayMicroseconds(uint32_t us);

/* ---------------------------------------------------------------------
 * Minimal Serial shim over USB CDC (extern "C" wrapper you fill in from
 * WCH's USB CDC-ACM example - see caveat above)
 * ------------------------------------------------------------------- */
class SerialClass {
public:
    void print(const char *s);
    void print(int v);
    void println(const char *s);
    void println(int v);
    void println();
};
extern SerialClass Serial;

#endif /* WCH_ARDUINO_COMPAT_H */