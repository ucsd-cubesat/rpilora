/*
 * spi.c
 *
 *  Created on: Feb 3, 2019
 *      Author: TritonCubed
 */

#include "bcm2835.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * Configures SPI with proper bit rate
 */
void spi_init(){

  if( stderr, !bcm2835_spi_begin() ){
    fprintf( stderr, "bcm2835_spi_begin failed.  Must run as root.\n" );
    exit( EXIT_FAILURE );
  }

  //The following five functions define the SPI communication operating
  //parameters.  Some are default values but redundancy never hurt anyone.

  //Sets bit order. The SX1278 is expecting to receive MSB first and will
  //also be transmitting back MSB first.  This is a default.
  bcm2835_spi_setBitOrder( BCM2835_SPI_BIT_ORDER_MSBFIRST );

  //Sets the clock polarity and phase. The LoRa module asks for CPOL = 0
  //and CPHA = 0.  This also happens to be the default.
  bcm2835_spi_setDataMode( BCM2835_SPI_MODE0 );

  //Sets the clock divider and therefore the clock speed.  The default is
  //65536 which results in a clock speed of 3.8 kHz. I'm using 2048 which
  //corresponds to 122 kHz.  Troubleshooting SPI connection.
  bcm2835_spi_setClockDivider( BCM2835_SPI_CLOCK_DIVIDER_65536 );

  //Specifies which chip select pins will be asserted when an SPI transfer
  //is made.  The RPI2 has two but we'll just be using RPI pin #24 also
  //known as "SPI_CE0_N". This is a default setting defined below.
  bcm2835_spi_chipSelect( BCM2835_SPI_CS0 );

  //Configured the polarity under which the chip select is considered active.
  //The SX1278 looks for a chip select active low.  This is a default setting
  //given below.
  bcm2835_spi_setChipSelectPolarity( BCM2835_SPI_CS0, LOW );

  //set a GPIO pin high to stabilize reset pin on sx1278
  //configure RPI pin 16 tp gpio output functionality
  //then set that pin high
  bcm2835_gpio_fsel( RPI_V2_GPIO_P1_16, BCM2835_GPIO_FSEL_OUTP );
  bcm2835_gpio_set( RPI_V2_GPIO_P1_16 );
}

/*
 * Closes the SPI session.
 */
void spi_close() {
  bcm2835_spi_end();
}
