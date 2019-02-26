/*
 * lorarx.c
 *
 * Author:   Christopher Madrigal
 * Modified: 21 February 2020
 *
 * To connect to the Pi, use this pinout:
 *
 * RPI             Chip
 * --------------------
 * Pin 14 (GND)      0V
 * Pin 16            RT
 * Pin 17 (3V3)     3V3
 * Pin 19 (MOSI)     SI
 * Pin 21 (MISO)     SO
 * Pin 23 (SCLK)     CK
 * Pin 24            CS
 */

#include "spi.h"
#include "lora.h"
#include "helper.h"
#include <bcm2835.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

/*
 * Signal capture
 */
void handle_signal( int signum ) {
  printf( "\r\n\r\nProcess killed ;(\r\nHave a nice day!\r\n\r\n" );
  lora_close();
  spi_close();
  bcm2835_close();
  exit( EXIT_SUCCESS );
}

//payload
char payload[25];

void rx_test() {
  
  uint8_t numBytes;
  int8_t rssidB;
  
  printf( "Starting RX Test...\r\n" );
  while( 1 ) {
    
    //switch back to cont.
    write_reg( REG_OP_MODE, LORA_RX_CONT );
    
    //wait for Rx to end
    uint8_t flag;
    while( !(flag = read_reg( REG_IRQ_FLAGS ) && FLAG_RX_DONE) ) {
      printf( "Num bytes available: %d\r\n", numBytes =  read_reg( REG_RX_NUM_BYTES ) );
    }
    
    //confirm Rx
    write_reg( REG_OP_MODE, LORA_STANDBY );
    write_reg( REG_IRQ_FLAGS, CLEAR_IRQ_FLAGS );
    
    //set SPI access to fifo at top of last Rx
    write_reg( REG_FIFO_ADDR_PTR, read_reg( REG_FIFO_RX_CURRENT_ADDR ) );
    
    //load payload from FIFO
    for( size_t i = 0; i < numBytes; i++ ){
      payload[i] = read_reg( REG_FIFO );
    }
    
    rssidB = read_reg( REG_PACKET_RSSI ) - 157;
    printf( "Packet received, strength %d dBm\r\n------------------------\r\n", rssidB );
    printf( payload );
    printf( "\r\n------------------------\r\n" );
  }
}

int main( int argc, char **argv ){
  
  //handle any signal errors
  signal( SIGINT, handle_signal );
  
  // BCM Lib initialization
  if( !bcm2835_init() ) {
    printf( "bcm2835_init failed.  Must run as root.\r\n\r\n" );
    return EXIT_FAILURE;
  }
  
  // setup spi
  spi_init();
  
  // boot sequence for LoRa standby
  if( lora_init() ) {
    rx_test();
  }
  
  lora_close();
  spi_close();
  bcm2835_close();
  
  return EXIT_SUCCESS;
}
