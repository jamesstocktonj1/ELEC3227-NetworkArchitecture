#ifndef TRANSPORT_TEST_H
#define TRANSPORT_TEST_H

#include "../include/transport.h"
#include "../include/application.h"
#include "../include/util.h"

#include <assert.h>
#include <stdio.h>


void main_transport_test(void);

void transport_state_machine_test(void);
void transport_state_connect_test(void);        // test CONNECT
void transport_state_accept_test(void);         // test ACCEPT
void transport_state_send_test(void);           // test SEND
void transport_state_ack_test(void);            // test ACK
void transport_state_nack_test(void);           // test NACK
void transport_state_close_test(void);          // test CLOSE

void transport_timeout_connect_test(void);
void transport_timeout_accept_test(void);
void transport_timeout_send_test(void);
void transport_timeout_ack_test(void);

void transport_state_connect_nack_test(void);   // test CONNECT NACK

void transport_communication_test(void);
void transport_resend_test(void);

void transport_encapsulate_data_test(void);
void transport_send_data_test(void);

void transport_timeout_test(void);

void transport_crc_test(void);
void transport_crc_mismatch_test(void);

#endif