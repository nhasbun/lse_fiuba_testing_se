#include <stddef.h>
#include "api_samples.h"
#include <stdbool.h>

#ifdef __API_SAMPLES_TESTING__
#include "stdlib.h"
#define pvPortMalloc(x) malloc(x)
#define vPortFree(x) free(x)
#else
#include "FreeRTOS.h"
#endif

// Defines -------------------------------------------------------------------------

// Bitwise operation macros
// https://www.codementor.io/@hbendali/c-c-macro-bit-operations-ztrat0et6
#define bitset(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define bitflip(byte,nbit)  ((byte) ^=  (1<<(nbit)))
#define bitcheck(byte,nbit) ((byte) &   (1<<(nbit))) >> nbit

#define SAMPLE_VALUE_MAX_BIT 8
#define SAMPLE_VALUES_ARRAY_MAX 64

// Types -------------------------------------------------------------------------

/**
 * Sample window is capable of bitwise storing samples.
 * Also values are stored in a 64 elements array.
 *
 * For 20 ms period this gives:
 * 8*64 = 512 samples
 * 512 * 20 ms = 10240 ms window coverage
 */
typedef struct sample_window {
    uint8_t values[SAMPLE_VALUES_ARRAY_MAX];
    struct sample_window * next_sample_window;
} sample_window;


typedef struct rec_session {
    uint16_t id;
    uint32_t sample_count;
    sample_window * first_sample_window;
    struct rec_session * previous_session;
    struct rec_session * next_session;
} rec_session;


static rec_session * first_session = NULL;
static rec_session * current_session = NULL;
static bool last_session_saved = false;


static void delete_all_sample_window(sample_window * first_sample_window);


/**
 * Create new rec rec_session.
 *
 * @return pointer actual recorded rec_session
 */
void  * new_session() {

    // We discard any existing alive session not requested for saving
    if (!read_save_last_session_flag()) {
        int32_t session_id = get_current_session_id();
        if (session_id != -1) {
            delete_session(session_id);
        }
    }

    set_save_last_session_flag(0);

    rec_session * new_session = pvPortMalloc(sizeof(rec_session));

    if (first_session == NULL) {
        first_session = new_session;
        current_session = first_session;

        current_session -> previous_session = NULL;
        current_session -> next_session = NULL;
        current_session -> id = 0;

    } else {

        rec_session * last_session = current_session;
        last_session -> next_session = new_session;

        current_session = new_session;
        current_session -> previous_session = last_session;
        current_session -> next_session = NULL;
        current_session -> id = last_session -> id + 1;
    }

    // Creating first sample window for rec_session
    sample_window * new_sample_window = pvPortMalloc(sizeof(sample_window));
    {
        // Clear memory to avoid bugs (when recycling memory we have leftovers)
        new_sample_window -> next_sample_window = NULL;
    }

    current_session -> first_sample_window = new_sample_window;
    current_session -> sample_count = 0;

    return (void *) current_session;
}

/**
 * Get last created rec session.
 *
 * Useful for immediately play recordings.
 * @return memory address of last session struct
 */
void * get_current_session() {
    return (void*) current_session;
}


/**
 * Access recorded rec_session pointer using id
 * @param id id of rec_session
 * @return pointer to rec_session if id matches, null otherwise
 */
void * get_session(uint16_t id) {

    rec_session * _session = NULL;

    if (first_session == NULL) return _session;

    for (rec_session * i = first_session; i != NULL; i = i -> next_session) {

        if (i->id == id) _session = i;
    }

    return _session;
}


/**
 * @brief Dynamically storing button sample values.
 *
 * Intended to be called periodically for recording values.
 * Reset flag helps when we are recording a new session but we
 * have a match with a previous deleted one.
 *
 * @param value value to be stored (1 or 0)
 * @param reset flag to indicate first sample incoming
 */
void save_sample(uint8_t value, bool reset) {

    static uint8_t array_id = 0;
    static uint8_t bit_id = 0;
    static rec_session * save_session = NULL;
    static sample_window * save_sample_window = NULL;


    if (save_session != current_session || reset == true) {
        array_id = 0;
        bit_id = 0;
        save_session = current_session;
        save_sample_window = save_session -> first_sample_window;
        save_sample_window -> next_sample_window = NULL;
    }

    if (value) {
        bitset(save_sample_window -> values[array_id], bit_id);

    } else {
        bitclear(save_sample_window -> values[array_id], bit_id);
    }

    current_session -> sample_count++;

    bit_id++;

    if (bit_id > SAMPLE_VALUE_MAX_BIT - 1) {
        bit_id = 0;
        array_id++;
    }

    if (array_id > SAMPLE_VALUES_ARRAY_MAX - 1) {
        array_id = 0;

        sample_window * new_sample_window = pvPortMalloc(sizeof(sample_window));
        save_sample_window -> next_sample_window = new_sample_window;
        save_sample_window = new_sample_window;
    }
}


/**
 * Get available rec_session list
 * @return list of id numbers, end of array is indicated with value -1
 */
int * get_session_list() {
    static int * id_list = NULL;

    if (id_list != NULL) vPortFree(id_list);

    // Session counter loop
    uint32_t session_count = 0;
    rec_session * session_pointer = first_session;

    while (session_pointer != NULL) {
        session_count++;
        session_pointer = session_pointer -> next_session;
    }
    //

    id_list = pvPortMalloc(sizeof(int) * (session_count + 1));
    int * start_id_list = id_list;

    for (rec_session * i = first_session; i != NULL; i = i -> next_session) {
        *id_list = i -> id;
        id_list++;
    }

    *id_list = -1;  // ending int list with -1 value

    // If last session in use is not stored we are not showing it on list
    if (last_session_saved == false && session_count > 0) *(id_list - 1) = -1;

    // returning id_list pointer to initial position, needed for correct work of free fn
    id_list = start_id_list;
    return id_list;
}


uint8_t get_sample(void * session, bool reset) {

    static uint32_t get_count = 0;
    static uint8_t array_id = 0;
    static uint8_t bit_id = 0;
    static rec_session * get_session = NULL;
    static sample_window * get_sample_window = NULL;

    if (session != get_session || reset) {

    	get_session = session;
        get_sample_window = get_session -> first_sample_window;
        get_count = 0;
        array_id = 0;
        bit_id = 0;
    }

    uint8_t value = bitcheck(get_sample_window -> values[array_id], bit_id);

    get_count++;

    bit_id++;

    if (bit_id > SAMPLE_VALUE_MAX_BIT - 1) {
        bit_id = 0;
        array_id++;
    }

    if (array_id > SAMPLE_VALUES_ARRAY_MAX - 1) {
        array_id = 0;

        get_sample_window = get_sample_window -> next_sample_window;
    }

    return value;
}


uint32_t get_sample_count(void * session) {

    return ((rec_session*) session) -> sample_count;
}


/**
 * Delete desired session from list.
 *
 * If no session exists it skips any action and return an error (code -1).
 *
 * @param id sessions are identified by an id number
 * @return 0 if session deleted correctly
 */
int8_t delete_session(uint16_t id) {

    // Safe checks
    if (first_session == NULL) return -1;

    rec_session * session_pointer = first_session;

    // Search for matching id loop
    for(;;) {
        if (session_pointer -> id == id) {
            // We have a match, leaving loop
            break;
        }

        if (session_pointer -> next_session ==  NULL) {
            // No more sessions left to look for matching id, leaving with error
            return -1;
        }

        session_pointer = session_pointer -> next_session;
    }

    // Borderline case when user somehow guesses id number of session alive
    if (session_pointer -> id == current_session -> id && last_session_saved == false) {
        return -1;
    }

    // We must keep sessions linked list together before deletion
    if (session_pointer -> previous_session == NULL && session_pointer -> next_session == NULL) {
        // Current session is alone
        first_session = NULL;
    }
    else if (session_pointer -> previous_session == NULL) {
        // Current session is first
        first_session = session_pointer -> next_session; // next session becomes first
        first_session -> previous_session = NULL; // first session doesn't have previous session

    } else if (session_pointer -> next_session == NULL) {
        // Current session is last
        session_pointer -> previous_session -> next_session = NULL;

    } else {
        // Current session is somewhere in the middle
        session_pointer -> previous_session -> next_session = session_pointer -> next_session;
        session_pointer -> next_session -> previous_session = session_pointer -> previous_session;
    }

    // Freeing mem for structs without references
    delete_all_sample_window(session_pointer -> first_sample_window); // delete samples
    vPortFree(session_pointer); // delete session info
    return 0;
}


/**
 * Returns current session id.
 *
 * Useful when user is not saving the current recorded session (we can delete it).
 *
 * @return -1 for errors, otherwise deliver session id as a uin16_t number
 */
int32_t get_current_session_id() {
    if (current_session == NULL) {
        return -1;
    } else {
        return (int32_t)(current_session -> id);
    }
}


// Private functions
static void delete_all_sample_window(sample_window * first_sample_window) {

    // Safe checks
    if (first_sample_window == NULL) return;

    // Releasing memory for sample window deleted
    sample_window * sample_window_pointer = first_sample_window;
    sample_window * next_sample_window = first_sample_window -> next_sample_window;

    for(;;) {
        vPortFree(sample_window_pointer);

        if (next_sample_window ==  NULL) break;
        else {
            sample_window_pointer = next_sample_window;
            next_sample_window = sample_window_pointer -> next_sample_window;
        }
    }
}

void set_save_last_session_flag(bool value) {
	last_session_saved = value;
}

bool read_save_last_session_flag(void) {
	return last_session_saved;
}
