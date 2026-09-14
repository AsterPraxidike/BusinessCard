/* =====================================================================
 * wch_arduino_compat.cpp
 * ===================================================================== */
#include "Wch_arduino_compat.h"

/* GPIO registers - names/behavior verified against CH583/CH582/CH581
 * datasheet V1.6, section 7.4. Provided by the WCH SDK/linker script;
 * declared here so this file compiles standalone against that SDK. */
extern volatile uint32_t R32_PA_DIR, R32_PA_OUT, R32_PA_CLR, R32_PA_PU, R32_PA_PD_DRV;
extern volatile uint32_t R32_PB_DIR, R32_PB_OUT, R32_PB_CLR, R32_PB_PU, R32_PB_PD_DRV;

static inline uint8_t pin_port(uint8_t pin) { return (pin >> 5) & 0x1; }
static inline uint8_t pin_bit(uint8_t pin)  { return pin & 0x1F; }

static inline volatile uint32_t *dir_reg(uint8_t port)    { return port == GPIO_PORT_A ? &R32_PA_DIR    : &R32_PB_DIR; }
static inline volatile uint32_t *out_reg(uint8_t port)    { return port == GPIO_PORT_A ? &R32_PA_OUT    : &R32_PB_OUT; }
static inline volatile uint32_t *clr_reg(uint8_t port)    { return port == GPIO_PORT_A ? &R32_PA_CLR    : &R32_PB_CLR; }
static inline volatile uint32_t *pu_reg(uint8_t port)     { return port == GPIO_PORT_A ? &R32_PA_PU     : &R32_PB_PU; }
static inline volatile uint32_t *pd_drv_reg(uint8_t port) { return port == GPIO_PORT_A ? &R32_PA_PD_DRV : &R32_PB_PD_DRV; }

void pinMode(uint8_t pin, uint8_t mode)
{
    uint8_t port = pin_port(pin);
    uint8_t bit  = pin_bit(pin);

    if (mode == OUTPUT) {
        *dir_reg(port) |= (1u << bit);
    } else {
        *dir_reg(port)   &= ~(1u << bit);
        *pu_reg(port)     &= ~(1u << bit); /* floating: no pull-up   */
        *pd_drv_reg(port) &= ~(1u << bit); /* floating: no pull-down */
    }
}

void setFloatingInput(uint8_t pin)
{
    pinMode(pin, INPUT);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    uint8_t port = pin_port(pin);
    uint8_t bit  = pin_bit(pin);

    if (value == HIGH) {
        *out_reg(port) |= (1u << bit);
    } else {
        *clr_reg(port) |= (1u << bit);
    }
}

/* ---------------------------------------------------------------------
 * ADC - STILL A STUB, same caveat as earlier in this conversation.
 * Fill this in against WCH's EVT ADC example (ADC.c/ADC.h in the
 * CH583/CH582 EVT package). The interface shape (blocking single
 * conversion, takes a channel number, returns raw counts) is right;
 * the register-level body underneath it is not verified.
 * ------------------------------------------------------------------- */
uint16_t analogRead(uint8_t adcChannel)
{
    /* PLACEHOLDER - replace with real WCH ADC single-conversion call */
    (void)adcChannel;
    return 0;
}

/* ---------------------------------------------------------------------
 * Timing - PLACEHOLDER busy-wait, replace with a calibrated delay
 * (e.g. driven off SysTick or a hardware timer) once Fsys is known.
 * ------------------------------------------------------------------- */
void delayMicroseconds(uint32_t us)
{
    volatile uint32_t cycles = us * 20; /* PLACEHOLDER - depends on Fsys */
    while (cycles--) { __asm__ volatile ("nop"); }
}

/* ---------------------------------------------------------------------
 * Serial - STILL A STUB over USB CDC, same caveat as earlier.
 * ------------------------------------------------------------------- */
extern "C" void usb_cdc_send_string(const char *str, uint16_t len);

static void cdc_send(const char *str)
{
    uint16_t len = 0;
    while (str[len] != '\0') len++;
    usb_cdc_send_string(str, len);
}

static void cdc_send_int(int v)
{
    char buf[12];
    int pos = 0;
    bool neg = v < 0;
    if (neg) v = -v;

    char tmp[10];
    int n = 0;
    if (v == 0) tmp[n++] = '0';
    while (v > 0) { tmp[n++] = '0' + (v % 10); v /= 10; }

    if (neg) buf[pos++] = '-';
    while (n > 0) buf[pos++] = tmp[--n];
    buf[pos] = '\0';

    cdc_send(buf);
}

void SerialClass::print(const char *s) { cdc_send(s); }
void SerialClass::print(int v)         { cdc_send_int(v); }
void SerialClass::println(const char *s) { cdc_send(s); cdc_send("\r\n"); }
void SerialClass::println(int v)         { cdc_send_int(v); cdc_send("\r\n"); }
void SerialClass::println()              { cdc_send("\r\n"); }

SerialClass Serial;