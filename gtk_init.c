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
 * $Id: gtk_init.c,v 1.51 2012/08/21 15:24:39 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/gadtools.h>
#include <proto/dos.h>
#include <proto/commodities.h>
/* I need exactly one place, where proto/muimaster.h is included..? */
#include <proto/muimaster.h>
#include <mui.h>
/* cross platform hooks */
#ifdef __AROS__
#include "include/SDI_hook.h"
#else
#include <SDI_hook.h>
#endif

#include "../version.h"
#include "classes.h"
#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

BOOL Open_Libs(void);
void Close_Libs(void);

#ifdef __MORPHOS__
/*#include "MorphOS/Library.h"
struct ExecBase *SysBase = NULL;
BOOL mgtk_openlibs(struct MyLibrary *LibBase)
{
	SysBase = LibBase->SysBase;
	return Open_Libs();
}    */

/*VOID mgtk_closelibs(APTR base)
{
	Close_Libs();
} */
#endif

BOOL open_own_libs() {

#if 0
  if(gtk_no_own_libs) {
    return TRUE;
  }
#endif

#ifndef __AMIGAOS4__
  if ((DOSBase      =(struct DosLibrary *)    OpenLibrary("dos.library",36)) )
  if ((IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",39)) )
  if ((GfxBase=      (struct GfxBase *)       OpenLibrary("graphics.library",0)) )
  if ((GadToolsBase=                          OpenLibrary("gadtools.library" ,39)) )

#else
  if ((GadToolsBase=                          OpenLibrary("gadtools.library" ,39)) )
  if ((IMUIMaster=   (struct Interface *)     GetInterface(MUIMasterBase,"main",1,NULL)) )
  if ((IGadTools=    (struct GadToolsIFace *) GetInterface(GadToolsBase, "main", 1, NULL)) ) 
#endif
  if ((MUIMasterBase=(struct Library *)       OpenLibrary(MUIMASTER_NAME,19)) )
  if ((UtilityBase=  (struct UtilityBase *)   OpenLibrary("utility.library", 0)))
    return TRUE;

  if(!GadToolsBase || !GfxBase || !IntuitionBase || !DOSBase || !MUIMasterBase || !UtilityBase) {
    return FALSE;
  }

  return TRUE;
}

BOOL Open_Libs(void ) {

  if (open_own_libs()) 
  if (mgtk_create_application_class())
  if (mgtk_create_rootgroup_class())
  if (mgtk_create_group_class())
  if (mgtk_create_hbox_class())
  if (mgtk_create_vbox_class())
  if (mgtk_create_window_class())
  if (mgtk_create_table_class())
  if (mgtk_create_spinbutton_class()) 
  if (mgtk_create_notebook_class()) 
  if (mgtk_create_progressbar_class()) 
//  if (mgtk_create_timer_class()) 
  if (mgtk_create_fixed_class()) 
  if (mgtk_create_checkbutton_class()) 
  if (mgtk_create_togglebutton_class()) 
  if (mgtk_create_radiobutton_class()) 
  if (mgtk_create_button_class()) 
  if (mgtk_create_menu_class()) 
  if (mgtk_create_entry_class()) 
  if (mgtk_create_toolbar_class()) 
  if (mgtk_create_list_class()) 
  if (mgtk_create_clist_class())
  if (mgtk_create_combo_class()) 
  if (mgtk_create_scale_class()) 
  if (mgtk_create_frame_class()) 
  if (mgtk_create_custom_class()) 
  if (mgtk_create_dialog_class())
//  if (mgtk_create_messagedialog_class())
//  if (mgtk_create_box_class())
  if (mgtk_create_label_class())
  if (mgtk_create_scrolledwindow_class())
  {
    DebOut("  Open_Libs was successfull\n");
    return (1);
  }

  Close_Libs();
  return(0);
}

void Close_Libs(void)
{

  DebOut("Close_Libs()\n");

  mgtk_delete_scrolledwindow_class();
  mgtk_delete_table_class();
  mgtk_delete_spinbutton_class();
  mgtk_delete_notebook_class();
  mgtk_delete_progressbar_class();
//  mgtk_delete_timer_class();
//  mgtk_delete_box_class();
  mgtk_delete_fixed_class();
  mgtk_delete_dialog_class();
//  mgtk_delete_messagedialog_class();
  mgtk_delete_checkbutton_class();
  mgtk_delete_togglebutton_class();
  mgtk_delete_radiobutton_class();
  mgtk_delete_button_class();
  mgtk_delete_menu_class();
  mgtk_delete_entry_class();
  mgtk_delete_toolbar_class();
  mgtk_delete_clist_class();
  mgtk_delete_list_class();
  mgtk_delete_custom_class();
  DebOut("Close_Libs combo start()\n");
  mgtk_delete_combo_class();
  DebOut("Close_Libs combo end()\n");
  mgtk_delete_scale_class();
  mgtk_delete_label_class();
  mgtk_delete_frame_class();
  mgtk_delete_hbox_class();
  mgtk_delete_vbox_class();
  mgtk_delete_group_class();
  mgtk_delete_rootgroup_class();
  mgtk_delete_window_class();
  mgtk_delete_application_class();

#ifndef __AMIGAOS4__
  if(!gtk_no_own_libs) {
    CloseLibrary((struct Library *)IntuitionBase);
    CloseLibrary((struct Library *)GfxBase);
    CloseLibrary((struct Library *)DOSBase);
    CloseLibrary((struct Library *)UtilityBase);
#else
    if (IMUIMaster)   DropInterface((struct Interface *)IMUIMaster);
    if (GadToolsBase) CloseLibrary(GadToolsBase);
    if (IGadTools)    DropInterface(IGadTools);
#endif
    CloseLibrary(MUIMasterBase);
  }

#ifdef __MORPHOS__
  /* Must clear these because pointers are reused later */

  CL_Application = NULL;
  CL_Window = NULL;
  CL_RootGroup = NULL;
 // CL_Box = NULL;
  CL_Custom = NULL;
  CL_Table = NULL;
  CL_SpinButton = NULL;
  CL_NoteBook = NULL;
  CL_ProgressBar = NULL;
  CL_Fixed = NULL;
  CL_CheckButton = NULL;
  CL_ToggleButton = NULL;
  CL_RadioButton = NULL;
  CL_Button = NULL;
  CL_Menu = NULL;
  CL_Entry = NULL;
  CL_Toolbar = NULL;
  CL_Dialog = NULL;
 // CL_MessageDialog = NULL;
  CL_Label = NULL;
  CL_List = NULL;
  CL_CList = NULL;
  CL_Scrolledwindow = NULL;

if(!gtk_no_own_libs) {
  MUIMasterBase = NULL;
  IntuitionBase = NULL;
  UtilityBase = NULL;
  GfxBase = NULL;
  DOSBase = NULL;
}
#endif

#ifndef __AROS__
  /* free mystack again */
  if(mynewstack) {
    StackSwap(&stackswap);
    FreeVec(mynewstack);
    mynewstack=NULL;
  }
#endif
  DebOut("Close_Libs done\n");
}

/*********************************
 * init_fundamentals
 *********************************/
void init_fundamentals() {

  DebOut("init_fundamentals()\n");

  /* some applications require a valid HOME variable */
  setenv("HOME","PROGDIR:",0); /* should not overwrite, if existing already*/

  g_type_init();
  g_signal_init();
}

/*********************************
 * gtk_init
 *
 * create MuiApp and init mgtk
 *
 *********************************/
extern char *builddate;

static struct Hook mgtk_broker_hook;

AROS_UFH3(void, mgtk_broker_func,
    AROS_UFHA(struct Hook *, h,      A0),
    AROS_UFHA(Object *     , object, A2),
    AROS_UFHA(CxMsg *      , msg,    A1))
{
    AROS_USERFUNC_INIT

    D(bug("gtk_init: Broker hook called\n"));
    if (CxMsgType(msg) == CXM_COMMAND) {

      if (CxMsgID(msg) == CXCMD_APPEAR) {

	    //This opens window if application was started with cx_popup=no
	    //set(global_mgtk->MuiApp, MUIA_Application_Iconified, FALSE);
      }
    }
    AROS_USERFUNC_EXIT
}

/* if we get WE_HAVE_OWN_LIBS as argv[0], then the opening main program opened all
 * libraries for us (hopefully)
 */
gboolean  gtk_init_check(int *argc, char ***argv) {
  char *tname;
  char *i;
  char *name;
  char *name2;
#ifndef __AROS__
  struct Task *mytask;
  ULONG  stacksize;
#endif
  /* helpers */
  int    myargc=*argc;
  char **myargv=*argv;

#if defined ENABLE_RT
  RT_Init();
#endif

  DebOut("gtk_init_check(%lx,%lx)\n",argc,argv);

  if(myargc>1) {
    if(!strcmp("WE_HAVE_OWN_LIBS", myargv[1])) {
      gtk_no_own_libs=1;
    }
  }


//#ifndef __MORPHOS__
  if ( ! Open_Libs() )
  {
    printf("GTK: fatal error: Cannot open libs\n");
    return(FALSE);
  }

#ifndef __AROS__
  /* Let us make sure we have enough stack-space .. */
  mytask = FindTask(NULL);
  stacksize = (ULONG)mytask->tc_SPUpper-(ULONG)mytask->tc_SPLower+(16*4096);
  mynewstack       = AllocVec(stacksize, 0L);
  stackswap.stk_Lower   = mynewstack;
  stackswap.stk_Upper   = (ULONG) mynewstack+stacksize;
  stackswap.stk_Pointer = (APTR)  stackswap.stk_Upper;

  if(mynewstack) {
    StackSwap(&stackswap);
  }
#endif


  global_mgtk=g_new0(struct mgtk_ObjApp,1);
#if 0
  global_mgtk->delayed_signals=NULL;
#endif

  if(!global_mgtk) {
    return(FALSE);
  }
//#endif

  /* get program name */
  tname=*argv[0];
  i=*argv[0];

  while(i[0]) {
    if((i[0]=='/') || (i[0]==':')) {
      tname=i+1;
    }
    i++;
  }

  if(tname[0]) {
    name=g_strdup_printf("GTK %s",tname);
    name2=g_strdup_printf("gtk-mui/%s %s (%s)",tname,LIB_VER,LIB_DATE);
  }
  else {
    name=g_strdup("GTK Application");
    name2=g_strdup_printf("gtk-mui %s (%s)",LIB_VER,LIB_DATE);
  }

  init_fundamentals();

  mgtk_broker_hook.h_Entry = (HOOKFUNC)AROS_ASMSYMNAME(mgtk_broker_func);

  global_mgtk->MuiApp = (APTR) NewObject(CL_Application->mcc_Class, NULL,
    MA_GtkApplication_Title,     (ULONG) name,
    MUIA_Application_Version ,   (ULONG) name2,
    MUIA_Application_Copyright , (ULONG) "GTK-MUI: LGPL",
    MUIA_Application_Author  ,   (ULONG) "GTK by the GTK-MUI Team",
    MUIA_Application_Description,(ULONG) "A program built with GTK-MUI",
    MUIA_Application_BrokerPri,   0,
    MUIA_Application_BrokerHook, (IPTR)  &mgtk_broker_hook,
    TAG_DONE);

   /* we must reference builddate somewhere, otherwise optimization might kill it ;)
    * Zune ignores MUIA_Application_Version after init anyhow 
    */
  set(global_mgtk->MuiApp,MUIA_Application_Version,(ULONG) builddate);
  set(global_mgtk->MuiApp,MUIA_Application_Version,(ULONG) name2);

  g_free(name);
  g_free(name2);

  /* MuiApp fails if MuiRootStrip failed */

  if (!global_mgtk->MuiApp) {
    printf("gtk_window_new: Cannot create application. (%ld,%ld)\n",(long int) global_mgtk->MuiApp,(long int) global_mgtk->MuiRootStrip);
    return(FALSE);
  }

#if 0
  DoMethod(mgtk->MuiRoot,MUIA_Window_Width,150);
  DoMethod(mgtk->MuiRoot,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
    mgtk->MuiApp,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
#endif

  DebOut("global_mgtk->MuiApp: %lx\n",global_mgtk->MuiApp);
  DebOut("global_mgtk->MuiRoot: %lx\n",global_mgtk->MuiRoot);
  DebOut("global_mgtk->MuiRootStrip: %lx\n",global_mgtk->MuiRootStrip);
#if 0
  DebOut("mgtk->MuiGroup: %lx\n",mgtk->Group);
#endif

/*
  mgtk->black_pen=ObtainBestPenA( ViewAddress()->ViewPort->ColorMap, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, NULL ); 
  */


  global_mgtk->white_black_optained=0;
  if ((global_mgtk->screen = LockPubScreen(0))) {
    if ((global_mgtk->dri = GetScreenDrawInfo(global_mgtk->screen))) {
      if ((global_mgtk->visualinfo = GetVisualInfoA(global_mgtk->screen, NULL))) {
        global_mgtk->white_pen=ObtainBestPen( global_mgtk->screen->ViewPort.ColorMap, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, OBP_FailIfBad, FALSE, TAG_DONE ); 
        global_mgtk->black_pen=ObtainBestPen( global_mgtk->screen->ViewPort.ColorMap, 0, 0, 0, OBP_FailIfBad, FALSE, TAG_DONE ); 
        global_mgtk->white_black_optained=1;
      }
      else {
        DebOut("WARNING: Init: Unable to GetVisualInfoA\n");
      }
    }
    else {
        DebOut("WARNING: Init: Unable to GetScreenDrawInfo\n");
    }
  }
  else {
    DebOut("WARNING: Init: Unable to LockPubScreen\n");
  }

  /* if anything above failed */
  if(!global_mgtk->white_black_optained) {
    global_mgtk->white_pen=MGTK_PEN_WHITE;
    global_mgtk->black_pen=MGTK_PEN_BLACK; /* naja.. */
  }

  DebOut("  gtk_init was successfull\n");

  return(TRUE);
}

void gtk_init(int *argc, char ***argv) {

  DebOut("gtk_init(%lx,%lx)\n",argc,argv);


  if(!gtk_init_check(argc,argv)) {
    ErrOut("Unable to gtk_init\n");
    exit(1);
  }
}
