#include "spi.h"
#include "clock.h"
#include <gpio.h>

#define SPI1_ADDR       0x40013000

static volatile uint32_t *SPI1_CR1      = (volatile uint32_t *)(SPI1_ADDR + 0x00);
static volatile uint32_t *SPI1_CR2      = (volatile uint32_t *)(SPI1_ADDR + 0x04);
static volatile uint32_t *SPI1_SR       = (volatile uint32_t *)(SPI1_ADDR + 0x08);
static volatile uint32_t *SPI1_DR       = (volatile uint32_t *)(SPI1_ADDR + 0x0C);
static volatile uint32_t *SPI1_CRCPR    = (volatile uint32_t *)(SPI1_ADDR + 0x10);
static volatile uint32_t *SPI1_RXCRCR   = (volatile uint32_t *)(SPI1_ADDR + 0x14);
static volatile uint32_t *SPI1_TXCRCR   = (volatile uint32_t *)(SPI1_ADDR + 0x18);

#define SPI_CR1_CPHA        (1 << 0)
#define SPI_CR1_CPOL        (1 << 1)
#define SPI_CR1_MSTR        (1 << 2)
#define SPI_CR1_BR_DIV16   (3 << 3)
#define SPI_CR1_SPE         (1 << 6)
#define SPI_CR1_LSBFIRST    (1 << 7)
#define SPI_CR1_SSI         (1 << 8)
#define SPI_CR1_SSM         (1 << 9)
#define SPI_CR1_RXONLY      (1 << 10)
#define SPI_CR1_DFF         (1 << 11)
#define SPI_CR1_BIDIMODE    (1 << 15)

#define SPI_SR_RXNE         (1 << 0)
#define SPI_SR_TXE          (1 << 1)
#define SPI_SR_MODF         (1 << 5)
#define SPI_SR_OVR          (1 << 6)
#define SPI_SR_BSY          (1 << 7)

#define SPI_TIMEOUT_COUNT   100000
#define SPI_DUMMY_BYTE      0xFF

static SPI_Status_t spi_wait_flag_set(volatile uint32_t *reg, uint32_t flag);
static SPI_Status_t spi_wait_flag_reset(volatile uint32_t *reg, uint32_t flag);
static void spi_clear_ovr_flag(void);
static SPI_Status_t spi_wait_flag_set(volatile uint32_t *reg, uint32_t flag)
{
    uint32_t timeout = SPI_TIMEOUT_COUNT;
    while (((*reg) & flag) == 0)
    {
        if (timeout-- == 0)
        {
            return SPI_TIMEOUT;
        }
    }
    return SPI_OK;
} // wait flag

static SPI_Status_t spi_wait_flag_reset(volatile uint32_t *reg, uint32_t flag)
{
    uint32_t timeout = SPI_TIMEOUT_COUNT;
    while (((*reg) & flag) != 0)
    {
        if (timeout-- == 0)
        {
            return SPI_TIMEOUT;
        }
    }
    return SPI_OK;
} // wait flag clear

static void spi_clear_ovr_flag(void)
{
    volatile uint32_t temp;
    temp = *SPI1_DR;
    temp = *SPI1_SR;
    (void)temp;
} // read DR then SR to clear ovrflag

void SPI_init(void)
{
    clock_enable_AHB1(GPIOA_peripheral);
    clock_enable_APB2(SPI1_peripheral);

    GPIOA_CONFIG(P5, GPIO_ALTERNATE, GPIO_PUSHPULL, GPIO_FAST, GPIO_NOPULL, 0, GPIO_AF5, 0); //clk
    GPIOA_CONFIG(P6, GPIO_ALTERNATE, GPIO_PUSHPULL, GPIO_FAST, GPIO_NOPULL, 0, GPIO_AF5, 0); //miso
    GPIOA_CONFIG(P7, GPIO_ALTERNATE, GPIO_PUSHPULL, GPIO_FAST, GPIO_NOPULL, 0, GPIO_AF5, 0); //mosi
    GPIOA_CONFIG(P4, GPIO_OUTPUT, GPIO_PUSHPULL, GPIO_FAST, GPIO_PULLUP, 1, 0, 0); //cs_icm42688
    GPIOA_CONFIG(P0, GPIO_OUTPUT, GPIO_PUSHPULL, GPIO_FAST, GPIO_PULLUP, 1, 0, 0); //cs_nrf
    GPIOA_CONFIG(P1, GPIO_OUTPUT, GPIO_PUSHPULL, GPIO_FAST, GPIO_PULLUP, 1, 0, 0); //cs_icm20948


    *SPI1_CR1 &= ~SPI_CR1_SPE; // off spi
    *SPI1_CR1 = 0;
    *SPI1_CR2 = 0; // clear old cau hinh

    *SPI1_CR1 |= SPI_CR1_MSTR; // stm = master
    *SPI1_CR1 |= SPI_CR1_BR_DIV16; // 011: fPCLK/16, ICM20948 SPI max is about 7MHz
    *SPI1_CR1 |= SPI_CR1_SSM; 
    *SPI1_CR1 |= SPI_CR1_SSI; // cs=1

    *SPI1_CR1 &= ~SPI_CR1_CPOL; // cpol =0
    *SPI1_CR1 &= ~SPI_CR1_CPHA; // cpha=0
    // => clock low, lay mau canh dau

    *SPI1_CR1 &= ~SPI_CR1_DFF; // 8bit 
    *SPI1_CR1 &= ~SPI_CR1_LSBFIRST; // Msb first
    *SPI1_CR1 &= ~SPI_CR1_RXONLY; 
    *SPI1_CR1 &= ~SPI_CR1_BIDIMODE; // mosi, miso full duplet

    *SPI1_CR1 |= SPI_CR1_SPE; // on spi
} // cau hinh

SPI_Status_t SPI_transfer_byte(uint8_t tx_data, uint8_t *rx_data)
{
    uint8_t received_data;

    if (((*SPI1_SR) & SPI_SR_OVR) != 0)
    {
        spi_clear_ovr_flag();
        // co the them return SPI_ERROR;
    } 

    if (spi_wait_flag_set(SPI1_SR, SPI_SR_TXE) != SPI_OK)
    {
        return SPI_TIMEOUT;
    }

    *((volatile uint8_t *)SPI1_DR) = tx_data;

    if (spi_wait_flag_set(SPI1_SR, SPI_SR_RXNE) != SPI_OK)
    {
        return SPI_TIMEOUT;
    }

    received_data = *((volatile uint8_t *)SPI1_DR);

    if (spi_wait_flag_reset(SPI1_SR, SPI_SR_BSY) != SPI_OK)
    {
        return SPI_TIMEOUT;
    }

    if (((*SPI1_SR) & (SPI_SR_OVR | SPI_SR_MODF)) != 0)
    {
        spi_clear_ovr_flag();
        return SPI_ERROR;
    }

    if (rx_data != 0)
    {
        *rx_data = received_data;
    }

    return SPI_OK;
}

SPI_Status_t SPI_transmit(const uint8_t *data, uint16_t len)
{
    SPI_Status_t status;

    if ((data == 0) && (len > 0))
    {
        return SPI_ERROR;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        status = SPI_transfer_byte(data[i], 0);
        if (status != SPI_OK)
        {
            return status;
        }
    }

    return SPI_OK;
}

SPI_Status_t SPI_receive(uint8_t *data, uint16_t len)
{
    SPI_Status_t status;

    if ((data == 0) && (len > 0))
    {
        return SPI_ERROR;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        status = SPI_transfer_byte(SPI_DUMMY_BYTE, &data[i]);
        if (status != SPI_OK)
        {
            return status;
        }
    }

    return SPI_OK;
}

SPI_Status_t SPI_transmit_receive(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len)
{
    SPI_Status_t status;

    if (((tx_data == 0) || (rx_data == 0)) && (len > 0))
    {
        return SPI_ERROR;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        status = SPI_transfer_byte(tx_data[i], &rx_data[i]);
        if (status != SPI_OK)
        {
            return status;
        }
    }
    return SPI_OK;
}

