#include "network_test.h"


void main_network_test() {

    fprintf(stderr, "main_network_test\n");
    //fprintf(stderr, "Queue test\n");
    //queue_test();
    //fprintf(stderr, "handle_tx test");
    handle_tx_test();





}

void queue_test()
{
    qrecord *buffer;
    qrecord queue[QUEUE_MAX_SIZE];
    uint8_t packet1[DATA_PACKET_SIZE_NO_TRAN];


    packet1[0]=1;
    packet1[1]=3;
    packet1[2]=6;
    packet1[3]=7;
    packet1[4]=8;

    enqueue(packet1, DATA_PACKET_SIZE_NO_TRAN);
    fprintf(stderr,"Enqueued packet with size %d\n", DATA_PACKET_SIZE_NO_TRAN);
    
    uint8_t packet2[DATA_PACKET_SIZE_NO_TRAN+1];
    packet2[0]=1;
    packet2[1]=3;
    packet2[2]=6;
    packet2[3]=7;
    packet2[4]=8;
    packet2[5]=10;

    enqueue(packet2, DATA_PACKET_SIZE_NO_TRAN+1);
    fprintf(stderr,"Enqueued packet with size %d\n", DATA_PACKET_SIZE_NO_TRAN + 1);

    dequeue(buffer);
    fprintf(stderr,"Dequeued packet with size %d\n", buffer->packet_size);
    dequeue(buffer);
    fprintf(stderr,"Dequeued packet with size %d\n", buffer->packet_size);
    dequeue(buffer);
    fprintf(stderr,"Dequeued packet with size %d\n", buffer->packet_size);
    dequeue(buffer);
    fprintf(stderr,"Dequeued packet with size %d\n", buffer->packet_size);





}

void handle_tx_test()
{
    
    net_rt_entry route_table[DEFAULT_NETWORK_SIZE];
    net_init();
    transportTxFlag = 0;
    uint8_t packet1[DATA_PACKET_SIZE_NO_TRAN];

    qrecord *buffer;
    uint8_t interface_buffer[NET_MAX_PACKET_SIZE];
    uint8_t rx_buffer[NET_MAX_PACKET_SIZE];
    
    route_table[0].next_hop = 0;
    
    for(int i=1; i<DEFAULT_NETWORK_SIZE; i++)
    {
        route_table[i].next_hop = UNKNOWN_NEXT_HOP;
        //fprintf("%d:, %d\n", i, route_table[i].next_hop);
    }


    packet1[0]=1;
    packet1[1]=3;
    packet1[2]=6;
    packet1[3]=7;
    packet1[4]=8;


    
    uint8_t packet2[DATA_PACKET_SIZE_NO_TRAN+1];
    packet2[0]=1;
    packet2[1]=3;
    packet2[2]=6;
    packet2[3]=7;
    packet2[4]=8;
    packet2[5]=10;

    rx_buffer[CONTROL_1_BYTE] = 0;
    rx_buffer[CONTROL_1_BYTE] |= RREP_ID<<6;

    fprintf(stderr, "%d:\n ",rx_buffer[CONTROL_1_BYTE]);

    rx_buffer[CONTROL_2_BYTE] = 2;
    rx_buffer[SRC_ADDRESS_BYTE] = 7;
    rx_buffer[DEST_ADDRESS_BYTE] = APP_ADDR;
    rx_buffer[RREP_SENDER_BYTE] = 6;
    rx_buffer[RREP_DEST_SEQ_BYTE] = 10;

    enqueue(packet1, 5);

            //dequeue(&buffer);
    

    net_tx_poll();

    net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);

    net_tx_poll();

    net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);



    net_handle_rx_packet(rx_buffer, RREP_PACKET_SIZE);



    enqueue(packet1, 5);

    
net_tx_poll();

    net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);

    transportTxFlag = 1;
    transportTxSegment.control = 2;
    transportTxSegment.source = 3;
    transportTxSegment.destination  = 4;
    transportTxSegment.length  = 0;

    net_tx_poll();

    net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);

    net_tx_poll();

    net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);

    net_tx_poll();

    net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);

    net_tx_poll();

    net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);

    fprintf(stderr, "%d", net_tx_poll() );

        net_handle_tx(interface_buffer);

    //print_packet_info(interface_buffer);

        net_handle_tx(interface_buffer);

    print_packet_info(interface_buffer);


}

void print_packet_info(uint8_t *packet)
{
        fprintf(stderr,"Packet type: %d\n" ,packet[CONTROL_1_BYTE] & (3<<6));
        fprintf(stderr,"Packet type: %d\n" ,packet[CONTROL_1_BYTE]);
        fprintf(stderr,"Control 2: %d\n" ,packet[CONTROL_2_BYTE]);
        fprintf(stderr,"Source: %d\n" ,packet[SRC_ADDRESS_BYTE]);
        fprintf(stderr, "Dest: %d\n" ,packet[DEST_ADDRESS_BYTE]);


}



