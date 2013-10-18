/*
Выдержка из http://easyelectronics.ru/avr-uchebnyj-kurs-ispolzovanie-shim.html

Например, надо нам прерывание каждую миллисекунду. И чтобы вот точно. Как
это реализовать проще? Через Режим СТС! Пусть у нас частота 8Мгц.

Прескалер будет равен 64, таким образом, частота тиков таймера составит 125000 Гц.
А нам надо прерывание с частотой 1000Гц. Поэтому настраиваем прерывание по совпадению с числом 125.

Дотикал до 125 — дал прерывание, обнулился. Дотикал до 125 — дал прерывание, обнулился. И так бесконечно, пока не выключим.

Вот вам и точная тикалка.

Нет, конечно, можно и вручную. Через переполнение, т.е. дотикал до переполнения,
загрузил в обработчике прерывания заново нужные значение TCNTх=255-125,
сделал нужные полезные дела и снова тикать до переполнения. Но ведь через СТС красивей! :)

У TIMER0 CTC нет, поэтому делаем вручную:
8000000 / 64 = 125000 / 125 = 1000 Hz = 1 ms
*/

#include <avr/interrupt.h>
#include <util/atomic.h>

#include "st.h"

#define ST_CTC_HANDMADE 255-125

volatile static uint32_t st_timer0_millis;

ISR(TIMER0_OVF_vect)
{
    st_timer0_millis++;
    TCNT0 = ST_CTC_HANDMADE;
}

uint32_t st_millis(void)
{
    uint32_t m;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        m = st_timer0_millis;
    }

    return m;
}

void inline st_init(void)
{
    // Set prescaler to 64
    TCCR0 |= (_BV(CS01) | _BV(CS00));

    // Enable interrupt
    TIMSK |= _BV(TOIE0);

    // Set default value
    TCNT0 = ST_CTC_HANDMADE;
}
