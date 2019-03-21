/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GPL License. See LICENSE for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "nmseffect.h"

int main(void)
{
	int termCols, spaces = 0;
	unsigned char *display_uc = NULL;
	char *display = NULL;
	char *head1Left = "DATANET PROC RECORD:  45-3456-W-3452";
	char *head1Right = "Transnet on/xc-3";
	char *head2Center = "FEDERAL RESERVE TRANSFER NODE";
	char *head3Center = "National Headquarters";
	char *head4Center = "************  Remote Systems Network Input Station  ************";
	char *head5Center = "================================================================";
	char *menu1 = "[1] Interbank Funds Transfer  (Code Prog: 485-GWU)";
	char *menu2 = "[2] International Telelink Access  (Code Lim: XRP-262)";
	char *menu3 = "[3] Remote Facsimile Send/Receive  (Code Tran:  2LZP-517)";
	char *menu4 = "[4] Regional Bank Interconnect  (Security Code:  47-B34)";
	char *menu5 = "[5] Update System Parameters  (Entry Auth. Req.)";
	char *menu6 = "[6] Remote Operator Logon/Logoff";
	char *foot1Center = "================================================================";
	char *foot2Center = "[ ] Select Option or ESC to Abort";

	// Get terminal dimentions (needed for centering)
	struct winsize w;
	// if not an interactive tty, w is not populated, resulting in UB
	if (ioctl(0, TIOCGWINSZ, &w) == -1)
	{
		perror("Input not from an interactive terminal");
		return 1;
	}
	termCols = w.ws_col;

	// Allocate space for our display string
	if ((display = malloc(20 * termCols)) == NULL)
	{
		fprintf(stderr, "Memory Allocation Error. Quitting!\n");
		return 1;
	}

	// Allocate space for our display string
	if ((display_uc = malloc(20 * termCols)) == NULL)
	{
		free(display);
		fprintf(stderr, "Memory Allocation Error. Quitting!\n");
		return 1;
	}

	memset(display, 0, 20 * termCols);
	memset(display_uc, 0, 20 * termCols);

	// Start building the display string
	strcpy(display, head1Left);

	spaces = termCols - strlen(head1Left) - strlen(head1Right);
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, head1Right);

	strcat(display, "\n");

	spaces = (termCols - strlen(head2Center)) / 2;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, head2Center);

	strcat(display, "\n");
	strcat(display, "\n");

	spaces = (termCols - strlen(head3Center)) / 2;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, head3Center);

	strcat(display, "\n");
	strcat(display, "\n");

	spaces = (termCols - strlen(head4Center)) / 2;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, head4Center);

	strcat(display, "\n");

	spaces = (termCols - strlen(head5Center)) / 2;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, head5Center);

	strcat(display, "\n");
	strcat(display, "\n");

	spaces = ((termCols - strlen(head5Center)) / 2) + 3;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, menu1);

	strcat(display, "\n");

	spaces = ((termCols - strlen(head5Center)) / 2) + 3;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, menu2);

	strcat(display, "\n");

	spaces = ((termCols - strlen(head5Center)) / 2) + 3;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, menu3);

	strcat(display, "\n");

	spaces = ((termCols - strlen(head5Center)) / 2) + 3;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, menu4);

	strcat(display, "\n");

	spaces = ((termCols - strlen(head5Center)) / 2) + 3;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, menu5);

	strcat(display, "\n");

	spaces = ((termCols - strlen(head5Center)) / 2) + 3;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, menu6);

	strcat(display, "\n");
	strcat(display, "\n");

	spaces = (termCols - strlen(foot1Center)) / 2;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, foot1Center);

	strcat(display, "\n");
	strcat(display, "\n");

	spaces = (termCols - strlen(foot2Center)) / 2;
	while (spaces > 0)
	{
		strcat(display, " ");
		--spaces;
	}
	strcat(display, foot2Center);

	nmseffect_set_clearscr(1);

	memcpy(display_uc, display, 20 * termCols);

	// Execute effect
	nmseffect_exec(display_uc, strlen(display));

	free(display);
	free(display_uc);

	return 0;
}
