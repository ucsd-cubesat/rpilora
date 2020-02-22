/* Filename: lora.h
 * Last Edited: 7 May 2019
 * Author: TritonCubed
 *
 * This is a small library for the RFM95/96/97/98 in LoRa mode.
 * This is an adaptatoin of the library for the SX1278 for the
 * "Shannon" payload. The module operates around 868MHz
 */

#include <sys/types.h>
#include <stdint.h>

/* These are the register addresses for the LoRa Mode */

/* FIFO read/write access */
#define REG_FIFO                    0x00

/* Operating mode & LoRa selection */
#define REG_OP_MODE                 0x01

/* MSB to LSB of RF carrier frequency */
#define REG_RF_FREQ_MSB             0x06
#define REG_RF_FREQ_MID             0x07
#define REG_RF_FREQ_LSB             0x08

/* PA selection and Output Power Control */
#define REG_PA_CONFIG               0x09

/* Control of PA ramp time, low phase nose PLL */
#define REG_PA_RAMP                 0x0A

/* Over Current Protection control */
#define REG_OCP                     0x0B

/* LNA Settings */
#define REG_LNA                     0x0C

/* FIFO SPI pointer */
#define REG_FIFO_ADDR_PTR           0x0D

/* Start TX data */
#define REG_FIFO_TX_BASE_ADDR       0x0E

/* Start RX data */
#define REG_FIFO_RX_BASE_ADDR       0x0F

/* Start of last packet */
#define REG_FIFO_RX_CURRENT_ADDR    0x10

/* Masks IRQs where bits are set */
#define REG_IRQ_FLAGS_MASK          0x11

/* IRQ's for different operations */
#define REG_IRQ_FLAGS               0x12

/* Number of bytes of last packet */
#define REG_RX_NUM_BYTES            0x13

/* Number of valid headrs since last RX */
#define REG_RX_HEADER_COUNT_MSB     0x14
#define REG_RX_HEADER_COUNT_LSB     0x15

/* Number of valid packets since last RX */
#define REG_RX_PACKET_COUNT_MSB     0x16
#define REG_RX_PACKET_COUNT_LSB     0x17

/* Modem status */
#define REG_MODEM_STAT              0x18

/* Estimation of Signal Noise Ratio on last packet */
#define REG_PACKET_SNR              0x19

/* Estimation of Recieved Signal Strength Indication of last packet*/
#define REG_PACKET_RSSI             0x1A

/* Current RSSI */
#define REG_RSSI_CURRENT            0x1B

/* Informatiion about in-use hopping channel */
#define REG_HOP_CHANNEL             0x1C

/* Modem configuration */
#define REG_MODEM_CONFIG1           0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_SYMB_TIMEOUT_LSB        0x1F
#define REG_MODEM_CONFIG3           0x26
#define REG_SYNC_WORD               0x39
#define REG_PA_DAC                  0x4D

/* Preamble length */
#define REG_PREAMBLE_LEN_MSB        0x20
#define REG_PREAMBLE_LEN_LSB        0x21

/* Payload length in bytes */
#define REG_PAYLOAD_LEN             0x22

/* Maximum payload length */
#define REG_MAX_PAYLOAD_LEN         0x23

/* Symbole periods between frequency hops */
#define REG_HOP_PERIOD              0x24

/* IRQ Flags */
#define CLEAR_IRQ_FLAGS        0xFF
#define FLAG_RX_TIMEOUT        0x80
#define FLAG_RX_DONE           0x40
#define FLAG_PAYLOAD_CRC_ERROR 0x20
#define FLAG_VALID_HEADER      0x10
#define FLAG_TX_DONE           0x08
#define FLAG_CAD_DONE          0x04
#define FLAG_FHSS_CHANGE_CHAN  0x02
#define FLAGS_CAD_DETECTED     0x01

/* RFM95 LoRa device modes */
#define LORA_MODE      0x80
#define FSK_MODE       0x00

#define LORA_SLEEP     0x88
#define FSK_SLEEP      0x08
#define LORA_STANDBY   0x89
#define LORA_TX_SYNTH  0x8A
#define LORA_TX        0x8B
#define LORA_RX_SYNTH  0x8C
#define LORA_RX_CONT   0x8D
#define LORA_RX_SINGLE 0x8E
#define LORA_CAD       0x8F

/* Register Defaults */
#define DEFAULT_FIFO_TX_BASE_ADDR 0x80
#define DEFAULT_FIFO_RX_BASE_ADDR 0x00
#define PA_BOOST                  0x80

/* Frequency Calcualtion */
#define LORA_FREQ(f) (((uint64_t)f << 19)/32E6)

/* Init Values */
#define BUFFER_SIZE 50
#define DEFAULT_LORA_FREQ LORA_FREQ(915E6)
#define DEFAULT_LORA_LNA 0x03
#define DEFAULT_LORA_AGC 0x04
#define DEFAULT_LORA_SYNC_WORD 'k'
#define DEFAULT_LORA_LEVEL 17

/*
 * Boots the lora module.
 * Requires SPI to be initialized.
 */
uint8_t lora_init();

/*
 * Cleans the lora module.
 */
void lora_close();

/*
 * Reads from the specified register.
 * Returns the value in the register.
 */
uint8_t read_reg( uint8_t reg );

/*
 * Writes to the specified register.
 * Returns the past value in the register.
 */
uint8_t write_reg( uint8_t reg, char data );
