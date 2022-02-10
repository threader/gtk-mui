/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2006 Oliver Brunner, Ilkka Lehtoranta
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
 * $Id: box.c,v 1.5 2007/10/25 15:23:47 o1i Exp $
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
	LONG homogeneous;
	LONG spacing;
};

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, struct IClass *cl);

/*******************************************
 * mNew
 *******************************************/
STATIC ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
	obj = (APTR)DoSuperMethodA(cl, obj, msg);

	if (obj)
	{
		GETDATA;
		struct TagItem *tag, *tags = ((struct opSet *)msg)->ops_AttrList;

		tag = FindTagItem(MUIA_Group_Horiz, tags);

		if (tag && tag->ti_Data)
			data->spacing = xget(obj, MUIA_Group_HorizSpacing);
		else
			data->spacing = xget(obj, MUIA_Group_VertSpacing);

		mSet(data, obj, (APTR)msg, cl);
	}

	return (ULONG)obj;
}

/*******************************************
 * mSet
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, struct IClass *cl)
{
  struct TagItem *tstate, *tag;

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
		case MA_GtkBox_Homogeneous: data->homogeneous = tag->ti_Data;
			SetSuperAttrs(cl, obj,
				MUIA_Group_SameWidth, tag->ti_Data,
				MUIA_Group_SameHeight, tag->ti_Data,
			TAG_DONE);
			break;

		case MA_GtkBox_Spacing:
			if ((LONG)tag->ti_Data >= 0)
			{
				data->spacing = tag->ti_Data;
				SetSuperAttrs(cl, obj,
					MUIA_Group_VertSpacing, tag->ti_Data,
					MUIA_Group_HorizSpacing, tag->ti_Data,
				TAG_DONE);
			}
			break;
    }
  }
}

/*******************************************
 * mGet
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
	ULONG rc;

	switch (msg->opg_AttrID)
	{
		case MA_GtkBox_Homogeneous: rc = data->homogeneous; break;
		case MA_GtkBox_Spacing    : rc = data->spacing; break;
		default: return DoSuperMethodA(cl, obj, (Msg)msg);
	}

	*msg->opg_Storage = rc;
	return TRUE;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg, cl); break;
    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_box_class(void)
{
  CL_Box = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Box ? 1 : 0;
}

void mgtk_delete_box_class(void)
{
  if (CL_Box)
  {
    MUI_DeleteCustomClass(CL_Box);
    CL_Box=NULL;
  }
}
