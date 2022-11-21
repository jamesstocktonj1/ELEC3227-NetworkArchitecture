#ifndef CONFIG_TEST_H
#define CONFIG_TEST_H

#include "../include/config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "getline.h"


void main_config_test(void);

void print_segment_test(void);
void print_packet_test(void);
void print_frame_test(void);

void crc16_compute_test(void);

#endif