#include <stdint.h>

#include "spi.h"
#include "delay.h"
#include "nRF24L01.h"

typedef struct
{
    uint16_t throttle;
    uint16_t yaw;
    uint16_t pitch;
    uint16_t roll;
} ControlPacket_t;

static const uint8_t rx_address[5] =
{
    'D', 'R', 'O', 'N', 'E'
};



volatile ControlPacket_t receive_packet;

volatile NRF24_Status_t init_result;

volatile NRF24_Status_t receive_result;

extern uint8_t fifo_status;

volatile uint32_t debug_mark = 0;

    volatile uint8_t config_debug;
    volatile uint8_t rf_ch_debug;
    volatile uint8_t rf_setup_debug;
    volatile uint8_t en_aa_debug;
    volatile uint8_t en_rxaddr_debug;
    volatile uint8_t payload_width_debug;

    volatile uint8_t rx_addr_p0_debug[5];

int main(void)
{

	delay_init();
    SPI_init();
    NRF24_InitPins();


    init_result = NRF24_InitReceiver(
           rx_address,
           sizeof(rx_address),
           sizeof(ControlPacket_t)
       );

       if (init_result != NRF24_OK)
       {
           while (1)
           {
               __asm volatile("NOP");
           }
       }

       while (1)
       {
           receive_result = NRF24_Receive(
               (uint8_t *)&receive_packet,
               sizeof(receive_packet)
           );

           if (receive_result == NRF24_OK)
           {
               __asm volatile("NOP");
           }
       }
}
