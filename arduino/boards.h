

#ifdef BOARD_GPS

/*
    D05 - PD5 - Debug
    D06 - PD6 - MOT A1 - GPS
    D07 - PD7 - EN StepUp
    D08 - PB0 - A6 Power Sig
    D09 - PB1 - MOT A2 - GPS LOW
    D10 - PB2 - MOT B2 - A6 LOW
    D11 - PB3 - MOT B1 - A6
    D12 - PB4 - LED 2
    D13 - PB5 - LED 1
    D14 / A0 - PC0 - GPS RX
    D15 / A1 - PC1 - A6 RX
    D16 / A2 - PC2 - A6 TX
    D17 / A3 - PC3 - A6 Int
    D18 / A4 / SDA - PC5 - A6 Reset
    D19 / A5 / SCL
    A7 - PC7*/

// inputs

// outputs
#define PIN_LED1 13
#define PIN_LED2 12

#define PIN_A6_POWER 11
#define PIN_A6_POWER_LOW 10
#define PIN_A6_POWER_SIG 8
#define PIN_A6_RESET 18
#define PIN_A6_INT 17
#define PIN_A6_RX 15
#define PIN_A6_TX 16

#define PIN_GPS_POWER 6
#define PIN_GPS_POWER_LOW 9
#define PIN_GPS_RX 14
#define PIN_GPS_TX 2

#endif
