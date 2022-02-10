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
 * $Id: Library.c,v 1.4 2006/01/05 01:11:36 itix Exp $
 *
 *****************************************************************************/

#include <stddef.h>

#include <exec/resident.h>
#include <proto/exec.h>

#include "Library.h"

#define COMPILE_VERSION  1
#define COMPILE_REVISION 0
#define COMPILE_DATE     "(5.1.2006)"
#define PROGRAM_VER      "1.0"

#define MGTK_SIZE 1024

/**********************************************************************
	Function prototypes
**********************************************************************/

STATIC struct Library *LIB_Open(void);
STATIC BPTR            LIB_Close(void);
STATIC BPTR            LIB_Expunge(void);
STATIC ULONG           LIB_Reserved(void);
STATIC struct Library *LIB_Init(struct MyLibrary *LibBase, BPTR SegList, struct ExecBase *SysBase);
STATIC APTR            LIB_GetGlobalData(struct MyLibrary *LibBase);

	// These are in gtk_init.c

BOOL mgtk_openlibs(APTR base);
VOID mgtk_closelibs(APTR base);

/**********************************************************************
	Library Header
**********************************************************************/

STATIC const char VerString[]	= "\0$VER: gtkmuippc.library " PROGRAM_VER " "COMPILE_DATE;
STATIC const char LibName[]	= "gtkmuippc.library";

#define PROTO(ret, name) ret name();
#define ENTRY(ret, name) (APTR)&name,

VOID (*DoExit)(int rc);

void exit(int rc)
{
	DoExit(rc);
}

PROTO(void,	gtk_adjustment_changed)
PROTO(APTR,	gtk_adjustment_new)
PROTO(void,	gtk_adjustment_set_value)
PROTO(APTR,	gtk_arrow_new)
PROTO(void,	gtk_box_pack_end)
PROTO(void,	gtk_box_pack_start)
PROTO(APTR,	gtk_button_new)
PROTO(APTR,	gtk_button_new_from_stock)
PROTO(APTR,	gtk_button_new_with_label)
PROTO(APTR,	gtk_check_button_new)
PROTO(APTR,	gtk_check_button_new_with_label)
PROTO(APTR,	gtk_check_button_new_with_mnemonic)
PROTO(void,	gtk_container_add)
PROTO(void,	gtk_container_set_border_width)
PROTO(APTR,	gtk_editable_get_chars)
PROTO(LONG, gtk_editable_get_editable)
PROTO(void,	gtk_editable_insert_text)
PROTO(void,	gtk_editable_select_region)
PROTO(void,	gtk_editable_set_editable)
PROTO(APTR,	gtk_entry_get_text)
PROTO(APTR,	gtk_entry_new)
PROTO(void,	gtk_entry_set_max_length)
PROTO(void,	gtk_entry_set_text)
PROTO(void,	gtk_entry_set_visibility)
PROTO(APTR,	gtk_file_selection_get_filename)
PROTO(APTR,	gtk_file_selection_new)
PROTO(void,	gtk_file_selection_set_filename)
PROTO(VOID, gtk_fixed_move)
PROTO(APTR, gtk_fixed_new)
PROTO(VOID, gtk_fixed_put)
PROTO(APTR, gtk_frame_new)
PROTO(void, gtk_frame_set_label)
PROTO(void, gtk_frame_set_label_align)
PROTO(void, gtk_frame_set_shadow_type)
PROTO(void, gtk_grab_add)
PROTO(APTR, gtk_grab_get_current)
PROTO(void, gtk_grab_remove)
PROTO(APTR, gtk_handle_box_new)
PROTO(APTR,	gtk_hbox_new)
PROTO(APTR,	gtk_hscale_new)
PROTO(APTR,	gtk_hscrollbar_new)
PROTO(APTR,	gtk_hseparator_new)
PROTO(APTR,	gtk_image_new)
PROTO(APTR,	gtk_image_new_from_file)
PROTO(void,	gtk_init)
PROTO(APTR,	gtk_label_new)
PROTO(void, gtk_label_set_text)
PROTO(void,	gtk_main)
PROTO(void,	gtk_main_quit)
PROTO(APTR,	gtk_menu_bar_new)
PROTO(APTR,	gtk_menu_get_attach_widget)
PROTO(APTR,	gtk_menu_item_new)
PROTO(APTR,	gtk_menu_item_new_with_label)
PROTO(void,	gtk_menu_item_set_submenu)
PROTO(APTR,	gtk_menu_new)
PROTO(void,	gtk_menu_shell_append)
PROTO(LONG, gtk_notebook_append_page)
PROTO(APTR, gtk_notebook_new)
PROTO(void, gtk_notebook_next_page)
PROTO(LONG, gtk_notebook_prepend_page)
PROTO(void, gtk_notebook_prev_page)
PROTO(APTR,	gtk_option_menu_new)
PROTO(APTR,	gtk_option_menu_set_menu)
PROTO(void, gtk_paint_arrow)
PROTO(void, gtk_paint_box)
PROTO(void, gtk_paint_check)
PROTO(void, gtk_paint_extension)
PROTO(void, gtk_paint_flat_box)
PROTO(void, gtk_paint_focus)
PROTO(void, gtk_paint_hline)
PROTO(void, gtk_paint_option)
PROTO(void, gtk_paint_polygon)
PROTO(void, gtk_paint_shadow)
PROTO(void, gtk_paint_slider)
PROTO(void, gtk_paint_vline)
PROTO(DOUBLE, gtk_progress_bar_get_fraction)
PROTO(LONG, gtk_progress_bar_get_orientation)
PROTO(APTR, gtk_progress_bar_get_text)
PROTO(APTR, gtk_progress_bar_new)
PROTO(APTR, gtk_progress_bar_pulse)
PROTO(void, gtk_progress_bar_set_fraction)
PROTO(void, gtk_progress_bar_set_orientation)
PROTO(void, gtk_progress_bar_set_text)
PROTO(APTR,	gtk_radio_button_get_group)
PROTO(APTR,	gtk_radio_button_group)
PROTO(APTR,	gtk_radio_button_new_with_label)
PROTO(APTR,	gtk_radio_button_new_with_label_from_widget)
PROTO(void,	gtk_range_set_update_policy)
PROTO(APTR, gtk_rc_get_style_by_paths)
PROTO(void,	gtk_scale_set_digits)
PROTO(void,	gtk_scale_set_draw_value)
PROTO(void,	gtk_scale_set_value_pos)
PROTO(DOUBLE, gtk_spin_button_get_value)
PROTO(DOUBLE, gtk_spin_button_get_value_as_float)
PROTO(LONG, gtk_spin_button_get_value_as_int)
PROTO(APTR, gtk_spin_button_new)
PROTO(void, gtk_spin_button_set_digits)
PROTO(void, gtk_spin_button_set_numeric)
PROTO(void, gtk_spin_button_set_wrap)
PROTO(APTR, gtk_style_attach)
PROTO(APTR, gtk_style_new)
PROTO(VOID, gtk_style_set_background)
PROTO(void,	gtk_table_attach)
PROTO(void,	gtk_table_attach_defaults)
PROTO(APTR,	gtk_table_new)
PROTO(void,	gtk_table_set_col_spacings)
PROTO(void,	gtk_table_set_row_spacings)
PROTO(LONG, gtk_timeout_add)
PROTO(VOID, gtk_timeout_remove)
PROTO(LONG,	gtk_toggle_button_get_active)
PROTO(APTR, gtk_toggle_button_new)
PROTO(APTR, gtk_toggle_button_new_with_label)
PROTO(APTR, gtk_toggle_button_new_with_mnemonic)
PROTO(void,	gtk_toggle_button_set_active)
PROTO(void,	gtk_tooltips_disable)
PROTO(void,	gtk_tooltips_enable)
PROTO(void,	gtk_tooltips_force_window)
PROTO(APTR,	gtk_tooltips_new)
PROTO(void,	gtk_tooltips_set_tip)
PROTO(APTR, gtk_type_class)
PROTO(APTR,	gtk_vbox_new)
PROTO(LONG, gtk_vbox_get_type)
PROTO(APTR,	gtk_vscale_new)
PROTO(void,	gtk_vscrollbar_new)
PROTO(void,	gtk_widget_destroy)
PROTO(APTR, gtk_widget_get_colormap)
PROTO(LONG, gtk_widget_get_events)
PROTO(LONG, gtk_widget_get_type)
PROTO(APTR, gtk_widget_get_visual)
PROTO(void,	gtk_widget_grab_default)
PROTO(void,	gtk_widget_hide)
PROTO(void,	gtk_widget_hide_all)
PROTO(void, gtk_widget_queue_draw)
PROTO(void, gtk_widget_realize)
PROTO(APTR,	gtk_widget_ref)
PROTO(void, gtk_widget_set_rc_style)
PROTO(void, gtk_widget_set_style)
PROTO(void,	gtk_widget_set_sensitive)
PROTO(void,	gtk_widget_set_size_request)
PROTO(void,	gtk_widget_set_state)
PROTO(void,	gtk_widget_set_usize)
PROTO(void,	gtk_widget_show)
PROTO(void,	gtk_widget_show_all)
PROTO(void, gtk_widget_style_get)
PROTO(void,	gtk_widget_unref)
PROTO(LONG,	gtk_window_get_resizable)
PROTO(APTR,	gtk_window_new)
PROTO(void,	gtk_window_set_title)
PROTO(void,	gtk_window_set_resizable)

	/* set exit() pointer */

VOID mgtk_init(int x, APTR y, APTR z)
{
	DoExit = z;
	gtk_init(x, y);
}

	/* These functions dont exist currently */

APTR gtk_check_button_new(void)
{
	return gtk_check_button_new_with_label(NULL);
}
APTR gtk_check_button_new_with_mnemonic(APTR x)
{
	return gtk_check_button_new_with_label(x);
}
LONG gtk_editable_get_editable(APTR x)
{
	return 0;
}
VOID gtk_frame_set_label(APTR x, APTR y)
{
}
VOID gtk_frame_set_label_align(void)
{
}
APTR gtk_toggle_button_new_with_mnemonic( void ) { return NULL; }
APTR gtk_toggle_button_new_with_label(APTR label) { return NULL; }

int printf(const char *fmt, ...) { return 0; }
void puts(const char *str) { }

STATIC const APTR FuncTable[] =
{
	(APTR)   FUNCARRAY_BEGIN,

	// Start m68k block for system functions.

	(APTR)   FUNCARRAY_32BIT_NATIVE, 
	(APTR)   LIB_Open,
	(APTR)   LIB_Close,
	(APTR)   LIB_Expunge,
	(APTR)   LIB_Reserved,
	(APTR)   -1,

	// Start sysv block for user functions.

	(APTR)   FUNCARRAY_32BIT_SYSTEMV,
	(APTR)	LIB_GetGlobalData,

ENTRY(void,	gtk_adjustment_changed)
ENTRY(APTR,	gtk_adjustment_new)
ENTRY(void,	gtk_adjustment_set_value)
ENTRY(APTR,	gtk_arrow_new)
ENTRY(void,	gtk_box_pack_end)
ENTRY(void,	gtk_box_pack_start)
ENTRY(APTR,	gtk_button_new)
ENTRY(APTR,	gtk_button_new_from_stock)
ENTRY(APTR,	gtk_button_new_with_label)
ENTRY(APTR,	gtk_check_button_new)
ENTRY(APTR,	gtk_check_button_new_with_label)
ENTRY(APTR,	gtk_check_button_new_with_mnemonic)
ENTRY(void,	gtk_container_add)
ENTRY(void,	gtk_container_set_border_width)
ENTRY(APTR,	gtk_editable_get_chars)
ENTRY(LONG, gtk_editable_get_editable)
ENTRY(void,	gtk_editable_insert_text)
ENTRY(void,	gtk_editable_select_region)
ENTRY(void,	gtk_editable_set_editable)
ENTRY(APTR,	gtk_entry_get_text)
ENTRY(APTR,	gtk_entry_new)
ENTRY(void,	gtk_entry_set_max_length)
ENTRY(void,	gtk_entry_set_text)
ENTRY(void,	gtk_entry_set_visibility)
ENTRY(APTR,	gtk_file_selection_get_filename)
ENTRY(APTR,	gtk_file_selection_new)
ENTRY(void,	gtk_file_selection_set_filename)
ENTRY(VOID, gtk_fixed_move)
ENTRY(APTR, gtk_fixed_new)
ENTRY(VOID, gtk_fixed_put)
ENTRY(APTR, gtk_frame_new)
ENTRY(void, gtk_frame_set_label)
ENTRY(void, gtk_frame_set_label_align)
ENTRY(void, gtk_frame_set_shadow_type)
ENTRY(void, gtk_grab_add)
ENTRY(APTR, gtk_grab_get_current)
ENTRY(void, gtk_grab_remove)
ENTRY(APTR, gtk_handle_box_new)
ENTRY(APTR,	gtk_hbox_new)
ENTRY(APTR,	gtk_hscale_new)
ENTRY(APTR,	gtk_hscrollbar_new)
ENTRY(APTR,	gtk_hseparator_new)
ENTRY(APTR,	gtk_image_new)
ENTRY(APTR,	gtk_image_new_from_file)
ENTRY(void,	mgtk_init)
ENTRY(APTR,	gtk_label_new)
ENTRY(void, gtk_label_set_text)
ENTRY(void,	gtk_main)
ENTRY(void,	gtk_main_quit)
ENTRY(APTR,	gtk_menu_bar_new)
ENTRY(APTR,	gtk_menu_get_attach_widget)
ENTRY(APTR,	gtk_menu_item_new)
ENTRY(APTR,	gtk_menu_item_new_with_label)
ENTRY(void,	gtk_menu_item_set_submenu)
ENTRY(APTR,	gtk_menu_new)
ENTRY(void,	gtk_menu_shell_append)
ENTRY(LONG, gtk_notebook_append_page)
ENTRY(APTR, gtk_notebook_new)
ENTRY(void, gtk_notebook_next_page)
ENTRY(LONG, gtk_notebook_prepend_page)
ENTRY(void, gtk_notebook_prev_page)
ENTRY(APTR,	gtk_option_menu_new)
ENTRY(APTR,	gtk_option_menu_set_menu)
ENTRY(void, gtk_paint_arrow)
ENTRY(void, gtk_paint_box)
ENTRY(void, gtk_paint_check)
ENTRY(void, gtk_paint_extension)
ENTRY(void, gtk_paint_flat_box)
ENTRY(void, gtk_paint_focus)
ENTRY(void, gtk_paint_hline)
ENTRY(void, gtk_paint_option)
ENTRY(void, gtk_paint_polygon)
ENTRY(void, gtk_paint_shadow)
ENTRY(void, gtk_paint_slider)
ENTRY(void, gtk_paint_vline)
ENTRY(DOUBLE, gtk_progress_bar_get_fraction)
ENTRY(LONG, gtk_progress_bar_get_orientation)
ENTRY(APTR, gtk_progress_bar_get_text)
ENTRY(APTR, gtk_progress_bar_new)
ENTRY(APTR, gtk_progress_bar_pulse)
ENTRY(void, gtk_progress_bar_set_fraction)
ENTRY(void, gtk_progress_bar_set_orientation)
ENTRY(void, gtk_progress_bar_set_text)
ENTRY(APTR,	gtk_radio_button_get_group)
ENTRY(APTR,	gtk_radio_button_group)
ENTRY(APTR,	gtk_radio_button_new_with_label)
ENTRY(APTR,	gtk_radio_button_new_with_label_from_widget)
ENTRY(void,	gtk_range_set_update_policy)
ENTRY(APTR, gtk_rc_get_style_by_paths)
ENTRY(void,	gtk_scale_set_digits)
ENTRY(void,	gtk_scale_set_draw_value)
ENTRY(void,	gtk_scale_set_value_pos)
ENTRY(DOUBLE, gtk_spin_button_get_value)
ENTRY(DOUBLE, gtk_spin_button_get_value_as_float)
ENTRY(LONG, gtk_spin_button_get_value_as_int)
ENTRY(APTR, gtk_spin_button_new)
ENTRY(void, gtk_spin_button_set_digits)
ENTRY(void, gtk_spin_button_set_numeric)
ENTRY(void, gtk_spin_button_set_wrap)
ENTRY(APTR, gtk_style_attach)
ENTRY(APTR, gtk_style_new)
ENTRY(VOID, gtk_style_set_background)
ENTRY(void,	gtk_table_attach)
ENTRY(void,	gtk_table_attach_defaults)
ENTRY(APTR,	gtk_table_new)
ENTRY(void,	gtk_table_set_col_spacings)
ENTRY(void,	gtk_table_set_row_spacings)
ENTRY(LONG, gtk_timeout_add)
ENTRY(VOID, gtk_timeout_remove)
ENTRY(LONG,	gtk_toggle_button_get_active)
ENTRY(APTR, gtk_toggle_button_new)
ENTRY(APTR, gtk_toggle_button_new_with_label)
ENTRY(APTR, gtk_toggle_button_new_with_mnemonic)
ENTRY(void,	gtk_toggle_button_set_active)
ENTRY(void,	gtk_tooltips_disable)
ENTRY(void,	gtk_tooltips_enable)
ENTRY(void,	gtk_tooltips_force_window)
ENTRY(APTR,	gtk_tooltips_new)
ENTRY(void,	gtk_tooltips_set_tip)
ENTRY(APTR, gtk_type_class)
ENTRY(APTR,	gtk_vbox_new)
ENTRY(LONG, gtk_vbox_get_type)
ENTRY(APTR,	gtk_vscale_new)
ENTRY(void,	gtk_vscrollbar_new)
ENTRY(void,	gtk_widget_destroy)
ENTRY(APTR, gtk_widget_get_colormap)
ENTRY(LONG, gtk_widget_get_events)
ENTRY(LONG, gtk_widget_get_type)
ENTRY(APTR, gtk_widget_get_visual)
ENTRY(void,	gtk_widget_grab_default)
ENTRY(void,	gtk_widget_hide)
ENTRY(void,	gtk_widget_hide_all)
ENTRY(void, gtk_widget_queue_draw)
ENTRY(void, gtk_widget_realize)
ENTRY(APTR,	gtk_widget_ref)
ENTRY(void, gtk_widget_set_rc_style)
ENTRY(void, gtk_widget_set_style)
ENTRY(void,	gtk_widget_set_sensitive)
ENTRY(void,	gtk_widget_set_size_request)
ENTRY(void,	gtk_widget_set_state)
ENTRY(void,	gtk_widget_set_usize)
ENTRY(void,	gtk_widget_show)
ENTRY(void,	gtk_widget_show_all)
ENTRY(void, gtk_widget_style_get)
ENTRY(void,	gtk_widget_unref)
ENTRY(LONG,	gtk_window_get_resizable)
ENTRY(APTR,	gtk_window_new)
ENTRY(void,	gtk_window_set_title)
ENTRY(void,	gtk_window_set_resizable)

	(APTR)   -1,

	(APTR)   FUNCARRAY_END
};

STATIC const ULONG InitTable[] =
{
	sizeof(struct MyLibrary),
	(ULONG)	FuncTable,
	(ULONG)	NULL,
	(ULONG)	LIB_Init
};

CONST struct Resident RomTag	=
{
	RTC_MATCHWORD,
	(struct Resident *)&RomTag,
	(struct Resident *)&RomTag+1,
	RTF_AUTOINIT | RTF_PPC | RTF_EXTENDED,
	COMPILE_VERSION,
	NT_LIBRARY,
	0,
	(char *)&LibName[0],
	(char *)&VerString[7],
	(APTR)&InitTable[0],
	COMPILE_REVISION, NULL
};

/**********************************************************************
	Globals
**********************************************************************/

CONST ULONG __abox__	= 1;

/**********************************************************************
	LIB_Reserved
**********************************************************************/

STATIC ULONG LIB_Reserved(void)
{
	return 0;
}

/**********************************************************************
	LIB_Init
**********************************************************************/

struct Library *GTKMUIPPCBase;

STATIC struct Library *LIB_Init(struct MyLibrary *LibBase, BPTR SegList, struct ExecBase *SysBase)
{
	GTKMUIPPCBase = &LibBase->Library;

	LibBase->Library.lib_Node.ln_Type = NT_LIBRARY;
	LibBase->Library.lib_Node.ln_Pri  = -5;
	LibBase->Library.lib_Node.ln_Name = (char *)LibName;
	LibBase->Library.lib_Flags    = LIBF_SUMUSED|LIBF_CHANGED;
	LibBase->Library.lib_Version  = COMPILE_VERSION;
	LibBase->Library.lib_Revision = COMPILE_REVISION;
	LibBase->Library.lib_IdString = (char *)&VerString[7];

	LibBase->SegList   = SegList;
	LibBase->Parent    = NULL;
	LibBase->SysBase   = SysBase;

	NEWLIST(&LibBase->TaskContext.TaskList);
	InitSemaphore(&LibBase->Semaphore);

	return &LibBase->Library;
}

/**********************************************************************
	RemoveLibrary
**********************************************************************/

STATIC VOID RemoveLibrary(struct MyLibrary *LibBase)
{
	struct ExecBase *SysBase = LibBase->SysBase;
	REMOVE(&LibBase->Library.lib_Node);
	FreeMem((APTR)((ULONG)(LibBase) - (ULONG)(LibBase->Library.lib_NegSize)), LibBase->Library.lib_NegSize + LibBase->Library.lib_PosSize);
}

/**********************************************************************
	LIB_Expunge
 ********************************************************************/

STATIC BPTR LIB_Expunge(void)
{
	struct MyLibrary *LibBase = (struct MyLibrary *)REG_A6;

	if (LibBase->Library.lib_OpenCnt == 0)
	{
		BPTR seglist = LibBase->SegList;

		RemoveLibrary(LibBase);
		return seglist;
	}

	LibBase->Library.lib_Flags |= LIBF_DELEXP;
	return 0;
}

/**********************************************************************
	LIB_Close
**********************************************************************/

STATIC BPTR LIB_Close(void)
{
	struct MyLibrary *LibBase;
	struct ExecBase *SysBase;
	BPTR SegList;

	LibBase = (struct MyLibrary *)REG_A6;
	SysBase = LibBase->SysBase;
	SegList = 0;

	if (LibBase->Parent)
	{
		struct MyLibrary *ChildBase = LibBase;

		if ((--ChildBase->Library.lib_OpenCnt) > 0)
			return NULL;

		LibBase	= ChildBase->Parent;

		REMOVE(&ChildBase->TaskContext.TaskNode.Node);
		FreeTaskPooled(ChildBase->TaskContext.TaskNode.GlobalData, MGTK_SIZE);
		FreeVecTaskPooled((APTR)((ULONG)(ChildBase) - (ULONG)(ChildBase->Library.lib_NegSize)));
	}

	ObtainSemaphore(&LibBase->Semaphore);

	LibBase->Library.lib_OpenCnt--;

	if (LibBase->Library.lib_OpenCnt == 0)
	{
		LibBase->Alloc = 0;
		mgtk_closelibs(LibBase);
	}

	ReleaseSemaphore(&LibBase->Semaphore);

	if (LibBase->Library.lib_Flags & LIBF_DELEXP)
	{
		REG_A6 = (ULONG)LibBase;
		SegList = LIB_Expunge();
	}

	return SegList;
}

/**********************************************************************
	LIB_Open
**********************************************************************/

STATIC struct Library *LIB_Open(void)
{
	struct MyLibrary *LibBase, *newbase, *childbase;
	struct ExecBase	*SysBase;
	struct Task *MyTask;
	struct TaskNode *ChildNode;
	ULONG MyBaseSize;

	LibBase = (struct MyLibrary *)REG_A6;
	SysBase = LibBase->SysBase;
	MyTask  = FindTask(NULL);

	/* Has this task already opened a child? */
	ForeachNode(&LibBase->TaskContext.TaskList, ChildNode)
	{
		if (ChildNode->Task == MyTask)
		{
			/* Yep, return it */
			childbase = (APTR)(((ULONG)ChildNode) - offsetof(struct MyLibrary, TaskContext.TaskNode.Node));
			childbase->Library.lib_Flags &= ~LIBF_DELEXP;
			childbase->Library.lib_OpenCnt++;

			return(&childbase->Library);
		}
	}

	childbase  = NULL;
	MyBaseSize = LibBase->Library.lib_NegSize + LibBase->Library.lib_PosSize;

	LibBase->Library.lib_Flags &= ~LIBF_DELEXP;
	LibBase->Library.lib_OpenCnt++;

	ObtainSemaphore(&LibBase->Semaphore);

	if (LibBase->Alloc == 0)
	{
		if (mgtk_openlibs(LibBase))
		{
			LibBase->Alloc = 1;
		}
		else
		{
			goto error;
		}
	}

	if ((newbase = AllocVecTaskPooled(MyBaseSize)) != NULL)
	{
		CopyMem((APTR)((ULONG)LibBase - (ULONG)LibBase->Library.lib_NegSize), newbase, MyBaseSize);
		childbase = (APTR)((ULONG)newbase + (ULONG)LibBase->Library.lib_NegSize);
		childbase->Parent = LibBase;
		childbase->Library.lib_OpenCnt = 1;

		childbase->TaskContext.TaskNode.GlobalData = AllocTaskPooled(MGTK_SIZE);

		if (childbase->TaskContext.TaskNode.GlobalData)
		{
			memset(childbase->TaskContext.TaskNode.GlobalData, 0, MGTK_SIZE);

			/* Register which task opened this child */
			childbase->TaskContext.TaskNode.Task = MyTask;
			ADDTAIL(&LibBase->TaskContext.TaskList, &childbase->TaskContext.TaskNode.Node);
		}
		else
		{
			FreeVecTaskPooled(newbase);
			childbase = NULL;
			goto error;
		}
	}
	else
	{
error:
		LibBase->Library.lib_OpenCnt--;

		if (LibBase->Library.lib_OpenCnt == 0)
		{
			LibBase->Alloc	= 0;
			mgtk_closelibs(LibBase);
		}
	}

	ReleaseSemaphore(&LibBase->Semaphore);

	return (struct Library *)childbase;

}

/**********************************************************************
	LIB_GetGlobalData
**********************************************************************/

STATIC APTR LIB_GetGlobalData(struct MyLibrary *LibBase)
{
	APTR data = LibBase->TaskContext.TaskNode.GlobalData;

	if (!LibBase->Parent)
	{
		struct TaskNode *ChildNode;
		struct ExecBase *SysBase;
		struct Task *MyTask;

		SysBase = LibBase->SysBase;
		MyTask  = FindTask(NULL);

		/* Has this task already opened a child? */
		ForeachNode(&LibBase->TaskContext.TaskList, ChildNode)
		{
			if (ChildNode->Task == MyTask)
			{
				data = ChildNode->GlobalData;
				break;
			}
		}
	}

	return data;
}
