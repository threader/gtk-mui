/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
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
 * $Id: rootgroup.c,v 1.15 2009/05/17 18:37:48 stefankl Exp $
 *
 *****************************************************************************/

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk_globals.h"

//#define DEBUG_MINMAX
struct Data
{
	LONG is_resizable;
	LONG defwidth, defheight;
};

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if (obj)
  {
    GETDATA;

    data->defwidth = -1;
    data->defheight = -1;
    data->is_resizable = -1;
  }

  return (ULONG)obj;
}

static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;
  
  switch (msg->opg_AttrID) {
    case MA_RootGroup_Resizable: rc = data->is_resizable; break;
    default                    : return DoSuperMethodA(cl, obj, (Msg)msg);
  }
  
  *msg->opg_Storage = rc;
  return TRUE;
}

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg)
{
  struct TagItem *tstate, *tag;
  ULONG relayout = 0;

	DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_RootGroup_Resizable:
        if (!(data->is_resizable & tag->ti_Data))
        {
          relayout = 1;
          data->is_resizable = tag->ti_Data;
        }
        break;

      case MA_DefWidth:
        if (data->defwidth != tag->ti_Data)
        {
          relayout = 1;
          data->defwidth = tag->ti_Data;
        }
        break;

      case MA_DefHeight:
        if (data->defheight != tag->ti_Data)
        {
          relayout = 1;
          data->defheight = tag->ti_Data;
        }
        break;
    }
  }

  if (relayout)
  {
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);
  }
}

static ULONG mAskMinMax(struct Data *data, APTR obj, struct MUIP_AskMinMax *msg, struct IClass *cl)
{
 struct MUI_MinMax *mm;
//	  DebOut("rootgroup mAskMinMax(%lx,%lx,%lx,%lx)\n",data,obj,msg,cl);

  DoSuperMethodA(cl, obj, msg);

  mm = msg->MinMaxInfo;

#ifdef DEBUG_MINMAX
	DebOut("\nrootgroup mAskMinMax after Super(%lx,%lx,%lx,%lx)\n",data,obj,msg,cl);
  DebOut("  mm->MinWidth: %d\n",mm->MinWidth);
  DebOut("  mm->MinHeight: %d\n",mm->MinHeight);
  DebOut("  mm->DefWidth: %d\n",mm->DefWidth);
  DebOut("  mm->DefHeight: %d\n",mm->DefHeight);
  DebOut("  mm->MaxWidth: %d\n",mm->MaxWidth);
  DebOut("  mm->MaxHeight: %d\n",mm->MaxHeight);
#endif

  if (!data->is_resizable || data->defwidth >=0 || data->defheight >= 0) {

    struct Window *window = _window(obj);
    LONG defwidth, defheight;
    LONG bl,br,bt,bb;

    /* during window_show, _window(obj) is NULL,
       on AROS accessing window->BorderLeft crashes
       with a core dump
    */
    DebOut("  window: %lx\n",window);

    if(window) {
      bl=window->BorderLeft;
      br=window->BorderRight;
      bt=window->BorderTop;
      bb=window->BorderBottom;
    }
    else {
      /* ? */
      bl=0;
      br=0;
      bt=0;
      bb=0;
    }

    if (data->defwidth < 0) {
      defwidth = mm->DefWidth;
    }
    else if (data->defwidth == 0) {
      defwidth = mm->MinWidth;
    }
    else {
      defwidth = data->defwidth - bl - br;

      if (defwidth < mm->MinWidth) {
        defwidth = mm->MinWidth;
      }
      else if (defwidth > mm->MaxWidth) {
        defwidth = mm->MaxWidth;
      }
    }

    if (data->defheight < 0)
      defheight = mm->DefHeight;
    else if (data->defheight == 0)
      defheight = mm->MinHeight;
    else
    {
      defheight = data->defheight - bt - bb;

      if (defheight < mm->MinHeight)
        defheight = mm->MinHeight;
      else if (defheight > mm->MaxHeight)
        defheight = mm->MaxHeight;
    }

    if (!data->is_resizable)
    {
      mm->MinWidth = mm->DefWidth = mm->MaxWidth = defwidth;
      mm->MinHeight = mm->DefHeight = mm->MaxHeight = defheight;
    }
    else
    {
      mm->DefWidth = defwidth;
      mm->DefHeight = defheight;
    }
#ifdef DEBUG_MINMAX
    DebOut("rootgroup mAskMinMax result:");
    DebOut("  mm->MinWidth: %d\n",mm->MinWidth);
    DebOut("  mm->MinHeight: %d\n",mm->MinHeight);
    DebOut("  mm->DefWidth: %d\n",mm->DefWidth);
    DebOut("  mm->DefHeight: %d\n",mm->DefHeight);
    DebOut("  mm->MaxWidth: %d\n",mm->MaxWidth);
    DebOut("  mm->MaxHeight: %d\n",mm->MaxHeight);
#endif
  }
  else {
    DebOut("  just returning 0\n");
  }

  return 0;
}

static ULONG mSetup(struct IClass *cl, APTR obj, Msg msg)
{
  ULONG rc = DoSuperMethodA(cl, obj, msg);
  
#if 0
  /* I am not sure, if we still need that..? -o1i */
  if (rc) {
    /* Actually, every root group should have its own style setting shared with children.
       In MUI it is possible have windows sitting on different screen inside same
       application context. So widgets should request their style from root object... -itix
    */
    mgtk_update_default_style(obj);
  }
#endif
    
  return rc;
}

BEGINMTABLE
GETDATA;

DebOut("(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);


  switch (msg->MethodID)
  {
    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);
    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case MUIM_AskMinMax : return mAskMinMax (data, obj, (APTR)msg, cl);
    case MUIM_Setup     : return mSetup     (cl, obj, (APTR)msg);
  }

ENDMTABLE

int mgtk_create_rootgroup_class(void)
{
  CL_RootGroup = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_RootGroup ? 1 : 0;
}

void mgtk_delete_rootgroup_class(void)
{
  if (CL_RootGroup)
  {
    MUI_DeleteCustomClass(CL_RootGroup);
    CL_RootGroup=NULL;
  }
}
