#include "unity.h"
#include "stdint.h"
#include "leds.h"

/**
 * 
 * Con la inicialización todos los LEDs quedan apagados.
 * Prender un LED individual.
 * Apagar un LED individual.
 * Prender y apagar múltiples LED’s.
 * Prender todos los LEDs de una vez.
 * Apagar todos los LEDs de una vez.
 * Consultar el estado de un LED que está encendido ==> con una fn
 * Consultar el estado de un LED que está apagado ==> hasta aća mínimo
 * Revisar limites de los parametros. ** opcional - casos de borde (led 1 - led 16)
 * Revisar parámetros fuera de los limites. ** en clases
 * 
 */

uint16_t leds_virtuales;

void setUp() {
    LedsCreate(&leds_virtuales);
}

void tearDown() {
}

// Con la inicialización todos los LEDs quedan apagados.
void test_todos_los_leds_inician_apagados() {
    uint16_t leds_virtuales = 0xFFFF;
    LedsCreate(&leds_virtuales);
    TEST_ASSERT_EQUAL(0x0, leds_virtuales);
}

// Prender un LED individual.
void test_prender_un_led_individual() {
    LedsSingleTurnOn(3);
    TEST_ASSERT_EQUAL(1 << 2, leds_virtuales);
}

// Apagar un LED individual.
void test_apagar_un_led_individual() {
    LedsSingleTurnOn(3);
    LedsSingleTurnOff(3);
    TEST_ASSERT_EQUAL(0, leds_virtuales);
}

// Prender y apagar múltiples LED’s.
void test_prender_y_apagar_varios_leds() {
    LedsSingleTurnOn(3);
    LedsSingleTurnOn(7);
    LedsSingleTurnOff(7);
    TEST_ASSERT_EQUAL(1 << 2, leds_virtuales);
}

// Prender todos los LEDs de una vez.
void test_prender_todos_los_leds() {
    LedsAllTurnOn();
    TEST_ASSERT_EQUAL(0xffff, leds_virtuales);
}

 // Apagar todos los LEDs de una vez.
 void test_apagar_todos_los_leds() {
    LedsAllTurnOn();
    LedsAllTurnOff();
    TEST_ASSERT_EQUAL(0x0, leds_virtuales);
 }

 // Consultar el estado de un LED que está encendido
 void test_estado_de_un_led_encendido() {
    LedsSingleTurnOn(12);
    LedsSingleTurnOn(14);

    TEST_ASSERT_EQUAL(1, LedsGetState(12));
    TEST_ASSERT_EQUAL(1, LedsGetState(14));
 }

 // Consultar el estado de un LED que está apagado
 void test_estado_de_un_led_apagado() {
    LedsAllTurnOn();
    LedsSingleTurnOff(6);
    LedsSingleTurnOff(7);

    TEST_ASSERT_EQUAL(0, LedsGetState(6));
    TEST_ASSERT_EQUAL(0, LedsGetState(7));
 }


// Revisar limites de los parametros
// - Testeamos casos 1, 16
 void test_revisar_limite_de_parametros() {

    /** Turn on tests */

    LedsSingleTurnOn(1);
    TEST_ASSERT_EQUAL(1, leds_virtuales);

    LedsAllTurnOff();

    LedsSingleTurnOn(16);
    TEST_ASSERT_EQUAL(1 << 15, leds_virtuales);

    /** Turn off tests */

    LedsAllTurnOn();

    LedsSingleTurnOff(1);
    TEST_ASSERT_EQUAL(0xffff - 1, leds_virtuales);

    LedsAllTurnOn();

    LedsSingleTurnOff(16);
    TEST_ASSERT_EQUAL(0xffff - (1 << 15), leds_virtuales);
 }

// Revisar parámetros fuera de los limites
 // - Se espera ignorar los valores fuera de rango 0 y 17+
 // - Se testea con todos los valores hasta alcanzar un overflow
 void test_revisar_parametros_fuera_de_limites() {

    /** Turn on tests, ignoring all */

    LedsSingleTurnOn(0);
    TEST_ASSERT_EQUAL(0, leds_virtuales);

    for (uint8_t i = 17; i > 0; i++) {
        LedsSingleTurnOn(i);
        TEST_ASSERT_EQUAL(0x0, leds_virtuales);
    }

    /** Turn off tests, ignoring all */

    LedsAllTurnOn();

    LedsSingleTurnOff(0);
    TEST_ASSERT_EQUAL(0xffff, leds_virtuales);

    for (uint8_t i = 17; i > 0; i++) {
        LedsSingleTurnOff(i);
        TEST_ASSERT_EQUAL(0xffff, leds_virtuales);
    }
 }
