/*
 * AVRAssembler1.asm
 *
 *  Created: 11/27/2015 7:11:40 PM
 *   Author: family
 */ 
.include "m8def.inc"
.org 0x0000
 ; Define pull-ups and set outputs high
; Define directions for port pins
rjmp main
main :
ldi r16,(1<<PC3)
ldi r17,(1<<DDC3)
out PORTC,r16
out DDRC,r17

