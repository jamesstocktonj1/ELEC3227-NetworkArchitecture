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


static uint8_t rreq_id; 


uint8_t *dllRxPacket;
uint8_t dllRxLength;
uint8_t dllRxReady;
qrecord dllTxPacket;
uint8_t dllTxFlag;





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
            int i;
            for (i=0; i<packet_size; i++)
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

void send_packet()
{
    static int count = 0;
    static int sendflag = 0;


    
    if(count==0 && dllRxReady )
        {   
            if(dequeue(&dllTxPacket))
            {
                count++;
                sendflag=1;
            }

        }
    else
        {
            if(sendflag)
            {
                if((route_table[dllTxPacket.packet[DEST_ADDRESS_BYTE]]).next_hop != UNKNOWN_NEXT_HOP)
                {
                    dllTxFlag=1;
                    count = 0;
                    
                }
                else
                {
                    send_rreq(dllTxPacket.packet[DEST_ADDRESS_BYTE]);
                    count++;
                }
                sendflag = 0;
            }


        }
        
}



void net_init(uint8_t node_address){
    net_seqnum = 0;
    route_table = calloc(DEFAULT_NETWORK_SIZE, sizeof(net_rt_entry));
    RREQ_ID_buffer = 0;
    net_node_address = node_address;
    front = -1;
    back = -1;
    route_table[0].next_hop = 0;
    int i;
    for(i=1; i<10; i++)
    {
        route_table[i].next_hop = 255;
    }

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

    rt_entry_src->dest_node = packet[SRC_ADDRESS_BYTE];
    rt_entry_src->next_hop = packet[RREQ_SENDER_BYTE];
    rt_entry_src->dest_seq = packet[RREQ_ORIG_SEQ_BYTE];
    rt_entry_src->hop_count = packet[RREQ_HOP_COUNT_BYTE] + 1;

    }


    if (packet[DEST_ADDRESS_BYTE] == net_node_address || rt_entry_dest->dest_seq > packet[RREQ_ORIG_SEQ_BYTE])
        return 1;

    return 0;

}

uint8_t net_handle_rrep ( uint8_t *packet){

    net_rt_entry *rt_entry_dest = &route_table[packet[DEST_ADDRESS_BYTE]];
    net_rt_entry *rt_entry_src = &route_table[packet[SRC_ADDRESS_BYTE]];

    if((rt_entry_dest->dest_seq < packet[RREP_DEST_SEQ_BYTE]) || ((rt_entry_src->dest_seq == packet[RREP_DEST_SEQ_BYTE]) && rt_entry_src->hop_count > packet[RREP_HOP_COUNT_BYTE] ) ){
        rt_entry_dest->dest_node = packet[SRC_ADDRESS_BYTE];
        rt_entry_dest->next_hop = packet[RREP_SENDER_BYTE];
        rt_entry_dest->dest_seq = packet[RREP_DEST_SEQ_BYTE];
        rt_entry_dest->hop_count = packet[RREP_HOP_COUNT_BYTE];
    
    }

    if(packet[DEST_ADDRESS_BYTE] == net_node_address)
        return 1;

    return 0;
    

}


uint8_t net_handle_data(uint8_t *packet, uint8_t length)
{
    if(packet[DEST_ADDRESS_BYTE] == net_node_address)
        {
            transportRxFlag = 1;
            transportRxSegment.control = packet[CONTROL_1_BYTE];
            transportRxSegment.control = transportRxSegment.control<<8;
            transportRxSegment.control = transportRxSegment.control + packet[CONTROL_2_BYTE];
            transportRxSegment.source = packet[SRC_ADDRESS_BYTE];
            transportRxSegment.destination = packet[DEST_ADDRESS_BYTE];
            transportRxSegment.length = packet[LENGTH_BYTE];
            memcpy(&(transportRxSegment.data), &packet[TRAN_SEGMENT_BYTE], length - DATA_PACKET_SIZE_NO_TRAN);
            transportRxSegment.checksum = packet[length - 1];
            
            return 1;
        }
    else
        {
            enqueue(packet, length);
            return 0;
        }
        
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
    enqueue(packet, tran_seg_length + DATA_PACKET_SIZE_NO_TRAN);


    if (&route_table[dest_node] == NULL)
        count++;


    if(count==REQUEST_MAX_AMOUNT)
    {
        count = 0;
        transportErrorFlag = 1;
        return 0;
    }

    return 1;
}

void send_rreq( uint8_t dest_node)
{
    net_seqnum++;


    uint8_t packet[RREQ_PACKET_SIZE];
    packet[CONTROL_1_BYTE] = 0;
    packet[CONTROL_1_BYTE] |= RREQ_ID<<6;
    packet[CONTROL_1_BYTE] |= RREQ_TTL<<4;
    packet[CONTROL_2_BYTE] = 0;
    packet[SRC_ADDRESS_BYTE] = net_node_address;
    packet[DEST_ADDRESS_BYTE] = dest_node;
    packet[LENGTH_BYTE] = RREQ_PACKET_SIZE;
    packet[RREQ_SENDER_BYTE] = net_node_address;
    packet[RREQ_ORIG_SEQ_BYTE] = net_seqnum;
    packet[RREQ_DEST_SEQ_BYTE] = 0;
    packet[RREQ_RREQ_ID_BYTE] = rreq_id;
    packet[RREQ_HOP_COUNT_BYTE] = 0;

    enqueue(packet, RREQ_PACKET_SIZE);
    rreq_id++;

}

void send_rrep(uint8_t *packet)
{
    net_seqnum++;

    uint8_t output_packet[RREP_PACKET_SIZE];
    output_packet[CONTROL_1_BYTE] = 0;
    output_packet[CONTROL_1_BYTE] |= RREP_ID<<6;
    output_packet[CONTROL_1_BYTE] |= RREP_TTL<<4;
    output_packet[CONTROL_2_BYTE] = 0;
    output_packet[SRC_ADDRESS_BYTE] = net_node_address;
    output_packet[DEST_ADDRESS_BYTE] = packet[SRC_ADDRESS_BYTE];
    output_packet[LENGTH_BYTE] = RREP_PACKET_SIZE;
    output_packet[RREP_DEST_SEQ_BYTE] = net_seqnum;
    output_packet[RREP_HOP_COUNT_BYTE] = 0;

    enqueue(output_packet, RREQ_PACKET_SIZE);


}


void resend_packet(uint8_t *packet, uint8_t length)
{
    packet[SRC_ADDRESS_BYTE] = net_node_address;
    uint8_t ttl = packet[CONTROL_1_BYTE]>>2 & 15;
    packet[CONTROL_1_BYTE] &= (15 << 2);
    packet[CONTROL_1_BYTE] |= ttl-1;
    packet[RREP_HOP_COUNT_BYTE] = packet[RREP_HOP_COUNT_BYTE] + 1;
    enqueue(packet, length);



                //SSSSSOOOOOOOLLLLLLLLVVVVVVEEEEEEE
                //Change the next_hop assignment : on enqueue => on dequeue (use the most recent routing table)
                //interface for transport layer
                //if no record in route table: enqueue anyway
                //poll to check if record in route table
                //if polled multiple times and no records: discard segment raise flag ( rerr ? )


    /*if((packet[0] & (3<<6)) == RREP_ID)
    {
        
        enqueue(packet, RREP_PACKET_SIZE);
    }
    if((packet[0] & (3<<6)) == DATA_ID)
    {
        packet[RREP_HOP_COUNT_BYTE] = packet[RREP_HOP_COUNT_BYTE] + 1;
        );
    }

       if((packet[0] & (3<<6)) == RREQ_ID)
    {
        //DO SOMETHING //
        //packet[RREP_HOP_COUNT_BYTE] = packet[RREP_HOP_COUNT_BYTE] + 1;
        enqueue(packet, length);
    }
*/
    



}






void net_handle_rerr(uint8_t *packer)

{

}










