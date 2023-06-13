#include <Arduino.h>
#include <main.h>
#include <mcp_can.h>
#include <SPI.h>
#include <HAL\Timer.h>

MCP_CAN CAN0(CS_PIN);

MessageData data; // Data contained by the CAN message packets
Packets packets;  // CAN message packets
SWTimer cycle_timer;
SWTimer value_timer;

void setup()
{
    // debugline
    Serial.begin(115200);

    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
        Serial.println("MCP2515 Initialized Successfully!");
    else
        Serial.println("Error Initializing MCP2515...");

    CAN0.setMode(MCP_NORMAL); // Change to normal mode to allow messages to be transmitted

    cycle_timer = SWTimer_construct(CYCLE_TIME);
    value_timer = SWTimer_construct(VALUE_TIME);
    SWTimer_start(&cycle_timer);
    SWTimer_start(&value_timer);
}

void loop()
{
    // Loop for iterating values
    if (SWTimer_expired(&value_timer))
    {
        cycleValues(&data);
        SWTimer_start(&value_timer);
    }

    // Loop for sending message
    if (SWTimer_expired(&cycle_timer))
    {
        constructPackets(&data, &packets);
        sendMessages(&packets);
        SWTimer_start(&cycle_timer);
    }
}

// Send CAN messages
void sendMessages(Packets *pkts_p)
{
    byte msg_status[5];
    byte good_status[5] = {CAN_OK, CAN_OK, CAN_OK, CAN_OK, CAN_OK};

    msg_status[0] = CAN0.sendMsgBuf(ID_1, 0, DLC, pkts_p->p1);
    msg_status[1] = CAN0.sendMsgBuf(ID_2, 0, DLC, pkts_p->p2);
    msg_status[2] = CAN0.sendMsgBuf(ID_3, 0, DLC, pkts_p->p3);
    msg_status[3] = CAN0.sendMsgBuf(ID_4, 0, DLC, pkts_p->p4);
    msg_status[4] = CAN0.sendMsgBuf(ID_5, 0, DLC, pkts_p->p5);

    // debugline
    if (memcmp(msg_status, good_status, 5) == 0)
        Serial.println("CAN message failed to send");
}

// Construct CAN packets
void constructPackets(MessageData *data_p, Packets *pkts_p)
{
    // Change endianess of every variable if necessary
    memcpy(&pkts_p->p1[0], &data_p->erpm, 4); // should use sizeof(data_p->erpm), but 4 is easier to read
    memcpy(&pkts_p->p1[4], &data_p->current_out, 2);
    memcpy(&pkts_p->p1[6], &data_p->duty_cycle, 2);

    memcpy(&pkts_p->p2[0], &data_p->ah_consum, 4);
    memcpy(&pkts_p->p2[4], &data_p->ah_regen, 4);

    memcpy(&pkts_p->p3[0], &data_p->wh_consum, 4);
    memcpy(&pkts_p->p3[4], &data_p->wh_regen, 4);

    memcpy(&pkts_p->p4[0], &data_p->mosfet_temp, 2);
    memcpy(&pkts_p->p4[2], &data_p->motor_temp, 2);
    memcpy(&pkts_p->p4[4], &data_p->current_in, 2);
    memcpy(&pkts_p->p4[6], &data_p->pid_position, 2);

    memcpy(&pkts_p->p5[0], &data_p->tachometer, 4);
    memcpy(&pkts_p->p5[4], &data_p->voltage_in, 2);
}

// Cycle through data values
void cycleValues(MessageData *data)
{
    static uint16_t i = 0;

    data->erpm = (i * 100 % 40000) - 10000;
    data->current_out = (i % 300 - 150) * MP_CURRENT;
    data->duty_cycle = (i / 100 % 2 - 1) * MP_DUTY;

    data->ah_consum = (i / 500 % 16) * MP_AH;
    data->ah_regen = (i / 2000 % 16) * MP_AH;

    data->wh_consum = (i / 2 % 800) * MP_AH;
    data->wh_regen = (i / 50 % 800) * MP_AH;

    data->mosfet_temp = ((i / 5 + 30) % 80) * MP_TEMP;
    data->motor_temp = ((i / 5 + 20) % 80) * MP_TEMP;
    data->current_in = (i % 80) * MP_CURRENT;
    data->pid_position = i % 50000;

    data->tachometer = (i * 100 % 1000000);
    data->voltage_in = (58 - (i / 20) % (58 - 40)) * MP_VOLTAGE;

    i++;
}

// Use if necessary
uint16_t endian16(uint16_t value)
{
    return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
}

uint32_t endian32(uint32_t value)
{
    return ((value & 0x000000FF) << 24) | ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8);
}