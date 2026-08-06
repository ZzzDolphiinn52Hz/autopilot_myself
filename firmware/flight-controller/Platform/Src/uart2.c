#include "uart2.h"
#include "clock.h"
#include <gpio.h>

#define USART2_ADDR     0x40004400

static volatile uint32_t *USART2_SR     = (volatile uint32_t *)(USART2_ADDR + 0x00);
static volatile uint32_t *USART2_DR     = (volatile uint32_t *)(USART2_ADDR + 0x04);
static volatile uint32_t *USART2_BRR    = (volatile uint32_t *)(USART2_ADDR + 0x08);
static volatile uint32_t *USART2_CR1    = (volatile uint32_t *)(USART2_ADDR + 0x0C);
static volatile uint32_t *USART2_CR2    = (volatile uint32_t *)(USART2_ADDR + 0x10);
static volatile uint32_t *USART2_CR3    = (volatile uint32_t *)(USART2_ADDR + 0x14);

#define USART_SR_TXE    (1 << 7)
#define USART_SR_TC     (1 << 6)
#define USART_SR_RXNE   (1 << 5)

#define USART_CR1_UE    (1 << 13)
#define USART_CR1_TE    (1 << 3)
#define USART_CR1_RE    (1 << 2)

static uint32_t uart2_calculate_brr(uint32_t pclk, uint32_t baudrate)
{
    return (pclk + (baudrate / 2U)) / baudrate;
}

void uart2_init(void)
{
    clock_enable_AHB1(GPIOA_peripheral);
    clock_enable_APB1(USART2_peripheral);

    GPIOA_CONFIG(P2, GPIO_ALTERNATE, GPIO_PUSHPULL, GPIO_FAST, GPIO_NOPULL, 0, GPIO_AF7, 0); 
    GPIOA_CONFIG(P3, GPIO_ALTERNATE, GPIO_PUSHPULL, GPIO_FAST, GPIO_PULLUP, 0, GPIO_AF7, 0); 

    *USART2_CR1 &= ~USART_CR1_UE;
    *USART2_CR2 = 0;
    *USART2_CR3 = 0;
    *USART2_BRR = uart2_calculate_brr(RCC_APB1_CLOCK_HZ, UART2_BAUDRATE);
    *USART2_CR1 = USART_CR1_TE | USART_CR1_RE;
    *USART2_CR1 |= USART_CR1_UE;
}

void uart2_send_char(char data)
{
    while ((*USART2_SR & USART_SR_TXE) == 0);
    *USART2_DR = (uint8_t)data;
} // send 1 ky tu

int uart2_read_char(char *data)
{
    if (data == 0)
    {
        return 0;
    }

    if ((*USART2_SR & USART_SR_RXNE) == 0)
    {
        return 0;
    }

    *data = (char)(*USART2_DR & 0xFF);
    return 1;
}

void uart2_send_string(const char *str)
{
    if (str == 0)
    {
        return;
    }

    while (*str != '\0')
    {
        uart2_send_char(*str);
        str++;
    }
} // send 1 chuoi => vong lap send 1 ky tu

void uart2_send_uint(uint32_t value)
{
    char buffer[10];
    uint8_t index = 0;

    if (value == 0)
    {
        uart2_send_char('0');
        return;
    }

    while (value > 0)
    {
        buffer[index++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (index > 0)
    {
        uart2_send_char(buffer[--index]);
    }
} // send 1 so uint nen tach tung chu so rồi gửi

void uart2_send_int(int32_t value)
{
    if (value < 0)
    {
        uart2_send_char('-');
        value = -value;
    }

    uart2_send_uint((uint32_t)value);
} // send 1 so nguyen co the am hoac duong

void uart2_send_float(float value, uint8_t decimal)
{
    uint32_t multiplier = 1;
    uint32_t fraction;

    if (value < 0.0f)
    {
        uart2_send_char('-');
        value = -value;
    }

    for (uint8_t i = 0; i < decimal; i++)
    {
        multiplier *= 10;
    }

    uart2_send_uint((uint32_t)value);

    if (decimal == 0)
    {
        return;
    }

    uart2_send_char('.');
    fraction = (uint32_t)((value - (float)((uint32_t)value)) * (float)multiplier + 0.5);

    if (fraction >= multiplier)
    {
        fraction = multiplier - 1;
    }

    for (uint32_t div = multiplier / 10; div > 0; div /= 10)
    {
        uart2_send_char((char)('0' + ((fraction / div) % 10)));
    }
}
