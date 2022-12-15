#include "util_test.h"


void main_util_test() {

    fprintf(stderr, "main_util_test\n");

    print_segment_test();
    print_packet_test();
    print_frame_test();

    crc16_compute_test();
}


void print_segment_test() {

    // redirect stdout
    FILE *output;
    output = freopen("build/temp.txt", "w+", stdout);

    // test
    Segment testSegment;
    testSegment.source = 0x12;
    testSegment.destination = 0x34;
    testSegment.length = 0x56;
    print_segment(testSegment);

    // read stdout
    fseek(output, 0, SEEK_SET);

    char * line = NULL;
    size_t len = 0;
    assert(getline(&line, &len, output) != -1);
    fclose(output);

    uint8_t nequal = strcmp(line, "Segment: 0x12 -> 0x34  Data Length: 0x56\n");
    if(nequal) {
        fprintf(stderr, "  FAIL: print_segment_test\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: print_segment_test\n");
}

void print_packet_test() {

    // redirect stdout
    FILE *output;
    output = freopen("build/temp.txt", "w+", stdout);

    // test
    Packet testPacket;
    testPacket.source = 0x12;
    testPacket.destination = 0x34;
    testPacket.length = 0x56;
    print_packet(testPacket);

    // read stdout
    fseek(output, 0, SEEK_SET);

    char * line = NULL;
    size_t len = 0;
    assert(getline(&line, &len, output) != -1);
    fclose(output);

    uint8_t nequal = strcmp(line, "Packet:  0x12 -> 0x34  Data Length: 0x56\n");
    if(nequal) {
        fprintf(stderr, "  FAIL: print_packet_test\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: print_packet_test\n");
}

void print_frame_test() {

    // redirect stdout
    FILE *output;
    output = freopen("build/temp.txt", "w+", stdout);

    // test
    Frame testFrame;
    testFrame.addressing = 0x1234;
    testFrame.length = 0x56;
    print_frame(testFrame);

    // read stdout
    fseek(output, 0, SEEK_SET);

    char * line = NULL;
    size_t len = 0;
    assert(getline(&line, &len, output) != -1);
    fclose(output);

    uint8_t nequal = strcmp(line, "Frame:  0x1234             Data Length: 0x56\n");
    if(nequal) {
        fprintf(stderr, "  FAIL: print_frame_test\n");
        assert(0);
    }

    fprintf(stderr, "  PASS: print_frame_test\n");
}


void crc16_compute_test() {

    uint8_t data[4] = {1, 2, 3, 4};
    uint16_t expChecksum = 0x2ba1;

    uint16_t testChecksum = crc16_compute(data, 4);

    if(expChecksum != testChecksum) {
        fprintf(stderr, "  FAIL: crc16_compute_test\n");
        fprintf(stderr, "        expected checksum 0x%04x but got 0x%04x\n", expChecksum, testChecksum);
        assert(0);
    }

    fprintf(stderr, "  PASS: crc_16_compute_test\n");
}
