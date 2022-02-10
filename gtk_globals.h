/*****************************************************************************
 *
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
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
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id: gtk_globals.h,v 1.28 2011/07/04 12:16:20 o1i Exp $
 *
 *****************************************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H 1
/* globals */

#include <gtk/gtk.h>
#define GETGLOBAL struct mgtk_ObjApp *mgtk = global_mgtk;
extern struct mgtk_ObjApp *global_mgtk;
extern struct Remember *rememberKey;

extern struct MUI_CustomClass *CL_Application;
extern struct MUI_CustomClass *CL_RootGroup;
extern struct MUI_CustomClass *CL_Group;
extern struct MUI_CustomClass *CL_HBox;
extern struct MUI_CustomClass *CL_VBox;
extern struct MUI_CustomClass *CL_Table;
extern struct MUI_CustomClass *CL_Custom;
extern struct MUI_CustomClass *CL_SpinButton;
extern struct MUI_CustomClass *CL_NoteBook;
extern struct MUI_CustomClass *CL_ProgressBar;
extern struct MUI_CustomClass *CL_Timer;
extern struct MUI_CustomClass *CL_Fixed;
extern struct MUI_CustomClass *CL_CheckButton;
extern struct MUI_CustomClass *CL_ToggleButton;
extern struct MUI_CustomClass *CL_RadioButton;
extern struct MUI_CustomClass *CL_Button;
extern struct MUI_CustomClass *CL_Menu;
extern struct MUI_CustomClass *CL_Entry;
extern struct MUI_CustomClass *CL_Toolbar;
extern struct MUI_CustomClass *CL_List;
extern struct MUI_CustomClass *CL_CList;
extern struct MUI_CustomClass *CL_Combo;
extern struct MUI_CustomClass *CL_Scale;
extern struct MUI_CustomClass *CL_Label;
extern struct MUI_CustomClass *CL_Frame;
extern struct MUI_CustomClass *CL_Dialog;
extern struct MUI_CustomClass *CL_Window;
extern struct MUI_CustomClass *CL_Scrolledwindow;

extern int gtk_do_main_quit;
extern int gtk_no_own_libs;

#ifndef __AROS__
extern struct StackSwapStruct stackswap;
extern APTR   mynewstack;
#endif

#endif
