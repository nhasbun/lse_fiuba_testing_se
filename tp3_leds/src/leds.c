#include "leds.h"


#define LEDS_OFFSET 1
#define FIRST_BIT_ON 1
#define LEDS_ALL_OFF 0x0
#define LEDS_ALL_ON 0xffff
#define TOTAL_LEDS 16

static uint16_t * puerto;

static uint16_t LedToMask(uint8_t led) {
    return (FIRST_BIT_ON << (led - LEDS_OFFSET));
}


void LedsCreate(uint16_t * address) {
    puerto = address;
    *address = LEDS_ALL_OFF;
}

void LedsSingleTurnOn(uint8_t led) {
    if (led > TOTAL_LEDS) return;

    *puerto |= LedToMask(led);
}

void LedsSingleTurnOff(uint8_t led) {
    if (led > TOTAL_LEDS) return;

    *puerto &= ~LedToMask(led);
}

void LedsAllTurnOn() {
    *puerto = LEDS_ALL_ON;
}

void LedsAllTurnOff() {
    *puerto = LEDS_ALL_OFF;
}

uint8_t LedsGetState(uint8_t led) {
    // Extracting single relevant bit of array

    uint16_t mask = 0x1;
    return (uint8_t) (*puerto >> (led - LEDS_OFFSET) & mask);
}