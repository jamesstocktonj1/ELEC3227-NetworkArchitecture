#include "../include/network.h"

static uint8_t net_seqnum; 
static net_rt_entry *route_table;
static uint8_t RREQ_ID_buffer;
static uint8_t net_node_address;



void net_init(uint8_t node_address){
    net_seqnum = 0;
    route_table = calloc(DEFAULT_NETWORK_SIZE, sizeof(net_rt_entry));
    RREQ_ID_buffer = 0;
    net_node_address = node_address;
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

        case RREP_ID: net_handle_rrep(*data);
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
    rt_entry_src->hop count = packet[RREQ_HOP_COUNT_BYTE] + 1;

    }


    if (packet[DEST_ADDRESS_BYTE] == net_node_address || rt_entry_dest->dest_seq > packet[RREQ_ORIG_SEQ_BYTE])
        return 1;

    return 0;

}

void net_handle_rrep ( uint8_t *packet){

    net_rt_entry *rt_entry_dest = route_table(packet[RREP_RREQ_DEST_ADDRESS]);

    if((rt_entry_dest->dest_seq < packet[RREP_DEST_SEQ_BYTE]) || ((rt_entry_src->dest_seq == packet[RREP_DEST_SEQ_BYTE]) && rt_entry_src->hop_count > packet[RREP_HOP_COUNT_BYTE] ) ){
    

}