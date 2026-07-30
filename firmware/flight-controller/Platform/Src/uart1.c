#include "clock.h"
#include <stdint.h>
#include <string.h>
//#include "led.h"
#include "uart1.h"

#define USART1_ADDR_BASE 0x40011000
#define GPIOB_ADDR_BASE 0x40020400
#define ADD_NVIC 0xE000E100
#define DMA2_ADDR_BASE 0x40026400


char recv_data[32];

void move_vect_table(){

	// vector table từ 0x00 đến 0x198, dời từ FLASH lên SRAM ( from 0x20000000 )


		uint8_t *ram = (uint8_t*) 0x20000000;
		     uint8_t *vectortable = (uint8_t*) 0x00000000;
		 for( int i = 0; i < 0x198 ; i++ ){
			 ram[i]=vectortable[i];
		 }

		 uint32_t *VTOR = (uint32_t*) 0xE000ED08;
		        *VTOR = 0x20000000; // move vector table


		 // rewrite the function uart1_rx_handler ( address: 0x000000D4 )
		       uint32_t* function_addr = (uint32_t*) 0x20000058 ;
		       function_addr = (uint32_t*) 0x200000D4;
		      *function_addr = ( uint32_t) (uart1_rx_handler) |1;
}


void UART1_init(){

	 clock_enable_AHB1(GPIOB_peripheral);
	// set pb6 as uart1_tx( AF07 ) ; set pb7 as uart1_rx ( AF07 )
	uint32_t* GPIOB_MODER = (uint32_t*) (GPIOB_ADDR_BASE + 0x00);

		*GPIOB_MODER &= ~( 0b1111 << 12 );
        *GPIOB_MODER |= ( 0b10 << 12 ) | ( 0b10 << 14 ) ;      // SET GPIOB IN ALTERNATE MODE

//	    *GPIOB_MODER &= ~( 0b11 << 14 );
//        *GPIOB_MODER |=  ( 0b10 << 14 ) ;      // SET PB7 IN ALTERNATE MODE


    uint32_t* GPIOB_AFRL = (uint32_t*) (GPIOB_ADDR_BASE + 0x20);
		*GPIOB_AFRL &= ~( 0xFF << 24 );
        *GPIOB_AFRL |= ( 7 << 24 ) | ( 7 << 28 ) ;  //SET AF07 FOR PB6-7

//    uint32_t* GPIOB_AFRL = (uint32_t*) (GPIOB_ADDR_BASE + 0x20);
//   		*GPIOB_AFRL &= ~( 0b1111 << 28 );
//           *GPIOB_AFRL |=  ( 7 << 28 ) ;  //SET AF07 FOR PB7

       // UART:
       //  + baudrate : 9600
       //  + frame
       //     * data len : 8 bit
       //     * parity check: none

      clock_enable_APB2(USART1_peripheral);   // RCC 16Mhz

       uint32_t* BRR = (uint32_t*) (USART1_ADDR_BASE + 0x08);
       		*BRR = ( 104 << 4 ) | ( 3 << 0 ); // BAUDRATE = 9600

       uint32_t* CR1 = (uint32_t*) (USART1_ADDR_BASE + 0x0C);
       		*CR1 &= ~( 1 << 10 ) ; // DISABLE PARITY
       		*CR1 &= ~( 1 << 12 ) ; // DATA LEN = 8 BIT
       		*CR1 |= ( 1 << 13 ) | ( 1 << 2 ) | ( 1 << 3 ) ; // enable usart, trans, receive

			#if 1
       		// enable RXNE interrupt
       		// WHEN RXNE is set, uart1 send interrupt event to NVIC
       	     	*CR1 |= ( 1 << 5 ) ;

       		// NVIC accept interrupt event, from UART1
			//uart1 in the position 37 ( ISER1- bit 5 )
       		uint32_t* ISER1 = (uint32_t*) (ADD_NVIC + 0x04);
       		*ISER1 |= (1 << 5 );
			#else
                  // use DMA to move data to RAM when RXNE is set 
				 uint32_t* CR3 = (uint32_t*) (USART1_ADDR_BASE + 0x14);
       		     *CR3 |= ( 1 << 6 ) ; // enable DMA receive

			#endif
       }

void UART1_SEND_1BYTE( char data )
{
	// before transmitting data, check the " status register "

	uint32_t* SR = (uint32_t*) (USART1_ADDR_BASE + 0x00);
	uint32_t* DR = (uint32_t*) (USART1_ADDR_BASE + 0x04);

	       	while ( ((*SR >> 7) & 1 ) == 0 );  // wait DR empty
	       	*DR = data;                    // write data to DR
	    	while ( ((*SR >> 6) & 1 ) == 0 );  // wait trans complete
            *SR &= ~( 1 << 6 );              // clear TC flag
}

void UART1_SEND_STRING( char * msg)
{
	int msg_len =  strlen(msg);
	for(int i=0;i<msg_len;i++)
	{
		UART1_SEND_1BYTE(msg[i]);
	}
}

char UART1_RECV_1BYTE()
{
	uint32_t* SR = (uint32_t*) (USART1_ADDR_BASE + 0x00);
	uint32_t* DR = (uint32_t*) (USART1_ADDR_BASE + 0x04);

	while ( ((*SR >> 5) & 1 ) == 0 ); // wait receive data ( RXNE flag )
	char recv_data = *DR;
	 return recv_data;
}


// CORE -> STARTUP ->g_pfnVector to check the function
//int rx_index = 0;
//void uart1_rx_handler()
//{
//	uint32_t* DR = (uint32_t*) (USART1_ADDR_BASE + 0x04);
//	recv_data[rx_index++] = *DR;
//	// Clear RXNE by reading the DR register or writing bit 0
//
//	if (strstr(recv_data,"LED ON ")!= NULL)
//	{
//		led_control(ORANGE_LED, LED_ON );
//		rx_index = 0;
//		memset(recv_data, 0, sizeof(recv_data));
//	}
//	else if(strstr(recv_data,"LED OFF")!= NULL)
//	{
//		led_control(ORANGE_LED,LED_OFF);
//		rx_index = 0;
//		memset(recv_data, 0, sizeof(recv_data));
//	}
//
//}

volatile uint8_t rx_index = 0;
volatile uint8_t uart_ready = 0;
void uart1_rx_handler(){

	 uint32_t *DR = (uint32_t *)(USART1_ADDR_BASE + 0x04);

	    char c = (char)(*DR);
	    if(uart_ready){
	    	return ;
	    }

	    if (rx_index == (sizeof(recv_data)-1))
	    {
	        recv_data[rx_index] = '\0';
	        uart_ready = 1;
	        rx_index = 0;
	    }
	    else
	    {
	        if (rx_index < sizeof(recv_data)-1)
	        {
	            recv_data[rx_index++] = c;
	        }
	        else rx_index = 0;
	    }
}



//void dma2_uart1_rx_init()
//{
//	clock_enable_AHB1(DMA2_peripheral);
//    // use DMA2 stream 5 channel 4
//	uint32_t* DMA_S5CR = (uint32_t*) (DMA2_ADDR_BASE + 0x10 + 0x18*5);
//
//    uint32_t* DMA_S5NDTR = (uint32_t*) (DMA2_ADDR_BASE + 0x14 + 0x18*5);
//
//	uint32_t* DMA_S5PAR = (uint32_t*) (DMA2_ADDR_BASE + 0x18 + 0x18*5);
//
//	uint32_t* DMA_S5M0AR = (uint32_t*) (DMA2_ADDR_BASE + 0x1C + 0x18*5);
//	/*
//	* number of data to transfer (string length of "LED ON" or "LED OFF" ): 7
//	* pheripheral address: from USART1_DR ( USART1_ADDR_BASE + 0x04 = 0x40011004 )
//  	* memory address: to rec_data[32] ( 0x20000428 )
//	*/
//      *DMA_S5NDTR = 7;
//	  *DMA_S5PAR = 0x40011004 ;
//	  *DMA_S5M0AR= recv_data;
//
//	  *DMA_S5CR |= (0b100<<25);    //channel 4
//	  *DMA_S5CR |= (1<<10);    // memory increment mode
//	  *DMA_S5CR |= (1<<0);     // enable DMA2
//	  *DMA_S5CR |= (1<<8);     // enable circular mode
//
//	  *DMA_S5CR |= (1<<4);     // enable transfer complete interrupt
//
//	  uint32_t* ISER2 = (uint32_t*) (ADD_NVIC + 0x08);
//       		*ISER2 |= (1 << 4 );  // enable DMA2 stream5 interrupt in NVIC

//}


//void dma2_stream5_handler()
//{
//	// clear transfer complete flag
//	uint32_t* DMA_HIFCR = (uint32_t*) (DMA2_ADDR_BASE + 0x0C);
//	*DMA_HIFCR |= (1<<11);  // clear TCIF5 flag
//	if (strstr(recv_data,"LED ON ")!= NULL)
//	{
//		led_control(ORANGE_LED, LED_ON );
//
//
//	}
//	else if(strstr(recv_data,"LED OFF")!= NULL)
//	{
//		led_control(ORANGE_LED,LED_OFF);
//
//	}
//	memset(recv_data, 0, 7);
//}











