/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005, 2006 Oliver Brunner
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
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id: gtk_globals.c,v 1.31 2011/07/04 12:16:20 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>
/* cross platform hooks */
#ifdef __AROS__
#include "include/SDI_hook.h"
#else
#include <SDI_hook.h>
#endif

#include <gtk/gtk.h>
#include "debug.h"

/* globals */
#ifndef __AMIGAOS4__
//struct GfxBase *GfxBase;
//struct IntuitionBase *IntuitionBase;
//struct Library *UtilityBase;
//struct DosLibrary *DOSBase;
//struct Library *GadToolsBase ; /* o1i: do we need it ? */
#else
struct GadToolsIFace *IGadTools;
struct MUIMasterIFace *IMUIMaster;
#endif
//struct Library  *MUIMasterBase;

//#ifndef __MORPHOS__
struct mgtk_ObjApp *global_mgtk;
struct Remember *rememberKey = NULL;
//#endif

struct MUI_CustomClass *CL_Application;
struct MUI_CustomClass *CL_Window;
struct MUI_CustomClass *CL_RootGroup;
struct MUI_CustomClass *CL_Box;
struct MUI_CustomClass *CL_HBox;
struct MUI_CustomClass *CL_VBox;
struct MUI_CustomClass *CL_Group;
struct MUI_CustomClass *CL_Custom;
struct MUI_CustomClass *CL_Table;
struct MUI_CustomClass *CL_SpinButton;
struct MUI_CustomClass *CL_NoteBook;
struct MUI_CustomClass *CL_ProgressBar;
struct MUI_CustomClass *CL_Fixed;
struct MUI_CustomClass *CL_CheckButton;
struct MUI_CustomClass *CL_ToggleButton;
struct MUI_CustomClass *CL_RadioButton;
struct MUI_CustomClass *CL_Button;
struct MUI_CustomClass *CL_Menu;
struct MUI_CustomClass *CL_Entry;
struct MUI_CustomClass *CL_Toolbar;
struct MUI_CustomClass *CL_Dialog;
struct MUI_CustomClass *CL_MessageDialog;
struct MUI_CustomClass *CL_Label;
struct MUI_CustomClass *CL_List;
struct MUI_CustomClass *CL_CList;
struct MUI_CustomClass *CL_Combo;
struct MUI_CustomClass *CL_Scale;
struct MUI_CustomClass *CL_Frame;
struct MUI_CustomClass *CL_Scrolledwindow;

int gtk_do_main_quit=0;
int gtk_no_own_libs=0;

#ifndef __AROS__
struct StackSwapStruct stackswap;
APTR   mynewstack;
#endif
