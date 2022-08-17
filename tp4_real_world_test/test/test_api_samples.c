#include "unity.h"
#include "api_samples.h"


// Bitwise operation macros
// https://www.codementor.io/@hbendali/c-c-macro-bit-operations-ztrat0et6
#define bitset(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define bitflip(byte,nbit)  ((byte) ^=  (1<<(nbit)))
#define bitcheck(byte,nbit) ((byte) &   (1<<(nbit))) >> nbit


/**
 * @brief Lista de requerimientos...
 * 
 * - Se puede crear y recuperar una nueva sesión representada por un puntero a memoria
 *     + Lista de sesiones no devuelve elementos (retorna -1)
 *     + Se se envía save, la lista devuelve un solo número de id en la lista (distinto de -1)
 *     + Si se crea una nueva sesión esta no debe aparecer en la lista (no se marca con save)
 *     + Ocupando el identificador se puede borrar una sesión existente
 * - Nuevas sesiones se pueden recuperar por su id
 * - Luego de almacenar una sesión la siguiente no se guarda
 * - Una sesión puede almacenar y recuperar 2 bits
 * - Una sesión puede almacenar y recuperar 2000 bits
 */

void setUp() {}

void tearDown() {}

// Se puede crear y recuperar una nueva sesión representada por un puntero a memoria
void test_new_session_valid() {

    void * session = new_session();

    TEST_ASSERT_FALSE(session == NULL);
    TEST_ASSERT(session == get_current_session());
}

// Lista de sesiones no devuelve elementos (retorna -1)
void test_empty_list() {
    int * id_list = get_session_list();
    TEST_ASSERT(*id_list == -1); 
}

// Se se envía save, la lista devuelve un solo número de id en la lista (distinto de -1)
void test_save_session_show_in_list() {
    set_save_last_session_flag(1);
    int * id_list = get_session_list();
    
    TEST_ASSERT(*id_list != -1);

    // Verificamos que no existan más elementos
    id_list++;
    TEST_ASSERT_TRUE(*id_list == -1);
}

// Si se crea una nueva sesión esta no debe aparecer en la lista (no se marca con save)
void test_new_session_after_save_dont_show() {
    new_session();
    int * id_list = get_session_list();

    TEST_ASSERT(*id_list != -1);
    id_list++;
    TEST_ASSERT(*id_list == -1);
}

// Ocupando el identificador se puede borrar la sesión existente
void test_delete_session() {
    int * id_list = get_session_list();

    int first_id = *id_list;
    delete_session(first_id);
    id_list = get_session_list();

    TEST_ASSERT(*id_list != first_id);
}

// Nuevas sesiones se pueden recuperar por su id
void test_session_recovery_by_id() {
    
    void * session = new_session();
    set_save_last_session_flag(1);  // set session for save
    new_session(); // saving is actually done when entering a new session
    
    int last_id;
    for(int * id_list = get_session_list(); *id_list != -1; id_list++) {
        last_id = *id_list;
    }

    TEST_ASSERT(get_session(last_id) == session);
}

// Una sesión puede almacenar y recuperar 2 bits
void test_storage_2bits() {
    void * session = new_session();

    save_sample(1, 1); // Second argument is first sample signal
    save_sample(1, 0);

    // Second argument of get sample indicates recover first sample signal
    TEST_ASSERT(get_sample(session, 1) == 1);
    TEST_ASSERT(get_sample(session, 0) == 1);
}

// Una sesión puede almacenar y recuperar 2000 bits
void test_storage_2000bits() {
    void * session = new_session();

    uint8_t value = 0;

    for (uint16_t i = 0; i < 2000; i++) {
        save_sample(value, !i);
        value = !value;
    }

    uint8_t expected_value = 0;
    for (uint16_t i = 0; i < 2000; i++) {
        TEST_ASSERT(get_sample(session, !i) == expected_value);
        expected_value = !expected_value;
    }
}