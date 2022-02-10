/*****************************************************************************
 * 
 * gtk-mui - a wrapper library to wrap GTK+ calls to MUI
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
 * E-Mail: gtk-mui "at" oliver-brunner.de
 *
 * $Id: classes.h,v 1.39 2013/03/22 10:09:30 o1i Exp $
 *
 *****************************************************************************/

#ifndef GTKMUI_CLASSES_H
#define GTKMUI_CLASSES_H

#ifndef SDI_HOOK_H
#ifndef __AROS__ /* AROS misses SDI_hook.h ..? */
#define MGTK_HOOK_OBJECT_POINTER Object *
#define MGTK_HOOK_APTR APTR
#define MGTK_USERFUNC_INIT
#define MGTK_USERFUNC_EXIT
#include <SDI_hook.h>
#else
#include "mgtk_aros.h"
#endif
#endif

#ifndef MUII_ImageButtonBack
#define MUII_ImageButtonBack MUII_ButtonBack
#endif

#include "gtk.h"

/* Some great VaporWare ideas.
**
*/

#define GETDATA struct Data *data = (struct Data *)INST_DATA(cl, obj)
#define DOSUPER DoSuperMethodA(cl, obj, (Msg)msg);

/**********************************************************************
  Some macros
**********************************************************************/

#if 0
#define	MGTK_NEWLIST(MyList)	\
do { \
      struct MinList *_MyList = (struct MinList *)(MyList); \
      _MyList->mlh_TailPred = (struct MinNode *)_MyList; \
      _MyList->mlh_Tail = (struct MinNode *)NULL; \
      _MyList->mlh_Head = (struct MinNode *)&_MyList->mlh_Tail; \
} while (0)
#endif
#ifdef __AMIGAOS4__
  /* does this work ? */
  #define MGTK_NEWLIST(MyList) NEWMINLIST(MyList)
#else /* !OS4 */
  #ifndef NEWLIST
    #define NEWLIST(MyList) \
      do { \
        struct MinList *_MyList = (struct MinList *)(MyList); \
        _MyList->mlh_TailPred = (struct MinNode *)_MyList; \
        _MyList->mlh_Tail = (struct MinNode *)NULL; \
        _MyList->mlh_Head = (struct MinNode *)&_MyList->mlh_Tail; \
      } while (0)
  #endif /* !NEWLIST */
  #ifndef MGTK_NEWLIST
    #define MGTK_NEWLIST(MyList) NEWLIST(&MyList)
  #endif
#endif /* OS4 */

#ifndef ADDTAIL
#define	ADDTAIL(MyList,MyNode) \
do { \
	struct MinList *_MyList = (struct MinList *)(MyList); \
	struct MinNode *_MyNode = (struct MinNode *)(MyNode); \
	struct MinNode *OldPredNode; \
	OldPredNode = _MyList->mlh_TailPred; \
	_MyNode->mln_Succ = (struct MinNode *)&_MyList->mlh_Tail; \
	_MyNode->mln_Pred = OldPredNode; \
	OldPredNode->mln_Succ = _MyNode; \
	_MyList->mlh_TailPred = _MyNode; \
} while (0)
#endif /* !ADDTAIL */

#ifndef REMOVE
#define	REMOVE(MyNode) \
({ \
	struct MinNode *_MyNode = (struct MinNode *)(MyNode); \
	struct MinNode *PredNode; \
	struct MinNode *SuccNode; \
	PredNode = _MyNode->mln_Pred; \
	SuccNode = _MyNode->mln_Succ; \
	PredNode->mln_Succ = SuccNode; \
	SuccNode->mln_Pred = PredNode; \
	_MyNode; \
})
#endif /* !REMOVE */

#ifndef ForeachNode
#define ForeachNode(l,n)  \
for (  \
	n = (void *)(((struct List *)(l))->lh_Head);  \
	((struct Node *)(n))->ln_Succ;  \
	n = (void *)(((struct Node *)(n))->ln_Succ)  \
)
#endif

#ifdef __MORPHOS__
/*************** MorphOS ******************/

  #define DISPATCHERARG struct IClass *cl = (struct IClass *)REG_A0; APTR obj = (APTR)REG_A2; Msg msg = (Msg)REG_A1;
  #define BEGINMTABLE static ULONG mDispatcherPPC(void); static struct EmulLibEntry mDispatcher = { TRAP_LIB, 0, (void (*)())&mDispatcherPPC }; static ULONG mDispatcherPPC(void) { DISPATCHERARG
  #define SETUPHOOK(x, func, data) { struct Hook *h = (x); h->h_Entry = (HOOKFUNC)&HookEntry; h->h_SubEntry = (HOOKFUNC)&func; h->h_Data = (APTR)data; }

#else 

#ifdef __AROS__
/*************** AROS ********************/

  #define BEGINMTABLE AROS_UFH3(static ULONG,mDispatcher,AROS_UFHA(struct IClass*, cl, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(Msg, msg, a1))
  #define SETUPHOOK(x, func, data) { struct Hook *h = (x); h->h_Entry = (HOOKFUNC)&func; h->h_Data = (APTR)data; }

#else
/************* AmigaOS 3/4 ***************/
  #define BEGINMTABLE static ULONG mDispatcher(REG(a0, struct IClass *cl), REG(a2, APTR obj), REG(a1, Msg msg)) {

/************* AmigaOS 4 ***************/
#ifdef __AMIGAOS4__
  #define SETUPHOOK(x, func, data) { struct Hook *h = (x); h->h_Entry = (HOOKFUNC)func; h->h_Data = (APTR)data; }
#else 
/************* AmigaOS 3 ***************/

#endif 
#endif
#endif

#define ENDMTABLE return DoSuperMethodA(cl, obj, msg); }


/**********************************************************************
	Classes
**********************************************************************/

int  mgtk_create_application_class(void);
void mgtk_delete_application_class(void);
int  mgtk_create_box_class(void);
void mgtk_delete_box_class(void);
int  mgtk_create_dialog_class(void);
void mgtk_delete_dialog_class(void);
int  mgtk_create_messagedialog_class(void);
void mgtk_delete_messagedialog_class(void);
int  mgtk_create_window_class(void);
void mgtk_delete_window_class(void);
int  mgtk_create_rootgroup_class(void);
void mgtk_delete_rootgroup_class(void);
int  mgtk_create_group_class(void);
void mgtk_delete_group_class(void);
int  mgtk_create_hbox_class(void);
void mgtk_delete_hbox_class(void);
int  mgtk_create_vbox_class(void);
void mgtk_delete_vbox_class(void);
int  mgtk_create_table_class(void);
void mgtk_delete_table_class(void);
int  mgtk_create_spinbutton_class(void);
void mgtk_delete_spinbutton_class(void);
int  mgtk_create_notebook_class(void);
void mgtk_delete_notebook_class(void);
int  mgtk_create_progressbar_class(void);
void mgtk_delete_progressbar_class(void);
int  mgtk_create_fixed_class(void);
void mgtk_delete_fixed_class(void);
int  mgtk_create_checkbutton_class(void);
void mgtk_delete_checkbutton_class(void);
int  mgtk_create_togglebutton_class(void);
void mgtk_delete_togglebutton_class(void);
int  mgtk_create_radiobutton_class(void);
void mgtk_delete_radiobutton_class(void);
int  mgtk_create_button_class(void);
void mgtk_delete_button_class(void);
int  mgtk_create_menu_class(void);
void mgtk_delete_menu_class(void);
int  mgtk_create_entry_class(void);
void mgtk_delete_entry_class(void);
int  mgtk_create_toolbar_class(void);
void mgtk_delete_toolbar_class(void);
int  mgtk_create_list_class(void);
void mgtk_delete_list_class(void);
int  mgtk_create_clist_class(void);
void mgtk_delete_clist_class(void);
int  mgtk_create_custom_class(void);
void mgtk_delete_custom_class(void);
int  mgtk_create_label_class(void);
void mgtk_delete_label_class(void);
int  mgtk_create_timer_class(void);
void mgtk_delete_timer_class(void);
int  mgtk_create_combo_class(void);
void mgtk_delete_combo_class(void);
int  mgtk_create_scale_class(void);
void mgtk_delete_scale_class(void);
int  mgtk_create_frame_class(void);
void mgtk_delete_frame_class(void);
int  mgtk_create_window_class(void);
void mgtk_delete_window_class(void);
int  mgtk_create_scrolledwindow_class(void);
void mgtk_delete_scrolledwindow_class(void);


ULONG mgtk_askminmax(struct IClass *cl, APTR obj, struct MUIP_AskMinMax *msg, LONG defwidth, LONG defheight);

/**********************************************************************
	Return IDs
**********************************************************************/

#define MV_Dialog_ReturnID_Done 1

/**********************************************************************
	Tags & methods
**********************************************************************/

#define GTK_TAGBASE 0xfece0000

/* Every MUI object have MUIA_UserData -- we use this as a MA_Widget so
 * we can use it as a global MA_Widget storage without modifying every
 * single class.
 */
/* this idea is great, thanks itix :) */
#define MA_Widget MUIA_UserData

#define AskMin_Limited    0
#define AskMin_Unlimited -1

enum
{
  MA_RootGroup_Resizable = GTK_TAGBASE,

	/* General widget tags */
	MA_GtkWidget_Activate,

  /* These are extensions to standard area class tags */

  MA_DefWidth,
  MA_DefHeight,
  MA_Data,

  /* Group */
  MA_Group_Fill,

  /* spinbutton */
  MA_Spin_Value,
  MA_Spin_String_Value,

  /* notebook */
  MM_NoteBook_AppendChild,
  MM_NoteBook_PrependChild,
  MM_NoteBook_PageNr,
  MM_NoteBook_PageNObject,
  MA_Note_Label,
  /* pre GLIB: MA_Note_Get_Child_Nr,*/
  MA_Note_NrChilds,
  MA_Show_Label,

  /* progressbar */
  MA_Pulse,

  /* fixed */
  MA_Fixed_Move,

  /* checkbutton */
  MA_Check_Label,
  /* radiobutton */
  MA_Radio_Label,

  /* checkbutton */
  /* togglebutton */
  /* radiobutton */
  MA_Toggle_Value,

  /* button */
  MA_Button_Label,

  /* application */
  MM_GtkApplication_AddLoner,
  MM_GtkApplication_AddTimer,
  MM_GtkApplication_RemTimer,
  MM_GtkApplication_TimerEvent,
  MA_GtkApplication_Title,

	/* window */
	MM_GtkWindow_AddChild,          /* we could use OM_ADDMEMBER/OM_REMMEMBER too... but better this way */
	MM_GtkWindow_RemChild,
	MA_GtkWindow_Decorated,
	MA_GtkWindow_DestroyWithParent,
	MA_GtkWindow_Parent,            /* parent window */
	MA_GtkWindow_Icon,
	MA_GtkWindow_Modal,
	MA_GtkWindow_Role,
	MA_GtkWindow_TypeHint,

	/* box */
	MA_GtkBox_Homogeneous,
	MA_GtkBox_Spacing,

	/* dialog */
	MM_GtkDialog_AddWidget,
	MM_GtkDialog_EnableResponse,
	MA_GtkDialog_DefaultResponse,
	MA_GtkDialog_Response,

	/* label */
	MA_GtkLabel_PangoText,

  /* entry */
  MA_Entry_Changed,

  /* list */
  MA_List_List,
  MA_CList_Columns,

  /* combo */
  MM_Combo_Itemlist_new,

  /* scale */
  MM_Scale_Pos,
  MM_Scale_Redraw,

  /* remove me again! */
  /* timer */
  MA_Timer_Intervall,
  MA_Timer_Function,
  MA_Timer_Data,
  MUIM_Class5_Trigger,
  /* end of remove me */

  /* frame */
  MA_GtkFrame_Label,

  /* menu */
  MA_Menu_Is_Item,
  MA_Menu_Attached,

  /* object */
  MA_Class_Get,
};

struct MUIP_NoteBook_AppendChild
{
	ULONG MethodID;
  GtkWidget *Widget;
  GtkLabel  *Label;
};

struct MUIP_NoteBook_PrependChild
{
	ULONG MethodID;
  GtkWidget *Widget;
  GtkLabel  *Label;
};

struct MUIP_GtkApplication_AddLoner
{
  ULONG MethodID;
  APTR  lonely_object;
};

struct MUIP_GtkApplication_AddTimer
{
	ULONG MethodID;
	ULONG interval;
	APTR  function;
	APTR  parameter;
};

struct MUIP_GtkApplication_RemTimer
{
  ULONG MethodID;
  APTR  source;
};

struct MUIP_GtkWindow_AddChild
{
  ULONG MethodID;
  APTR  ChildWindow;
};

struct MUIP_GtkWindow_RemChild
{
  ULONG MethodID;
  APTR  ChildWindow;
};

struct MUIP_GtkDialog_AddWidget
{
	ULONG MethodID;
	APTR  WidgetObj;
	LONG  Response;
};

struct MUIP_GtkDialog_EnableResponse
{
	ULONG MethodID;
	LONG  Response;
	LONG  Enable;
};

struct MUIP_NoteBook_PageNr
{
  ULONG MethodID;
  GtkWidget *widget;
  Object *obj;
};

struct MUIP_NoteBook_PageNObject
{
  ULONG MethodID;
  ULONG n;
};


struct MUIP_Itemlist_new { 
  ULONG MethodID; 
  GList *list; 
};

struct MUIP_ScalePOS { 
  ULONG MethodID; 
  guint pos; 
};

struct MUIP_ScaleDraw { 
  ULONG MethodID; 
  ULONG redraw; 
};

struct MA_Fixed_Move_Data {
  GtkWidget *widget;
  int x;
  int y;
};

GtkWidget *mgtk_get_widget_from_obj(Object *obj);

#endif /* GTKMUI_CLASSES_H */

