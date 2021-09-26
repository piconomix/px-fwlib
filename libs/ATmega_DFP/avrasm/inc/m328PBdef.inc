;***** THIS IS A MACHINE GENERATED FILE - DO NOT EDIT ********************

;*************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;*
;* Number            : AVR000
;* File Name         : m328PBdef.inc
;* Title             : Register/Bit Definitions for the ATmega328PB
;* Created           : 2021-07-13 10:39
;* Version           : 1.00
;* Support e-mail    : avr@atmel.com
;* Target MCU        : ATmega328PB
;*
;* DESCRIPTION
;* When including this file in the assembly program file, all I/O register
;* names and I/O register bit names appearing in the data book can be used.
;* In addition, the six registers forming the three data pointers X, Y and
;* Z have been assigned names XL - ZH. Highest RAM address for Internal
;* SRAM is also defined
;*
;*************************************************************************

#ifndef _M328PBDEF_INC_
#define _M328PBDEF_INC_


#pragma partinc 0

; ***** SPECIFY DEVICE ***************************************************
.device	ATmega328PB
#pragma AVRPART ADMIN PART_NAME ATmega328PB
.equ	SIGNATURE_000	= 0x1E
.equ	SIGNATURE_001	= 0x95
.equ	SIGNATURE_002	= 0x16

#pragma AVRPART CORE CORE_VERSION V2E


; ***** I/O REGISTER DEFINITIONS *****************************************
; NOTE:
; Definitions marked "MEMORY MAPPED" are extended I/O ports
; and cannot be used with IN/OUT instructions
.equ	TWAMR1	= 0xDD ; MEMORY MAPPED
.equ	TWCR1	= 0xDC ; MEMORY MAPPED
.equ	TWDR1	= 0xDB ; MEMORY MAPPED
.equ	TWAR1	= 0xDA ; MEMORY MAPPED
.equ	TWSR1	= 0xD9 ; MEMORY MAPPED
.equ	TWBR1	= 0xD8 ; MEMORY MAPPED
.equ	UDR1	= 0xCE ; MEMORY MAPPED
.equ	UBRR1H	= 0xCD ; MEMORY MAPPED
.equ	UBRR1L	= 0xCC ; MEMORY MAPPED
.equ	UCSR1D	= 0xCB ; MEMORY MAPPED
.equ	UCSR1C	= 0xCA ; MEMORY MAPPED
.equ	UCSR1B	= 0xC9 ; MEMORY MAPPED
.equ	UCSR1A	= 0xC8 ; MEMORY MAPPED
.equ	UDR0	= 0xC6 ; MEMORY MAPPED
.equ	UBRR0H	= 0xC5 ; MEMORY MAPPED
.equ	UBRR0L	= 0xC4 ; MEMORY MAPPED
.equ	UCSR0D	= 0xC3 ; MEMORY MAPPED
.equ	UCSR0C	= 0xC2 ; MEMORY MAPPED
.equ	UCSR0B	= 0xC1 ; MEMORY MAPPED
.equ	UCSR0A	= 0xC0 ; MEMORY MAPPED
.equ	TWAMR0	= 0xBD ; MEMORY MAPPED
.equ	TWCR0	= 0xBC ; MEMORY MAPPED
.equ	TWDR0	= 0xBB ; MEMORY MAPPED
.equ	TWAR0	= 0xBA ; MEMORY MAPPED
.equ	TWSR0	= 0xB9 ; MEMORY MAPPED
.equ	TWBR0	= 0xB8 ; MEMORY MAPPED
.equ	ASSR	= 0xB6 ; MEMORY MAPPED
.equ	OCR2B	= 0xB4 ; MEMORY MAPPED
.equ	OCR2A	= 0xB3 ; MEMORY MAPPED
.equ	TCNT2	= 0xB2 ; MEMORY MAPPED
.equ	TCCR2B	= 0xB1 ; MEMORY MAPPED
.equ	TCCR2A	= 0xB0 ; MEMORY MAPPED
.equ	SPDR1	= 0xAE ; MEMORY MAPPED
.equ	SPSR1	= 0xAD ; MEMORY MAPPED
.equ	SPCR1	= 0xAC ; MEMORY MAPPED
.equ	OCR4BH	= 0xAB ; MEMORY MAPPED
.equ	OCR4BL	= 0xAA ; MEMORY MAPPED
.equ	OCR4AH	= 0xA9 ; MEMORY MAPPED
.equ	OCR4AL	= 0xA8 ; MEMORY MAPPED
.equ	ICR4H	= 0xA7 ; MEMORY MAPPED
.equ	ICR4L	= 0xA6 ; MEMORY MAPPED
.equ	TCNT4H	= 0xA5 ; MEMORY MAPPED
.equ	TCNT4L	= 0xA4 ; MEMORY MAPPED
.equ	TCCR4C	= 0xA2 ; MEMORY MAPPED
.equ	TCCR4B	= 0xA1 ; MEMORY MAPPED
.equ	TCCR4A	= 0xA0 ; MEMORY MAPPED
.equ	OCR3BH	= 0x9B ; MEMORY MAPPED
.equ	OCR3BL	= 0x9A ; MEMORY MAPPED
.equ	OCR3AH	= 0x99 ; MEMORY MAPPED
.equ	OCR3AL	= 0x98 ; MEMORY MAPPED
.equ	ICR3H	= 0x97 ; MEMORY MAPPED
.equ	ICR3L	= 0x96 ; MEMORY MAPPED
.equ	TCNT3H	= 0x95 ; MEMORY MAPPED
.equ	TCNT3L	= 0x94 ; MEMORY MAPPED
.equ	TCCR3C	= 0x92 ; MEMORY MAPPED
.equ	TCCR3B	= 0x91 ; MEMORY MAPPED
.equ	TCCR3A	= 0x90 ; MEMORY MAPPED
.equ	OCR1BH	= 0x8B ; MEMORY MAPPED
.equ	OCR1BL	= 0x8A ; MEMORY MAPPED
.equ	OCR1AH	= 0x89 ; MEMORY MAPPED
.equ	OCR1AL	= 0x88 ; MEMORY MAPPED
.equ	ICR1H	= 0x87 ; MEMORY MAPPED
.equ	ICR1L	= 0x86 ; MEMORY MAPPED
.equ	TCNT1H	= 0x85 ; MEMORY MAPPED
.equ	TCNT1L	= 0x84 ; MEMORY MAPPED
.equ	TCCR1C	= 0x82 ; MEMORY MAPPED
.equ	TCCR1B	= 0x81 ; MEMORY MAPPED
.equ	TCCR1A	= 0x80 ; MEMORY MAPPED
.equ	DIDR1	= 0x7F ; MEMORY MAPPED
.equ	DIDR0	= 0x7E ; MEMORY MAPPED
.equ	ADMUX	= 0x7C ; MEMORY MAPPED
.equ	ADCSRB	= 0x7B ; MEMORY MAPPED
.equ	ADCSRA	= 0x7A ; MEMORY MAPPED
.equ	ADCH	= 0x79 ; MEMORY MAPPED
.equ	ADCL	= 0x78 ; MEMORY MAPPED
.equ	PCMSK3	= 0x73 ; MEMORY MAPPED
.equ	TIMSK4	= 0x72 ; MEMORY MAPPED
.equ	TIMSK3	= 0x71 ; MEMORY MAPPED
.equ	TIMSK2	= 0x70 ; MEMORY MAPPED
.equ	TIMSK1	= 0x6F ; MEMORY MAPPED
.equ	TIMSK0	= 0x6E ; MEMORY MAPPED
.equ	PCMSK2	= 0x6D ; MEMORY MAPPED
.equ	PCMSK1	= 0x6C ; MEMORY MAPPED
.equ	PCMSK0	= 0x6B ; MEMORY MAPPED
.equ	EICRA	= 0x69 ; MEMORY MAPPED
.equ	PCICR	= 0x68 ; MEMORY MAPPED
.equ	OSCCAL	= 0x66 ; MEMORY MAPPED
.equ	PRR1	= 0x65 ; MEMORY MAPPED
.equ	PRR0	= 0x64 ; MEMORY MAPPED
.equ	XFDCSR	= 0x62 ; MEMORY MAPPED
.equ	CLKPR	= 0x61 ; MEMORY MAPPED
.equ	WDTCSR	= 0x60 ; MEMORY MAPPED
.equ	SREG	= 0x3F ; 
.equ	SPH	= 0x3E ; 
.equ	SPL	= 0x3D ; 
.equ	SPMCSR	= 0x37 ; 
.equ	MCUCR	= 0x35 ; 
.equ	MCUSR	= 0x34 ; 
.equ	SMCR	= 0x33 ; 
.equ	ACSRA	= 0x30 ; 
.equ	ACSR	= 0x30 ; 
.equ	ACSRB	= 0x2F ; 
.equ	SPDR0	= 0x2E ; 
.equ	SPSR0	= 0x2D ; 
.equ	SPCR0	= 0x2C ; 
.equ	GPIOR2	= 0x2B ; 
.equ	GPIOR1	= 0x2A ; 
.equ	OCR0B	= 0x28 ; 
.equ	OCR0A	= 0x27 ; 
.equ	TCNT0	= 0x26 ; 
.equ	TCCR0B	= 0x25 ; 
.equ	TCCR0A	= 0x24 ; 
.equ	GTCCR	= 0x23 ; 
.equ	EEARH	= 0x22 ; 
.equ	EEARL	= 0x21 ; 
.equ	EEDR	= 0x20 ; 
.equ	EECR	= 0x1F ; 
.equ	GPIOR0	= 0x1E ; 
.equ	EIMSK	= 0x1D ; 
.equ	EIFR	= 0x1C ; 
.equ	PCIFR	= 0x1B ; 
.equ	TIFR4	= 0x19 ; 
.equ	TIFR3	= 0x18 ; 
.equ	TIFR2	= 0x17 ; 
.equ	TIFR1	= 0x16 ; 
.equ	TIFR0	= 0x15 ; 
.equ	PORTE	= 0x0E ; 
.equ	DDRE	= 0x0D ; 
.equ	PINE	= 0x0C ; 
.equ	PORTD	= 0x0B ; 
.equ	DDRD	= 0x0A ; 
.equ	PIND	= 0x09 ; 
.equ	PORTC	= 0x08 ; 
.equ	DDRC	= 0x07 ; 
.equ	PINC	= 0x06 ; 
.equ	PORTB	= 0x05 ; 
.equ	DDRB	= 0x04 ; 
.equ	PINB	= 0x03 ; 


; ***** BIT DEFINITIONS **************************************************

; ***** USART0 *****************
; UDR0 - USART I/O Data Register 0
.equ	UDR00	= 0 ; USART I/O Data Register 0 Bit 0
.equ	UDR01	= 1 ; USART I/O Data Register 0 Bit 1
.equ	UDR02	= 2 ; USART I/O Data Register 0 Bit 2
.equ	UDR03	= 3 ; USART I/O Data Register 0 Bit 3
.equ	UDR04	= 4 ; USART I/O Data Register 0 Bit 4
.equ	UDR05	= 5 ; USART I/O Data Register 0 Bit 5
.equ	UDR06	= 6 ; USART I/O Data Register 0 Bit 6
.equ	UDR07	= 7 ; USART I/O Data Register 0 Bit 7

; UCSR0A - USART Control and Status Register A
.equ	MPCM0	= 0 ; Multi-processor Communication Mode
.equ	U2X0	= 1 ; Double the USART transmission speed
.equ	UPE0	= 2 ; Parity Error
.equ	DOR0	= 3 ; Data overRun
.equ	FE0	= 4 ; Framing Error
.equ	UDRE0	= 5 ; USART Data Register Empty
.equ	TXC0	= 6 ; USART Transmitt Complete
.equ	RXC0	= 7 ; USART Receive Complete

; UCSR0B - USART Control and Status Register B
.equ	TXB80	= 0 ; Transmit Data Bit 8
.equ	RXB80	= 1 ; Receive Data Bit 8
.equ	UCSZ02	= 2 ; Character Size - together with UCSZ0 in UCSR0C
.equ	TXEN0	= 3 ; Transmitter Enable
.equ	RXEN0	= 4 ; Receiver Enable
.equ	UDRIE0	= 5 ; USART Data register Empty Interrupt Enable
.equ	TXCIE0	= 6 ; TX Complete Interrupt Enable
.equ	RXCIE0	= 7 ; RX Complete Interrupt Enable

; UCSR0C - USART Control and Status Register C
.equ	UCPOL0	= 0 ; Clock Polarity
.equ	UCSZ00	= 1 ; Character Size - together with UCSZ2 in UCSR0B
.equ	UCSZ01	= 2 ; Character Size - together with UCSZ2 in UCSR0B
.equ	USBS0	= 3 ; Stop Bit Select
.equ	UPM00	= 4 ; Parity Mode Bits
.equ	UPM01	= 5 ; Parity Mode Bits
.equ	UMSEL00	= 6 ; USART Mode Select
.equ	UMSEL01	= 7 ; USART Mode Select

; UCSR0D - USART Control and Status Register D
.equ	SFDE	= 5 ; Start frame detection enable
.equ	RXS	= 6 ; USART RX Start
.equ	RXSIE	= 7 ; USART RX Start Interrupt Enable

; UBRR0 - USART Baud Rate Register Bytes
.equ	UBRR0H0	= 0 ; USART Baud Rate Register Bytes High Bit 8
.equ	UBRR0H1	= 1 ; USART Baud Rate Register Bytes High Bit 9
.equ	UBRR0H2	= 2 ; USART Baud Rate Register Bytes High Bit 10
.equ	UBRR0H3	= 3 ; USART Baud Rate Register Bytes High Bit 11

.equ	UBRR0L0	= 0 ; USART Baud Rate Register Bytes Low Bit 0
.equ	UBRR0L1	= 1 ; USART Baud Rate Register Bytes Low Bit 1
.equ	UBRR0L2	= 2 ; USART Baud Rate Register Bytes Low Bit 2
.equ	UBRR0L3	= 3 ; USART Baud Rate Register Bytes Low Bit 3
.equ	UBRR0L4	= 4 ; USART Baud Rate Register Bytes Low Bit 4
.equ	UBRR0L5	= 5 ; USART Baud Rate Register Bytes Low Bit 5
.equ	UBRR0L6	= 6 ; USART Baud Rate Register Bytes Low Bit 6
.equ	UBRR0L7	= 7 ; USART Baud Rate Register Bytes Low Bit 7


; ***** USART1 *****************
; UDR1 - USART I/O Data Register
.equ	UDR10	= 0 ; USART I/O Data Register Bit 0
.equ	UDR11	= 1 ; USART I/O Data Register Bit 1
.equ	UDR12	= 2 ; USART I/O Data Register Bit 2
.equ	UDR13	= 3 ; USART I/O Data Register Bit 3
.equ	UDR14	= 4 ; USART I/O Data Register Bit 4
.equ	UDR15	= 5 ; USART I/O Data Register Bit 5
.equ	UDR16	= 6 ; USART I/O Data Register Bit 6
.equ	UDR17	= 7 ; USART I/O Data Register Bit 7

; UCSR1A - USART Control and Status Register A
.equ	MPCM1	= 0 ; Multi-processor Communication Mode
.equ	U2X1	= 1 ; Double the USART transmission speed
.equ	UPE1	= 2 ; Parity Error
.equ	DOR1	= 3 ; Data overRun
.equ	FE1	= 4 ; Framing Error
.equ	UDRE1	= 5 ; USART Data Register Empty
.equ	TXC1	= 6 ; USART Transmitt Complete
.equ	RXC1	= 7 ; USART Receive Complete

; UCSR1B - USART Control and Status Register B
.equ	TXB81	= 0 ; Transmit Data Bit 8
.equ	RXB81	= 1 ; Receive Data Bit 8
.equ	UCSZ12	= 2 ; Character Size - together with UCSZ0 in UCSR1C
.equ	TXEN1	= 3 ; Transmitter Enable
.equ	RXEN1	= 4 ; Receiver Enable
.equ	UDRIE1	= 5 ; USART Data register Empty Interrupt Enable
.equ	TXCIE1	= 6 ; TX Complete Interrupt Enable
.equ	RXCIE1	= 7 ; RX Complete Interrupt Enable

; UCSR1C - USART Control and Status Register C
.equ	UCPOL1	= 0 ; Clock Polarity
.equ	UCSZ10	= 1 ; Character Size - together with UCSZ12 in UCSR1B
.equ	UCSZ11	= 2 ; Character Size - together with UCSZ12 in UCSR1B
.equ	USBS1	= 3 ; Stop Bit Select
.equ	UPM10	= 4 ; Parity Mode Bits
.equ	UPM11	= 5 ; Parity Mode Bits
.equ	UMSEL10	= 6 ; USART Mode Select
.equ	UMSEL11	= 7 ; USART Mode Select

; UCSR1D - USART Control and Status Register D
.equ	SFDE1	= 5 ; Start frame detection enable
.equ	RXS1	= 6 ; USART RX Start
.equ	RXSIE1	= 7 ; USART RX Start Interrupt Enable

; UBRR1 - USART Baud Rate Register Bytes
.equ	UBRR1H0	= 0 ; USART Baud Rate Register Bytes High Bit 8
.equ	UBRR1H1	= 1 ; USART Baud Rate Register Bytes High Bit 9
.equ	UBRR1H2	= 2 ; USART Baud Rate Register Bytes High Bit 10
.equ	UBRR1H3	= 3 ; USART Baud Rate Register Bytes High Bit 11

.equ	UBRR1L0	= 0 ; USART Baud Rate Register Bytes Low Bit 0
.equ	UBRR1L1	= 1 ; USART Baud Rate Register Bytes Low Bit 1
.equ	UBRR1L2	= 2 ; USART Baud Rate Register Bytes Low Bit 2
.equ	UBRR1L3	= 3 ; USART Baud Rate Register Bytes Low Bit 3
.equ	UBRR1L4	= 4 ; USART Baud Rate Register Bytes Low Bit 4
.equ	UBRR1L5	= 5 ; USART Baud Rate Register Bytes Low Bit 5
.equ	UBRR1L6	= 6 ; USART Baud Rate Register Bytes Low Bit 6
.equ	UBRR1L7	= 7 ; USART Baud Rate Register Bytes Low Bit 7


; ***** TWI0 *****************
; TWAMR0 - TWI (Slave) Address Mask Register
.equ	TWAM0	= 1 ; 
.equ	TWAM1	= 2 ; 
.equ	TWAM2	= 3 ; 
.equ	TWAM3	= 4 ; 
.equ	TWAM4	= 5 ; 
.equ	TWAM5	= 6 ; 
.equ	TWAM6	= 7 ; 

; TWBR0 - TWI Bit Rate register
.equ	TWBR00	= 0 ; TWI Bit Rate register Bit 0
.equ	TWBR01	= 1 ; TWI Bit Rate register Bit 1
.equ	TWBR02	= 2 ; TWI Bit Rate register Bit 2
.equ	TWBR03	= 3 ; TWI Bit Rate register Bit 3
.equ	TWBR04	= 4 ; TWI Bit Rate register Bit 4
.equ	TWBR05	= 5 ; TWI Bit Rate register Bit 5
.equ	TWBR06	= 6 ; TWI Bit Rate register Bit 6
.equ	TWBR07	= 7 ; TWI Bit Rate register Bit 7

; TWCR0 - TWI Control Register
.equ	TWIE	= 0 ; TWI Interrupt Enable
.equ	TWEN	= 2 ; TWI Enable Bit
.equ	TWWC	= 3 ; TWI Write Collition Flag
.equ	TWSTO	= 4 ; TWI Stop Condition Bit
.equ	TWSTA	= 5 ; TWI Start Condition Bit
.equ	TWEA	= 6 ; TWI Enable Acknowledge Bit
.equ	TWINT	= 7 ; TWI Interrupt Flag

; TWSR0 - TWI Status Register
.equ	TWPS0	= 0 ; TWI Prescaler
.equ	TWPS1	= 1 ; TWI Prescaler
.equ	TWS3	= 3 ; TWI Status
.equ	TWS4	= 4 ; TWI Status
.equ	TWS5	= 5 ; TWI Status
.equ	TWS6	= 6 ; TWI Status
.equ	TWS7	= 7 ; TWI Status

; TWDR0 - TWI Data register
.equ	TWDR00	= 0 ; TWI Data register Bit 0
.equ	TWDR01	= 1 ; TWI Data register Bit 1
.equ	TWDR02	= 2 ; TWI Data register Bit 2
.equ	TWDR03	= 3 ; TWI Data register Bit 3
.equ	TWDR04	= 4 ; TWI Data register Bit 4
.equ	TWDR05	= 5 ; TWI Data register Bit 5
.equ	TWDR06	= 6 ; TWI Data register Bit 6
.equ	TWDR07	= 7 ; TWI Data register Bit 7

; TWAR0 - TWI (Slave) Address register
.equ	TWGCE	= 0 ; TWI General Call Recognition Enable Bit
.equ	TWA0	= 1 ; TWI (Slave) Address register Bits
.equ	TWA1	= 2 ; TWI (Slave) Address register Bits
.equ	TWA2	= 3 ; TWI (Slave) Address register Bits
.equ	TWA3	= 4 ; TWI (Slave) Address register Bits
.equ	TWA4	= 5 ; TWI (Slave) Address register Bits
.equ	TWA5	= 6 ; TWI (Slave) Address register Bits
.equ	TWA6	= 7 ; TWI (Slave) Address register Bits


; ***** TWI1 *****************
; TWAMR1 - TWI (Slave) Address Mask Register
.equ	TWAM10	= 1 ; 
.equ	TWAM11	= 2 ; 
.equ	TWAM12	= 3 ; 
.equ	TWAM13	= 4 ; 
.equ	TWAM14	= 5 ; 
.equ	TWAM15	= 6 ; 
.equ	TWAM16	= 7 ; 

; TWBR1 - TWI Bit Rate register
.equ	TWBR10	= 0 ; TWI Bit Rate register Bit 0
.equ	TWBR11	= 1 ; TWI Bit Rate register Bit 1
.equ	TWBR12	= 2 ; TWI Bit Rate register Bit 2
.equ	TWBR13	= 3 ; TWI Bit Rate register Bit 3
.equ	TWBR14	= 4 ; TWI Bit Rate register Bit 4
.equ	TWBR15	= 5 ; TWI Bit Rate register Bit 5
.equ	TWBR16	= 6 ; TWI Bit Rate register Bit 6
.equ	TWBR17	= 7 ; TWI Bit Rate register Bit 7

; TWCR1 - TWI Control Register
.equ	TWIE1	= 0 ; TWI Interrupt Enable
.equ	TWEN1	= 2 ; TWI Enable Bit
.equ	TWWC1	= 3 ; TWI Write Collition Flag
.equ	TWSTO1	= 4 ; TWI Stop Condition Bit
.equ	TWSTA1	= 5 ; TWI Start Condition Bit
.equ	TWEA1	= 6 ; TWI Enable Acknowledge Bit
.equ	TWINT1	= 7 ; TWI Interrupt Flag

; TWSR1 - TWI Status Register
.equ	TWPS10	= 0 ; TWI Prescaler
.equ	TWPS11	= 1 ; TWI Prescaler
.equ	TWS13	= 3 ; TWI Status
.equ	TWS14	= 4 ; TWI Status
.equ	TWS15	= 5 ; TWI Status
.equ	TWS16	= 6 ; TWI Status
.equ	TWS17	= 7 ; TWI Status

; TWDR1 - TWI Data register
.equ	TWDR10	= 0 ; TWI Data register Bit 0
.equ	TWDR11	= 1 ; TWI Data register Bit 1
.equ	TWDR12	= 2 ; TWI Data register Bit 2
.equ	TWDR13	= 3 ; TWI Data register Bit 3
.equ	TWDR14	= 4 ; TWI Data register Bit 4
.equ	TWDR15	= 5 ; TWI Data register Bit 5
.equ	TWDR16	= 6 ; TWI Data register Bit 6
.equ	TWDR17	= 7 ; TWI Data register Bit 7

; TWAR1 - TWI (Slave) Address register
.equ	TWAR10	= 0 ; TWI (Slave) Address register Bit 0
.equ	TWAR11	= 1 ; TWI (Slave) Address register Bit 1
.equ	TWAR12	= 2 ; TWI (Slave) Address register Bit 2
.equ	TWAR13	= 3 ; TWI (Slave) Address register Bit 3
.equ	TWAR14	= 4 ; TWI (Slave) Address register Bit 4
.equ	TWAR15	= 5 ; TWI (Slave) Address register Bit 5
.equ	TWAR16	= 6 ; TWI (Slave) Address register Bit 6
.equ	TWAR17	= 7 ; TWI (Slave) Address register Bit 7


; ***** TC1 *****************
; TIMSK1 - Timer/Counter Interrupt Mask Register
.equ	TOIE1	= 0 ; Timer/Counter1 Overflow Interrupt Enable
.equ	OCIE1A	= 1 ; Timer/Counter1 Output CompareA Match Interrupt Enable
.equ	OCIE1B	= 2 ; Timer/Counter1 Output CompareB Match Interrupt Enable
.equ	ICIE1	= 5 ; Timer/Counter1 Input Capture Interrupt Enable

; TIFR1 - Timer/Counter Interrupt Flag register
.equ	TOV1	= 0 ; Timer/Counter1 Overflow Flag
.equ	OCF1A	= 1 ; Output Compare Flag 1A
.equ	OCF1B	= 2 ; Output Compare Flag 1B
.equ	ICF1	= 5 ; Input Capture Flag 1

; TCCR1A - Timer/Counter1 Control Register A
.equ	WGM10	= 0 ; Waveform Generation Mode
.equ	WGM11	= 1 ; Waveform Generation Mode
.equ	COM1B0	= 4 ; Compare Output Mode 1B, bits
.equ	COM1B1	= 5 ; Compare Output Mode 1B, bits
.equ	COM1A0	= 6 ; Compare Output Mode 1A, bits
.equ	COM1A1	= 7 ; Compare Output Mode 1A, bits

; TCCR1B - Timer/Counter1 Control Register B
.equ	CS10	= 0 ; Prescaler source of Timer/Counter 1
.equ	CS11	= 1 ; Prescaler source of Timer/Counter 1
.equ	CS12	= 2 ; Prescaler source of Timer/Counter 1
.equ	WGM12	= 3 ; Waveform Generation Mode
.equ	WGM13	= 4 ; Waveform Generation Mode
.equ	ICES1	= 6 ; Input Capture 1 Edge Select
.equ	ICNC1	= 7 ; Input Capture 1 Noise Canceler

; TCCR1C - Timer/Counter1 Control Register C
.equ	FOC1B	= 6 ; 
.equ	FOC1A	= 7 ; 

; TCNT1 - Timer/Counter1 Bytes
.equ	TCNT1H0	= 0 ; Timer/Counter1 Bytes High Bit 8
.equ	TCNT1H1	= 1 ; Timer/Counter1 Bytes High Bit 9
.equ	TCNT1H2	= 2 ; Timer/Counter1 Bytes High Bit 10
.equ	TCNT1H3	= 3 ; Timer/Counter1 Bytes High Bit 11
.equ	TCNT1H4	= 4 ; Timer/Counter1 Bytes High Bit 12
.equ	TCNT1H5	= 5 ; Timer/Counter1 Bytes High Bit 13
.equ	TCNT1H6	= 6 ; Timer/Counter1 Bytes High Bit 14
.equ	TCNT1H7	= 7 ; Timer/Counter1 Bytes High Bit 15

.equ	TCNT1L0	= 0 ; Timer/Counter1 Bytes Low Bit 0
.equ	TCNT1L1	= 1 ; Timer/Counter1 Bytes Low Bit 1
.equ	TCNT1L2	= 2 ; Timer/Counter1 Bytes Low Bit 2
.equ	TCNT1L3	= 3 ; Timer/Counter1 Bytes Low Bit 3
.equ	TCNT1L4	= 4 ; Timer/Counter1 Bytes Low Bit 4
.equ	TCNT1L5	= 5 ; Timer/Counter1 Bytes Low Bit 5
.equ	TCNT1L6	= 6 ; Timer/Counter1 Bytes Low Bit 6
.equ	TCNT1L7	= 7 ; Timer/Counter1 Bytes Low Bit 7

; OCR1A - Timer/Counter1 Output Compare Register Bytes
.equ	OCR1AH0	= 0 ; Timer/Counter1 Output Compare Register Bytes High Bit 8
.equ	OCR1AH1	= 1 ; Timer/Counter1 Output Compare Register Bytes High Bit 9
.equ	OCR1AH2	= 2 ; Timer/Counter1 Output Compare Register Bytes High Bit 10
.equ	OCR1AH3	= 3 ; Timer/Counter1 Output Compare Register Bytes High Bit 11
.equ	OCR1AH4	= 4 ; Timer/Counter1 Output Compare Register Bytes High Bit 12
.equ	OCR1AH5	= 5 ; Timer/Counter1 Output Compare Register Bytes High Bit 13
.equ	OCR1AH6	= 6 ; Timer/Counter1 Output Compare Register Bytes High Bit 14
.equ	OCR1AH7	= 7 ; Timer/Counter1 Output Compare Register Bytes High Bit 15

.equ	OCR1AL0	= 0 ; Timer/Counter1 Output Compare Register Bytes Low Bit 0
.equ	OCR1AL1	= 1 ; Timer/Counter1 Output Compare Register Bytes Low Bit 1
.equ	OCR1AL2	= 2 ; Timer/Counter1 Output Compare Register Bytes Low Bit 2
.equ	OCR1AL3	= 3 ; Timer/Counter1 Output Compare Register Bytes Low Bit 3
.equ	OCR1AL4	= 4 ; Timer/Counter1 Output Compare Register Bytes Low Bit 4
.equ	OCR1AL5	= 5 ; Timer/Counter1 Output Compare Register Bytes Low Bit 5
.equ	OCR1AL6	= 6 ; Timer/Counter1 Output Compare Register Bytes Low Bit 6
.equ	OCR1AL7	= 7 ; Timer/Counter1 Output Compare Register Bytes Low Bit 7

; OCR1B - Timer/Counter1 Output Compare Register Bytes
.equ	OCR1BH0	= 0 ; Timer/Counter1 Output Compare Register Bytes High Bit 8
.equ	OCR1BH1	= 1 ; Timer/Counter1 Output Compare Register Bytes High Bit 9
.equ	OCR1BH2	= 2 ; Timer/Counter1 Output Compare Register Bytes High Bit 10
.equ	OCR1BH3	= 3 ; Timer/Counter1 Output Compare Register Bytes High Bit 11
.equ	OCR1BH4	= 4 ; Timer/Counter1 Output Compare Register Bytes High Bit 12
.equ	OCR1BH5	= 5 ; Timer/Counter1 Output Compare Register Bytes High Bit 13
.equ	OCR1BH6	= 6 ; Timer/Counter1 Output Compare Register Bytes High Bit 14
.equ	OCR1BH7	= 7 ; Timer/Counter1 Output Compare Register Bytes High Bit 15

.equ	OCR1BL0	= 0 ; Timer/Counter1 Output Compare Register Bytes Low Bit 0
.equ	OCR1BL1	= 1 ; Timer/Counter1 Output Compare Register Bytes Low Bit 1
.equ	OCR1BL2	= 2 ; Timer/Counter1 Output Compare Register Bytes Low Bit 2
.equ	OCR1BL3	= 3 ; Timer/Counter1 Output Compare Register Bytes Low Bit 3
.equ	OCR1BL4	= 4 ; Timer/Counter1 Output Compare Register Bytes Low Bit 4
.equ	OCR1BL5	= 5 ; Timer/Counter1 Output Compare Register Bytes Low Bit 5
.equ	OCR1BL6	= 6 ; Timer/Counter1 Output Compare Register Bytes Low Bit 6
.equ	OCR1BL7	= 7 ; Timer/Counter1 Output Compare Register Bytes Low Bit 7

; ICR1 - Timer/Counter1 Input Capture Register Bytes
.equ	ICR1H0	= 0 ; Timer/Counter1 Input Capture Register Bytes High Bit 8
.equ	ICR1H1	= 1 ; Timer/Counter1 Input Capture Register Bytes High Bit 9
.equ	ICR1H2	= 2 ; Timer/Counter1 Input Capture Register Bytes High Bit 10
.equ	ICR1H3	= 3 ; Timer/Counter1 Input Capture Register Bytes High Bit 11
.equ	ICR1H4	= 4 ; Timer/Counter1 Input Capture Register Bytes High Bit 12
.equ	ICR1H5	= 5 ; Timer/Counter1 Input Capture Register Bytes High Bit 13
.equ	ICR1H6	= 6 ; Timer/Counter1 Input Capture Register Bytes High Bit 14
.equ	ICR1H7	= 7 ; Timer/Counter1 Input Capture Register Bytes High Bit 15

.equ	ICR1L0	= 0 ; Timer/Counter1 Input Capture Register Bytes Low Bit 0
.equ	ICR1L1	= 1 ; Timer/Counter1 Input Capture Register Bytes Low Bit 1
.equ	ICR1L2	= 2 ; Timer/Counter1 Input Capture Register Bytes Low Bit 2
.equ	ICR1L3	= 3 ; Timer/Counter1 Input Capture Register Bytes Low Bit 3
.equ	ICR1L4	= 4 ; Timer/Counter1 Input Capture Register Bytes Low Bit 4
.equ	ICR1L5	= 5 ; Timer/Counter1 Input Capture Register Bytes Low Bit 5
.equ	ICR1L6	= 6 ; Timer/Counter1 Input Capture Register Bytes Low Bit 6
.equ	ICR1L7	= 7 ; Timer/Counter1 Input Capture Register Bytes Low Bit 7

; GTCCR - General Timer/Counter Control Register
.equ	PSRSYNC	= 0 ; Prescaler Reset Timer/Counter1 and Timer/Counter0
.equ	TSM	= 7 ; Timer/Counter Synchronization Mode


; ***** TC3 *****************
; TIMSK3 - Timer/Counter Interrupt Mask Register
.equ	TOIE3	= 0 ; Timer/Counter3 Overflow Interrupt Enable
.equ	OCIE3A	= 1 ; Timer/Counter3 Output Compare Match A Interrupt Enable
.equ	OCIE3B	= 2 ; Timer/Counter3 Output Compare Match B Interrupt Enable
.equ	ICIE3	= 5 ; Timer/Counter3 Input Capture Interrupt Enable

; TIFR3 - Timer/Counter3 Interrupt Flag register
.equ	TOV3	= 0 ; Timer/Counter3 Overflow Flag
.equ	OCF3A	= 1 ; Output Compare Flag 3A
.equ	OCF3B	= 2 ; Output Compare Flag 3B
.equ	ICF3	= 5 ; Timer/Counter3 Input Capture Flag

; TCCR3A - Timer/Counter3 Control Register A
.equ	WGM30	= 0 ; Waveform Genration Mode
.equ	WGM31	= 1 ; Waveform Genration Mode
.equ	COM3B0	= 4 ; Compare Output Mode bits
.equ	COM3B1	= 5 ; Compare Output Mode bits
.equ	COM3A0	= 6 ; Compare Output Mode bits
.equ	COM3A1	= 7 ; Compare Output Mode bits

; TCCR3B - Timer/Counter3 Control Register B
.equ	CS30	= 0 ; Clock Select bits
.equ	CS31	= 1 ; Clock Select bits
.equ	CS32	= 2 ; Clock Select bits
.equ	WGM32	= 3 ; Waveform Generation Mode bit 2
.equ	WGM33	= 4 ; Waveform Generation Mode bit 3
.equ	ICES3	= 6 ; Input Capture Edge Select
.equ	ICNC3	= 7 ; Input Capture Noise Canceler

; TCCR3C - Timer/Counter3 Control Register C
.equ	FOC3B	= 6 ; Force Output Compare for Channel B
.equ	FOC3A	= 7 ; Force Output Compare for Channel A

; TCNT3 - Timer/Counter3 Bytes
.equ	TCNT3H0	= 0 ; Timer/Counter3 Bytes High Bit 8
.equ	TCNT3H1	= 1 ; Timer/Counter3 Bytes High Bit 9
.equ	TCNT3H2	= 2 ; Timer/Counter3 Bytes High Bit 10
.equ	TCNT3H3	= 3 ; Timer/Counter3 Bytes High Bit 11
.equ	TCNT3H4	= 4 ; Timer/Counter3 Bytes High Bit 12
.equ	TCNT3H5	= 5 ; Timer/Counter3 Bytes High Bit 13
.equ	TCNT3H6	= 6 ; Timer/Counter3 Bytes High Bit 14
.equ	TCNT3H7	= 7 ; Timer/Counter3 Bytes High Bit 15

.equ	TCNT3L0	= 0 ; Timer/Counter3 Bytes Low Bit 0
.equ	TCNT3L1	= 1 ; Timer/Counter3 Bytes Low Bit 1
.equ	TCNT3L2	= 2 ; Timer/Counter3 Bytes Low Bit 2
.equ	TCNT3L3	= 3 ; Timer/Counter3 Bytes Low Bit 3
.equ	TCNT3L4	= 4 ; Timer/Counter3 Bytes Low Bit 4
.equ	TCNT3L5	= 5 ; Timer/Counter3 Bytes Low Bit 5
.equ	TCNT3L6	= 6 ; Timer/Counter3 Bytes Low Bit 6
.equ	TCNT3L7	= 7 ; Timer/Counter3 Bytes Low Bit 7

; OCR3A - Timer/Counter3 Output Compare Register Bytes
.equ	OCR3AH0	= 0 ; Timer/Counter3 Output Compare Register Bytes High Bit 8
.equ	OCR3AH1	= 1 ; Timer/Counter3 Output Compare Register Bytes High Bit 9
.equ	OCR3AH2	= 2 ; Timer/Counter3 Output Compare Register Bytes High Bit 10
.equ	OCR3AH3	= 3 ; Timer/Counter3 Output Compare Register Bytes High Bit 11
.equ	OCR3AH4	= 4 ; Timer/Counter3 Output Compare Register Bytes High Bit 12
.equ	OCR3AH5	= 5 ; Timer/Counter3 Output Compare Register Bytes High Bit 13
.equ	OCR3AH6	= 6 ; Timer/Counter3 Output Compare Register Bytes High Bit 14
.equ	OCR3AH7	= 7 ; Timer/Counter3 Output Compare Register Bytes High Bit 15

.equ	OCR3AL0	= 0 ; Timer/Counter3 Output Compare Register Bytes Low Bit 0
.equ	OCR3AL1	= 1 ; Timer/Counter3 Output Compare Register Bytes Low Bit 1
.equ	OCR3AL2	= 2 ; Timer/Counter3 Output Compare Register Bytes Low Bit 2
.equ	OCR3AL3	= 3 ; Timer/Counter3 Output Compare Register Bytes Low Bit 3
.equ	OCR3AL4	= 4 ; Timer/Counter3 Output Compare Register Bytes Low Bit 4
.equ	OCR3AL5	= 5 ; Timer/Counter3 Output Compare Register Bytes Low Bit 5
.equ	OCR3AL6	= 6 ; Timer/Counter3 Output Compare Register Bytes Low Bit 6
.equ	OCR3AL7	= 7 ; Timer/Counter3 Output Compare Register Bytes Low Bit 7

; OCR3B - Timer/Counter3 Output Compare Register Bytes
.equ	OCR3BH0	= 0 ; Timer/Counter3 Output Compare Register Bytes High Bit 8
.equ	OCR3BH1	= 1 ; Timer/Counter3 Output Compare Register Bytes High Bit 9
.equ	OCR3BH2	= 2 ; Timer/Counter3 Output Compare Register Bytes High Bit 10
.equ	OCR3BH3	= 3 ; Timer/Counter3 Output Compare Register Bytes High Bit 11
.equ	OCR3BH4	= 4 ; Timer/Counter3 Output Compare Register Bytes High Bit 12
.equ	OCR3BH5	= 5 ; Timer/Counter3 Output Compare Register Bytes High Bit 13
.equ	OCR3BH6	= 6 ; Timer/Counter3 Output Compare Register Bytes High Bit 14
.equ	OCR3BH7	= 7 ; Timer/Counter3 Output Compare Register Bytes High Bit 15

.equ	OCR3BL0	= 0 ; Timer/Counter3 Output Compare Register Bytes Low Bit 0
.equ	OCR3BL1	= 1 ; Timer/Counter3 Output Compare Register Bytes Low Bit 1
.equ	OCR3BL2	= 2 ; Timer/Counter3 Output Compare Register Bytes Low Bit 2
.equ	OCR3BL3	= 3 ; Timer/Counter3 Output Compare Register Bytes Low Bit 3
.equ	OCR3BL4	= 4 ; Timer/Counter3 Output Compare Register Bytes Low Bit 4
.equ	OCR3BL5	= 5 ; Timer/Counter3 Output Compare Register Bytes Low Bit 5
.equ	OCR3BL6	= 6 ; Timer/Counter3 Output Compare Register Bytes Low Bit 6
.equ	OCR3BL7	= 7 ; Timer/Counter3 Output Compare Register Bytes Low Bit 7

; ICR3 - Timer/Counter3 Input Capture Register Bytes
.equ	ICR3H0	= 0 ; Timer/Counter3 Input Capture Register Bytes High Bit 8
.equ	ICR3H1	= 1 ; Timer/Counter3 Input Capture Register Bytes High Bit 9
.equ	ICR3H2	= 2 ; Timer/Counter3 Input Capture Register Bytes High Bit 10
.equ	ICR3H3	= 3 ; Timer/Counter3 Input Capture Register Bytes High Bit 11
.equ	ICR3H4	= 4 ; Timer/Counter3 Input Capture Register Bytes High Bit 12
.equ	ICR3H5	= 5 ; Timer/Counter3 Input Capture Register Bytes High Bit 13
.equ	ICR3H6	= 6 ; Timer/Counter3 Input Capture Register Bytes High Bit 14
.equ	ICR3H7	= 7 ; Timer/Counter3 Input Capture Register Bytes High Bit 15

.equ	ICR3L0	= 0 ; Timer/Counter3 Input Capture Register Bytes Low Bit 0
.equ	ICR3L1	= 1 ; Timer/Counter3 Input Capture Register Bytes Low Bit 1
.equ	ICR3L2	= 2 ; Timer/Counter3 Input Capture Register Bytes Low Bit 2
.equ	ICR3L3	= 3 ; Timer/Counter3 Input Capture Register Bytes Low Bit 3
.equ	ICR3L4	= 4 ; Timer/Counter3 Input Capture Register Bytes Low Bit 4
.equ	ICR3L5	= 5 ; Timer/Counter3 Input Capture Register Bytes Low Bit 5
.equ	ICR3L6	= 6 ; Timer/Counter3 Input Capture Register Bytes Low Bit 6
.equ	ICR3L7	= 7 ; Timer/Counter3 Input Capture Register Bytes Low Bit 7


; ***** TC4 *****************
; TIMSK4 - Timer/Counter4 Interrupt Mask Register
.equ	TOIE4	= 0 ; Timer/Counter4 Overflow Interrupt Enable
.equ	OCIE4A	= 1 ; Timer/Counter4 Output Compare Match A Interrupt Enable
.equ	OCIE4B	= 2 ; Timer/Counter4 Output Compare Match B Interrupt Enable
.equ	ICIE4	= 5 ; Timer/Counter4 Input Capture Interrupt Enable

; TIFR4 - Timer/Counter4 Interrupt Flag register
.equ	TOV4	= 0 ; Timer/Counter4 Overflow Flag
.equ	OCF4A	= 1 ; Output Compare Flag 4A
.equ	OCF4B	= 2 ; Output Compare Flag 4B
.equ	ICF4	= 5 ; Timer/Counter4 Input Capture Flag

; TCCR4A - Timer/Counter4 Control Register A
.equ	WGM40	= 0 ; Waveform Generation Mode
.equ	WGM41	= 1 ; Waveform Generation Mode
.equ	COM4B0	= 4 ; Compare Output Mode bits
.equ	COM4B1	= 5 ; Compare Output Mode bits
.equ	COM4A0	= 6 ; Compare Output Mode bits
.equ	COM4A1	= 7 ; Compare Output Mode bits

; TCCR4B - Timer/Counter4 Control Register B
.equ	CS40	= 0 ; Clock Select bits
.equ	CS41	= 1 ; Clock Select bits
.equ	CS42	= 2 ; Clock Select bits
.equ	WGM42	= 3 ; Waveform Generation Mode bit 2
.equ	WGM43	= 4 ; Waveform Generation Mode bit 3
.equ	ICES4	= 6 ; Input Capture Edge Select
.equ	ICNC4	= 7 ; Input Capture Noise Canceler

; TCCR4C - Timer/Counter4 Control Register C
.equ	FOC4B	= 6 ; Force Output Compare for Channel B
.equ	FOC4A	= 7 ; Force Output Compare for Channel A

; TCNT4 - Timer/Counter4 Bytes
.equ	TCNT4H0	= 0 ; Timer/Counter4 Bytes High Bit 8
.equ	TCNT4H1	= 1 ; Timer/Counter4 Bytes High Bit 9
.equ	TCNT4H2	= 2 ; Timer/Counter4 Bytes High Bit 10
.equ	TCNT4H3	= 3 ; Timer/Counter4 Bytes High Bit 11
.equ	TCNT4H4	= 4 ; Timer/Counter4 Bytes High Bit 12
.equ	TCNT4H5	= 5 ; Timer/Counter4 Bytes High Bit 13
.equ	TCNT4H6	= 6 ; Timer/Counter4 Bytes High Bit 14
.equ	TCNT4H7	= 7 ; Timer/Counter4 Bytes High Bit 15

.equ	TCNT4L0	= 0 ; Timer/Counter4 Bytes Low Bit 0
.equ	TCNT4L1	= 1 ; Timer/Counter4 Bytes Low Bit 1
.equ	TCNT4L2	= 2 ; Timer/Counter4 Bytes Low Bit 2
.equ	TCNT4L3	= 3 ; Timer/Counter4 Bytes Low Bit 3
.equ	TCNT4L4	= 4 ; Timer/Counter4 Bytes Low Bit 4
.equ	TCNT4L5	= 5 ; Timer/Counter4 Bytes Low Bit 5
.equ	TCNT4L6	= 6 ; Timer/Counter4 Bytes Low Bit 6
.equ	TCNT4L7	= 7 ; Timer/Counter4 Bytes Low Bit 7

; OCR4A - Timer/Counter4 Output Compare Register Bytes
.equ	OCR4AH0	= 0 ; Timer/Counter4 Output Compare Register Bytes High Bit 8
.equ	OCR4AH1	= 1 ; Timer/Counter4 Output Compare Register Bytes High Bit 9
.equ	OCR4AH2	= 2 ; Timer/Counter4 Output Compare Register Bytes High Bit 10
.equ	OCR4AH3	= 3 ; Timer/Counter4 Output Compare Register Bytes High Bit 11
.equ	OCR4AH4	= 4 ; Timer/Counter4 Output Compare Register Bytes High Bit 12
.equ	OCR4AH5	= 5 ; Timer/Counter4 Output Compare Register Bytes High Bit 13
.equ	OCR4AH6	= 6 ; Timer/Counter4 Output Compare Register Bytes High Bit 14
.equ	OCR4AH7	= 7 ; Timer/Counter4 Output Compare Register Bytes High Bit 15

.equ	OCR4AL0	= 0 ; Timer/Counter4 Output Compare Register Bytes Low Bit 0
.equ	OCR4AL1	= 1 ; Timer/Counter4 Output Compare Register Bytes Low Bit 1
.equ	OCR4AL2	= 2 ; Timer/Counter4 Output Compare Register Bytes Low Bit 2
.equ	OCR4AL3	= 3 ; Timer/Counter4 Output Compare Register Bytes Low Bit 3
.equ	OCR4AL4	= 4 ; Timer/Counter4 Output Compare Register Bytes Low Bit 4
.equ	OCR4AL5	= 5 ; Timer/Counter4 Output Compare Register Bytes Low Bit 5
.equ	OCR4AL6	= 6 ; Timer/Counter4 Output Compare Register Bytes Low Bit 6
.equ	OCR4AL7	= 7 ; Timer/Counter4 Output Compare Register Bytes Low Bit 7

; OCR4B - Timer/Counter4 Output Compare Register Bytes
.equ	OCR4BH0	= 0 ; Timer/Counter4 Output Compare Register Bytes High Bit 8
.equ	OCR4BH1	= 1 ; Timer/Counter4 Output Compare Register Bytes High Bit 9
.equ	OCR4BH2	= 2 ; Timer/Counter4 Output Compare Register Bytes High Bit 10
.equ	OCR4BH3	= 3 ; Timer/Counter4 Output Compare Register Bytes High Bit 11
.equ	OCR4BH4	= 4 ; Timer/Counter4 Output Compare Register Bytes High Bit 12
.equ	OCR4BH5	= 5 ; Timer/Counter4 Output Compare Register Bytes High Bit 13
.equ	OCR4BH6	= 6 ; Timer/Counter4 Output Compare Register Bytes High Bit 14
.equ	OCR4BH7	= 7 ; Timer/Counter4 Output Compare Register Bytes High Bit 15

.equ	OCR4BL0	= 0 ; Timer/Counter4 Output Compare Register Bytes Low Bit 0
.equ	OCR4BL1	= 1 ; Timer/Counter4 Output Compare Register Bytes Low Bit 1
.equ	OCR4BL2	= 2 ; Timer/Counter4 Output Compare Register Bytes Low Bit 2
.equ	OCR4BL3	= 3 ; Timer/Counter4 Output Compare Register Bytes Low Bit 3
.equ	OCR4BL4	= 4 ; Timer/Counter4 Output Compare Register Bytes Low Bit 4
.equ	OCR4BL5	= 5 ; Timer/Counter4 Output Compare Register Bytes Low Bit 5
.equ	OCR4BL6	= 6 ; Timer/Counter4 Output Compare Register Bytes Low Bit 6
.equ	OCR4BL7	= 7 ; Timer/Counter4 Output Compare Register Bytes Low Bit 7

; ICR4 - Timer/Counter4 Input Capture Register Bytes
.equ	ICR4H0	= 0 ; Timer/Counter4 Input Capture Register Bytes High Bit 8
.equ	ICR4H1	= 1 ; Timer/Counter4 Input Capture Register Bytes High Bit 9
.equ	ICR4H2	= 2 ; Timer/Counter4 Input Capture Register Bytes High Bit 10
.equ	ICR4H3	= 3 ; Timer/Counter4 Input Capture Register Bytes High Bit 11
.equ	ICR4H4	= 4 ; Timer/Counter4 Input Capture Register Bytes High Bit 12
.equ	ICR4H5	= 5 ; Timer/Counter4 Input Capture Register Bytes High Bit 13
.equ	ICR4H6	= 6 ; Timer/Counter4 Input Capture Register Bytes High Bit 14
.equ	ICR4H7	= 7 ; Timer/Counter4 Input Capture Register Bytes High Bit 15

.equ	ICR4L0	= 0 ; Timer/Counter4 Input Capture Register Bytes Low Bit 0
.equ	ICR4L1	= 1 ; Timer/Counter4 Input Capture Register Bytes Low Bit 1
.equ	ICR4L2	= 2 ; Timer/Counter4 Input Capture Register Bytes Low Bit 2
.equ	ICR4L3	= 3 ; Timer/Counter4 Input Capture Register Bytes Low Bit 3
.equ	ICR4L4	= 4 ; Timer/Counter4 Input Capture Register Bytes Low Bit 4
.equ	ICR4L5	= 5 ; Timer/Counter4 Input Capture Register Bytes Low Bit 5
.equ	ICR4L6	= 6 ; Timer/Counter4 Input Capture Register Bytes Low Bit 6
.equ	ICR4L7	= 7 ; Timer/Counter4 Input Capture Register Bytes Low Bit 7


; ***** TC2 *****************
; TIMSK2 - Timer/Counter Interrupt Mask register
.equ	TOIE2	= 0 ; Timer/Counter2 Overflow Interrupt Enable
.equ	OCIE2A	= 1 ; Timer/Counter2 Output Compare Match A Interrupt Enable
.equ	OCIE2B	= 2 ; Timer/Counter2 Output Compare Match B Interrupt Enable

; TIFR2 - Timer/Counter Interrupt Flag Register
.equ	TOV2	= 0 ; Timer/Counter2 Overflow Flag
.equ	OCF2A	= 1 ; Output Compare Flag 2A
.equ	OCF2B	= 2 ; Output Compare Flag 2B

; TCCR2A - Timer/Counter2 Control Register A
.equ	WGM20	= 0 ; Waveform Genration Mode
.equ	WGM21	= 1 ; Waveform Genration Mode
.equ	COM2B0	= 4 ; Compare Output Mode bits
.equ	COM2B1	= 5 ; Compare Output Mode bits
.equ	COM2A0	= 6 ; Compare Output Mode bits
.equ	COM2A1	= 7 ; Compare Output Mode bits

; TCCR2B - Timer/Counter2 Control Register B
.equ	CS20	= 0 ; Clock Select bits
.equ	CS21	= 1 ; Clock Select bits
.equ	CS22	= 2 ; Clock Select bits
.equ	WGM22	= 3 ; Waveform Generation Mode
.equ	FOC2B	= 6 ; Force Output Compare B
.equ	FOC2A	= 7 ; Force Output Compare A

; TCNT2 - Timer/Counter2
.equ	TCNT20	= 0 ; Timer/Counter2 Bit 0
.equ	TCNT21	= 1 ; Timer/Counter2 Bit 1
.equ	TCNT22	= 2 ; Timer/Counter2 Bit 2
.equ	TCNT23	= 3 ; Timer/Counter2 Bit 3
.equ	TCNT24	= 4 ; Timer/Counter2 Bit 4
.equ	TCNT25	= 5 ; Timer/Counter2 Bit 5
.equ	TCNT26	= 6 ; Timer/Counter2 Bit 6
.equ	TCNT27	= 7 ; Timer/Counter2 Bit 7

; OCR2B - Timer/Counter2 Output Compare Register B
.equ	OCR2B0	= 0 ; Timer/Counter2 Output Compare Register B Bit 0
.equ	OCR2B1	= 1 ; Timer/Counter2 Output Compare Register B Bit 1
.equ	OCR2B2	= 2 ; Timer/Counter2 Output Compare Register B Bit 2
.equ	OCR2B3	= 3 ; Timer/Counter2 Output Compare Register B Bit 3
.equ	OCR2B4	= 4 ; Timer/Counter2 Output Compare Register B Bit 4
.equ	OCR2B5	= 5 ; Timer/Counter2 Output Compare Register B Bit 5
.equ	OCR2B6	= 6 ; Timer/Counter2 Output Compare Register B Bit 6
.equ	OCR2B7	= 7 ; Timer/Counter2 Output Compare Register B Bit 7

; OCR2A - Timer/Counter2 Output Compare Register A
.equ	OCR2A0	= 0 ; Timer/Counter2 Output Compare Register A Bit 0
.equ	OCR2A1	= 1 ; Timer/Counter2 Output Compare Register A Bit 1
.equ	OCR2A2	= 2 ; Timer/Counter2 Output Compare Register A Bit 2
.equ	OCR2A3	= 3 ; Timer/Counter2 Output Compare Register A Bit 3
.equ	OCR2A4	= 4 ; Timer/Counter2 Output Compare Register A Bit 4
.equ	OCR2A5	= 5 ; Timer/Counter2 Output Compare Register A Bit 5
.equ	OCR2A6	= 6 ; Timer/Counter2 Output Compare Register A Bit 6
.equ	OCR2A7	= 7 ; Timer/Counter2 Output Compare Register A Bit 7

; ASSR - Asynchronous Status Register
.equ	TCR2BUB	= 0 ; Timer/Counter Control Register2 Update Busy
.equ	TCR2AUB	= 1 ; Timer/Counter Control Register2 Update Busy
.equ	OCR2BUB	= 2 ; Output Compare Register 2 Update Busy
.equ	OCR2AUB	= 3 ; Output Compare Register2 Update Busy
.equ	TCN2UB	= 4 ; Timer/Counter2 Update Busy
.equ	AS2	= 5 ; Asynchronous Timer/Counter2
.equ	EXCLK	= 6 ; Enable External Clock Input

; GTCCR - General Timer Counter Control register
.equ	PSRASY	= 1 ; Prescaler Reset Timer/Counter2


; ***** ADC *****************
; ADMUX - The ADC multiplexer Selection Register
.equ	MUX0	= 0 ; Analog Channel Selection Bits
.equ	MUX1	= 1 ; Analog Channel Selection Bits
.equ	MUX2	= 2 ; Analog Channel Selection Bits
.equ	MUX3	= 3 ; Analog Channel Selection Bits
.equ	ADLAR	= 5 ; Left Adjust Result
.equ	REFS0	= 6 ; Reference Selection Bits
.equ	REFS1	= 7 ; Reference Selection Bits

; ADC - ADC Data Register Bytes
.equ	ADCH0	= 0 ; ADC Data Register Bytes High Bit 8
.equ	ADCH1	= 1 ; ADC Data Register Bytes High Bit 9
.equ	ADCH2	= 2 ; ADC Data Register Bytes High Bit 10
.equ	ADCH3	= 3 ; ADC Data Register Bytes High Bit 11
.equ	ADCH4	= 4 ; ADC Data Register Bytes High Bit 12
.equ	ADCH5	= 5 ; ADC Data Register Bytes High Bit 13
.equ	ADCH6	= 6 ; ADC Data Register Bytes High Bit 14
.equ	ADCH7	= 7 ; ADC Data Register Bytes High Bit 15

.equ	ADCL0	= 0 ; ADC Data Register Bytes Low Bit 0
.equ	ADCL1	= 1 ; ADC Data Register Bytes Low Bit 1
.equ	ADCL2	= 2 ; ADC Data Register Bytes Low Bit 2
.equ	ADCL3	= 3 ; ADC Data Register Bytes Low Bit 3
.equ	ADCL4	= 4 ; ADC Data Register Bytes Low Bit 4
.equ	ADCL5	= 5 ; ADC Data Register Bytes Low Bit 5
.equ	ADCL6	= 6 ; ADC Data Register Bytes Low Bit 6
.equ	ADCL7	= 7 ; ADC Data Register Bytes Low Bit 7

; ADCSRA - The ADC Control and Status register A
.equ	ADPS0	= 0 ; ADC Prescaler Select Bits
.equ	ADPS1	= 1 ; ADC Prescaler Select Bits
.equ	ADPS2	= 2 ; ADC Prescaler Select Bits
.equ	ADIE	= 3 ; ADC Interrupt Enable
.equ	ADIF	= 4 ; ADC Interrupt Flag
.equ	ADATE	= 5 ; ADC Auto Trigger Enable
.equ	ADSC	= 6 ; ADC Start Conversion
.equ	ADEN	= 7 ; ADC Enable

; ADCSRB - The ADC Control and Status register B
.equ	ADTS0	= 0 ; ADC Auto Trigger Source bits
.equ	ADTS1	= 1 ; ADC Auto Trigger Source bits
.equ	ADTS2	= 2 ; ADC Auto Trigger Source bits
.equ	ACME	= 6 ; Analog Comparator Multiplexer Enable

; DIDR0 - Digital Input Disable Register
.equ	ADC0D	= 0 ; ADC Digital Input Disable
.equ	ADC1D	= 1 ; ADC Digital Input Disable
.equ	ADC2D	= 2 ; ADC Digital Input Disable
.equ	ADC3D	= 3 ; ADC Digital Input Disable
.equ	ADC4D	= 4 ; ADC Digital Input Disable
.equ	ADC5D	= 5 ; ADC Digital Input Disable
.equ	ADC6D	= 6 ; ADC Digital Input Disable
.equ	ADC7D	= 7 ; ADC Digital Input Disable


; ***** AC *****************
; ACSRA - Analog Comparator Control And Status Register-A
.equ	ACSRA0	= 0 ; Analog Comparator Control And Status Register-A Bit 0
.equ	ACSRA1	= 1 ; Analog Comparator Control And Status Register-A Bit 1
.equ	ACSRA2	= 2 ; Analog Comparator Control And Status Register-A Bit 2
.equ	ACSRA3	= 3 ; Analog Comparator Control And Status Register-A Bit 3
.equ	ACSRA4	= 4 ; Analog Comparator Control And Status Register-A Bit 4
.equ	ACSRA5	= 5 ; Analog Comparator Control And Status Register-A Bit 5
.equ	ACSRA6	= 6 ; Analog Comparator Control And Status Register-A Bit 6
.equ	ACSRA7	= 7 ; Analog Comparator Control And Status Register-A Bit 7

; ACSR - Analog Comparator Control And Status Register
.equ	ACIS0	= 0 ; Analog Comparator Interrupt Mode Select bits
.equ	ACIS1	= 1 ; Analog Comparator Interrupt Mode Select bits
.equ	ACIC	= 2 ; Analog Comparator Input Capture Enable
.equ	ACIE	= 3 ; Analog Comparator Interrupt Enable
.equ	ACI	= 4 ; Analog Comparator Interrupt Flag
.equ	ACO	= 5 ; Analog Compare Output
.equ	ACBG	= 6 ; Analog Comparator Bandgap Select
.equ	ACD	= 7 ; Analog Comparator Disable

; ACSRB - Analog Comparator Control And Status Register-B
.equ	ACOE	= 0 ; Analog Comparator Output Enable

; DIDR1 - Digital Input Disable Register 1
.equ	AIN0D	= 0 ; AIN0 Digital Input Disable
.equ	AIN1D	= 1 ; AIN1 Digital Input Disable


; ***** PORTB *****************
; PORTB - Port B Data Register
.equ	PORTB0	= 0 ; Port B Data Register Bit 0
.equ	PORTB1	= 1 ; Port B Data Register Bit 1
.equ	PORTB2	= 2 ; Port B Data Register Bit 2
.equ	PORTB3	= 3 ; Port B Data Register Bit 3
.equ	PORTB4	= 4 ; Port B Data Register Bit 4
.equ	PORTB5	= 5 ; Port B Data Register Bit 5
.equ	PORTB6	= 6 ; Port B Data Register Bit 6
.equ	PORTB7	= 7 ; Port B Data Register Bit 7

; DDRB - Port B Data Direction Register
.equ	DDRB0	= 0 ; Port B Data Direction Register Bit 0
.equ	DDRB1	= 1 ; Port B Data Direction Register Bit 1
.equ	DDRB2	= 2 ; Port B Data Direction Register Bit 2
.equ	DDRB3	= 3 ; Port B Data Direction Register Bit 3
.equ	DDRB4	= 4 ; Port B Data Direction Register Bit 4
.equ	DDRB5	= 5 ; Port B Data Direction Register Bit 5
.equ	DDRB6	= 6 ; Port B Data Direction Register Bit 6
.equ	DDRB7	= 7 ; Port B Data Direction Register Bit 7

; PINB - Port B Input Pins
.equ	PINB0	= 0 ; Port B Input Pins Bit 0
.equ	PINB1	= 1 ; Port B Input Pins Bit 1
.equ	PINB2	= 2 ; Port B Input Pins Bit 2
.equ	PINB3	= 3 ; Port B Input Pins Bit 3
.equ	PINB4	= 4 ; Port B Input Pins Bit 4
.equ	PINB5	= 5 ; Port B Input Pins Bit 5
.equ	PINB6	= 6 ; Port B Input Pins Bit 6
.equ	PINB7	= 7 ; Port B Input Pins Bit 7


; ***** PORTC *****************
; PORTC - Port C Data Register
.equ	PORTC0	= 0 ; Port C Data Register Bit 0
.equ	PORTC1	= 1 ; Port C Data Register Bit 1
.equ	PORTC2	= 2 ; Port C Data Register Bit 2
.equ	PORTC3	= 3 ; Port C Data Register Bit 3
.equ	PORTC4	= 4 ; Port C Data Register Bit 4
.equ	PORTC5	= 5 ; Port C Data Register Bit 5
.equ	PORTC6	= 6 ; Port C Data Register Bit 6

; DDRC - Port C Data Direction Register
.equ	DDRC0	= 0 ; Port C Data Direction Register Bit 0
.equ	DDRC1	= 1 ; Port C Data Direction Register Bit 1
.equ	DDRC2	= 2 ; Port C Data Direction Register Bit 2
.equ	DDRC3	= 3 ; Port C Data Direction Register Bit 3
.equ	DDRC4	= 4 ; Port C Data Direction Register Bit 4
.equ	DDRC5	= 5 ; Port C Data Direction Register Bit 5
.equ	DDRC6	= 6 ; Port C Data Direction Register Bit 6

; PINC - Port C Input Pins
.equ	PINC0	= 0 ; Port C Input Pins Bit 0
.equ	PINC1	= 1 ; Port C Input Pins Bit 1
.equ	PINC2	= 2 ; Port C Input Pins Bit 2
.equ	PINC3	= 3 ; Port C Input Pins Bit 3
.equ	PINC4	= 4 ; Port C Input Pins Bit 4
.equ	PINC5	= 5 ; Port C Input Pins Bit 5
.equ	PINC6	= 6 ; Port C Input Pins Bit 6


; ***** PORTD *****************
; PORTD - Port D Data Register
.equ	PORTD0	= 0 ; Port D Data Register Bit 0
.equ	PORTD1	= 1 ; Port D Data Register Bit 1
.equ	PORTD2	= 2 ; Port D Data Register Bit 2
.equ	PORTD3	= 3 ; Port D Data Register Bit 3
.equ	PORTD4	= 4 ; Port D Data Register Bit 4
.equ	PORTD5	= 5 ; Port D Data Register Bit 5
.equ	PORTD6	= 6 ; Port D Data Register Bit 6
.equ	PORTD7	= 7 ; Port D Data Register Bit 7

; DDRD - Port D Data Direction Register
.equ	DDRD0	= 0 ; Port D Data Direction Register Bit 0
.equ	DDRD1	= 1 ; Port D Data Direction Register Bit 1
.equ	DDRD2	= 2 ; Port D Data Direction Register Bit 2
.equ	DDRD3	= 3 ; Port D Data Direction Register Bit 3
.equ	DDRD4	= 4 ; Port D Data Direction Register Bit 4
.equ	DDRD5	= 5 ; Port D Data Direction Register Bit 5
.equ	DDRD6	= 6 ; Port D Data Direction Register Bit 6
.equ	DDRD7	= 7 ; Port D Data Direction Register Bit 7

; PIND - Port D Input Pins
.equ	PIND0	= 0 ; Port D Input Pins Bit 0
.equ	PIND1	= 1 ; Port D Input Pins Bit 1
.equ	PIND2	= 2 ; Port D Input Pins Bit 2
.equ	PIND3	= 3 ; Port D Input Pins Bit 3
.equ	PIND4	= 4 ; Port D Input Pins Bit 4
.equ	PIND5	= 5 ; Port D Input Pins Bit 5
.equ	PIND6	= 6 ; Port D Input Pins Bit 6
.equ	PIND7	= 7 ; Port D Input Pins Bit 7


; ***** PORTE *****************
; PORTE - Port E Data Register
.equ	PORTE0	= 0 ; Port E Data Register Bit 0
.equ	PORTE1	= 1 ; Port E Data Register Bit 1
.equ	PORTE2	= 2 ; Port E Data Register Bit 2
.equ	PORTE3	= 3 ; Port E Data Register Bit 3

; DDRE - Port E Data Direction Register
.equ	DDRE0	= 0 ; Port E Data Direction Register Bit 0
.equ	DDRE1	= 1 ; Port E Data Direction Register Bit 1
.equ	DDRE2	= 2 ; Port E Data Direction Register Bit 2
.equ	DDRE3	= 3 ; Port E Data Direction Register Bit 3

; PINE - Port E Input Pins
.equ	PINE0	= 0 ; Port E Input Pins Bit 0
.equ	PINE1	= 1 ; Port E Input Pins Bit 1
.equ	PINE2	= 2 ; Port E Input Pins Bit 2
.equ	PINE3	= 3 ; Port E Input Pins Bit 3


; ***** TC0 *****************
; OCR0B - Timer/Counter0 Output Compare Register
.equ	OCR0B0	= 0 ; Timer/Counter0 Output Compare Register Bit 0
.equ	OCR0B1	= 1 ; Timer/Counter0 Output Compare Register Bit 1
.equ	OCR0B2	= 2 ; Timer/Counter0 Output Compare Register Bit 2
.equ	OCR0B3	= 3 ; Timer/Counter0 Output Compare Register Bit 3
.equ	OCR0B4	= 4 ; Timer/Counter0 Output Compare Register Bit 4
.equ	OCR0B5	= 5 ; Timer/Counter0 Output Compare Register Bit 5
.equ	OCR0B6	= 6 ; Timer/Counter0 Output Compare Register Bit 6
.equ	OCR0B7	= 7 ; Timer/Counter0 Output Compare Register Bit 7

; OCR0A - Timer/Counter0 Output Compare Register
.equ	OCR0A0	= 0 ; Timer/Counter0 Output Compare Register Bit 0
.equ	OCR0A1	= 1 ; Timer/Counter0 Output Compare Register Bit 1
.equ	OCR0A2	= 2 ; Timer/Counter0 Output Compare Register Bit 2
.equ	OCR0A3	= 3 ; Timer/Counter0 Output Compare Register Bit 3
.equ	OCR0A4	= 4 ; Timer/Counter0 Output Compare Register Bit 4
.equ	OCR0A5	= 5 ; Timer/Counter0 Output Compare Register Bit 5
.equ	OCR0A6	= 6 ; Timer/Counter0 Output Compare Register Bit 6
.equ	OCR0A7	= 7 ; Timer/Counter0 Output Compare Register Bit 7

; TCNT0 - Timer/Counter0
.equ	TCNT00	= 0 ; Timer/Counter0 Bit 0
.equ	TCNT01	= 1 ; Timer/Counter0 Bit 1
.equ	TCNT02	= 2 ; Timer/Counter0 Bit 2
.equ	TCNT03	= 3 ; Timer/Counter0 Bit 3
.equ	TCNT04	= 4 ; Timer/Counter0 Bit 4
.equ	TCNT05	= 5 ; Timer/Counter0 Bit 5
.equ	TCNT06	= 6 ; Timer/Counter0 Bit 6
.equ	TCNT07	= 7 ; Timer/Counter0 Bit 7

; TCCR0B - Timer/Counter Control Register B
.equ	CS00	= 0 ; Clock Select
.equ	CS01	= 1 ; Clock Select
.equ	CS02	= 2 ; Clock Select
.equ	WGM02	= 3 ; 
.equ	FOC0B	= 6 ; Force Output Compare B
.equ	FOC0A	= 7 ; Force Output Compare A

; TCCR0A - Timer/Counter Control Register A
.equ	WGM00	= 0 ; Waveform Generation Mode
.equ	WGM01	= 1 ; Waveform Generation Mode
.equ	COM0B0	= 4 ; Compare Output Mode, Fast PWM
.equ	COM0B1	= 5 ; Compare Output Mode, Fast PWM
.equ	COM0A0	= 6 ; Compare Output Mode, Phase Correct PWM Mode
.equ	COM0A1	= 7 ; Compare Output Mode, Phase Correct PWM Mode

; TIMSK0 - Timer/Counter0 Interrupt Mask Register
.equ	TOIE0	= 0 ; Timer/Counter0 Overflow Interrupt Enable
.equ	OCIE0A	= 1 ; Timer/Counter0 Output Compare Match A Interrupt Enable
.equ	OCIE0B	= 2 ; Timer/Counter0 Output Compare Match B Interrupt Enable

; TIFR0 - Timer/Counter0 Interrupt Flag register
.equ	TOV0	= 0 ; Timer/Counter0 Overflow Flag
.equ	OCF0A	= 1 ; Timer/Counter0 Output Compare Flag 0A
.equ	OCF0B	= 2 ; Timer/Counter0 Output Compare Flag 0B

; GTCCR - General Timer/Counter Control Register


; ***** EXINT *****************
; EICRA - External Interrupt Control Register
.equ	ISC00	= 0 ; External Interrupt Sense Control 0 Bits
.equ	ISC01	= 1 ; External Interrupt Sense Control 0 Bits
.equ	ISC10	= 2 ; External Interrupt Sense Control 1 Bits
.equ	ISC11	= 3 ; External Interrupt Sense Control 1 Bits

; EIMSK - External Interrupt Mask Register
.equ	INT0	= 0 ; External Interrupt Request 1 Enable
.equ	INT1	= 1 ; External Interrupt Request 1 Enable

; EIFR - External Interrupt Flag Register
.equ	INTF0	= 0 ; External Interrupt Flags
.equ	INTF1	= 1 ; External Interrupt Flags

; PCICR - Pin Change Interrupt Control Register
.equ	PCIE0	= 0 ; Pin Change Interrupt Enables
.equ	PCIE1	= 1 ; Pin Change Interrupt Enables
.equ	PCIE2	= 2 ; Pin Change Interrupt Enables
.equ	PCIE3	= 3 ; Pin Change Interrupt Enables

; PCMSK3 - Pin Change Mask Register 3
.equ	PCINT24	= 0 ; Pin Change Enable Masks
.equ	PCINT25	= 1 ; Pin Change Enable Masks
.equ	PCINT26	= 2 ; Pin Change Enable Masks
.equ	PCINT27	= 3 ; Pin Change Enable Masks

; PCMSK2 - Pin Change Mask Register 2
.equ	PCINT16	= 0 ; Pin Change Enable Masks
.equ	PCINT17	= 1 ; Pin Change Enable Masks
.equ	PCINT18	= 2 ; Pin Change Enable Masks
.equ	PCINT19	= 3 ; Pin Change Enable Masks
.equ	PCINT20	= 4 ; Pin Change Enable Masks
.equ	PCINT21	= 5 ; Pin Change Enable Masks
.equ	PCINT22	= 6 ; Pin Change Enable Masks
.equ	PCINT23	= 7 ; Pin Change Enable Masks

; PCMSK1 - Pin Change Mask Register 1
.equ	PCINT8	= 0 ; Pin Change Enable Masks
.equ	PCINT9	= 1 ; Pin Change Enable Masks
.equ	PCINT10	= 2 ; Pin Change Enable Masks
.equ	PCINT11	= 3 ; Pin Change Enable Masks
.equ	PCINT12	= 4 ; Pin Change Enable Masks
.equ	PCINT13	= 5 ; Pin Change Enable Masks
.equ	PCINT14	= 6 ; Pin Change Enable Masks

; PCMSK0 - Pin Change Mask Register 0
.equ	PCINT0	= 0 ; Pin Change Enable Masks
.equ	PCINT1	= 1 ; Pin Change Enable Masks
.equ	PCINT2	= 2 ; Pin Change Enable Masks
.equ	PCINT3	= 3 ; Pin Change Enable Masks
.equ	PCINT4	= 4 ; Pin Change Enable Masks
.equ	PCINT5	= 5 ; Pin Change Enable Masks
.equ	PCINT6	= 6 ; Pin Change Enable Masks
.equ	PCINT7	= 7 ; Pin Change Enable Masks

; PCIFR - Pin Change Interrupt Flag Register
.equ	PCIF0	= 0 ; Pin Change Interrupt Flags
.equ	PCIF1	= 1 ; Pin Change Interrupt Flags
.equ	PCIF2	= 2 ; Pin Change Interrupt Flags
.equ	PCIF3	= 3 ; Pin Change Interrupt Flags


; ***** SPI0 *****************
; SPDR0 - SPI Data Register
.equ	SPDR00	= 0 ; SPI Data Register Bit 0
.equ	SPDR01	= 1 ; SPI Data Register Bit 1
.equ	SPDR02	= 2 ; SPI Data Register Bit 2
.equ	SPDR03	= 3 ; SPI Data Register Bit 3
.equ	SPDR04	= 4 ; SPI Data Register Bit 4
.equ	SPDR05	= 5 ; SPI Data Register Bit 5
.equ	SPDR06	= 6 ; SPI Data Register Bit 6
.equ	SPDR07	= 7 ; SPI Data Register Bit 7

; SPSR0 - SPI Status Register
.equ	SPI2X	= 0 ; Double SPI Speed Bit
.equ	WCOL	= 6 ; Write Collision Flag
.equ	SPIF	= 7 ; SPI Interrupt Flag

; SPCR0 - SPI Control Register
.equ	SPR0	= 0 ; SPI Clock Rate Selects
.equ	SPR1	= 1 ; SPI Clock Rate Selects
.equ	CPHA	= 2 ; Clock Phase
.equ	CPOL	= 3 ; Clock polarity
.equ	MSTR	= 4 ; Master/Slave Select
.equ	DORD	= 5 ; Data Order
.equ	SPE	= 6 ; SPI Enable
.equ	SPIE	= 7 ; SPI Interrupt Enable


; ***** SPI1 *****************
; SPDR1 - SPI Data Register
.equ	SPDR10	= 0 ; SPI Data Register Bit 0
.equ	SPDR11	= 1 ; SPI Data Register Bit 1
.equ	SPDR12	= 2 ; SPI Data Register Bit 2
.equ	SPDR13	= 3 ; SPI Data Register Bit 3
.equ	SPDR14	= 4 ; SPI Data Register Bit 4
.equ	SPDR15	= 5 ; SPI Data Register Bit 5
.equ	SPDR16	= 6 ; SPI Data Register Bit 6
.equ	SPDR17	= 7 ; SPI Data Register Bit 7

; SPSR1 - SPI Status Register
.equ	SPI2X1	= 0 ; Double SPI Speed Bit
.equ	WCOL1	= 6 ; Write Collision Flag
.equ	SPIF1	= 7 ; SPI Interrupt Flag

; SPCR1 - SPI Control Register
.equ	SPR10	= 0 ; SPI Clock Rate Selects
.equ	SPR11	= 1 ; SPI Clock Rate Selects
.equ	CPHA1	= 2 ; Clock Phase
.equ	CPOL1	= 3 ; Clock polarity
.equ	MSTR1	= 4 ; Master/Slave Select
.equ	DORD1	= 5 ; Data Order
.equ	SPE1	= 6 ; SPI Enable
.equ	SPIE1	= 7 ; SPI Interrupt Enable


; ***** WDT *****************
; WDTCSR - Watchdog Timer Control Register
.equ	WDE	= 3 ; Watch Dog Enable
.equ	WDCE	= 4 ; Watchdog Change Enable
.equ	WDP0	= 0 ; Watchdog Timer Prescaler Bits
.equ	WDP1	= 1 ; Watchdog Timer Prescaler Bits
.equ	WDP2	= 2 ; Watchdog Timer Prescaler Bits
.equ	WDP3	= 5 ; Watchdog Timer Prescaler Bits
.equ	WDIE	= 6 ; Watchdog Timeout Interrupt Enable
.equ	WDIF	= 7 ; Watchdog Timeout Interrupt Flag


; ***** PTC *****************
; ***** CPU *****************
; PRR0 - Power Reduction Register 0
.equ	PRADC	= 0 ; Power Reduction ADC
.equ	PRUSART0	= 1 ; Power Reduction USART0
.equ	PRSPI0	= 2 ; Power Reduction Serial Peripheral Interface 1
.equ	PRTIM1	= 3 ; Power Reduction Timer/Counter1
.equ	PRUSART1	= 4 ; Power Reduction USART1
.equ	PRTIM0	= 5 ; Power Reduction Timer/Counter0
.equ	PRTIM2	= 6 ; Power Reduction Timer/Counter2
.equ	PRTWI0	= 7 ; Power Reduction TWI0

; PRR1 - Power Reduction Register 1
.equ	PRTIM3	= 0 ; Power Reduction Timer/Counter3
.equ	PRSPI1	= 2 ; Power Reduction Serial Peripheral Interface 1
.equ	PRTIM4	= 3 ; Power Reduction Timer/Counter4
.equ	PRPTC	= 4 ; Power Reduction Peripheral Touch Controller
.equ	PRTWI1	= 5 ; Power Reduction TWI1

; OSCCAL - Oscillator Calibration Value
.equ	OSCCAL0	= 0 ; Oscillator Calibration
.equ	OSCCAL1	= 1 ; Oscillator Calibration
.equ	OSCCAL2	= 2 ; Oscillator Calibration
.equ	OSCCAL3	= 3 ; Oscillator Calibration
.equ	OSCCAL4	= 4 ; Oscillator Calibration
.equ	OSCCAL5	= 5 ; Oscillator Calibration
.equ	OSCCAL6	= 6 ; Oscillator Calibration
.equ	OSCCAL7	= 7 ; Oscillator Calibration

; CLKPR - Clock Prescale Register
.equ	CLKPS0	= 0 ; Clock Prescaler Select Bits
.equ	CLKPS1	= 1 ; Clock Prescaler Select Bits
.equ	CLKPS2	= 2 ; Clock Prescaler Select Bits
.equ	CLKPS3	= 3 ; Clock Prescaler Select Bits
.equ	CLKPCE	= 7 ; Clock Prescaler Change Enable

; SREG - Status Register
.equ	SREG_C	= 0 ; Carry Flag
.equ	SREG_Z	= 1 ; Zero Flag
.equ	SREG_N	= 2 ; Negative Flag
.equ	SREG_V	= 3 ; Two's Complement Overflow Flag
.equ	SREG_S	= 4 ; Sign Bit
.equ	SREG_H	= 5 ; Half Carry Flag
.equ	SREG_T	= 6 ; Bit Copy Storage
.equ	SREG_I	= 7 ; Global Interrupt Enable

; SP - Stack Pointer
.equ	SPH0	= 0 ; Stack Pointer High Bit 8
.equ	SPH1	= 1 ; Stack Pointer High Bit 9
.equ	SPH2	= 2 ; Stack Pointer High Bit 10
.equ	SPH3	= 3 ; Stack Pointer High Bit 11

.equ	SPL0	= 0 ; Stack Pointer Low Bit 0
.equ	SPL1	= 1 ; Stack Pointer Low Bit 1
.equ	SPL2	= 2 ; Stack Pointer Low Bit 2
.equ	SPL3	= 3 ; Stack Pointer Low Bit 3
.equ	SPL4	= 4 ; Stack Pointer Low Bit 4
.equ	SPL5	= 5 ; Stack Pointer Low Bit 5
.equ	SPL6	= 6 ; Stack Pointer Low Bit 6
.equ	SPL7	= 7 ; Stack Pointer Low Bit 7

; SPMCSR - Store Program Memory Control and Status Register
.equ	SPMEN	= 0 ; Store Program Memory
.equ	PGERS	= 1 ; Page Erase
.equ	PGWRT	= 2 ; Page Write
.equ	BLBSET	= 3 ; Boot Lock Bit Set
.equ	RWWSRE	= 4 ; Read-While-Write section read enable
.equ	SIGRD	= 5 ; Signature Row Read
.equ	RWWSB	= 6 ; Read-While-Write Section Busy
.equ	SPMIE	= 7 ; SPM Interrupt Enable

; MCUCR - MCU Control Register
.equ	IVCE	= 0 ; 
.equ	IVSEL	= 1 ; 
.equ	PUD	= 4 ; 
.equ	BODSE	= 5 ; BOD Sleep Enable
.equ	BODS	= 6 ; BOD Sleep

; MCUSR - MCU Status Register
.equ	PORF	= 0 ; Power-on reset flag
.equ	EXTRF	= 1 ; External Reset Flag
.equ	BORF	= 2 ; Brown-out Reset Flag
.equ	WDRF	= 3 ; Watchdog Reset Flag

; SMCR - Sleep Mode Control Register
.equ	SE	= 0 ; Sleep Enable
.equ	SM0	= 1 ; Sleep Mode Select Bits
.equ	SM1	= 2 ; Sleep Mode Select Bits
.equ	SM2	= 3 ; Sleep Mode Select Bits

; GPIOR2 - General Purpose I/O Register 2
.equ	GPIOR20	= 0 ; General Purpose I/O Register 2 Bit 0
.equ	GPIOR21	= 1 ; General Purpose I/O Register 2 Bit 1
.equ	GPIOR22	= 2 ; General Purpose I/O Register 2 Bit 2
.equ	GPIOR23	= 3 ; General Purpose I/O Register 2 Bit 3
.equ	GPIOR24	= 4 ; General Purpose I/O Register 2 Bit 4
.equ	GPIOR25	= 5 ; General Purpose I/O Register 2 Bit 5
.equ	GPIOR26	= 6 ; General Purpose I/O Register 2 Bit 6
.equ	GPIOR27	= 7 ; General Purpose I/O Register 2 Bit 7

; GPIOR1 - General Purpose I/O Register 1
.equ	GPIOR10	= 0 ; General Purpose I/O Register 1 Bit 0
.equ	GPIOR11	= 1 ; General Purpose I/O Register 1 Bit 1
.equ	GPIOR12	= 2 ; General Purpose I/O Register 1 Bit 2
.equ	GPIOR13	= 3 ; General Purpose I/O Register 1 Bit 3
.equ	GPIOR14	= 4 ; General Purpose I/O Register 1 Bit 4
.equ	GPIOR15	= 5 ; General Purpose I/O Register 1 Bit 5
.equ	GPIOR16	= 6 ; General Purpose I/O Register 1 Bit 6
.equ	GPIOR17	= 7 ; General Purpose I/O Register 1 Bit 7

; GPIOR0 - General Purpose I/O Register 0
.equ	GPIOR00	= 0 ; General Purpose I/O Register 0 Bit 0
.equ	GPIOR01	= 1 ; General Purpose I/O Register 0 Bit 1
.equ	GPIOR02	= 2 ; General Purpose I/O Register 0 Bit 2
.equ	GPIOR03	= 3 ; General Purpose I/O Register 0 Bit 3
.equ	GPIOR04	= 4 ; General Purpose I/O Register 0 Bit 4
.equ	GPIOR05	= 5 ; General Purpose I/O Register 0 Bit 5
.equ	GPIOR06	= 6 ; General Purpose I/O Register 0 Bit 6
.equ	GPIOR07	= 7 ; General Purpose I/O Register 0 Bit 7


; ***** EEPROM *****************
; EEAR - EEPROM Address Register Bytes
.equ	EEARH0	= 0 ; EEPROM Address Register Bytes High Bit 8
.equ	EEARH1	= 1 ; EEPROM Address Register Bytes High Bit 9

.equ	EEARL0	= 0 ; EEPROM Address Register Bytes Low Bit 0
.equ	EEARL1	= 1 ; EEPROM Address Register Bytes Low Bit 1
.equ	EEARL2	= 2 ; EEPROM Address Register Bytes Low Bit 2
.equ	EEARL3	= 3 ; EEPROM Address Register Bytes Low Bit 3
.equ	EEARL4	= 4 ; EEPROM Address Register Bytes Low Bit 4
.equ	EEARL5	= 5 ; EEPROM Address Register Bytes Low Bit 5
.equ	EEARL6	= 6 ; EEPROM Address Register Bytes Low Bit 6
.equ	EEARL7	= 7 ; EEPROM Address Register Bytes Low Bit 7

; EEDR - EEPROM Data Register
.equ	EEDR0	= 0 ; EEPROM Data Register Bit 0
.equ	EEDR1	= 1 ; EEPROM Data Register Bit 1
.equ	EEDR2	= 2 ; EEPROM Data Register Bit 2
.equ	EEDR3	= 3 ; EEPROM Data Register Bit 3
.equ	EEDR4	= 4 ; EEPROM Data Register Bit 4
.equ	EEDR5	= 5 ; EEPROM Data Register Bit 5
.equ	EEDR6	= 6 ; EEPROM Data Register Bit 6
.equ	EEDR7	= 7 ; EEPROM Data Register Bit 7

; EECR - EEPROM Control Register
.equ	EERE	= 0 ; EEPROM Read Enable
.equ	EEPE	= 1 ; EEPROM Write Enable
.equ	EEMPE	= 2 ; EEPROM Master Write Enable
.equ	EERIE	= 3 ; EEPROM Ready Interrupt Enable
.equ	EEPM0	= 4 ; EEPROM Programming Mode Bits
.equ	EEPM1	= 5 ; EEPROM Programming Mode Bits


; ***** FUSE *****************
; EXTENDED - 
.equ	BODLEVEL0	= 0 ; Brown-out Detector trigger level
.equ	BODLEVEL1	= 1 ; Brown-out Detector trigger level
.equ	BODLEVEL2	= 2 ; Brown-out Detector trigger level
.equ	CFD	= 3 ; Clock Failure Detection

; HIGH - 
.equ	BOOTRST	= 0 ; Boot Reset vector Enabled
.equ	BOOTSZ0	= 1 ; Select boot size
.equ	BOOTSZ1	= 2 ; Select boot size
.equ	EESAVE	= 3 ; Preserve EEPROM through the Chip Erase cycle
.equ	WDTON	= 4 ; Watch-dog Timer always on
.equ	SPIEN	= 5 ; Serial program downloading (SPI) enabled
.equ	DWEN	= 6 ; Debug Wire enable
.equ	RSTDISBL	= 7 ; Reset Disabled (Enable PC6 as i/o pin)

; LOW - 
.equ	SUT_CKSEL0	= 0 ; Select Clock Source
.equ	SUT_CKSEL1	= 1 ; Select Clock Source
.equ	SUT_CKSEL2	= 2 ; Select Clock Source
.equ	SUT_CKSEL3	= 3 ; Select Clock Source
.equ	SUT_CKSEL4	= 4 ; Select Clock Source
.equ	SUT_CKSEL5	= 5 ; Select Clock Source
.equ	CKOUT	= 6 ; Clock output on PORTB0
.equ	CKDIV8	= 7 ; Divide clock by 8 internally


; ***** LOCKBIT *****************
; LOCKBIT - 
.equ	LB1	= 0 ; Memory Lock
.equ	LB2	= 1 ; Memory Lock
.equ	BLB01	= 2 ; Boot Loader Protection Mode
.equ	BLB02	= 3 ; Boot Loader Protection Mode
.equ	BLB11	= 4 ; Boot Loader Protection Mode
.equ	BLB12	= 5 ; Boot Loader Protection Mode


; ***** CFD *****************
; XFDCSR - XOSC Failure Detection Control and Status Register
.equ	XFDIE	= 0 ; Failure Detection Interrupt Enable
.equ	XFDIF	= 1 ; Failure Detection Interrupt Flag

; ***** CPU REGISTER DEFINITIONS *****************************************
.def	XH	= r27
.def	XL	= r26
.def	YH	= r29
.def	YL	= r28
.def	ZH	= r31
.def	ZL	= r30

; ***** DATA MEMORY DECLARATIONS *****************************************
.equ	FLASHSTART	= 0x0000 ; Note: Word address
.equ	FLASHEND	= 0x3FFF ; Note: Word address
.equ	FLASHPAGESIZE	= 0x0040 ; Note: Size in words

.equ	BOOTSECTION1START	= 0x3F00 ; Note: Word address
.equ	BOOTSECTION1END	= 0x3FFF ; Note: Word address
.equ	BOOTSECTION1PAGESIZE	= 0x0040 ; Note: Size in words

.equ	BOOTSECTION2START	= 0x3E00 ; Note: Word address
.equ	BOOTSECTION2END	= 0x3FFF ; Note: Word address
.equ	BOOTSECTION2PAGESIZE	= 0x0040 ; Note: Size in words

.equ	BOOTSECTION3START	= 0x3C00 ; Note: Word address
.equ	BOOTSECTION3END	= 0x3FFF ; Note: Word address
.equ	BOOTSECTION3PAGESIZE	= 0x0040 ; Note: Size in words

.equ	BOOTSECTION4START	= 0x3800 ; Note: Word address
.equ	BOOTSECTION4END	= 0x3FFF ; Note: Word address
.equ	BOOTSECTION4PAGESIZE	= 0x0040 ; Note: Size in words

.equ	IOEND	= 0x00FF

.equ	SRAM_START	= 0x0100
.equ	SRAM_SIZE	= 2048
.equ	RAMEND	= 0x08FF

.equ	E2END	= 0x03FF
.equ	EEPROMEND	= 0x03FF
.equ	EEADRBITS	= 10

.equ	XRAMEND	= 0x0000

#pragma AVRPART MEMORY PROG_FLASH 32768
#pragma AVRPART MEMORY INT_SRAM SIZE 2048
#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x100

#pragma AVRPART MEMORY EEPROM 1024

; ***** BOOTLOADER DECLARATIONS ******************************************
.equ	PAGESIZE	= 0x0040 ; Note: Size in words

.equ	FIRSTBOOTSTART	= 0x3F00 ; Note: Word address
.equ	FIRSTBOOTPAGESIZE	= 0x0040 ; Note: Size in words
.equ	FIRSTBOOTEND	= 0x0000 ; Note: Word address

.equ	SECONDBOOTSTART	= 0x3E00 ; Note: Word address
.equ	SECONDBOOTPAGESIZE	= 0x0040 ; Note: Size in words
.equ	SECONDBOOTEND	= 0x0000 ; Note: Word address

.equ	THIRDBOOTSTART	= 0x3C00 ; Note: Word address
.equ	THIRDBOOTPAGESIZE	= 0x0040 ; Note: Size in words
.equ	THIRDBOOTEND	= 0x0000 ; Note: Word address

.equ	FOURTHBOOTSTART	= 0x3800 ; Note: Word address
.equ	FOURTHBOOTPAGESIZE	= 0x0040 ; Note: Size in words
.equ	FOURTHBOOTEND	= 0x0000 ; Note: Word address

.equ	 SMALLBOOTSTART	= FIRSTBOOTSTART ;
.equ	 LARGEBOOTSTART	= FOURTHBOOTSTART ;


; ***** INTERRUPT VECTORS ************************************************
.equ	INT0addr	 = 0x0002 ; External Interrupt Request 0
.equ	INT1addr	 = 0x0004 ; External Interrupt Request 1
.equ	PCINT0addr	 = 0x0006 ; Pin Change Interrupt Request 0
.equ	PCINT1addr	 = 0x0008 ; Pin Change Interrupt Request 1
.equ	PCINT2addr	 = 0x000A ; Pin Change Interrupt Request 2
.equ	WDTaddr	 = 0x000C ; Watchdog Time-out Interrupt
.equ	TIMER2_COMPAaddr	 = 0x000E ; Timer/Counter2 Compare Match A
.equ	TIMER2_COMPBaddr	 = 0x0010 ; Timer/Counter2 Compare Match B
.equ	TIMER2_OVFaddr	 = 0x0012 ; Timer/Counter2 Overflow
.equ	TIMER1_CAPTaddr	 = 0x0014 ; Timer/Counter1 Capture Event
.equ	TIMER1_COMPAaddr	 = 0x0016 ; Timer/Counter1 Compare Match A
.equ	TIMER1_COMPBaddr	 = 0x0018 ; Timer/Counter1 Compare Match B
.equ	TIMER1_OVFaddr	 = 0x001A ; Timer/Counter1 Overflow
.equ	TIMER0_COMPAaddr	 = 0x001C ; TimerCounter0 Compare Match A
.equ	TIMER0_COMPBaddr	 = 0x001E ; TimerCounter0 Compare Match B
.equ	TIMER0_OVFaddr	 = 0x0020 ; Timer/Couner0 Overflow
.equ	SPI0_STCaddr	 = 0x0022 ; SPI Serial Transfer Complete
.equ	USART0_RXaddr	 = 0x0024 ; USART0 Rx Complete
.equ	USART0_UDREaddr	 = 0x0026 ; USART0, Data Register Empty
.equ	USART0_TXaddr	 = 0x0028 ; USART0 Tx Complete
.equ	ADCaddr	 = 0x002A ; ADC Conversion Complete
.equ	EE_READYaddr	 = 0x002C ; EEPROM Ready
.equ	ANALOG_COMPaddr	 = 0x002E ; Analog Comparator
.equ	TWI0addr	 = 0x0030 ; Two-wire Serial Interface
.equ	SPM_Readyaddr	 = 0x0032 ; Store Program Memory Read
.equ	USART0_STARTaddr	 = 0x0034 ; USART0 Start frame detection
.equ	PCINT3addr	 = 0x0036 ; Pin Change Interrupt Request 3
.equ	USART1_RXaddr	 = 0x0038 ; USART1 Rx Complete
.equ	USART1_UDREaddr	 = 0x003A ; USART1, Data Register Empty
.equ	USART1_TXaddr	 = 0x003C ; USART1 Tx Complete
.equ	USART1_STARTaddr	 = 0x003E ; USART1 Start frame detection
.equ	TIMER3_CAPTaddr	 = 0x0040 ; Timer/Counter3 Capture Event
.equ	TIMER3_COMPAaddr	 = 0x0042 ; Timer/Counter3 Compare Match A
.equ	TIMER3_COMPBaddr	 = 0x0044 ; Timer/Counter3 Compare Match B
.equ	TIMER3_OVFaddr	 = 0x0046 ; Timer/Counter3 Overflow
.equ	CFDaddr	 = 0x0048 ; Clock failure detection interrupt
.equ	PTC_EOCaddr	 = 0x004A ; PTC End of conversion
.equ	PTC_WCOMPaddr	 = 0x004C ; PTC Window comparator mode
.equ	SPI1_STCaddr	 = 0x004E ; SPI1 Serial Transfer Complete
.equ	TWI1addr	 = 0x0050 ; TWI Transfer Complete
.equ	TIMER4_CAPTaddr	 = 0x0052 ; Timer/Counter4 Capture Event
.equ	TIMER4_COMPAaddr	 = 0x0054 ; Timer/Counter4 Compare Match A
.equ	TIMER4_COMPBaddr	 = 0x0056 ; Timer/Counter4 Compare Match B
.equ	TIMER4_OVFaddr	 = 0x0058 ; Timer/Counter4 Overflow

.equ	INT_VECTORS_SIZE	= 90 ; size in words

#endif /* _M328PBDEF_INC_ */

; ***** END OF FILE ******************************************************



