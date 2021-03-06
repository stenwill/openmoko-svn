/***************************************************************************
 *   Copyright (C) 2005 by Dominic Rath                                    *
 *   Dominic.Rath@gmx.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sys/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/parport.h>
#include <linux/ppdev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

/* parallel port cable description
 */
typedef struct cable_s
{
	char* name;
	u8 TDO_MASK;	/* status port bit containing current TDO value */
	u8 TRST_MASK;	/* data port bit for TRST */
	u8 TMS_MASK;	/* data port bit for TMS */
	u8 TCK_MASK;	/* data port bit for TCK */
	u8 TDI_MASK;	/* data port bit for TDI */
	u8 SRST_MASK;	/* data port bit for SRST */
	u8 OUTPUT_INVERT;	/* data port bits that should be inverted */
	u8 INPUT_INVERT;	/* status port that should be inverted */
	u8 PORT_INIT;	/* initialize data port with this value */
} cable_t;

cable_t cables[] = 
{	
	/* name				tdo   trst  tms   tck   tdi   srst  o_inv i_inv init */
	{ "wiggler",			0x80, 0x10, 0x02, 0x04, 0x08, 0x01, 0x01, 0x80, 0x80 },
	{ "old_amt_wiggler",		0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x11, 0x80, 0x80 },
	{ "chameleon",			0x80, 0x00, 0x04, 0x01, 0x02, 0x00, 0x00, 0x80, 0x00 },
	{ "dlc5",				0x10, 0x00, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x10 },
	{ "triton",				0x80, 0x08, 0x04, 0x01, 0x02, 0x00, 0x00, 0x80, 0x00 },
	{ NULL,					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

/* configuration */
char* parport_cable;
unsigned long parport_port;

/* interface variables
 */
static cable_t* cable;
static u8 dataport_value = 0x0;

static int device_handle;

/* low level command set
 */
int parport_read(void);
void parport_write(int tck, int tms, int tdi);
void parport_reset(int trst, int srst);

int parport_speed(int speed);
int parport_register_commands(struct command_context_s *cmd_ctx);
int parport_init(void);
int parport_quit(void);

/* interface commands */
int parport_handle_parport_port_command(struct command_context_s *cmd_ctx, char *cmd, char **args, int argc);
int parport_handle_parport_cable_command(struct command_context_s *cmd_ctx, char *cmd, char **args, int argc);

jtag_interface_t parport_interface = 
{
	.name = "parport",
	
	.execute_queue = bitbang_execute_queue,

	.support_statemove = 0,

	.speed = parport_speed,	
	.register_commands = parport_register_commands,
	.init = parport_init,
	.quit = parport_quit,
};

bitbang_interface_t parport_bitbang =
{
	.read = parport_read,
	.write = parport_write,
	.reset = parport_reset
};

int parport_read(void)
{
	int data = 0;
	
	ioctl(device_handle, PPRSTATUS, & data);
	if ((data ^ cable->INPUT_INVERT) & cable->TDO_MASK)
		return 1;
	else
		return 0;
}

void parport_write(int tck, int tms, int tdi)
{
	u8 output;
	int i = jtag_speed + 1;
	
	if (tck)
		dataport_value |= cable->TCK_MASK;
	else
		dataport_value &= ~cable->TCK_MASK;
	
	if (tms)
		dataport_value |= cable->TMS_MASK;
	else
		dataport_value &= ~cable->TMS_MASK;
	
	if (tdi)
		dataport_value |= cable->TDI_MASK;
	else
		dataport_value &= ~cable->TDI_MASK;
		
	output = dataport_value ^ cable->OUTPUT_INVERT;

	while (i-- > 0)
		ioctl(device_handle, PPWDATA, &output);
}

/* (1) assert or (0) deassert reset lines */
void parport_reset(int trst, int srst)
{
	u8 output;
	DEBUG("trst: %i, srst: %i", trst, srst);

	if (trst == 0)
		dataport_value |= cable->TRST_MASK;
	else if (trst == 1)
		dataport_value &= ~cable->TRST_MASK;

	if (srst == 0)
		dataport_value |= cable->SRST_MASK;
	else if (srst == 1)
		dataport_value &= ~cable->SRST_MASK;
	
	output = dataport_value ^ cable->OUTPUT_INVERT;
	
	ioctl(device_handle, PPWDATA, &output);
}

int parport_speed(int speed)
{
	jtag_speed = speed;
	
	return ERROR_OK;
}

int parport_register_commands(struct command_context_s *cmd_ctx)
{
	register_command(cmd_ctx, NULL, "parport_port", parport_handle_parport_port_command,
		COMMAND_CONFIG, NULL);
	register_command(cmd_ctx, NULL, "parport_cable", parport_handle_parport_cable_command,
		COMMAND_CONFIG, NULL);

	return ERROR_OK;
}

int parport_init(void)
{
	cable_t *cur_cable;
	char buffer[256];
	int i = 0;
	
	cur_cable = cables;
	
	if ((parport_cable == NULL) || (parport_cable[0] == 0))
	{
		parport_cable = "wiggler";
		WARNING("No parport cable specified, using default 'wiggler'");
	}
	
	while (cur_cable->name)
	{
		if (strcmp(cur_cable->name, parport_cable) == 0)
		{
			cable = cur_cable;
			break;
		}
		cur_cable++;
	}

	if (!cable)
	{
		ERROR("No matching cable found for %s", parport_cable);
		return ERROR_JTAG_INIT_FAILED;
	}
	
	dataport_value = cable->PORT_INIT;
	
	if (device_handle>0)
	{
		ERROR("device is already opened");
		return ERROR_JTAG_INIT_FAILED;
	}

	snprintf(buffer, 256, "/dev/parport%d", parport_port);
	device_handle = open(buffer, O_WRONLY);
	
	if (device_handle<0)
	{
		ERROR("cannot open device. check it exists and that user read and write rights are set");
		return ERROR_JTAG_INIT_FAILED;
	}

	i=ioctl(device_handle, PPCLAIM);
	if (i<0)
	{
		ERROR("cannot claim device");
		return ERROR_JTAG_INIT_FAILED;
	}

	i = PARPORT_MODE_COMPAT;
	i= ioctl(device_handle, PPSETMODE, & i);
	if (i<0)
	{
		ERROR(" cannot set compatible mode to device");
		return ERROR_JTAG_INIT_FAILED;
	}

	i = IEEE1284_MODE_COMPAT;
	i = ioctl(device_handle, PPNEGOT, & i);
	if (i<0)
	{
		ERROR("cannot set compatible 1284 mode to device");
		return ERROR_JTAG_INIT_FAILED;
	}
	
	parport_reset(0, 0);
	parport_write(0, 0, 0);

	bitbang_interface = &parport_bitbang;	

	return ERROR_OK;
}

int parport_quit(void)
{

	return ERROR_OK;
}

int parport_handle_parport_port_command(struct command_context_s *cmd_ctx, char *cmd, char **args, int argc)
{
	if (argc == 0)
		return ERROR_OK;

	/* only if the port wasn't overwritten by cmdline */
	if (parport_port == 0)
		parport_port = strtoul(args[0], NULL, 0);

	return ERROR_OK;
}

int parport_handle_parport_cable_command(struct command_context_s *cmd_ctx, char *cmd, char **args, int argc)
{
	if (argc == 0)
		return ERROR_OK;

	/* only if the cable name wasn't overwritten by cmdline */
	if (parport_cable == 0)
	{
		parport_cable = malloc(strlen(args[0]) + sizeof(char));
		strcpy(parport_cable, args[0]);
	}

	return ERROR_OK;
}
