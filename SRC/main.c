/*
 * main.cpp.c
 *
 * Created: 6/2/2017 8:23:19 PM
 * Author : Dillon
 */ 

#include <avr/io.h>
#include <timer.h>
#include <bit.h>
#include "io.c"


void display(unsigned char Data, unsigned char column);
void displaytime(unsigned long period);


void displaytime(unsigned long period)
{
	unsigned long convert;

	if(period % 100 == 0)
	{
		convert = period / 100;
		convert = 20 - convert;

		if(convert > 9)
		{
			display(convert,17);
		}
		else
		{
			LCD_DisplayString(18," ");
			LCD_Cursor(17);
			LCD_WriteData(convert + '0');
			
		}
	}
}

void display(unsigned char Data, unsigned char column)
{
	unsigned char ones;
	unsigned char tenth;
	unsigned char newC = column + 1;

	if(Data > 9)
	{
		ones = Data % 10;
		tenth = (Data - ones)/10;
		LCD_Cursor(column);
		LCD_WriteData(tenth + '0');
		LCD_Cursor(newC);
		LCD_WriteData(ones + '0');

	}
	else
	{
		LCD_Cursor(column);
		LCD_WriteData(Data + '0');
	}
	
}

unsigned char button;
unsigned char score;
unsigned char ScoreCount = 2;
enum State1{Init, Idle, AddScore, Wait} state1;

void Level1()
{
	switch(state1)
	{
		case Init:
			state1 = Idle;
			break;

		case Idle:
			if(button)
			{
				state1 = AddScore;
			}
			else
			{
				state1 = Idle;
			}
			break;

		case AddScore:
			state1 = Wait;
			break;

		case Wait:
			if(button)
			{
				state1 = Wait;
			}
			else
			{
				state1 = Idle;
			}
			break;

		default:
			state1 = Init;
			break;
	}
	switch(state1)
	{
		case Init:
			score = 0;
			break;

		case Idle:
			//print score
			display(score, 1);
			break;

		case AddScore:
			score += ScoreCount;
			display(score, 1);
			//print score
			//add sound 
			break;

		case Wait:
			break;
	}
}


int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00;
	PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();

	unsigned long cnt1 = 0;
	unsigned long Level1Period = 0;
	unsigned long Level2Period = 0;
	unsigned long Level3Period = 0;
	
	unsigned char lvl1 = 1;
	unsigned char lvl2,lvl3 = 0;

	TimerSet(10);
	TimerOn();
    while(1) 
    {
		button = ~PINA & 0x01;
		//20 sec = 20000
		if(Level1Period < 2000 && lvl1)
		{
			cnt1++;
			if(cnt1 > 1)
			{
				Level1();
				cnt1 = 0;
			}
			Level1Period++;
			if(Level1Period >= 1000)
			{
				ScoreCount = 3;
			}
			displaytime(Level1Period);
		}
		else
		{
			if(score >= 6)
			{
				//5 second intermission function()
				lvl1 = 0;
				lvl2 = 1;
			}
			else
			{
				LCD_DisplayString(1,"Game Over!");
				LCD_ClearScreen();
			}
		}

		//else add intermission and set variables to lvl 2



		while(!TimerFlag);
		TimerFlag = 0;
    }
}

