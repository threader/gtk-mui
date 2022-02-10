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
 * $Id: Library.h,v 1.3 2006/01/05 01:11:36 itix Exp $
 *
 *****************************************************************************/

#ifndef	__GTKMUI_LIBRARY_H__
#define	__GTKMUI_LIBRARY_H__

#ifndef	DOS_DOS_H
#include	<dos/dos.h>
#endif

#ifndef	EXEC_LIBRARIES_H
#include	<exec/libraries.h>
#endif

#ifndef	EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

struct TaskNode
{
	struct MinNode  Node;
	struct Task    *Task;
	APTR				GlobalData;
};

struct MyLibrary
{
	struct Library    Library;
	WORD              Alloc;
	BPTR              SegList;
	struct ExecBase  *SysBase;
	struct MyLibrary *Parent;

	union
	{
		struct MinList  TaskList;
		struct TaskNode TaskNode;
	} TaskContext;

	struct SignalSemaphore Semaphore;
};

#endif	/* __GTKMUI_LIBRARY_H__ */
