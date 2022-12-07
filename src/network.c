#include "../include/network.h"
#include <stdlib.h>
#include <string.h>

static uint8_t net_seqnum; 
static net_rt_entry *route_table;
static uint8_t RREQ_ID_buffer;
static uint8_t net_node_address;

static qrecord queue[QUEUE_MAX_SIZE];
static int8_t front;
static int8_t back;

static uint8_t node_unreachable; 

static uint8_t next_hop; 




uint8_t enqueue(uint8_t *packet, uint8_t packet_size)
{
    if(back == QUEUE_MAX_SIZE - 1 )
        {
            return 0;
        }
    else
        {
            if(front == -1)
                front = 0;
            back++;
            for (int i=0; i<packet_size; i++)
            {
                queue[back].packet[i] = packet[i];
            }
            queue[back].packet_size = packet_size;
            return 1;
            
        }
}

uint8_t dequeue (qrecord *buffer)
{
    if(front == -1)
        return 0;
    else
        {
            qrecord temp = queue[front];
            front++;
            if (front > back)
                front = back = -1;
            *buffer = temp;
            return 1;
        }
}


void net_init(uint8_t node_address){
    net_seqnum = 0;
    route_table = calloc(DEFAULT_NETWORK_SIZE, sizeof(net_rt_entry));
    RREQ_ID_buffer = 0;
    net_node_address = node_address;
    front = -1;
    back = -1;
}


void net_handle_rx_packet(uint8_t *packet, uint8_t length){

    uint8_t masked = packet[0] & (3<<6);
    switch (masked)
    {
        case RREQ_ID: 
        {
            if(net_handle_rreq(packet))
                {
                    send_rrep(packet);
                }
                
            else
                {
                    resend_packet(packet, length);
                }
                
        }
        break;

        case RREP_ID: 
        {
            if(!net_handle_rrep(packet))
                {
                    send_rrep(packet);
                }
                
        }
        break;

        case RERR_ID: 
            net_handle_rerr(packet);
        break;

        case DATA_ID: 
            net_handle_data(packet, length);
        break;
    }
};

uint8_t net_handle_rreq ( uint8_t *packet){

    net_rt_entry *rt_entry_src = &route_table[packet[SRC_ADDRESS_BYTE]];
    net_rt_entry *rt_entry_dest = &route_table[packet[DEST_ADDRESS_BYTE]];

    if((rt_entry_src->dest_seq < packet[RREQ_ORIG_SEQ_BYTE]) || ((rt_entry_src->dest_seq == packet[RREQ_ORIG_SEQ_BYTE]) && rt_entry_src->hop_count > packet[RREQ_HOP_COUNT_BYTE] ) ){

    rt_entry_src->dest_node = packet[RREQ_ORIG_ADDRESS_BYTE];
    rt_entry_src->next_hop = packet[SRC_ADDRESS_BYTE];
    rt_entry_src->dest_seq = packet[RREQ_ORIG_SEQ_BYTE];
    rt_entry_src->hop_count = packet[RREQ_HOP_COUNT_BYTE] + 1;

    }


    if (packet[DEST_ADDRESS_BYTE] == net_node_address || rt_entry_dest->dest_seq > packet[RREQ_ORIG_SEQ_BYTE])
        return 1;

    return 0;

}

uint8_t net_handle_rrep ( uint8_t *packet){

    net_rt_entry *rt_entry_dest = &route_table[packet[RREP_RREQ_DEST_ADDRESS_BYTE]];
    net_rt_entry *rt_entry_src = &route_table[packet[SRC_ADDRESS_BYTE]];

    if((rt_entry_dest->dest_seq < packet[RREP_DEST_SEQ_BYTE]) || ((rt_entry_src->dest_seq == packet[RREP_DEST_SEQ_BYTE]) && rt_entry_src->hop_count > packet[RREP_HOP_COUNT_BYTE] ) ){
        rt_entry_dest->dest_node = packet[RREP_RREQ_DEST_ADDRESS_BYTE];
        rt_entry_dest->next_hop = packet[SRC_ADDRESS_BYTE];
        rt_entry_dest->dest_seq = packet[RREP_DEST_SEQ_BYTE];
        rt_entry_dest->hop_count = packet[RREP_HOP_COUNT_BYTE];
    
    }

    if(packet[RREP_RREQ_ORIG_ADDRESS_BYTE] == net_node_address)
        return 1;

    return 0;
    

}

uint8_t send_data (  uint8_t dest_node,  uint8_t time_to_live, uint8_t *tran_segment, uint8_t tran_seg_length)
{
    static uint8_t count = 0;

    uint8_t packet[tran_seg_length + DATA_PACKET_SIZE_NO_TRAN ];
    packet[CONTROL_1_BYTE] |= DATA_ID<<6;
    packet[CONTROL_1_BYTE] |= time_to_live<<4;
    packet[CONTROL_2_BYTE] = 0;
    packet[SRC_ADDRESS_BYTE] = net_node_address;
    packet[DEST_ADDRESS_BYTE] = dest_node;
    packet[LENGTH_BYTE] = tran_seg_length + DATA_PACKET_SIZE_NO_TRAN ;
    memcpy(&packet[TRAN_SEGMENT_BYTE], tran_segment, tran_seg_length);



    if (&route_table[dest_node] != NULL)
        enqueue(packet, tran_seg_length + DATA_PACKET_SIZE_NO_TRAN);
    else 
    {
        send_rreq(dest_node);
        count++;
    }


    

    if(count==REQUEST_MAX_AMOUNT)
    {
        count = 0;
        node_unreachable = 1;
        return 0;
    }

    return 1;
}

void send_rreq( uint8_t dest_node)
{
    static uint8_t count = 0;

    uint8_t packet[RREQ_PACKET_SIZE];
    packet[CONTROL_1_BYTE] |= RREQ_ID<<6;
    packet[CONTROL_1_BYTE] |= RREQ_TTL<<4;
    packet[CONTROL_2_BYTE] = 0;
    packet[SRC_ADDRESS_BYTE] = net_node_address;
    packet[DEST_ADDRESS_BYTE] = dest_node;
    packet[LENGTH_BYTE] = RREQ_PACKET_SIZE;
    packet[RREQ_ORIG_ADDRESS_BYTE] = net_node_address;
    packet[RREQ_ORIG_SEQ_BYTE] = net_seqnum;
    packet[RREQ_DEST_SEQ_BYTE] = 0;
    packet[RREQ_HOP_COUNT_BYTE] = 0;

    enqueue(packet, RREQ_PACKET_SIZE);

}

void resend_packet(uint8_t *packet, uint8_t length)
{
    packet[SRC_ADDRESS_BYTE] = net_node_address;
    uint8_t ttl = packet[CONTROL_1_BYTE]>>2 & 15;
    packet[CONTROL_1_BYTE] &= (15 << 2);
    packet[CONTROL_1_BYTE] |= ttl-1;
    next_hop = route_table[packet[DEST_ADDRESS_BYTE]].next_hop;

    if((packet[0] & (3<<6)) == RREP_ID)
    {
        packet[RREP_HOP_COUNT_BYTE] = packet[RREP_HOP_COUNT_BYTE] + 1;
        enqueue(packet, RREP_PACKET_SIZE);
    }
    if((packet[0] & (3<<6)) == DATA_ID)
    {
        packet[RREP_HOP_COUNT_BYTE] = packet[RREP_HOP_COUNT_BYTE] + 1;
        enqueue(packet, length);
    }

       if((packet[0] & (3<<6)) == RREQ_ID)
    {
        //DO SOMETHING //
        //packet[RREP_HOP_COUNT_BYTE] = packet[RREP_HOP_COUNT_BYTE] + 1;
        enqueue(packet, length);
    }

    



}

void send_rrep(uint8_t *packet)
{

}

void net_handle_data(uint8_t *packet, uint8_t length)
{

}


void net_handle_rerr(uint8_t *packer)

{

}










