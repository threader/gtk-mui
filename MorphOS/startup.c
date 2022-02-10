/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2006 Ilkka Lehtoranta
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Ilkka Lehtoranta
 *
 * E-Mail: ilkleht "at" isoveli.org
 *
 * $Id: startup.c,v 1.2 2006/01/05 01:11:36 itix Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <proto/dos.h>

#include "Library.h"
#include "../gtk_globals.h"

struct CTDT
{
	int	(*fp)(void);
	long	priority;
};

struct HunkSegment
{
	unsigned int Size;
	struct HunkSegment *Next;
};

// change the libnix pool size by definig _MSTEP var

int                ThisRequiresConstructorHandling;
int                __nocommandline;
char              *__commandline;
unsigned long      __commandlen;
int                __argc;
const char       **__argv;
struct WBStartup  *_WBenchMsg;
const char        *_ProgramName = "";
BPTR               __curdir;

APTR               libnix_mempool = NULL;

extern const unsigned long	__ctrslist[];
extern const unsigned long	__dtrslist[];
extern const struct CTDT	__ctdtlist[];
STATIC struct CTDT *sort_ctdt(struct CTDT **last);
STATIC struct CTDT *ctdt, *last_ctdt;

/**********************************************************************
	libnix_start
**********************************************************************/

ULONG libnix_start(struct MyLibrary *LibBase)
{
	ctdt = sort_ctdt(&last_ctdt);

	while (ctdt < last_ctdt)
	{
		if (ctdt->priority >= 0)
		{
			if(ctdt->fp() != 0)
			{
				return 0;
			}
		}
		ctdt++;
	}

	malloc(0);

	return 1;
}

/**********************************************************************
	libnix_finish
**********************************************************************/

void libnix_finish(APTR base)
{
	ctdt = (struct CTDT *)__ctdtlist;
	while (ctdt < last_ctdt)
	{
		if (ctdt->priority < 0)
		{
			if(ctdt->fp != (int (*)(void)) -1)
			{
				ctdt->fp();
			}
		}
		ctdt++;
	}
}

/**********************************************************************
	Constructors
**********************************************************************/

STATIC int comp_ctdt(struct CTDT *a, struct CTDT *b)
{
	if (a->priority == b->priority)
		return (0);
	if ((unsigned long)a->priority < (unsigned long) b->priority)
		return (-1);

	return (1);
}

STATIC struct CTDT *sort_ctdt(struct CTDT **last)
{
	struct HunkSegment *seg;
	struct CTDT *last_ctdt;

	seg = (struct HunkSegment *)(((unsigned int)__ctdtlist) - sizeof(struct HunkSegment));
	last_ctdt = (struct CTDT *)(((unsigned int)seg) + seg->Size);

	qsort((struct CTDT *)__ctdtlist, last_ctdt - __ctdtlist, sizeof(*__ctdtlist), (int (*)(const void *, const void *))comp_ctdt);

	*last = last_ctdt;

	return ((struct CTDT *) __ctdtlist);
}

/*
 * .pctors,.pdtors instead of .init,.fini
 * as the later are std *CODE* sections.
 */

asm("\n\
	.section \".ctdt\",\"a\",@progbits\n\
__ctdtlist:\n\
	.long -1,-1\n\
	");
