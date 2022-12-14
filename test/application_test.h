#ifndef APPLICATION_TEST_H
#define APPLICATION_TEST_H

#include "../include/application.h"
#include "../include/util.h"

#include <assert.h>
#include <stdio.h>


void main_application_test(void);

uint8_t compare_data(uint8_t *data1, uint8_t *data2, uint8_t len);
void print_data(uint8_t *data1, uint8_t *data2, uint8_t len);

void encrypt_data_test(void);
void decrypt_data_test(void);
void dual_encryption_test(void);

void application_data_test(void);

#endif