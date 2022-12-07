#ifndef NETWORK_H
#define NETWORK_H

#include "util.h"

#define NET_MAX_SEGMENT_SIZE 121
#define NET_MAX_PACKET_SIZE 128

#define ROUTE_TIMEOUT_MS 2000
#define REQUEST_TIMEOUT_MS 500
#define REQUEST_MAX_AMOUNT 3
#define DEFAULT_NETWORK_SIZE 10

#define RREQ_ID 0
#define RREP_ID 1
#define RERR_ID 2
#define DATA_ID 3

#define CONTROL_1_BYTE 0
#define CONTROL_2_BYTE 1
#define SRC_ADDRESS_BYTE 2
#define DEST_ADDRESS_BYTE 3 
#define LENGTH_BYTE 4
#define TRAN_SEGMENT_BYTE 5

#define RREQ_ORIG_ADDRESS_BYTE 5
#define RREQ_ORIG_SEQ_BYTE 6
#define RREQ_DEST_SEQ_BYTE 7
#define RREQ_RREQ_ID_BYTE 8
#define RREQ_HOP_COUNT_BYTE 9

#define RREP_RREQ_DEST_ADDRESS_BYTE 5 
#define RREP_RREQ_ORIG_ADDRESS_BYTE 6
#define RREP_DEST_SEQ_BYTE 7
#define RREP_HOP_COUNT_BYTE 8

#define RREQ_PACKET_SIZE 10
#define RREP_PACKET_SIZE 9
#define RERR_PACKET_SIZE 7
#define DATA_PACKET_SIZE_NO_TRAN 5

#define QUEUE_MAX_SIZE 5

#define REQUEST_MAX_AMOUNT 3
#define RREQ_TTL 10




typedef struct {
    uint8_t dest_node;
    uint8_t next_hop; 
    uint8_t dest_seq;
    uint8_t hop_count;
} net_rt_entry;

typedef struct qrecord
{
    uint8_t packet[NET_MAX_PACKET_SIZE];
    uint8_t packet_size;
} qrecord;


void net_init(uint8_t node_address);

void net_handle_rx_packet(uint8_t *packet, uint8_t length);

uint8_t  net_handle_rreq(uint8_t *packet);
uint8_t  net_handle_rrep(uint8_t *packet);
void net_handle_rerr(uint8_t *packet);
void net_handle_data(uint8_t *packet, uint8_t length);

void send_rreq( uint8_t dest_node);
uint8_t send_data (  uint8_t dest_node,  uint8_t time_to_live, uint8_t *tran_segment, uint8_t tran_seg_length);
void send_rrep(uint8_t *packet);
void resend_packet(uint8_t *packet, uint8_t length);




#endif