#include "application_test.h"


void main_application_test() {

    fprintf(stderr, "main_application_test\n");

    crc16_compute_test();
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
