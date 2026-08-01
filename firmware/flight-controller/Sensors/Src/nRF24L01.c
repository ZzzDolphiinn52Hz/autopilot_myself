#include "nRF24L01.h"
#include "clock.h"
#include "spi.h"
#include "delay.h"




void NRF24_InitPins()
{
  clock_enable_AHB1(GPIOA_peripheral);

    /* CE Output */

    *GPIOA_MODER &= ~(0b11 << (NRF24_CE_PIN * 2));
    *GPIOA_MODER |=  (0b01 << (NRF24_CE_PIN * 2));

    /* CSN Output */

    *GPIOA_MODER &= ~(0b11 << (NRF24_CSN_PIN * 2));
    *GPIOA_MODER |=  (0b01 << (NRF24_CSN_PIN * 2));

    /* Push Pull */

    *GPIOA_OTYPER &= ~((1U << NRF24_CE_PIN) |
                       (1U << NRF24_CSN_PIN));

    /* Medium/Fast speed */

    *GPIOA_OSPEEDR &= ~((0b11 << (NRF24_CE_PIN * 2)) |
                        (0b11 << (NRF24_CSN_PIN * 2)));

    *GPIOA_OSPEEDR |=  ((0b10 << (NRF24_CE_PIN * 2)) |
                        (0b10 << (NRF24_CSN_PIN * 2)));

    /* No Pull */

    *GPIOA_PUPDR &= ~((0b11 << (NRF24_CE_PIN * 2)) |
                      (0b11 << (NRF24_CSN_PIN * 2)));

    /* Idle state */

    NRF24_CE_Low();
    NRF24_CSN_High();



}

void NRF24_CE_Low(void)
{
    *GPIOA_BSRR = (1U << (NRF24_CE_PIN + 16));
}

void NRF24_CE_High(void)
{
    *GPIOA_BSRR = (1U << NRF24_CE_PIN);
    delay_us(130);
}

void NRF24_CSN_Low(void)
{
    *GPIOA_BSRR = (1U << (NRF24_CSN_PIN + 16));
}

void NRF24_CSN_High(void)
{
    *GPIOA_BSRR = (1U << NRF24_CSN_PIN);
}


NRF24_Status_t NRF24_SendCommand( uint8_t command, uint8_t *status)
{
        SPI_Status_t spi_status;
    uint8_t received_status = 0U;
    NRF24_CSN_Low();

    spi_status = SPI_transfer_byte(command, &received_status);

    NRF24_CSN_High();

    if (spi_status == SPI_TIMEOUT)
    {
        return NRF24_TIMEOUT;
    }

    if (spi_status != SPI_OK)
    {
        return NRF24_SPI_ERROR;
    }

    if (status != 0)
    {
        *status = received_status;
    }

    return NRF24_OK;
    }

NRF24_Status_t NRF24_ReadRegister( uint8_t reg, uint8_t *value, uint8_t *status)
{
    SPI_Status_t spi_status;
    uint8_t received_status = 0U;

    if (value == NULL)
    {
        return NRF24_INVALID_PARAM;
    }

    if (reg > NRF24_REG_FEATURE)
    {
        return NRF24_INVALID_PARAM;
    }
    NRF24_CSN_Low();

    /*
     * Byte 1:
     * MOSI gửi command R_REGISTER.
     * MISO trả về STATUS.
     */
    spi_status = SPI_transfer_byte(
        NRF24_CMD_R_REGISTER |
        (reg & NRF24_REGISTER_MASK),
        &received_status
    );

    if (spi_status != SPI_OK)
    {
        NRF24_CSN_High();

        if (spi_status == SPI_TIMEOUT)
        {
            return NRF24_TIMEOUT;
        }

        return NRF24_SPI_ERROR;
    }

    /*
     * Byte 2:
     * MOSI gửi dummy byte để tạo clock.
     * MISO trả về giá trị register.
     */
    spi_status = SPI_transfer_byte(
        NRF24_SPI_DUMMY_BYTE,
        value
    );

    NRF24_CSN_High();

    if (spi_status != SPI_OK)
    {
        if (spi_status == SPI_TIMEOUT)
        {
            return NRF24_TIMEOUT;
        }

        return NRF24_SPI_ERROR;
    }

    if (status != NULL)
    {
        *status = received_status;
    }

    return NRF24_OK;
}

NRF24_Status_t NRF24_WriteRegister( uint8_t reg, uint8_t value, uint8_t *status)
{
    SPI_Status_t spi_status;
    uint8_t received_status = 0U;

    if (reg > NRF24_REG_FEATURE)
    {
        return NRF24_INVALID_PARAM;
    }
    NRF24_CSN_Low();

    spi_status = SPI_transfer_byte(
        NRF24_CMD_W_REGISTER |
        (reg & NRF24_REGISTER_MASK),
        &received_status
    );

    if (spi_status != SPI_OK)
    {
        NRF24_CSN_High();

        if (spi_status == SPI_TIMEOUT)
        {
            return NRF24_TIMEOUT;
        }

        return NRF24_SPI_ERROR;
    }

    spi_status = SPI_transfer_byte(value, NULL);

    NRF24_CSN_High();

    if (spi_status != SPI_OK)
    {
        if (spi_status == SPI_TIMEOUT)
        {
            return NRF24_TIMEOUT;
        }

        return NRF24_SPI_ERROR;
    }

    if (status != NULL)
    {
        *status = received_status;
    }

    if (reg == NRF24_REG_CONFIG && (value & NRF24_CONFIG_PWR_UP))
    {
        delay_ms(2);
    }

    return NRF24_OK;
}

NRF24_Status_t NRF24_ReadRegisterMulti( uint8_t reg, uint8_t *data, uint8_t length, uint8_t *status)
{
    uint8_t received_status;
    uint8_t i;

    if ((data == NULL) || (length == 0U))
    {
        return NRF24_ERROR;
    }

    /*
     * Register address của nRF24 chỉ dùng 5 bit thấp.
     */
    reg &= NRF24_REGISTER_MASK;

    NRF24_CSN_Low();
    
    /*
     * Byte đầu tiên:
     * MOSI: R_REGISTER | register address
     * MISO: STATUS
     */
    if (SPI_transfer_byte(
            NRF24_CMD_R_REGISTER | reg,
            &received_status
        ) != SPI_OK)
    {
        NRF24_CSN_High();
        return NRF24_ERROR;
    }
   
    /*
     * Gửi dummy byte để tạo clock,
     * đồng thời nhận các byte dữ liệu từ nRF24.
     */
    for (i = 0U; i < length; i++)
    {
        if (SPI_transfer_byte(
                NRF24_CMD_NOP,
                &data[i]
            ) != SPI_OK)
        {
            NRF24_CSN_High();
            return NRF24_ERROR;
        }
    }
   
    NRF24_CSN_High();

    if (status != NULL)
    {
        *status = received_status;
    }

    return NRF24_OK;
}

NRF24_Status_t NRF24_WriteRegisterMulti( uint8_t reg, const uint8_t *data, uint8_t length, uint8_t *status)
{
    uint8_t received_status;
    uint8_t received_dummy;
    uint8_t i;

    if ((data == NULL) || (length == 0U))
    {
        return NRF24_ERROR;
    }

    /*
     * Register address của nRF24 chỉ dùng 5 bit thấp.
     */
    reg &= NRF24_REGISTER_MASK;

    NRF24_CSN_Low();
    
    /*
     * Byte đầu tiên:
     * MOSI: W_REGISTER | register address
     * MISO: STATUS
     */
    if (SPI_transfer_byte(
            NRF24_CMD_W_REGISTER | reg,
            &received_status
        ) != SPI_OK)
    {
        NRF24_CSN_High();
        return NRF24_ERROR;
    }
   
    /*
     * Gửi dummy byte để tạo clock,
     * không cần nhận các byte gửi về từ nRF24
     */
    for (i = 0U; i < length; i++)
    {
        if (SPI_transfer_byte( data[i],&received_dummy) != SPI_OK)
        {
            NRF24_CSN_High();
            return NRF24_ERROR;
        }
    }
  
    NRF24_CSN_High();

    if (status != NULL)
    {
        *status = received_status;
    }

    return NRF24_OK;
}

NRF24_Status_t NRF24_ReadPayload(uint8_t *data, uint8_t length,uint8_t *status)
{
    uint8_t received_status;
    uint8_t i;

    if ((data == NULL) || (length == 0U))
    {
        return NRF24_ERROR;
    }

    NRF24_CSN_Low();

    if (SPI_transfer_byte(
            NRF24_CMD_R_RX_PAYLOAD,
            &received_status
        ) != SPI_OK)
    {
        NRF24_CSN_High();
        return NRF24_ERROR;
    }

    for (i = 0; i < length; i++)
    {
        if (SPI_transfer_byte(
                NRF24_CMD_NOP,
                &data[i]
            ) != SPI_OK)
        {
            NRF24_CSN_High();
            return NRF24_ERROR;
        }
    }

    NRF24_CSN_High();

    if (status != NULL)
    {
        *status = received_status;
    }

    return NRF24_OK;
}

NRF24_Status_t NRF24_WritePayload(const uint8_t *data, uint8_t length, uint8_t *status)
{
    uint8_t received_status;
    uint8_t received_dummy;
    uint8_t i;

    if ((data == NULL) || (length == 0U) || (length > 32U))
    {
        return NRF24_ERROR;
    }


    NRF24_CSN_Low();



    if (SPI_transfer_byte( NRF24_CMD_W_TX_PAYLOAD , &received_status ) != SPI_OK)
    {
        NRF24_CSN_High();
        return NRF24_ERROR;
    }


    for (i = 0U; i < length; i++)
    {
        if (SPI_transfer_byte( data[i],&received_dummy) != SPI_OK)
        {
            NRF24_CSN_High();
            return NRF24_ERROR;
        }
    }

    NRF24_CSN_High();

    if (status != NULL)
    {
        *status = received_status;
    }

    return NRF24_OK;
}

NRF24_DataStatus_t NRF24_IsDataAvailable(void)
{
uint8_t fifo_status;

if(NRF24_ReadRegister(NRF24_REG_FIFO_STATUS, &fifo_status, NULL) != NRF24_OK)
{
   return NRF24_NO_DATA;
}

if ((fifo_status & NRF24_FIFO_RX_EMPTY) == 0U)
{
    return NRF24_DATA_AVAILABLE;
}
 return NRF24_NO_DATA;
}
