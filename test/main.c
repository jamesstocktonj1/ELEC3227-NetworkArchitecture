#include <stdio.h>

#include "../include/config.h"
#include "application_test.h"
#include "transport_test.h"
#include "network_test.h"
#include "datalink_test.h"
#include "config_test.h"


int main() {

    main_config_test();

    main_application_test();
    main_transport_test();
    main_network_test();
    main_datalink_test();
}