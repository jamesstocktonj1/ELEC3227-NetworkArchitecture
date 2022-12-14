#include "application_test.h"


void main_application_test() {

    fprintf(stderr, "main_application_test\n");

    application_init();

    encrypt_data_test();
    decrypt_data_test();
    dual_encryption_test();

    application_data_test();
}


uint8_t compare_data(uint8_t *data1, uint8_t *data2, uint8_t len) {
    for(int i=0; i<len; i++) {
        if(data1[i] != data2[i]) {
            return 1;
        }
    }
    return 0;
}

void print_data(uint8_t *data1, uint8_t *data2, uint8_t len) {

    fprintf(stderr, "  Expected ");
    for(int i=0; i<len; i++) {
        fprintf(stderr, " 0x%02x", data1[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "  But got  ");
    for(int i=0; i<len; i++) {
        fprintf(stderr, " 0x%02x", data2[i]);
    }
    fprintf(stderr, "\n");
}



void encrypt_data_test() {
    const uint8_t test_len = 4;
    const uint16_t test_key = 0xab56;
    uint8_t testData[4] = { 0x12, 0x34, 0x56, 0x78 };
    uint8_t expeData[4] = { 0xb9, 0x62, 0xfd, 0x2e };

    encrypt_data(testData, test_len, test_key);

    if(compare_data(expeData, testData, test_len)) {
        fprintf(stderr, "  FAIL: encrypt_data_test\n");
        print_data(expeData, testData, test_len);
        assert(0);
    }

    fprintf(stderr, "  PASS: encrypt_data_test\n");
}

void decrypt_data_test() {
    const uint8_t test_len = 4;
    const uint16_t test_key = 0xab56;
    uint8_t testData[4] = { 0xb9, 0x62, 0xfd, 0x2e };
    uint8_t expeData[4] = { 0x12, 0x34, 0x56, 0x78 };

    decrypt_data(testData, test_len, test_key);

    if(compare_data(expeData, testData, test_len)) {
        fprintf(stderr, "  FAIL: decrypt_data_test\n");
        print_data(expeData, testData, test_len);
        assert(0);
    }

    fprintf(stderr, "  PASS: decrypt_data_test\n");
}

void dual_encryption_test() {
    const uint8_t test_len = 4;
    const uint16_t test_key = 0x72e9;
    uint8_t testData[4] = { 0x33, 0xa7, 0x2e, 0xfb };
    uint8_t expeData[4] = { 0x33, 0xa7, 0x2e, 0xfb };

    encrypt_data(testData, test_len, test_key);
    decrypt_data(testData, test_len, test_key);

    if(compare_data(expeData, testData, test_len)) {
        fprintf(stderr, "  FAIL: dual_encryption_test\n");
        print_data(expeData, testData, test_len);
        assert(0);
    }

    fprintf(stderr, "  PASS: dual_encryption_test\n");
}

void application_data_test() {
    uint8_t testTxData[3] = {0x01, 0x02, 0x03};
    applicationTxFlag = 0;

    fprintf(stderr, "  Original Data: ");
    int i;
    for(i=0; i<3; i++) {
        fprintf(stderr, "0x%04x, ", testTxData[i]);
    }
    fprintf(stderr, "\n");

    uint8_t isTx = application_handle_tx(testTxData, 3, 0x01, APP_ADDR);
    if(isTx == 0) {
        fprintf(stderr, "  FAIL: application_tx_data_test\n");
        fprintf(stderr, "  Application Failed to send data\n");
        assert(0);
    }

    fprintf(stderr, "  Encrypted Data: ");
    for(i=0; i<3; i++) {
        fprintf(stderr, "0x%04x, ", applicationTxData[i]);
    }
    fprintf(stderr, "\n");

    memcpy(applicationRxData, applicationTxData, applicationTxLength);
    applicationRxLength = applicationTxLength;
    applicationRxFlag = applicationTxFlag;
    applicationRxEncryption = applicationTxEncryption;


    uint8_t testRxData[3];
    uint8_t testPort;

    uint8_t isRx = application_handle_rx(testRxData, testPort);
    if(isRx == 0) {
        fprintf(stderr, "  FAIL: application_tx_data_test\n");
        fprintf(stderr, "  Application failed to recieve data\n");
        assert(0);
    }
    else if(isRx != 3) {
        fprintf(stderr, "  FAIL: application_tx_data_test\n");
        fprintf(stderr, "  Expected data of length 3 but got %d\n", isRx);
        assert(0);
    }

    uint8_t isDiff = compare_data(testRxData, testTxData, isRx);
    if(isDiff) {
        fprintf(stderr, "  FAIL: application_tx_data_test\n");
        print_data(testRxData, testTxData, isRx);
        assert(0);
    }

    fprintf(stderr, "  Decrypted Data: ");
    for(i=0; i<3; i++) {
        fprintf(stderr, "0x%04x, ", testTxData[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "  PASS: application_tx_data_test\n");
}
