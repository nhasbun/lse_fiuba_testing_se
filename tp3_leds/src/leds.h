#pragma once

#include "stdint.h"

void LedsCreate(uint16_t * address);
void LedsSingleTurnOn(uint8_t led);
void LedsSingleTurnOff(uint8_t led);
void LedsAllTurnOn();
void LedsAllTurnOff();
uint8_t LedsGetState(uint8_t led);
