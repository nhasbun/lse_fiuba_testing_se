#pragma once

#include "stdint.h"
#include <stdbool.h>


void * new_session();
void * get_current_session();
int32_t get_current_session_id();
void * get_session(uint16_t id);
int * get_session_list();
void save_sample(uint8_t value, bool reset);
uint8_t get_sample(void * session, bool reset);
uint32_t get_sample_count(void * session);
int8_t delete_session(uint16_t id);
void set_save_last_session_flag(bool value);
bool read_save_last_session_flag(void);

