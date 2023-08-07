#include <Arduino.h>

//CS Pin
#define CS_PIN 10

//Timing settings
#define CYCLE_TIME 100 //ms. Affects how frequently CAN messages are sent
#define VALUE_TIME 1 //ms. Affects how quickly value cycle

//Arbitration IDs
// VESC IDs are left shifted by 8 bits!!!
#define ID_0 0x000 << 8
#define ID_1 0x009 << 8
#define ID_2 0x00E << 8
#define ID_3 0x00F << 8
#define ID_4 0x010 << 8
#define ID_5 0x01B << 8

//Data length code
#define DLC 0x8

//Multipliers for certain values
#define MP_DUTY 1000
#define MP_CURRENT 10
#define MP_AH 10000 //actually 10000, but integer overflow
#define MP_TEMP 10
#define MP_VOLTAGE 10

struct _MessageData
{
    //Message 1
    int32_t erpm;
    int16_t current_out;
    uint16_t duty_cycle;

    //Message 2
    uint32_t ah_consum;
    uint32_t ah_regen;

    //Message 3
    uint32_t wh_consum;
    uint32_t wh_regen;

    //Message 4
    uint16_t mosfet_temp;
    uint16_t motor_temp;
    uint16_t current_in;
    uint16_t pid_position;

    //Message 5
    uint32_t tachometer;
    uint16_t voltage_in;
    uint16_t reserved;
};
typedef struct _MessageData MessageData;

struct _Packets
{
    //8 byte packets for corresponding CAN messages
    byte p1[8];
    byte p2[8];
    byte p3[8];
    byte p4[8];
    byte p5[8];
};
typedef struct _Packets Packets;

// Cycles through values for the messages
void cycleValues(MessageData *data_p);

// Send messages over CAN
void sendMessages(Packets *pkts_p);

// Construct messages
void constructPackets(MessageData *data, Packets *pkts_p);

// Change endianess of a 16 bit value
uint16_t htons(uint16_t value);

// Change endianess of a 32 bit value
uint32_t htonl(uint32_t value);