#include "../include/network.h"

static uint8_t net_seqnum; 
static net_rt_entry *route_table;
static uint8_t RREQ_ID_buffer;
static uint8_t net_node_address;

static qrecord queue[QUEUE_MAX_SIZE];
static int8_t front;
static int8_t back;

typedef struct qrecord
{
    uint8_t[NET_MAX_PACKET_SIZE] packet;
    uint8_t packet_size;
} qrecord;


bool enqueue(uint8_t *packet, uint8_t packet_size)
{
    if(back = QUEUE_MAX_SIZE - 1 )
        {
            return 0;
        }
    else
        {
            if(front == -1)
                front = 0;
            back++;
            qrecord temp = queue[back];
            for (int i=0, i<packet_size, i++)
            {
                temp->packet[i] = packet[i];
            }
            temp->packet_size = packet_size;
            return 1;
            
        }
}

bool dequeue (qrecord *buffer)
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


bool net_handle_rx_packet(uint8_t *packet){

    uint8 masked = data[0] & (3<<6);
    switch (masked)
    {
        case RREQ_ID: 
            if(net_handle_rreq(*data));
                send_rrep(*data);
            else
                rebroadcast_rreq(*data);

        case RREP_ID: 
            if(!net_handle_rrep(*data));
                send_rrep(*data);

        case RERR_ID: net_handle_rerr(*data);
        case DATA_ID: net_handle_data(*data);
    }
};

bool net_handle_rreq ( uint8_t *packet){

    net_rt_entry *rt_entry_src = route_table(packet[SRC_ADDRESS_BYTE]);
    net_rt_entry *rt_entry_dest = route_table(packet[DEST_ADDRESS_BYTE]);

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

bool net_handle_rrep ( uint8_t *packet){

    net_rt_entry *rt_entry_dest = route_table(packet[RREP_RREQ_DEST_ADDRESS_BYTE]);

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