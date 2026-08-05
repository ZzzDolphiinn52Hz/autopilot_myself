#include <stdint.h>
#include "delay.h"
#include "spi.h"
#include "nRF24L01.h"

 uint8_t config_value = 0U;
 uint8_t config_value1 = 0U;
 uint8_t rf_ch_value = 0U;
 uint8_t rf_setup_value = 0U;
 uint8_t fifo_status_value = 0U;

int main(void)
{
    SPI_init();
    NRF24_InitPins();

    delay_init();



    	 NRF24_ReadRegister(
    	        NRF24_REG_CONFIG,
    	        (uint8_t *)&config_value,
    	        NULL
    	    );
    	 NRF24_WriteRegister(NRF24_REG_CONFIG ,20 ,NULL);

    	    NRF24_ReadRegister(
    	    		NRF24_REG_CONFIG,
    	        (uint8_t *)&config_value1,
    	        NULL
    	    );

    	    NRF24_ReadRegister(
    	        NRF24_REG_RF_SETUP,
    	        (uint8_t *)&rf_setup_value,
    	        NULL
    	    );

    	    NRF24_ReadRegister(
    	        NRF24_REG_FIFO_STATUS,
    	        (uint8_t *)&fifo_status_value,
    	        NULL
    	    );

    	    while (1)
    	        {

    	    	__asm volatile("NOP");
    }
}
