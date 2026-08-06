#ifndef INC_UART1_H_
#define INC_UART1_H_
#include <stdint.h>

void UART1_init();
void UART1_SEND_1BYTE( char data );
void UART1_SEND_STRING( char * msg);
char UART1_RECV_1BYTE();
void uart1_rx_handler();
void dma2_uart1_rx_init();

extern char recv_data[32];
//extern int rx_index;

extern volatile uint8_t rx_index ;
extern volatile uint8_t uart_ready ;


#endif /* INC_UART1_H_ */
