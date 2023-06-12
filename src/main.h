#include <Arduino.h>

//Arbitration IDs
#define ID_0 0x000
#define ID_1 0x009
#define ID_2 0x00E
#define ID_3 0x00F
#define ID_4 0x010
#define ID_5 0x01B

//Data length code
#define DLC 0x8

//Multipliers for certain values
#define MP_DUTY 1000
#define MP_CURRENT 10
#define MP_AH 10000
#define MP_TEMP 10
#define MP_VOLTAGE 10

#define CYCLE_TIME 200 //ms
#define VALUE_TIME 1 //ms

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

// Cycles through values for the messages
void cycleValues(MessageData *data_p);

// Construct messages and send over CAN
void sendMessages(MessageData *data_p);