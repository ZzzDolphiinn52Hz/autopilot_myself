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
        NULL
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

    spi_status = SPI_transfer_byte(value, &received_status);

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
   // uint8_t received_dummy;
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
            NULL
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
        if (SPI_transfer_byte( data[i],&received_status) != SPI_OK)
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
   // uint8_t received_dummy;
    uint8_t i;

    if ((data == NULL) || (length == 0U) || (length > 32U))
    {
        return NRF24_ERROR;
    }


    NRF24_CSN_Low();



    if (SPI_transfer_byte( NRF24_CMD_W_TX_PAYLOAD ,NULL ) != SPI_OK)
    {
        NRF24_CSN_High();
        return NRF24_ERROR;
    }


    for (i = 0U; i < length; i++)
    {
        if (SPI_transfer_byte( data[i],&received_status) != SPI_OK)
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


NRF24_Status_t NRF24_SetChannel(uint8_t channel)
{
    if (channel > NRF24_MAX_CHANNEL)
    {
        return NRF24_INVALID_PARAM;
    }

    return NRF24_WriteRegister(
        NRF24_REG_RF_CH,
        channel,
        NULL
    );
}

NRF24_Status_t NRF24_SetDataRate(NRF24_DataRate_t data_rate)
{
    NRF24_Status_t result;
    uint8_t rf_setup;

    result = NRF24_ReadRegister(
        NRF24_REG_RF_SETUP,
        &rf_setup,
        NULL
    );

    if (result != NRF24_OK)
    {
        return result;
    }

    /*
     * Xóa hai bit RF_DR_LOW và RF_DR_HIGH.
     */
    rf_setup &= (uint8_t)~(
        NRF24_RF_SETUP_RF_DR_LOW |
        NRF24_RF_SETUP_RF_DR_HIGH
    );

    switch (data_rate)
    {
        case NRF24_DATA_RATE_1MBPS:
            /* Cả hai bit bằng 0 */
            break;

        case NRF24_DATA_RATE_2MBPS:
            rf_setup |= NRF24_RF_SETUP_RF_DR_HIGH;
            break;

        case NRF24_DATA_RATE_250KBPS:
            rf_setup |= NRF24_RF_SETUP_RF_DR_LOW;
            break;

        default:
            return NRF24_INVALID_PARAM;
    }

    return NRF24_WriteRegister(
        NRF24_REG_RF_SETUP,
        rf_setup,
        NULL
    );
}


NRF24_Status_t NRF24_SetPayloadSize(uint8_t pipe, uint8_t payload_size)
{
    if ((pipe > 5U) ||
        (payload_size == 0U) ||
        (payload_size > NRF24_MAX_PAYLOAD_SIZE))
    {
        return NRF24_INVALID_PARAM;
    }

    return NRF24_WriteRegister((uint8_t)(NRF24_REG_RX_PW_P0 + pipe), payload_size,NULL);
}


NRF24_Status_t NRF24_OpenReadingPipe0(const uint8_t *address, uint8_t address_length)
{
    NRF24_Status_t result;
    uint8_t en_rxaddr;

    if ((address == NULL) ||
        (address_length < 3U) ||
        (address_length > 5U))
    {
        return NRF24_INVALID_PARAM;
    }

    /*
     * Ghi địa chỉ đầy đủ cho pipe 0.
     */
    result = NRF24_WriteRegisterMulti(
        NRF24_REG_RX_ADDR_P0,
        address,
        address_length,
        NULL
    );

    if (result != NRF24_OK)
    {
        return result;
    }

    /*
     * Bật pipe 0 trong EN_RXADDR.
     */
    result = NRF24_ReadRegister(
        NRF24_REG_EN_RXADDR,
        &en_rxaddr,
        NULL
    );

    if (result != NRF24_OK)
    {
        return result;
    }

    en_rxaddr |= (1U << 0);

    return NRF24_WriteRegister(
        NRF24_REG_EN_RXADDR,
        en_rxaddr,
        NULL
    );
}


NRF24_Status_t NRF24_FlushRX(void)
{
    return NRF24_SendCommand(
        NRF24_CMD_FLUSH_RX,
        NULL
    );
}


NRF24_Status_t NRF24_ClearInterrupts(void)
{
    return NRF24_WriteRegister(
        NRF24_REG_STATUS,
        (NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT),
        NULL
    );
}


