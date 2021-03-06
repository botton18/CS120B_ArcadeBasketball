
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
void displaytime(unsigned long period, unsigned long length);
void TurnMotor();


void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

unsigned char Check()
{
	if(ADC > 260)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



void displaytime(unsigned long period, unsigned long length)
{
	unsigned long convert;

	if(period % 100 == 0)
	{
		convert = period / 100;
		convert = length - convert;

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

int num = (90/5.625) * 64;
int counter = 0;
int bcounter = 0;
int isCW = 1;

void TurnMotor()
{
	
	
	
	if((counter < num) && isCW)
	{
		CW();
		++counter;
	}
	else if((bcounter < num) && !isCW)
	{
		CCW();
		++bcounter;
	}
	else if(!isCW)
	{
		isCW = 1;
		counter = 0;
	}
	else if(isCW)
	{
		isCW = 0;
		bcounter = 0;
	}

}


unsigned char R[] = {0x01,0x03,0x02,0x06,0x04,0x0C,0x08,0x09};
unsigned char C[] = {0x09,0x08,0x0C,0x04,0x06,0x02,0x03,0x01};
int i = 0;
int j = 0;
unsigned char forward = 1;
enum states{CInit, Move}state;
enum back{_Init,_Move}bstate;
void CW()
{
	switch(state)
	{
		case CInit:
		state = Move;
		break;
		case Move:
		state = Move;
		break;
	}
	switch(state)
	{
		case CInit:
		break;
		case Move:
		if(i < 8)
		{
			PORTB = R[i];
			++i;
		}
		else
		{
			i = 0;
		}
		break;
		
	}
}

void CCW()
{
	switch(bstate)
	{
		case _Init:
		bstate = _Move;
		break;
		case _Move:
		bstate = _Move;
		break;
	}
	switch(bstate)
	{
		case _Init:
		break;
		case _Move:
		if(j < 8)
		{
			PORTB = C[j];
			++j;
		}
		else
		{
			j = 0;
		}
		break;
		
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
			if(Check())
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
			if(Check())
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
	DDRB = 0xFF;
	PORTB = 0x00;
	ADC_init();

	// Initializes the LCD display
	LCD_init();

	unsigned char limit[] = {6,10};

	unsigned long cnt1 = 0;
	unsigned long Level1Period = 0;
	unsigned long intermission = 0;
	
	unsigned char lvl = 0;

	unsigned char motor = 0;

	TimerSet(10);
	TimerOn();
    while(1) 
    {
		button = ~PINA & 0x01;
		//20 sec = 20000
	
		if(Level1Period < 2000)
		{
			if(motor)
			{
				TurnMotor();
			}
			
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
			else
			{
				ScoreCount = 2;
			}
			displaytime(Level1Period,20);
		}
		else
		{
			if(lvl == 3 || score < limit[lvl])
			{
				LCD_DisplayString(1,"Game Over!");
				LCD_ClearScreen();
			}
			else if(score >= limit[lvl])
			{
				if(intermission < 500)
				{
					displaytime(intermission,5);
					++intermission;
				}
				else
				{
					lvl++; //increment score limit
					motor = 1; //activate motor
					Level1Period = 0; //rest timer to continue loop
					intermission = 0;
					LCD_ClearScreen();
				}
			}
		}

		//else add intermission and set variables to lvl 2



		while(!TimerFlag);
		TimerFlag = 0;
    }
}
