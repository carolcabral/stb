idf_component_register(SRCS "ble.c" "gatt_svr.c" 
                    INCLUDE_DIRS "include"
                    REQUIRES bt interrupts)
