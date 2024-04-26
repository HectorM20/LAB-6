//***********************************************************************************************
// Universidad del Valle de Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Héctor Martínez - 22369
// Proyecto: postlab 6
// Hardware: atmega328p
//***********************************************************************************************

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define F_CPU 16000000
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

void USART_init(void);
void USART_send(unsigned char data);
unsigned char USART_receive(void);
void USART_putstring(char* StringPtr);
void ADC_init();
uint16_t ADC_read(uint8_t ch);

int main(void)
{
	USART_init(); // Inicializa USART
	ADC_init();  // Inicializar ADC
	char buffer[20]; // Búfer para conversión de cadenas

	while(1)
	{
// Menú
		USART_putstring("Menu:\n");
		USART_putstring("1. Leer potenciometro\n");
		USART_putstring("2. Enviar ASCII\n");


// Esperar la respuesta del usuario
		unsigned char option = USART_receive();


		if (option == '1')
		{
//Lee el valor del potenciómetro
			uint16_t pot_value = ADC_read(0); // potenciómetro conectado al pin A0

// Convertir el valor del potenciómetro a cadena
			sprintf(buffer, "Valor del potenciometro: %d\n", pot_value);

//Envía el valor del potenciómetro 
			USART_putstring(buffer);
		}
		else if (option == '2')
		{
//Enviar ASCII
			USART_putstring("Enviando ASCII...\n");
			USART_send('A'); // Example: Send 'A' character
		}
		else
		{
// Opción inválida
			USART_putstring("Opcion invalida. Ingrese 1 o 2.\n");
		}

		_delay_ms(100);
	}
}

void USART_init(void)
{
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
}

void USART_send(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

unsigned char USART_receive(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void USART_putstring(char* StringPtr)
{
	while(*StringPtr != 0x00)
	{
		USART_send(*StringPtr);
		StringPtr++;
	}
}

void ADC_init()
{
// Establece la referencia de ADC a AVCC
	ADMUX |= (1 << REFS0);

// Establece el prescaler ADC en 128
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

// Habilitar ADC
	ADCSRA |= (1 << ADEN);
}

uint16_t ADC_read(uint8_t ch)
{
// Selecciona el canal ADC con máscara de seguridad
	ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);

// Iniciar conversión única
	ADCSRA |= (1 << ADSC);

// Espera a que se complete la conversión
	while (ADCSRA & (1 << ADSC));

// Devuelve el valor ADC
	return ADC;
}
