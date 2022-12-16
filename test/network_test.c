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
    qrecord buffer;
    qrecord queue[QUEUE_MAX_SIZE];
    uint8_t packet1[DATA_PACKET_SIZE_NO_TRAN];
    net_init(APP_ADDR);



    fprintf(stderr,"Dequeued packet with size %d\n", buffer.packet_size);
    fprintf(stderr,"Is empty %d\n", dequeue(&buffer));

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

    
    fprintf(stderr,"Is full %d\n", enqueue(packet2, DATA_PACKET_SIZE_NO_TRAN+1));
    fprintf(stderr,"Enqueued packet with size %d\n", DATA_PACKET_SIZE_NO_TRAN + 1);

    fprintf(stderr,"Is empty %d\n", dequeue(&buffer));
    fprintf(stderr,"Dequeued packet with size %d\n", buffer.packet_size);
    

    fprintf(stderr,"Is full %d\n", enqueue(packet1, DATA_PACKET_SIZE_NO_TRAN));
    fprintf(stderr,"En packet with size %d\n", DATA_PACKET_SIZE_NO_TRAN);
    

    
    fprintf(stderr,"Is empty %d\n", dequeue(&buffer));
    fprintf(stderr,"Dequeued packet with size %d\n", buffer.packet_size);
    
    fprintf(stderr,"Is empty %d\n", dequeue(&buffer));
    fprintf(stderr,"Dequeued packet with size %d\n", buffer.packet_size);
    





}

void handle_tx_test()
{
    
    net_rt_entry route_table[DEFAULT_NETWORK_SIZE];
    net_init(APP_ADDR);
    transport_init(4);
    transportTxFlag = 0;
    uint8_t packet1[DATA_PACKET_SIZE_NO_TRAN];

    qrecord buffer;
    qrecord queue[QUEUE_MAX_SIZE];
    uint8_t tx_buffer[NET_MAX_PACKET_SIZE];
    uint8_t rx_buffer[NET_MAX_PACKET_SIZE];
    
    route_table[0].next_hop = 0;
    
    for(int i=1; i<DEFAULT_NETWORK_SIZE; i++)
    {
        route_table[i].next_hop = UNKNOWN_NEXT_HOP;
        //fprintf("%d:, %d\n", i, route_table[i].next_hop);
    }


    fprintf(stderr, "Transport layer wants to send a packet\n\n");




    //net_handle_rx_packet(rx_buffer, RREQ_PACKET_SIZE);
    net_transport_poll();
    if(net_tx_poll())
    {
        buffer = net_handle_tx();
    }
   

    net_transport_poll();
    if(net_tx_poll())
    {
        buffer = net_handle_tx();
    }

    net_transport_poll();
    if(net_tx_poll())
    {
        buffer = net_handle_tx();
    }

    uint8_t app_data[4] = {1,2,3,4};
    transportTxFlag = 1;
    transportTxAddress = 5;
    transportTxSegment.control  = 4;
    transportTxSegment.source = 6;
    transportTxSegment.destination = 7;
    transportTxSegment.data[0] = 0;
    transportTxSegment.data[1] = 1;
    transportTxSegment.data[2] = 2;
    transportTxSegment.length = 3;

    uint8_t temp = transportTxSegment.data[2];

    

     fprintf(stderr, "Transport layer wants to send a packet\n\n");


    net_transport_poll();
    if(net_tx_poll())
    {
        buffer = net_handle_tx();
    }
    


    net_transport_poll();
    if(net_tx_poll())
    {
        buffer = net_handle_tx();
    }

    net_transport_poll();
    if(net_tx_poll())
    {
        buffer = net_handle_tx();
    }

    net_transport_poll();
    if(net_tx_poll())
    {
        buffer = net_handle_tx();
    }


}

void print_packet_info(uint8_t *packet)
{
        fprintf(stderr,"Packet type: %d\n" ,packet[CONTROL_1_BYTE] & (3<<6));
        fprintf(stderr,"Packet type: %d\n" ,packet[CONTROL_1_BYTE]);
        fprintf(stderr,"Control 2: %d\n" ,packet[CONTROL_2_BYTE]);
        fprintf(stderr,"Source: %d\n" ,packet[SRC_ADDRESS_BYTE]);
        fprintf(stderr, "Dest: %d\n" ,packet[DEST_ADDRESS_BYTE]);


}



