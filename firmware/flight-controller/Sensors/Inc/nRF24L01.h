#ifndef NRF24L01_H_
#define NRF24L01_H_

#include <stddef.h>
#include <stdint.h>

/* nRF24 Pin Definition */

#define NRF24_CE_PIN      15
#define NRF24_CSN_PIN     0

/* =========================================================
 * nRF24L01+ SPI Commands
 * ========================================================= */

#define NRF24_CMD_R_REGISTER          0x00
#define NRF24_CMD_W_REGISTER          0x20

#define NRF24_CMD_R_RX_PAYLOAD        0x61
#define NRF24_CMD_W_TX_PAYLOAD        0xA0

#define NRF24_CMD_FLUSH_TX            0xE1
#define NRF24_CMD_FLUSH_RX            0xE2
#define NRF24_CMD_REUSE_TX_PL         0xE3

#define NRF24_CMD_R_RX_PL_WID         0x60
#define NRF24_CMD_W_ACK_PAYLOAD       0xA8
#define NRF24_CMD_W_TX_PAYLOAD_NOACK  0xB0

#define NRF24_CMD_NOP                 0xFF

/*
 * R_REGISTER và W_REGISTER chỉ dùng 5 bit thấp
 * để chứa địa chỉ thanh ghi.
 */
#define NRF24_REGISTER_MASK           0x1FU


/* =========================================================
 * nRF24L01+ Register Map
 * ========================================================= */

#define NRF24_REG_CONFIG              0x00
#define NRF24_REG_EN_AA               0x01
#define NRF24_REG_EN_RXADDR           0x02
#define NRF24_REG_SETUP_AW            0x03
#define NRF24_REG_SETUP_RETR          0x04
#define NRF24_REG_RF_CH               0x05
#define NRF24_REG_RF_SETUP            0x06
#define NRF24_REG_STATUS              0x07
#define NRF24_REG_OBSERVE_TX          0x08
#define NRF24_REG_RPD                 0x09

#define NRF24_REG_RX_ADDR_P0          0x0A
#define NRF24_REG_RX_ADDR_P1          0x0B
#define NRF24_REG_RX_ADDR_P2          0x0C
#define NRF24_REG_RX_ADDR_P3          0x0D
#define NRF24_REG_RX_ADDR_P4          0x0E
#define NRF24_REG_RX_ADDR_P5          0x0F

#define NRF24_REG_TX_ADDR             0x10

#define NRF24_REG_RX_PW_P0            0x11
#define NRF24_REG_RX_PW_P1            0x12
#define NRF24_REG_RX_PW_P2            0x13
#define NRF24_REG_RX_PW_P3            0x14
#define NRF24_REG_RX_PW_P4            0x15
#define NRF24_REG_RX_PW_P5            0x16

#define NRF24_REG_FIFO_STATUS         0x17
#define NRF24_REG_DYNPD               0x1C
#define NRF24_REG_FEATURE             0x1D


/* =========================================================
 * CONFIG Register Bits
 * ========================================================= */

#define NRF24_CONFIG_PRIM_RX          (1 << 0)
#define NRF24_CONFIG_PWR_UP           (1 << 1)
#define NRF24_CONFIG_CRCO             (1 << 2)
#define NRF24_CONFIG_EN_CRC           (1 << 3)
#define NRF24_CONFIG_MASK_MAX_RT      (1 << 4)
#define NRF24_CONFIG_MASK_TX_DS       (1 << 5)
#define NRF24_CONFIG_MASK_RX_DR       (1 << 6)


/* =========================================================
 * EN_AA Register Bits
 * ========================================================= */

#define NRF24_ENAA_P0                 (1 << 0)
#define NRF24_ENAA_P1                 (1 << 1)
#define NRF24_ENAA_P2                 (1 << 2)
#define NRF24_ENAA_P3                 (1 << 3)
#define NRF24_ENAA_P4                 (1 << 4)
#define NRF24_ENAA_P5                 (1 << 5)


/* =========================================================
 * EN_RXADDR Register Bits
 * ========================================================= */

#define NRF24_ERX_P0                  (1 << 0)
#define NRF24_ERX_P1                  (1 << 1)
#define NRF24_ERX_P2                  (1 << 2)
#define NRF24_ERX_P3                  (1 << 3)
#define NRF24_ERX_P4                  (1 << 4)
#define NRF24_ERX_P5                  (1 << 5)


/* =========================================================
 * SETUP_AW Register Values
 * ========================================================= */

#define NRF24_ADDRESS_WIDTH_3_BYTES   0x01
#define NRF24_ADDRESS_WIDTH_4_BYTES   0x02
#define NRF24_ADDRESS_WIDTH_5_BYTES   0x03


/* =========================================================
 * RF_SETUP Register Bits
 * ========================================================= */

#define NRF24_RF_SETUP_LNA_HCURR      (1 << 0)
#define NRF24_RF_SETUP_RF_PWR_LOW     (1 << 1)
#define NRF24_RF_SETUP_RF_PWR_HIGH    (1 << 2)
#define NRF24_RF_SETUP_RF_DR_HIGH     (1 << 3)
#define NRF24_RF_SETUP_PLL_LOCK       (1 << 4)
#define NRF24_RF_SETUP_RF_DR_LOW      (1 << 5)
#define NRF24_RF_SETUP_CONT_WAVE      (1 << 7)


/* =========================================================
 * STATUS Register Bits
 * ========================================================= */

#define NRF24_STATUS_TX_FULL          (1 << 0)

#define NRF24_STATUS_RX_P_NO_MASK     (0x07 << 1)
#define NRF24_STATUS_RX_P_NO_SHIFT    1

#define NRF24_STATUS_MAX_RT           (1 << 4)
#define NRF24_STATUS_TX_DS            (1 << 5)
#define NRF24_STATUS_RX_DR            (1 << 6)

/* =========================================================
 * FIFO_STATUS Register Bits
 * ========================================================= */

#define NRF24_FIFO_RX_EMPTY           (1 << 0)
#define NRF24_FIFO_RX_FULL            (1 << 1)

#define NRF24_FIFO_TX_EMPTY           (1 << 4)
#define NRF24_FIFO_TX_FULL            (1 << 5)
#define NRF24_FIFO_TX_REUSE           (1 << 6)


/* =========================================================
 * DYNPD Register Bits
 * ========================================================= */

#define NRF24_DPL_P0                  (1 << 0)
#define NRF24_DPL_P1                  (1 << 1)
#define NRF24_DPL_P2                  (1 << 2)
#define NRF24_DPL_P3                  (1 << 3)
#define NRF24_DPL_P4                  (1 << 4)
#define NRF24_DPL_P5                  (1 << 5)


/* =========================================================
 * FEATURE Register Bits
 * ========================================================= */

#define NRF24_FEATURE_EN_DYN_ACK      (1 << 0)
#define NRF24_FEATURE_EN_ACK_PAY      (1 << 1)
#define NRF24_FEATURE_EN_DPL          (1 << 2)


/* =========================================================
 * General Constants
 * ========================================================= */

#define NRF24_MAX_PAYLOAD_SIZE        32
#define NRF24_MAX_ADDRESS_WIDTH       5
#define NRF24_MIN_RF_CHANNEL          0
#define NRF24_MAX_RF_CHANNEL          125

#define NRF24_SPI_DUMMY_BYTE          0xFF


/* =========================================================
 * Driver Status
 * ========================================================= */

typedef enum
{
    NRF24_OK = 0,
    NRF24_ERROR,
    NRF24_SPI_ERROR,
    NRF24_TIMEOUT,
    NRF24_INVALID_PARAM,
    NRF24_NO_DATAA
} NRF24_Status_t;

/* =========================================================
 * Data Status
 * ========================================================= */
typedef enum
{
    NRF24_NO_DATA = 0,
    NRF24_DATA_AVAILABLE = 1
} NRF24_DataStatus_t;


/* =========================================================
 * Data Rate 
 * ========================================================= */
typedef enum
{
    NRF24_DATA_RATE_1MBPS = 0,
    NRF24_DATA_RATE_2MBPS,
    NRF24_DATA_RATE_250KBPS
} NRF24_DataRate_t;


/* =========================================================
 * Basic Driver Functions
 * ========================================================= */


/**
 * @brief Khởi tạo các chân CE và CSN của nRF24L01+.
 */
void NRF24_InitPins(void);

/**
 * @brief Đưa chân CE xuống mức thấp.
 */
void NRF24_CE_Low(void);

/**
 * @brief Đưa chân CE lên mức cao.
 */
void NRF24_CE_High(void);

/**
 * @brief Đưa chân CSN xuống mức thấp.
 */
void NRF24_CSN_Low(void);

/**
 * @brief Đưa chân CSN lên mức cao.
 */
void NRF24_CSN_High(void);

/**
 * @brief Đọc một thanh ghi 1 byte.
 *
 * @param reg       Địa chỉ thanh ghi.
 * @param value     Con trỏ nhận giá trị đọc được.
 * @param status    Con trỏ nhận byte STATUS, có thể truyền NULL.
 *
 * @return NRF24_Status_t
 */
NRF24_Status_t NRF24_ReadRegister(uint8_t reg, uint8_t *value, uint8_t *status);

/**
 * @brief Ghi một giá trị 1 byte vào thanh ghi.
 *
 * @param reg       Địa chỉ thanh ghi.
 * @param value     Giá trị cần ghi.
 * @param status    Con trỏ nhận byte STATUS, có thể truyền NULL.
 *
 * @return NRF24_Status_t
 */
NRF24_Status_t NRF24_WriteRegister(uint8_t reg, uint8_t value, uint8_t *status);

/**
 * @brief Đọc nhiều byte từ một thanh ghi.
 *
 * Thường dùng cho RX_ADDR_P0, RX_ADDR_P1 hoặc TX_ADDR.
 *
 * @param reg       Địa chỉ thanh ghi.
 * @param data      Buffer nhận dữ liệu.
 * @param length    Số byte cần đọc.
 * @param status    Con trỏ nhận byte STATUS, có thể truyền NULL.
 *
 * @return NRF24_Status_t
 */
NRF24_Status_t NRF24_ReadRegisterMulti(uint8_t reg, uint8_t *data, uint8_t length, uint8_t *status);

/**
 * @brief Ghi nhiều byte vào một thanh ghi.
 *
 * Thường dùng cho RX_ADDR_P0, RX_ADDR_P1 hoặc TX_ADDR.
 *
 * @param reg       Địa chỉ thanh ghi.
 * @param data      Buffer chứa dữ liệu cần ghi.
 * @param length    Số byte cần ghi.
 * @param status    Con trỏ nhận byte STATUS, có thể truyền NULL.
 *
 * @return NRF24_Status_t
 */
NRF24_Status_t NRF24_WriteRegisterMulti( uint8_t reg, const uint8_t *data, uint8_t length, uint8_t *status);

/**
 * @brief Gửi một command không kèm dữ liệu.
 *
 * Ví dụ: FLUSH_TX, FLUSH_RX hoặc NOP.
 *
 * @param command   Command cần gửi.
 * @param status    Con trỏ nhận byte STATUS, có thể truyền NULL.
 *
 * @return NRF24_Status_t
 */
NRF24_Status_t NRF24_SendCommand(uint8_t command, uint8_t *status);

/**
 * @brief Đọc một payload từ packet đầu tiên trong RX FIFO.
 *
 * Hàm gửi command R_RX_PAYLOAD, sau đó tạo xung clock SPI bằng
 * các dummy byte để nhận toàn bộ payload từ nRF24L01+.
 * Khi payload được đọc hoàn tất, packet tương ứng được lấy ra khỏi RX FIFO.
 *
 * @param data      Buffer nhận payload.
 * @param length    Số byte cần đọc, hợp lệ từ 1 đến NRF24_MAX_PAYLOAD_SIZE.
 * @param status    Con trỏ nhận byte STATUS trả về cùng command,
 *                  có thể truyền NULL nếu không cần sử dụng.
 *
 * @return NRF24_OK nếu đọc thành công.
 * @return NRF24_ERROR nếu tham số không hợp lệ hoặc giao tiếp SPI thất bại.
 */
NRF24_Status_t NRF24_ReadPayload( uint8_t *data, uint8_t length, uint8_t *status);


/**
 * @brief Ghi một payload vào TX FIFO để chuẩn bị truyền qua RF.
 *
 * Hàm gửi command W_TX_PAYLOAD, sau đó ghi lần lượt các byte trong
 * buffer data vào TX FIFO. Hàm này chỉ nạp dữ liệu vào FIFO;
 * nRF24L01+ chỉ bắt đầu phát khi được đặt đúng TX mode và kích chân CE.
 *
 * @param data      Buffer chứa payload cần ghi.
 * @param length    Số byte cần ghi, hợp lệ từ 1 đến NRF24_MAX_PAYLOAD_SIZE.
 * @param status    Con trỏ nhận byte STATUS trả về cùng command,
 *                  có thể truyền NULL nếu không cần sử dụng.
 *
 * @return NRF24_OK nếu ghi thành công.
 * @return NRF24_ERROR nếu tham số không hợp lệ hoặc giao tiếp SPI thất bại.
 */
NRF24_Status_t NRF24_WritePayload(const uint8_t *data, uint8_t length, uint8_t *status);


/**
 * @brief Kiểm tra RX FIFO có chứa ít nhất một packet hay không.
 *
 * Hàm đọc thanh ghi FIFO_STATUS và kiểm tra bit RX_EMPTY.
 * RX_EMPTY bằng 0 nghĩa là RX FIFO đang có dữ liệu chờ đọc.
 * Hàm này không đọc và cũng không xóa payload khỏi RX FIFO.
 *
 * @return NRF24_DATA_AVAILABLE nếu RX FIFO có ít nhất một packet.
 * @return NRF24_NO_DATA nếu RX FIFO rỗng hoặc không đọc được FIFO_STATUS.
 */
NRF24_DataStatus_t NRF24_IsDataAvailable(void);

NRF24_Status_t NRF24_SetChannel(uint8_t channel);

NRF24_Status_t NRF24_SetDataRate(NRF24_DataRate_t data_rate);

NRF24_Status_t NRF24_SetPayloadSize(uint8_t pipe, uint8_t payload_size);

NRF24_Status_t NRF24_OpenReadingPipe0(const uint8_t *address, uint8_t address_length);

NRF24_Status_t NRF24_FlushRX(void);

NRF24_Status_t NRF24_ClearInterrupts(void);

NRF24_Status_t NRF24_StartListening(void);

NRF24_Status_t NRF24_Receive(uint8_t *data, uint8_t length);

NRF24_Status_t NRF24_InitReceiver(const uint8_t *address, uint8_t address_length, uint8_t payload_size);


#endif /* NRF24L01_H_ */



