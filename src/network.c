#include "../include/network.h"
#include "../include/util.h"
#include <stdlib.h>


static uint8_t net_seqnum; 
static net_rt_entry route_table[DEFAULT_NETWORK_SIZE];
static uint8_t RREQ_ID_buffer;
static uint8_t net_node_address;

static qrecord queue[QUEUE_MAX_SIZE];
static int front;
static int back;
static qrecord tx_buffer;


static uint8_t rreq_id; 



uint8_t *dllRxPacket;
uint8_t dllRxLength;
uint8_t dllRxReady;
qrecord dllTxPacket;
uint8_t dllTxFlag;

uint8_t *net_tx_buffer[NET_MAX_PACKET_SIZE];

static uint16_t net_timer_rt;
static uint16_t net_timer_rreq;


void net_reset_timer_rreq()
{
    net_timer_rreq = NET_RREQ_TIMEOUT_MS;
}

void net_reset_timer_rt()
{
    net_timer_rt = NET_ROUTE_TABLE_TIMEOUT_MS;
}

void net_update_timer_rt()
{
        if(net_timer_rt) 
        {
            net_timer_rt--;
        }
}

void net_update_timer_rreq()
{
        if(net_timer_rreq) 
        {
            net_timer_rreq--;
        }
}



uint8_t net_timeout_rt() {
    return net_timer_rt == 0;
}

uint8_t net_timeout_rreq() {
    return net_timer_rreq == 0;
}


void net_handle_timeout_rt()
{
    if (net_timeout_rt())
    {
        int i;
        for (i=0; i<DEFAULT_NETWORK_SIZE; i++)
        {
            route_table[i].dest_node = i;
            route_table[i].dest_seq = 0;
            route_table[i].next_hop = 255;
            route_table[i].hop_count = 255;
        }
    }
}




uint8_t enqueue(uint8_t *packet, uint8_t packet_size)
{
    if(back == QUEUE_MAX_SIZE - 1 )
        {
            back = -1;
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

            #ifdef NET_DBG_QUEUE
            fprintf(stderr,"\npacket enqueued %d\n", back);
            for (i = 0; i < queue[back].packet_size; i++) fprintf(stderr, "%02x ", queue[back].packet[i]);
                printf("\n");
            #endif

            return 1;
            
        }
}

uint8_t dequeue (qrecord *buffer)
{
        if (front == -1)
        {
            return 0;
        }
            *buffer = queue[front];
            #ifdef NET_DBG_QUEUE
            fprintf(stderr,"\npacket dequeued %d\n", front);
            uint8_t i;
            for (i = 0; i < queue[back].packet_size; i++) fprintf(stderr, "%02x ", queue[front].packet[i]);
                printf("\n");
            #endif
            front++;
            if (front > back)
                front = back = -1;

            return 1;
}

void net_transport_poll()
{
    if(transportTxFlag == 1)
    {
         #ifdef NET_DEBUG
        fprintf(stderr, "Transport Queue Packet\n");
        #endif
        uint8_t tx_packet[121];


        tx_packet[CONTROL_1_BYTE] = transportTxSegment.control>>8;
        tx_packet[CONTROL_2_BYTE] = transportTxSegment.control;
        tx_packet[SRC_ADDRESS_BYTE] = transportTxSegment.source;
        tx_packet[DEST_ADDRESS_BYTE] = transportTxSegment.destination;
        tx_packet[LENGTH_BYTE] = transportTxSegment.length;
        memcpy(tx_packet + TRAN_SEGMENT_BYTE, transportTxSegment.data, transportTxSegment.length );
        tx_packet[TRAN_SEGMENT_BYTE + transportTxSegment.length] = transportTxSegment.checksum>>8;
        tx_packet[TRAN_SEGMENT_BYTE + transportTxSegment.length + 1] = transportTxSegment.checksum&0xff;
        send_data(transportTxAddress, tx_packet, transportTxSegment.length + DATA_PACKET_SIZE_NO_TRAN );

        // printf("transport buffer: ");
        // uint8_t i;
        // for (i = 0; i < (7+transportTxSegment.length); i++) printf("%02x ", tx_packet[i]);
        // printf("\n");
                

        transportTxFlag = 0;

    }

}

uint8_t net_tx_poll()
{
    if(dequeue(&tx_buffer))
    {
        return 1;
    }
    return 0 ;
}

qrecord net_handle_tx()
{
    qrecord output;
    output.packet_size = 0;
    output.next_hop = 0;


    uint8_t masked = tx_buffer.packet[0] & (3<<6);
    masked = masked>>6;
    if(masked == RREQ_ID)
    {
#ifdef NET_DBG_ROUTING
        fprintf(stderr,"RREQ wil be sent\n");
#endif
        memcpy(output.packet, tx_buffer.packet, tx_buffer.packet_size);
        output.next_hop = 0;
        output.packet_size = RREQ_PACKET_SIZE;
        net_reset_timer_rreq();
        return output;
    }


    else if (route_table[tx_buffer.packet[DEST_ADDRESS_BYTE]].next_hop != UNKNOWN_NEXT_HOP)
    {
#ifdef NET_DBG_ROUTING
        fprintf(stderr,"Route table entry exists\n");
#endif  
        memcpy(output.packet, tx_buffer.packet, tx_buffer.packet_size);
        output.next_hop = route_table[tx_buffer.packet[DEST_ADDRESS_BYTE]].next_hop;
        output.packet_size = tx_buffer.packet_size;
        return output;
    }
    else
    {
        if(net_timeout_rreq())
        {  
#ifdef NET_DBG_ROUTING
            fprintf(stderr,"No route table entry\n");
#endif    
            send_rreq(tx_buffer.packet[DEST_ADDRESS_BYTE]);
        }

        enqueue(tx_buffer.packet, tx_buffer.packet_size);

        return output;
    }

}


/*uint8_t net_handle_tx(uint8_t *packet)
{
    if(transportTxFlag)
    {
       uint8_t tx_packet[7+transportTxSegment.length];

            tx_packet[CONTROL_1_BYTE] = transportTxSegment.control>>8;
            tx_packet[CONTROL_2_BYTE] = transportTxSegment.control;
            tx_packet[SRC_ADDRESS_BYTE] = transportTxSegment.source;
            tx_packet[DEST_ADDRESS_BYTE] = transportTxSegment.destination;
            tx_packet[LENGTH_BYTE] = transportTxSegment.length;
            memcpy(&tx_packet[TRAN_SEGMENT_BYTE], &(transportRxSegment.data), transportTxSegment.length );
            tx_packet[TRAN_SEGMENT_BYTE + transportTxSegment.length] = transportTxSegment.checksum>>8;
            tx_packet[TRAN_SEGMENT_BYTE + transportTxSegment.length + 1] = transportTxSegment.checksum;
            send_data(transportTxAddress, transportTxSegment.data, transportTxSegment.length );

    }
    static uint8_t stop_dequeue = 0;
    static uint8_t count = 0;
    static qrecord buffer;

    if(!stop_dequeue)
    {
        dequeue(&buffer);
    }

    fprintf(stderr,"dest: %d\n", buffer.packet[DEST_ADDRESS_BYTE]);

    if (route_table[buffer.packet[DEST_ADDRESS_BYTE]].next_hop != UNKNOWN_NEXT_HOP)
    {
        fprintf(stderr,"Route table entry exists\n");
        memcpy(packet, buffer.packet, buffer.packet_size);
        stop_dequeue = 0;
        return 1;
    }
    else   
    {
        fprintf(stderr,"No route table entry\n");
        send_rreq(buffer.packet[DEST_ADDRESS_BYTE], packet);
        stop_dequeue = 1;
        count++;
        if(count == 4)
        {
            count = 0;
            stop_dequeue = 0;
        }
        return 0;
    }
}*/





void net_init(uint8_t net_node_addr){
    net_timer_rreq = 0;
    net_reset_timer_rt();
    net_seqnum = 0;
    RREQ_ID_buffer = 0;
    net_node_address = net_node_addr;
    front = -1;
    back = -1;
    route_table[0].next_hop = 0;
    int i;
    for(i=1; i<DEFAULT_NETWORK_SIZE; i++)
    {
        route_table[i].next_hop = 255;
        route_table[i].dest_seq = 0;
        route_table[i].hop_count = 255;
        route_table[i].dest_node = i;
    }

    printf("Network layer initialised\n");

}


void net_handle_rx_packet(uint8_t *packet, uint8_t length){

    uint8_t masked = packet[0] & (3<<6);
    masked = masked>>6;

    switch (masked)
 
    {
        case RREQ_ID: 
        {
        #ifdef NET_DEBUG
        fprintf(stderr, "RREQ packet received\n");
        #endif
            if(net_handle_rreq(packet))
                {
                    #ifdef NET_DEBUG
                    fprintf(stderr,"RREP queued\n");
                    #endif
                    send_rrep(packet);
                }
                
        }
        break;

        case RREP_ID: 
        {
            #ifdef NET_DEBUG
            fprintf(stderr, "RREP packet received\n");
            #endif
            net_handle_rrep(packet);
                
        }
        break;

        case RERR_ID: 
        {
            #ifdef NET_DEBUG
            fprintf(stderr, "RERR packet received\n");
            #endif
            net_handle_rerr(packet);
        }
        break;

        case DATA_ID: 
        {
            #ifdef NET_DEBUG
            fprintf(stderr, "DATA packet received\n");
            #endif
            net_handle_data(packet, length);
        }
        break;
    }
};

uint8_t net_handle_rreq ( uint8_t *packet){



    if((route_table[packet[SRC_ADDRESS_BYTE]].dest_seq < packet[RREQ_ORIG_SEQ_BYTE]) || ((route_table[packet[DEST_ADDRESS_BYTE]].dest_seq == packet[RREQ_ORIG_SEQ_BYTE]) && route_table[packet[DEST_ADDRESS_BYTE]].hop_count > packet[RREQ_HOP_COUNT_BYTE] ) ){

    route_table[packet[SRC_ADDRESS_BYTE]].dest_node = packet[SRC_ADDRESS_BYTE];
    route_table[packet[SRC_ADDRESS_BYTE]].next_hop = packet[RREQ_SENDER_BYTE];
    route_table[packet[SRC_ADDRESS_BYTE]].dest_seq = packet[RREQ_ORIG_SEQ_BYTE];
    route_table[packet[SRC_ADDRESS_BYTE]].hop_count = packet[RREQ_HOP_COUNT_BYTE] + 1;
#ifdef NET_DBG_ROUTING
    //printf("Route table updated\n");
#endif
    }

    printf("RREQ RX To: %d\n", packet[DEST_ADDRESS_BYTE]);

    if (packet[DEST_ADDRESS_BYTE] == net_node_address || route_table[packet[DEST_ADDRESS_BYTE]].dest_seq > packet[RREQ_ORIG_SEQ_BYTE])
        return 1;
    else
        {        
        uint8_t ttl = packet[CONTROL_1_BYTE]>>2 & 15;

        packet[CONTROL_1_BYTE] = 0;
        packet[CONTROL_1_BYTE] |= RREQ_ID<<6;
        packet[CONTROL_1_BYTE] |= (ttl-1)<<2;
        packet[RREQ_SENDER_BYTE] = net_node_address;
        enqueue(packet, RREQ_PACKET_SIZE);
        }



    return 0;

}

uint8_t net_handle_rrep ( uint8_t *packet){



    fprintf(stderr,"%d\n", packet[SRC_ADDRESS_BYTE] );

    

    if((route_table[packet[SRC_ADDRESS_BYTE]].dest_seq < packet[RREP_DEST_SEQ_BYTE]) || ((route_table[packet[SRC_ADDRESS_BYTE]].dest_seq == packet[RREP_DEST_SEQ_BYTE]) && route_table[packet[SRC_ADDRESS_BYTE]].hop_count > packet[RREP_HOP_COUNT_BYTE] ) ){
        route_table[packet[SRC_ADDRESS_BYTE]].dest_node = packet[DEST_ADDRESS_BYTE];
        route_table[packet[SRC_ADDRESS_BYTE]].next_hop = packet[RREP_SENDER_BYTE];
        route_table[packet[SRC_ADDRESS_BYTE]].dest_seq = packet[RREP_DEST_SEQ_BYTE];
        route_table[packet[SRC_ADDRESS_BYTE]].hop_count= packet[RREP_HOP_COUNT_BYTE];

        #ifdef NET_DEBUG
        fprintf(stderr, "Route table updated\n");
        #endif
    }

    if(packet[DEST_ADDRESS_BYTE] == net_node_address)
        return 1;
    else
    {   uint8_t ttl = packet[CONTROL_1_BYTE]>>2 & 15;
        packet[CONTROL_1_BYTE] &= 0 & (15 << 2);
        packet[CONTROL_1_BYTE] |= (ttl-1)<<2;
        packet[RREP_SENDER_BYTE] = net_node_address;
        packet[RREP_NEXTHOP_BYTE] = route_table[SRC_ADDRESS_BYTE].next_hop;
        enqueue(packet, RREP_PACKET_SIZE);
    }


    return 0;
    

}


uint8_t net_handle_data(uint8_t *packet, uint8_t length)
{
    if(packet[DEST_ADDRESS_BYTE] == net_node_address)
        {
            uint8_t *tran_packet = packet+TRAN_SEGMENT_BYTE;
            uint8_t tran_length = packet[LENGTH_BYTE];

            transportRxFlag = 1;
            transportRxAddress = packet[DEST_ADDRESS_BYTE];

            transportRxSegment.control = (tran_packet[0] << 8) | tran_packet[1];
            transportRxSegment.source = tran_packet[2];
            transportRxSegment.destination = tran_packet[3];
            transportRxSegment.length = tran_packet[4];
            memcpy(transportRxSegment.data, &tran_packet[5], tran_packet[4]);
            transportRxSegment.checksum = (tran_packet[tran_length-2] << 8) | tran_packet[tran_length-1];
            
            return 1;
        }
    else
        {
            uint8_t ttl = packet[CONTROL_1_BYTE]>>2 & 15;
            packet[CONTROL_1_BYTE] &= 0 & (15 << 2);
            packet[CONTROL_1_BYTE] |= (ttl-1)<<2;
            enqueue(packet, length);
            return 0;
        }
        
}


uint8_t send_data (  uint8_t dest_node, uint8_t *tran_segment, uint8_t tran_seg_length)
{
    uint8_t packet[tran_seg_length + DATA_PACKET_SIZE_NO_TRAN ];
    packet[CONTROL_1_BYTE] = DATA_ID<<6;
    packet[CONTROL_1_BYTE] |= DEFAULT_TTL<<2;
    packet[CONTROL_2_BYTE] = 0;
    packet[SRC_ADDRESS_BYTE] = net_node_address;
    packet[DEST_ADDRESS_BYTE] = dest_node;
    packet[LENGTH_BYTE] = tran_seg_length ;
    memcpy(&packet[TRAN_SEGMENT_BYTE], tran_segment, tran_seg_length);

    uint16_t crc = crc16_compute(packet, tran_seg_length + DATA_PACKET_SIZE_NO_TRAN);
    packet[TRAN_SEGMENT_BYTE+tran_seg_length] = crc>>8;
    packet[TRAN_SEGMENT_BYTE+tran_seg_length+1] = crc;

    enqueue(packet, tran_seg_length + DATA_PACKET_SIZE_NO_TRAN);

    /*if (&route_table[dest_node] == NULL)
        count++;


    if(count==REQUEST_MAX_AMOUNT)
    {
        count = 0;
        transportErrorFlag = 1;
        return 0;
    }*/

    return 1;
}

void send_rreq( uint8_t dest_node)
{
    net_seqnum++;

    uint8_t packet[RREQ_PACKET_SIZE];
    packet[CONTROL_1_BYTE] = 0;
    packet[CONTROL_1_BYTE] |= RREQ_ID<<6;
    packet[CONTROL_1_BYTE] |= RREQ_TTL<<2;
    packet[CONTROL_2_BYTE] = 0;
    packet[SRC_ADDRESS_BYTE] = net_node_address;
    packet[DEST_ADDRESS_BYTE] = dest_node;
    packet[LENGTH_BYTE] = RREQ_PACKET_SIZE;
    packet[RREQ_SENDER_BYTE] = net_node_address;
    packet[RREQ_ORIG_SEQ_BYTE] = net_seqnum;
    packet[RREQ_DEST_SEQ_BYTE] = 0;
    packet[RREQ_RREQ_ID_BYTE] = rreq_id;
    packet[RREQ_HOP_COUNT_BYTE] = 0;

    uint16_t crc = crc16_compute(packet, RREQ_PACKET_SIZE );
    packet[10] = crc>>8;
    packet[11] = crc;

    enqueue(packet,RREQ_PACKET_SIZE);

    rreq_id++;

}

void send_rrep(uint8_t *packet)
{
    net_seqnum++;

    uint8_t output_packet[RREP_PACKET_SIZE];
    output_packet[CONTROL_1_BYTE] = 0;
    output_packet[CONTROL_1_BYTE] |= RREP_ID<<6;
    output_packet[CONTROL_1_BYTE] |= RREP_TTL<<2;
    output_packet[CONTROL_2_BYTE] = 0;
    output_packet[SRC_ADDRESS_BYTE] = net_node_address;
    output_packet[DEST_ADDRESS_BYTE] = packet[SRC_ADDRESS_BYTE];
    output_packet[RREP_SENDER_BYTE] = net_node_address;
    output_packet[LENGTH_BYTE] = RREP_PACKET_SIZE;
    output_packet[RREP_DEST_SEQ_BYTE] = net_seqnum;
    output_packet[RREP_HOP_COUNT_BYTE] = 0;


    uint16_t crc = crc16_compute(packet, RREP_PACKET_SIZE );
    output_packet[9] = crc>>8;
    output_packet[10] = crc;

    enqueue(output_packet, RREP_PACKET_SIZE);

    printf("Route Reply!!!!!\n");


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










