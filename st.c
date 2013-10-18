/*
�������� �� http://easyelectronics.ru/avr-uchebnyj-kurs-ispolzovanie-shim.html

��������, ���� ��� ���������� ������ ������������. � ����� ��� �����. ���
��� ����������� �����? ����� ����� ���! ����� � ��� ������� 8���.

��������� ����� ����� 64, ����� �������, ������� ����� ������� �������� 125000 ��.
� ��� ���� ���������� � �������� 1000��. ������� ����������� ���������� �� ���������� � ������ 125.

������� �� 125 � ��� ����������, ���������. ������� �� 125 � ��� ����������, ���������. � ��� ����������, ���� �� ��������.

��� ��� � ������ �������.

���, �������, ����� � �������. ����� ������������, �.�. ������� �� ������������,
�������� � ����������� ���������� ������ ������ �������� TCNT�=255-125,
������ ������ �������� ���� � ����� ������ �� ������������. �� ���� ����� ��� ��������! :)

� TIMER0 CTC ���, ������� ������ �������:
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
