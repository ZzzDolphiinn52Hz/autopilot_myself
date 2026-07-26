#ifndef INC_UART6_H_
#define INC_UART6_H_

#include <stdint.h>

#define UART6_BAUDRATE 115200U

void uart6_init(void);
int uart6_read_char(char *data);
void uart6_send_char(char data);
void uart6_send_string(const char *str);
void uart6_send_uint(uint32_t value);
void uart6_send_int(int32_t value);
void uart6_send_float(float value, uint8_t decimal);

#endif /* INC_UART6_H_ */
