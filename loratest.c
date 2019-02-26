/*
 * loratest.c
 *
 *  Created on: Feb 17, 2019
 *      Author: TritonCubed
 *
 *  RPI             Chip
 *  --------------------
 *  Pin 14 (GND)      0V
 *  Pin 24            CS
 *  Pin 16            RT
 *  Pin 17 (3V3)     3V3
 *  Pin 23 (SCLK)     CK
 *  Pin 21 (MISO)     SO
 *  Pin 19 (MOSI)     SI
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
  fprintf( stderr, "\n\nProcess killed ;(\nHave a nice day!\n\n" );
  lora_close();
  spi_close();
  bcm2835_close();
  exit( EXIT_SUCCESS );
}

//payload
char payload[25];

/**
 * Helper function to construct payload.
 */
void buildPayload( ) {
  //stub
  //memset( payload, 0, sizeof( payload ) );
  strcpy( payload, "Hello, world!" );
}

void tx_test() {
  
  write_reg( REG_OP_MODE, LORA_SLEEP ); //put lora into sleep mode
  write_reg( REG_FIFO_TX_BASE_ADDR, 0x00 ); //use the whole stack for TX
  write_reg( REG_OP_MODE, LORA_STANDBY ); //LoRa now ready
  
  printf( "Starting TX Test...\n" );
  while( 1 ) {
    
    //make the payload
    buildPayload( payload, sizeof( payload ) );
    
    //set SPI access to fifo to the Tx base reg
    write_reg( REG_FIFO_ADDR_PTR, read_reg( REG_FIFO_TX_BASE_ADDR ) );
    
    //load payload into FIFO
    for( size_t i = 0; i < sizeof( payload ); i++ ){
      write_reg( REG_FIFO, payload[i] );
    }
    
    //commence Tx
    write_reg( REG_OP_MODE, LORA_TX );
    
    //wait for Tx to end
    while( read_reg( REG_IRQ_FLAGS ) != FLAG_TX_DONE ) {}
    
    //confirm Tx
    if( read_reg( REG_OP_MODE ) == LORA_STANDBY ) {
      write_reg( REG_IRQ_FLAGS, CLEAR_IRQ_FLAGS );
      fprintf( stdout, "Transmitted payload.\n" );
    }
    else {
      //reinit lora module
      lora_init( sizeof( payload ) );
      fprintf( stderr, "LoRa was reinitialized.\n" );
    }
    
    //wait 5 seconds
    time_delay( 0.5 );
  }
}

void rx_test() {
  
  write_reg( REG_OP_MODE, LORA_SLEEP ); //put lora into sleep mode
  write_reg( REG_FIFO_RX_BASE_ADDR, 0x00 ); //use the whole stack for RX
  write_reg( REG_OP_MODE, LORA_STANDBY );
  
  printf( "Starting RX Test...\n" );
  while( 1 ) {
    
    write_reg( REG_OP_MODE, LORA_RX_CONT ); //switch back to cont.
    
    //wait for Rx to end
    while( read_reg( REG_IRQ_FLAGS ) != ( FLAG_RX_DONE | FLAG_VALID_HEADER ) ) {}
    
    //confirm Rx
    write_reg( REG_OP_MODE, LORA_STANDBY );
    write_reg( REG_IRQ_FLAGS, CLEAR_IRQ_FLAGS );
    
    //set SPI access to fifo at top of last Rx
    write_reg( REG_FIFO_ADDR_PTR, read_reg( REG_FIFO_RX_CURRENT_ADDR ) );
    
    //load payload from FIFO
    for( size_t i = 0; i < read_reg( REG_RX_NUM_BYTES ); i++ ){
      payload[i] = read_reg( REG_FIFO );
    }
    
    int8_t rssidB = read_reg( REG_PACKET_RSSI ) - 157;
    printf( "Packet received, strength %d dBm\n------------------------\n", rssidB );
    printf( payload );
    printf( "\n------------------------\n" );
  }
}

int main( int argc, char **argv ){
  
  if( argc < 2 ) {
    fprintf( stderr, "Please specify \'t\' for a TX test, or \'r\' for an RX test.\n" );
    return EXIT_FAILURE;
  }
  
  if( argc != 2 ) {
    printf( "\nThis program only takes one argument.\n" );
  }
  
  char test = argv[1][0];
  
  //handle any signal errors
  signal( SIGINT, handle_signal );
  
  // BCM Lib initialization
  if( !bcm2835_init() ) {
    fprintf( stderr, "bcm2835_init failed.  Must run as root.\n" );
    return EXIT_FAILURE;
  }
  
  spi_init();  //setup spi
  lora_init( sizeof( payload ) ); //boot sequence to LoRa standby
  
  if( test == 't' ) {
    tx_test();
  }
  else if( test == 'r' ) {
    rx_test();
  }
  else {
    fprintf( stderr, "Please specify \'t\' for a TX test, or \'r\' for an RX test.\n" );
    return EXIT_FAILURE;
  }
  
  lora_close();
  spi_close();
  bcm2835_close();
  
  return EXIT_SUCCESS;
}
