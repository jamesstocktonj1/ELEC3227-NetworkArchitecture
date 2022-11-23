#include <stdio.h>

#include "../include/util.h"
#include "application_test.h"
#include "network_test.h"
#include "datalink_test.h"
#include "util_test.h"


int main() {

    main_util_test();

    main_application_test();
    main_network_test();
    main_datalink_test();
}