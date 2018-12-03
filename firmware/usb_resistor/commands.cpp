#include "commands.h"
#include "command.h"

#define SerialUSB Serial

extern int32_t dataEnabled;

#define I2C_ADDR_R1 (0x2E)
#define I2C_ADDR_R2 (0x2C)
#define I2C_ADDR_R3 (0x2F)
#define I2C_ADDR_R4 (0x2E) //Second I2C bus
#define I2C_ADDR_R5 (0x2C) //Seconds I2C bus

bool sendCmdAD5272Bus0(uint8_t i2cAddr, uint8_t cmd, uint16_t value);
bool readCmdAD5272Bus0(uint8_t i2cAddr, uint8_t cmd, uint16_t *ptrvalue);
bool sendCmdAD5272Bus1(uint8_t i2cAddr, uint8_t cmd, uint16_t value);
bool readCmdAD5272Bus1(uint8_t i2cAddr, uint8_t cmd, uint16_t *ptrvalue);

#define COMMANDS_PROMPT (":>")
sCmdUart UsbUart;

static int isPowerOfTwo (unsigned int x)
{
	while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
		x /= 2;
	return (x == 1);
}


CMD_STR(help,"Displays this message");
CMD_STR(ohms,"set/gets Ohms, example 'ohms R1 50000' to read 'ohms R1' ");

//List of supported commands
sCommand Cmds[] =
{
		COMMAND(help),
		COMMAND(ohms),

		{"",0,""}, //End of list signal
};

// print out the help strings for the commands
static int ohms_cmd(sCmdUart *ptrUart,int argc, char * argv[])
{
	sCommand cmd_list;
	int pot;
	int32_t ohms;
	uint16_t value;

	if (argc >= 1)
	{
		pot=(argv[0][1]-'0');
		if ((argv[0][0] != 'r' && argv[0][0] != 'R') || pot<=0 || pot>5)
		{
			CommandPrintf(ptrUart,"First Argument should be 'Rx' where x is 1-5\n\r");
			return 1;
		}
	}
	if (argc >= 2)
	{
		ohms=atol(argv[1]);
		if (ohms<=0 || ohms>=100000)
		{
			CommandPrintf(ptrUart,"First Argument should be 'Rx' where x is 1-5\n\r");
			return 1;
		}
		if (pot == 1)
		{
			sendCmdAD5272Bus0(I2C_ADDR_R1, 0x01, (ohms*1023)/100000);
		}
		if (pot == 2)
		{
			sendCmdAD5272Bus0(I2C_ADDR_R2, 0x01, (ohms*1023)/100000);
		}
		if (pot == 3)
		{
			sendCmdAD5272Bus0(I2C_ADDR_R3, 0x01, (ohms*1023)/100000);
		}
		if (pot == 4)
		{
			sendCmdAD5272Bus1(I2C_ADDR_R4, 0x01, (ohms*1023)/100000);
		}
		if (pot == 5)
		{
			sendCmdAD5272Bus1(I2C_ADDR_R5, 0x01, (ohms*1023)/100000);
		}

	}
	if (pot == 1)
	{
		readCmdAD5272Bus0(I2C_ADDR_R1, 0x02, &value);
		ohms=(value*100000)/1023;
		CommandPrintf(ptrUart,"R1 is %ld ohms\n\r",ohms);
	}

	if (pot == 2)
	{
		readCmdAD5272Bus0(I2C_ADDR_R2, 0x02, &value);
		ohms=(value*100000)/1023;
		CommandPrintf(ptrUart,"R2 is %ld ohms\n\r",ohms);
	}

	if (pot == 3)
	{
		readCmdAD5272Bus0(I2C_ADDR_R3, 0x02, &value);
		ohms=(value*100000)/1023;
		CommandPrintf(ptrUart,"R3 is %ld ohms\n\r",ohms);
	}

	if (pot == 4)
	{
		readCmdAD5272Bus1(I2C_ADDR_R4, 0x02, &value);
		ohms=(value*100000)/1023;
		CommandPrintf(ptrUart,"R4 is %ld ohms\n\r",ohms);
	}

	if (pot == 5)
	{
		readCmdAD5272Bus1(I2C_ADDR_R5, 0x02, &value);
		ohms=(value*100000)/1023;
		CommandPrintf(ptrUart,"R5 is %ld ohms\n\r",ohms);
	}

	return 0;
}


// print out the help strings for the commands
static int help_cmd(sCmdUart *ptrUart,int argc, char * argv[])
{
	sCommand cmd_list;
	int i;

	//now let's parse the command
	i=0;
	memcpy(&cmd_list, &Cmds[i], sizeof(sCommand));
	while(cmd_list.function!=0)
	{

		CommandPrintf(ptrUart,(cmd_list.name));
		CommandPrintf(ptrUart,(" - "));
		CommandPrintf(ptrUart,(cmd_list.help));
		CommandPrintf(ptrUart,("\n\r"));
		i=i+1;
		memcpy(&cmd_list, &Cmds[i], sizeof(sCommand));
	}
	return 0;
}





uint8_t kbhit(void)
{
	return SerialUSB.available();
	//return SerialUSB.peek() != -1;
}
uint8_t getChar(void)
{
	return SerialUSB.read();
}
uint8_t putch(char data)
{
	return SerialUSB.write((uint8_t)data);
}




void commandsInit(void)
{
	CommandInit(&UsbUart, kbhit, getChar, putch ,NULL); //set up the UART structure

	SerialUSB.print(COMMANDS_PROMPT);
}

int commandsProcess(void)
{
	return CommandProcess(&UsbUart,Cmds,' ',COMMANDS_PROMPT);
}
