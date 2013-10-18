/*
    System tick timer for ATmega8 @ 8Mhz on 8bit TIMER0
 */

#ifndef ST_H
#define ST_H

typedef unsigned long int uint32_t;

uint32_t st_millis(void);
void st_init(void);

#endif    // ST_H
