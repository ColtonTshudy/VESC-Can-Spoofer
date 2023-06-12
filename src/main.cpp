#include <Arduino.h>
#include <main.h>
#include <mcp_can.h>
#include <SPI.h>
#include <HAL\Timer.h>

MCP_CAN CAN0(10); // Set CS to pin 10

MessageData data; // Data contained by the CAN message packets
SWTimer cycle_timer;
SWTimer value_timer;

void setup()
{
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
    if (SWTimer_expired(&value_timer)){
        cycleValues(&data);
        SWTimer_start(&value_timer);
    }

    // Loop for sending message
    if (SWTimer_expired(&cycle_timer)){
        sendMessages(&data);
        SWTimer_start(&cycle_timer);
    }
}

// Send CAN messages
void sendMessages(MessageData *data)
{

}

// Cycle through data values
void cycleValues(MessageData *data)
{
    static uint16_t i = 0;

    data->erpm = (i*100 % 40000)-10000;
    data->current_out = i % 300-150;
    data->duty_cycle = i/100 % 2 - 1;
    
    data->ah_consum = i/500 % 16;
    data->ah_regen = i/2000 % 16;

    data->wh_consum = i/2 % 800;
    data->wh_regen = i/50 % 800;

    data->mosfet_temp = (i/5+30) % 80;
    data->motor_temp = (i/5+20) % 80;
    data->current_in = i % 80;
    data->pid_position = i % 50000;

    data->tachometer = (i*100 % 1000000);
    data->voltage_in = 58-(i/20) % (58-40);

    i++;
}