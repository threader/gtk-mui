/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
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
 * Oliver Brunner
 *
 * E-Mail: gtk-mui "at" oliver-brunner.de
 *
 * $Id: messagedialog.c,v 1.4 2007/10/25 15:23:48 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data
{

};

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;
DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);


	switch (msg->MethodID)
	{
	}

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_messagedialog_class(void)
{
  CL_MessageDialog = MUI_CreateCustomClass(NULL, NULL, CL_Dialog, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_MessageDialog ? 1 : 0;
}

void mgtk_delete_messagedialog_class(void)
{
  if (CL_MessageDialog)
  {
    MUI_DeleteCustomClass(CL_MessageDialog);
    CL_MessageDialog=NULL;
  }
}
