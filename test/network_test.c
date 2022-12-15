#include "network_test.h"


void main_network_test() {

    fprintf(stderr, "main_network_test\n");





}

void queue_test()
{

    qrecord queue[QUEUE_MAX_SIZE];
    int packet[DATA_PACKET_SIZE_NO_TRAN];


    packet[0]=1;
    packet[1]=3;
    packet[2]=6;
    packet[3]=7;
    packet[4]=8;

    enqueue(packet, DATA_PACKET_SIZE_NO_TRAN);
    printf("Enqueued packet with size 5")
    
    packet[DATA_PACKET_SIZE_NO_TRAN+1];
    packet[0]=1;
    packet[1]=3;
    packet[2]=6;
    packet[3]=7;
    packet[4]=8;
    packet[5]=10;

    enqueue(packet, DATA_PACKET_SIZE_NO_TRAN+1);
    printf("Enqueued packet with size 6");





}


