#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include "../include/util.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "getline.h"


void main_util_test(void);

void print_segment_test(void);
void print_packet_test(void);
void print_frame_test(void);

void crc16_compute_test(void);

#endif