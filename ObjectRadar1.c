/*
 * ObjectRadar1.c
 *
 * Created: 12/24/2017 1:10:00 AM
 *  Author: Rifat & Abir
 */ 


#undef F_CPU
#define F_CPU 1000000UL
#define D4 eS_PORTC4
#define D5 eS_PORTC5
#define D6 eS_PORTC6
#define D7 eS_PORTC7
#define RS eS_PORTC2
#define EN eS_PORTC3

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h" /* Include LCD header file */

static volatile int pulse = 0;
static volatile int i = 0;

void servo_init(void)//Initialing servo motor timer1
{
	
	TCCR1A |= 1<<WGM11 | 1<<COM1B1 | 1<<COM1B0 |1<<COM1A1 | 1<<COM1A0;
	TCCR1B |= 1<<WGM13 | 1<<WGM12 | 1<<CS10;
	ICR1 = 19999;
	GICR=0x00;
	MCUCR=0x00;
}

void INT_init()//Initialing interrupt
{
	GICR|=(1<<INT0);//PIND2 is for echo pin
	MCUCR|=(1<<ISC00);
	TCCR1A = 0;
}

void trigger_signal(void)//PIND0 for trigger signal
{
	PORTD|=(1<<PIND0);
	_delay_us(15);
	PORTD &=~(1<<PIND0);
}

unsigned char ROL(unsigned char number,int shiftamount)
{
	int i;
	for(i=0;i<shiftamount;i++)
	{
		
		unsigned char MSB=number & 0x80;
		MSB=MSB>>7;
		number=number<<1;
		number=number|MSB;
	}
	return number;
}

void drawChar(unsigned char *character )
{
	int i;
	for(i=0;i<10;i++)
	{
		int j;
		unsigned char row=0x01;
		 
		for(j=0;j< 8 ;j++)
		{
			PORTA=ROL(row,j);
			//PORTA =row;
			PORTB=character[j];
			if(j==3 || j==4)
			{
				PORTC &=0b11111100;
			}
			else
			{
				PORTC|=0b00000011;
			}
			//row <<= 1;
			_delay_us(500);
		}
		
	}
}



int main(void)
{
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRC=0xFF;
	DDRD = 0b11111011;
	
	unsigned char tmp1,tmp2=0,tmp3=0,tmp4=0,tmp5=0,tmp6,tmp7,tmp8,tmp9,tmp10,tmp11,tmp12,tmp13,tmp14;
	unsigned char araA[10],araB[10];
	unsigned char temporary1,temporary2,temporary3,temporary4,temporary5,temporary6;
	
	MCUCSR = (1<<JTD);
	MCUCSR = (1<<JTD);
	
	Lcd4_Init();
	Lcd4_Clear();
	
	PORTA=0x18;
	PORTB=0xFF;
	PORTC&=0b11111100;
	_delay_ms(5000);
	
	int16_t COUNTA = 0;
	char SHOWA [16];
	

	servo_init();
	OCR1B = ICR1 - 600; //18000
	OCR1A=ICR1-600;
	_delay_ms(1000);

	while (1)
	{
		//Initial condition///////////////////////////////////////////////////////////////// 0 degree
		OCR1B=ICR1-600;
		OCR1A=ICR1-600;
		_delay_ms(1000);
		
		INT_init();
		sei();
		PORTA=0x18;
		//PORTB=~PORTA;
		PORTC &=0b11111100;
		for(int dd=0;dd<100;dd++)
		//while(1)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Initial position");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTB= 0b11011111;
				
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTB= 0b10111111;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTB= 0b01111111;
			}
			
			tmp1=PORTB;
			
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			//_delay_us(50);
			
		}
		cli();
		//_delay_ms(1000);
		
		//45 degree left rotation from initial/////////////////////////////////////////////////////////////
		servo_init();
		OCR1B=ICR1-1050;
		_delay_ms(1000);
		INT_init();
		sei();
		PORTA=0x18;
		PORTB= tmp1;
		PORTC &=0b11111100;
		for(int dd=0;dd<100;dd++)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			PORTA=0x18;
			PORTB= tmp1;
			PORTC &=0b11111100;
			
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("  45 deg left   ");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTA= 0b00000100;
				PORTB= 0b11011111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTA= 0b00000010;
				PORTB= 0b10111111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTA= 0b00000001;
				PORTB= 0b01111111;
				PORTC|=0b00000011;
			}
			
			tmp2=PORTA;
			tmp3=PORTB;
			
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			
			
			PORTA=0x18;
			PORTB=tmp1;
			PORTC &=0b11111100;
		}
		cli();
		_delay_us(50);
		PORTA=tmp2;
		PORTB=tmp3;
		PORTC|=0b00000011;
		_delay_us(50);
		PORTA=0x18;
		PORTB=tmp1;
		PORTC &=0b11111100;
		//_delay_ms(1000);
		
		//90 degree left rotation from initial////////////////////////////////////////////////////////////
		servo_init();
		OCR1B=ICR1-1500;
		_delay_ms(1000);
		INT_init();
		sei();
		PORTA=tmp2;
		PORTB=tmp3;
		PORTC |=0b00000011;
		_delay_ms(5);
		PORTA=0x18;
		PORTB= tmp1;
		PORTC &=0b11111100;
		for(int dd=0;dd<100;dd++)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			PORTA=tmp2;
			PORTB=tmp3;
			PORTC |=0b00000011;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1;
			PORTC &=0b11111100;
			
			Lcd4_Set_Cursor(1,2);
			Lcd4_Write_String("90 deg left   ");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTA= 0b00000100;
				PORTB=0b11100111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTA= 0b00000010;
				PORTB=0b11100111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTA= 0b00000001;
				PORTB=0b11100111;
				PORTC|=0b00000011;
			}
			
			tmp4=PORTA;
			tmp5=PORTB;
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			
			PORTA=tmp2;
			PORTB=tmp3;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=0x18;
			PORTB= tmp1;
			PORTC &=0b11111100;
			
		}
		cli();
		PORTA=tmp4;
		PORTB=tmp5;
		PORTC|=0b00000011;
		_delay_us(50);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB= tmp1;
		PORTC &=0b11111100;
		
		//_delay_ms(1000);
		
		//135 degree left rotation from initial////////////////////////////////////////////////////////////////////
		servo_init();
		OCR1B=ICR1-1950;
		_delay_ms(1000);
		INT_init();
		sei();
		
		PORTA=tmp4;
		PORTB=tmp5;
		PORTC|=0b00000011;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1;
		PORTC &=0b11111100;
		
		for(int dd=0;dd<100;dd++)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			PORTA=tmp4;
			PORTB=tmp5;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1;
			PORTC &=0b11111100;
			
			Lcd4_Set_Cursor(1,2);
			Lcd4_Write_String("135 deg left  ");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTA= 0b00000100;
				PORTB= 0b11111011;
				PORTC|=0b00000011;
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTA= 0b00000010;
				PORTB= 0b11111101;
				PORTC|=0b00000011;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTA= 0b00000001;
				PORTB= 0b11111110;
				PORTC|=0b00000011;
			}
			tmp6=PORTA;
			tmp7=PORTB;
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			
			PORTA=tmp4;
			PORTB=tmp5;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1;
			PORTC &=0b11111100;
			
		}
		cli();
		PORTA=tmp6;
		PORTB=tmp7;
		PORTC|=0b00000011;
		_delay_us(50);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1;
		PORTC&=0b11111100;
		//_delay_ms(1000);
		
		
		//180 degree left  rotation from initial////////////////////////////////////////////////////
		servo_init();
		OCR1B=ICR1-2400;
		
		_delay_ms(1000);
		INT_init();
		sei();
		
		PORTA=tmp6;
		PORTB=tmp7;
		PORTC|=0b00000011;
		_delay_ms(5);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1;
		PORTC &=0b11111100;
		
		for(int dd=0;dd<100;dd++)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			Lcd4_Set_Cursor(1,2);
			Lcd4_Write_String("180 deg left  ");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTB= 0b11111011;
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTB= 0b11111101;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTB= 0b11111110;
			}
			
			tmp8=PORTB;
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			
			PORTA=tmp6;
			PORTB=tmp7;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp4;
			PORTB=tmp5;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1;
			PORTC &=0b11111100;
		}
		cli();
		
		PORTA=tmp6;
		PORTB=tmp7;
		PORTC|=0b00000011;
		_delay_ms(5);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1 & tmp8;
		PORTC &=0b11111100;
		
		//_delay_ms(1000);
		
		//135 degree right rotation from initial//////////////////////////////////////
		servo_init();
		OCR1A=ICR1-1050;
		//OCR1B=ICR1-1950;
		_delay_ms(1000);
		INT_init();
		sei();
		
		PORTA=tmp6;
		PORTB=tmp7;
		PORTC|=0b00000011;
		_delay_ms(5);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1 & tmp8;
		PORTC &=0b11111100;
		
		for(int dd=0;dd<100;dd++)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			PORTA=tmp6;
			PORTB=tmp7;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp4;
			PORTB=tmp5;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1 & tmp8;
			PORTC&=0b11111100;
			
			Lcd4_Set_Cursor(1,2);
			Lcd4_Write_String("225 deg left  ");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			/////////////////////////////////////////////////////////
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTA= 0b00100000;
				PORTB= 0b11111011;
				PORTC|=0b00000011;
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTA= 0b01000000;
				PORTB= 0b11111101;
				PORTC|=0b00000011;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTA= 0b10000000;
				PORTB= 0b11111110;
				PORTC|=0b00000011;
			}
			
			tmp9=PORTA;
			tmp10=PORTB;
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			
			PORTA=tmp6;
			PORTB=tmp7;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp4;
			PORTB=tmp5;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1 & tmp8;
			PORTC&= 0b11111100;
			
		}
		cli();
		PORTA=tmp9;
		PORTB=tmp10;
		PORTC|=0b00000011;
		_delay_us(50);
		PORTA=tmp6;
		PORTB=tmp7;
		_delay_ms(5);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1 & tmp8;
		PORTC &=0b11111100;
		//_delay_ms(1000);
		
		//90 degree right rotation from initial//////////////////////////////////
		servo_init();
		OCR1A=ICR1-1500;
		_delay_ms(1000);
		INT_init();
		sei();
		
		PORTA=tmp9;
		PORTB=tmp10;
		PORTC|=0b00000011;
		_delay_ms(5);
		PORTA=tmp6;
		PORTB=tmp7;
		_delay_ms(5);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1 & tmp8;
		PORTC&=~0b00000011;
		
		for(int dd=0;dd<100;dd++)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			PORTA=tmp9;
			PORTB=tmp10;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp6;
			PORTB=tmp7;
			_delay_ms(5);
			PORTA=tmp4;
			PORTB=tmp5;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1 & tmp8;
			PORTC&=~0b00000011;
			
			Lcd4_Set_Cursor(1,2);
			Lcd4_Write_String("270 deg left  ");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTA= 0b00100000;
				PORTB=0b11100111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTA= 0b01000000;
				PORTB=0b11100111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTA= 0b10000000;
				PORTB=0b11100111;
				PORTC|=0b00000011;
			}
			
			tmp11=PORTA;
			tmp12=PORTB;
			
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			
			PORTA=tmp9;
			PORTB=tmp10;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp6;
			PORTB=tmp7;
			_delay_ms(5);
			PORTA=tmp4;
			PORTB=tmp5;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1 & tmp8;
			PORTC&=~0b00000011;
			
		}
		cli();
		PORTA=tmp11;
		PORTB=tmp12;
		PORTC|=0b00000011;
		_delay_us(50);
		PORTA=tmp9;
		PORTB=tmp10;
		_delay_ms(5);
		PORTA=tmp6;
		PORTB=tmp7;
		_delay_ms(5);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1 & tmp8;
		PORTC&=~0b00000011;
		//_delay_ms(1000);
		
		//45 degree right rotation from initial/////////////////////////////////////////////////////
		servo_init();
		OCR1A=ICR1-1950;
		_delay_ms(1000);
		INT_init();
		sei();
		
		PORTA=tmp11;
		PORTB=tmp12;
		PORTC|=0b00000011;
		_delay_ms(5);
		PORTA=tmp9;
		PORTB=tmp10;
		_delay_ms(5);
		PORTA=tmp6;
		PORTB=tmp7;
		_delay_ms(5);
		PORTA=tmp4;
		PORTB=tmp5;
		_delay_ms(5);
		PORTA=tmp2;
		PORTB=tmp3;
		_delay_ms(5);
		PORTA=0x18;
		PORTB=tmp1 & tmp8;
		PORTC&=~0b00000011;
		
		for(int dd=0;dd<100;dd++)
		{
			trigger_signal();
			COUNTA=pulse/58;
			
			PORTA=tmp11;
			PORTB=tmp12;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp9;
			PORTB=tmp10;
			_delay_ms(5);
			PORTA=tmp6;
			PORTB=tmp7;
			_delay_ms(5);
			PORTA=tmp4;
			PORTB=tmp5;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1 & tmp8;
			PORTC&=~0b00000011;
			
			Lcd4_Set_Cursor(1,2);
			Lcd4_Write_String("315 deg left ");
			Lcd4_Set_Cursor(2,0);
			Lcd4_Write_String ("DISTANCE=");
			
			
			if(COUNTA>=0 && COUNTA<=10)
			{
				PORTA= 0b00100000;
				PORTB= 0b11011111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>10 && COUNTA<=20)
			{
				PORTA= 0b01000000;
				PORTB= 0b10111111;
				PORTC|=0b00000011;
			}
			else if(COUNTA>20 || COUNTA<0)
			{
				PORTA= 0b10000000;
				PORTB= 0b01111111;
				PORTC|=0b00000011;
			}
			
			tmp13=PORTA;
			tmp14=PORTB;
			
			dtostrf(COUNTA, 2, 2, SHOWA);
			strcat(SHOWA,"cm ");
			Lcd4_Set_Cursor(2,9);
			Lcd4_Write_String(SHOWA);
			
			PORTA=tmp11;
			PORTB=tmp12;
			PORTC|=0b00000011;
			_delay_ms(5);
			PORTA=tmp9;
			PORTB=tmp10;
			_delay_ms(5);
			PORTA=tmp6;
			PORTB=tmp7;
			_delay_ms(5);
			PORTA=tmp4;
			PORTB=tmp5;
			_delay_ms(5);
			PORTA=tmp2;
			PORTB=tmp3;
			_delay_ms(5);
			PORTA=0x18;
			PORTB=tmp1 & tmp8;
			PORTC&=~0b00000011;
			
		}
		cli();
		araA[0]=tmp13;araA[1]=tmp11;araA[2]=tmp9;araA[3]=tmp6;araA[4]=tmp4;araA[5]=tmp2;
		araB[0]=tmp14;araB[1]=tmp12;araB[2]=tmp10;araB[3]=tmp7;araB[4]=tmp5;araB[5]=tmp3;
		//for(int kkr=0;kkr<50;kkr++)
		//{
			temporary1=0xFF;
			for(int f=0;f<6;f++)
			{
				if(araA[f]==0b00000001)
				{
					temporary1&=araB[f];
				}
			}
			//PORTA=0x01;
			//PORTB=temporary1;
			//PORTC|=0b00000011;
			//_delay_us(40);
			
			temporary2=0xFF;
			for(int f=0;f<6;f++)
			{
				if(araA[f]==0b00000010)
				{
					temporary2&=araB[f];
				}
			}
			//PORTA=0x02;
			//PORTB=temporary2;
			//_delay_us(50);
			
			temporary3=0xFF;
			for(int f=0;f<6;f++)
			{
				if(araA[f]==0b00000100)
				{
					temporary3&=araB[f];
				}
			}
			//PORTA=0x04;
			//PORTB=temporary3;
			//_delay_us(50);
			
			//PORTA=0x18;
			//PORTB=tmp1 & tmp8;
			//PORTC &=0b11111100;
			//
			//_delay_us(50);
			
			temporary4=0xFF;
			for(int f=0;f<6;f++)
			{
				if(araA[f]==0b00100000)
				{
					temporary4&=araB[f];
				}
			}
			//PORTA=0x20;
			//PORTB=temporary4;
			//PORTC|=0b00000011;
			//
			//_delay_us(50);
			
			temporary5=0xFF;
			for(int f=0;f<6;f++)
			{
				if(araA[f]==0b01000000)
				{
					temporary5&=araB[f];
				}
			}
			//PORTA=0x40;
			//PORTB=temporary5;
			//
			//_delay_us(50);
			temporary6=0xFF;
			for(int f=0;f<6;f++)
			{
				if(araA[f]==0b10000000)
				{
					temporary6&=araB[f];
				}
			}
			//PORTA=0x80;
			//PORTB=temporary6;
			//
			//_delay_us(50);
			
		//}
		unsigned char final[]={temporary1,temporary2,temporary3,tmp1 & tmp8,tmp1 & tmp8,temporary4,temporary5,temporary6};
			
			for(int as=0;as<500;as++)
			{
				drawChar(final);
			}
		
		
		servo_init();
		
	}
}

ISR(INT0_vect)
{
	if (i==1)
	{
		TCCR1B=0;
		pulse=TCNT1;
		TCNT1=0;
		i=0;
	}
	if (i==0)
	{
		TCCR1B|=(1<<CS10);
		i=1;
	}
}