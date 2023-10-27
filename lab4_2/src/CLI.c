/**
 * @file CLI.c
 *
 * @brief Function Definitions for Command Line Interface (CLI) Module.
 *
 * This file includes the implementation of functions that correspond to the prototypes defined
 * in the `../headers/CLI.h` header file. The CLI module facilitates communication with a host terminal
 * and enables interactive command execution and feedback.
 *
 * @note Functions in this file correspond to the CLI module in `../headers/CLI.h`.
 *
 * @author Mubashir Hussain
 * @studentID 200396797
 */

#include "../headers/CLI.h"
#include "../headers/USART.h"
#include "../headers/util.h"
#include <string.h>
#include "stm32f10x.h"
#include <stdio.h>

uint16_t counter = 0;
uint16_t counter1 = 0;


void CLI_Transmit(uint8_t *pData, uint16_t Size)
{
	for(int i = 0	; i < Size; i++)
	{
		sendByte(pData[i]);
	}
}

void CLI_Receive(uint8_t *pData, int* id)
{
		pData[*id] = getByte();
	
		switch(pData[*id])
		{
			case BACKSPACE:
				if(*id == 0) 
				{
					*id = *id - 1;
				}
				else 
				{
					*id = *id - 2;
					sendByte(BACKSPACE);
					sendByte(' ');
					sendByte(BACKSPACE);
				}
			break;
	
			case CARRIAGE_RETURN:
				if(parseReceivedData(pData, *id) != 1)
				{
					newPromptLine();
					*id = -1;
				}
			
			break;
				
			case SPACE:
				sendByte(SPACE);
				*id = -1;
				
				break;
			default:
				sendByte(pData[*id]);
			break;
		}
	*id = *id + 1;
}

int parseReceivedData(uint8_t *pData, int Size)
{
	sendByte(NEW_LINE_FEED);
	sendByte(CARRIAGE_RETURN);
	
	if(strncmp((char*)pData, "ledon\r", 6) == 0)
	{
		uint8_t buffer[] = "On-board led is ON";
		CLI_Transmit(buffer, sizeof(buffer));
		onboardLEDconfig(1);		
	} 
	else if(strncmp((char*)pData, "ledoff\r", 7) == 0)
	{
		uint8_t buffer[] = "On-board led is OFF";
		CLI_Transmit(buffer, sizeof(buffer));
		onboardLEDconfig(0);
	}
	else if(strncmp((char*)pData, "ledstate\r", 8) == 0)
	{
		if(GPIOA->IDR & NUC_GREEN_ON)
		{
			uint8_t buffer[] = "led is on";
			CLI_Transmit(buffer, sizeof(buffer));
		}
		else 
		{
			uint8_t buffer[] = "led is off";
			CLI_Transmit(buffer, sizeof(buffer));
		}
	}
	else if(strncmp((char*)pData, "help\r", 5) == 0)
	{
		uint8_t buffer[] = "Help command Currently the commands available are 'ledon', 'ledoff', 'ledstate'. If a command is typed incorrectly an error prompt of 'invalid command' will show up. If the wrong command is typed, backspacing is available";
		CLI_Transmit(buffer, sizeof(buffer));
	}else if(strncmp((char*)pData, "clear\r", 6) == 0)
	{
		clearTerminal();
		
		return 1;
	}
	else
	{
		uint8_t buffer[] = "Unknown command:";
		CLI_Transmit(buffer, sizeof(buffer));
		CLI_Transmit(pData, Size);
	}
}

void sendPromptArrows(void)
{
	sendByte('>');
	sendByte('>');
}

void newPromptLine(void)
{
	sendByte(NEW_LINE_FEED);
	sendByte(CARRIAGE_RETURN);
	sendPromptArrows();
}
void placeCursor(int row, int col)
{
	uint8_t bigbuff[20];
	
	sprintf((char*)bigbuff, "\x1b[%d;%dH", row, col);
	CLI_Transmit(bigbuff, strlen((char*)(bigbuff)));
}

void clearTerminal(void)
{
	uint8_t buffer[] = "\x1b[2J";
	CLI_Transmit(buffer, sizeof(buffer));
}

void prepareTerminal(void)
{
	clearTerminal();
	
	uint8_t set_scroll_row[] = "\x1b[8;r";
	CLI_Transmit(set_scroll_row, sizeof(set_scroll_row));
		
	placeCursor(8,0);
	sendPromptArrows();
}

void updateStatusWindow(void)
{
		counter++;
		counter1++;
	
	// save the current position of the cursor
		uint8_t cur_pos[20];
		sprintf((char*)cur_pos, "\x1b[s");
		CLI_Transmit(cur_pos, strlen((char*)(cur_pos)));
	
		uint8_t clearbuffer[20]= "                    ";
			
			//first data
			placeCursor(1,0);
			CLI_Transmit(clearbuffer, 20);
			placeCursor(1,0);
			uint8_t bigbuff[20];
			sprintf((char*)bigbuff, "counter:%d", counter);
			CLI_Transmit(bigbuff, strlen((char*)(bigbuff)));
	
			//second data
			placeCursor(2,0);
			CLI_Transmit(clearbuffer, 20);
			placeCursor(2,0);
			uint8_t bigbuff1[20];
			sprintf((char*)bigbuff1, "counter2:%d", counter1);
			CLI_Transmit(bigbuff1, strlen((char*)(bigbuff1)));
			
	// return to the original address
		uint8_t cur_pos2[20];
		sprintf((char*)cur_pos2, "\x1b[u");
		CLI_Transmit(cur_pos2, strlen((char*)(cur_pos2)));
}