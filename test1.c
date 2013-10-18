/*************************************************************************
Title:    example program for the Interrupt controlled UART library
Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:     $Id: test_uart.c,v 1.5 2012/09/14 17:59:08 peter Exp $
Software: AVR-GCC 3.4, AVRlibc 1.4
Hardware: any AVR with built-in UART, tested on AT90S8515 at 4 Mhz

DESCRIPTION:
          This example shows how to use the UART library uart.c

*************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>

#include "uart.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define UART_BAUD_RATE      9600

#define ATAP 2


#define START_BYTE 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13
#define MAX_FRAME_DATA_SIZE 110
#define NO_ERROR 0
#define CHECKSUM_FAILURE 1
#define PACKET_EXCEEDS_BYTE_ARRAY_LENGTH 2
#define UNEXPECTED_START_BYTE 3
#define API_ID_INDEX 3

uint8_t b;
uint8_t _pos;
uint8_t _checksumTotal;
bool _escape = false;
uint8_t _response_ERROR;
uint8_t _response_MsbLength;
uint8_t _response_LsbLength;
uint8_t _response_ApiId;
uint8_t _response_Checksum;
uint8_t _response_FrameLength; 
bool    _response_Available = false;
uint8_t _response_FrameData[32];
      
void resetResponse(){

      _pos = 0;
	  _response_Checksum = 0;
	  _response_Available = false;
      _response_ERROR = 0;

	  for (uint8_t i = 0; i < 32; i++){
	  // uart_putc(_response_FrameData[i]);
	  _response_FrameData[i] = 0;
	  uart_flush();////////////////////////////////////////////////////////////////////////
	  PORTA = 0x00;///////////////////////////////////////////////

	  }

}

uint16_t getPacketLength() {
        return ((_response_MsbLength << 8) & 0xff) + (_response_LsbLength & 0xff);
}


void readPacket() {
        // reset previous response
        if ((_response_Available == true) || (_response_ERROR > 0)) {
                // discard previous packet and start over
                resetResponse();
        }


    while (uart_available()) {


        b = uart_getc();

        if (_pos > 0 && b == START_BYTE && ATAP == 2) {
                // new packet start before previous packeted completed -- discard previous packet and start over
                _response_ERROR = UNEXPECTED_START_BYTE;
				 uart_putc(_response_ERROR);
                return;
        }

                if (_pos > 0 && b == ESCAPE) {
                        if (uart_available()) {
                                b = uart_getc();
                                b = 0x20 ^ b;
                        } else {
                                // escape byte.  next byte will be
                                _escape = true;
                                continue;
                        }
                }


                if (_escape == true) {
                        b = 0x20 ^ b;
                        _escape = false;
                }


                // checksum includes all bytes starting with api id
                if (_pos >= API_ID_INDEX) {
                        _checksumTotal+= b;
                }


        switch(_pos) {
                        case 0:
                        if (b == START_BYTE) {
                                _pos++;
                        }
                        break;
                  
				        case 1:
                                // length msb
                                _response_MsbLength = b;
                                _pos++;
                                break;
 
                        case 2:
                                // length lsb
                                _response_LsbLength = b;
                                _pos++;
                                break;

                        case 3:
                                _response_ApiId = b;
                                _pos++;
                                break;

                        default:
                                // starts at fifth byte

                                if (_pos > MAX_FRAME_DATA_SIZE) {
                                        // exceed max size.  should never occur
                                        _response_ERROR = PACKET_EXCEEDS_BYTE_ARRAY_LENGTH;
										 uart_putc(_response_ERROR);
                                        return;
                                }

                                // check if we're at the end of the packet
                                // packet length does not include start, length, or checksum bytes, so add 3
                                if (_pos == (getPacketLength() + 3)) {
                                        // verify checksum
                                        

                                        if ((_checksumTotal & 0xff) == 0xff) {
                                                _response_Checksum = b;
                                                _response_Available = true;
												uart_puts_P("OK\n");///////////////////////////
												DDRA = 0x02;////////////////////////////////////
												PORTA = 0x02;//////////////////////////

                                                _response_ERROR = NO_ERROR;
                                        } else {
                                                // checksum failed
                                                _response_ERROR = CHECKSUM_FAILURE;
												  uart_putc(_response_ERROR);
	
                                        }

                                        // minus 4 because we start after start,msb,lsb,api and up to but not including checksum
                                        // e.g. if frame was one byte, _pos=4 would be the byte, pos=5 is the checksum, where end stop reading
                                        _response_FrameLength = (_pos - 4);

                                        // reset state vars
                                        _pos = 0;

                                        _checksumTotal = 0;

                                        return;
                                } else {
                                        // add to packet array, starting with the fourth byte of the apiFrame
                                        _response_FrameData[_pos - 4] = b;
                                        _pos++;
                                }
        }
    }
}

int main(void)
{
 
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    
    sei();
          
    uart_puts_P("String stored in FLASH\n");
    
    uart_putc('\r');
    
    
	for(;;)  {


			readPacket();

 
	}
}
