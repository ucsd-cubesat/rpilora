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
char payload[25] = "Hello, world!";

void tx_test() {
  
  printf( "Starting TX Test...\r\n" );
  while( 1 ) {
    
    //set SPI access to fifo to the Tx base reg
    write_reg( REG_FIFO_ADDR_PTR, read_reg( REG_FIFO_TX_BASE_ADDR ) );
    
    //load payload into FIFO
    for( size_t i = 0; i < sizeof( payload ); i++ ){
      write_reg( REG_FIFO, payload[i] );
    }
    
    //commence Tx
    write_reg( REG_OP_MODE, LORA_TX );
    
    //wait for Tx to end
    while( (read_reg( REG_IRQ_FLAGS ) & FLAG_TX_DONE) == 0 ) {}
    
    //confirm Tx
    if( read_reg( REG_OP_MODE ) == LORA_STANDBY ) {
      write_reg( REG_IRQ_FLAGS, CLEAR_IRQ_FLAGS );
      printf( "Transmitted payload.\r\n" );
    }
    else {
      //reinit lora module
      lora_init();
      printf( "LoRa was reinitialized.\r\n" );
    }
    
    //wait 5 seconds
    time_delay( 0.5 );
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
    tx_test();
  }
  
  lora_close();
  spi_close();
  bcm2835_close();
  
  return EXIT_SUCCESS;
}
