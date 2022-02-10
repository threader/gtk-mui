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
 * $Id: label.c,v 1.11 2013/03/22 10:09:30 o1i Exp $
 *
 * merged from HEAD/1.1
 *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include "mui.h"

#include "debug.h"
#include "classes.h"
#include "gtk_globals.h"

typedef enum {
   PANGO_NONE,
   PANGO_SPAN,
   PANGO_BOLD,
   PANGO_BIG,
   PANGO_ITALIC,
   PANGO_STRIKETHROUGH,
   PANGO_SUBSCRIPT,
   PANGO_SUPERSCRIPT,
   PANGO_SMALL,
   PANGO_MONOSPACE,
   PANGO_UNDERLINE,
} mPANGOTAG;

struct PangoTag {
   CONST_STRPTR tagname;
   mPANGOTAG    tagnum;
};

STATIC CONST struct PangoTag PangoTag[] = {
   { "span"         , PANGO_SPAN          },
   { "b"            , PANGO_BOLD          },
   { "big"          , PANGO_BIG           },
   { "italic"       , PANGO_ITALIC        },
   { "strikethrough", PANGO_STRIKETHROUGH },
   { "sub"          , PANGO_SUBSCRIPT     },
   { "sup"          , PANGO_SUPERSCRIPT   },
   { "small"        , PANGO_SMALL         },
   { "tt"           , PANGO_MONOSPACE     },
   { "u"            , PANGO_UNDERLINE     },
   { NULL           , PANGO_NONE          },
};

struct TextNode {
   struct MinNode link;

   mPANGOTAG pangotag;
   BYTE   enable;

   LONG   textlen;
   STRPTR text;
};

struct Data {
   struct MinList   textlist;
   CONST_STRPTR     text;
   LONG             nrlines;
   gchar**          line;
   LONG             defwidth;
   LONG             defheight;
   struct Hook      LayoutHook;
   struct RastPort *rp;
};

/*******************************************************
 * struct RastPort *getrp(APTR obj, struct Data *data) 
 *
 * create and return a rastport for an object
 *******************************************************/
struct RastPort *getrp(APTR obj, struct Data *data) {
  struct RastPort *rp;
  struct TextFont *font;

  if(data->rp) {
    return data->rp;
  }

  font = _font(obj);
  if(!font) {
    return NULL;
  }

  rp=mgtk_allocvec(sizeof(*rp), MEMF_ANY);

  if(!rp) {
    ErrOut("not enough memeory for a rastport !?\n");
    return NULL;
  }

  InitRastPort(rp);
  SetFont(rp,font);

  return rp;
}


/*******************************************************
 * static LONG nrlines(CONST_STRPTR text)
 *
 * return number of lines in string (\n) 
 *******************************************************/
static LONG nrlines(CONST_STRPTR text) {
  int  i=0;
  LONG c=1;

  while(text[i] != (char) 0) {
    if(text[i] == '\n') {
      c++;
    }
    i++;
  }
  DebOut("nrlines(%s)=%d\n",text,c);
  return c;
}

/*******************************************************
 * WORD textlength_splitline(APTR obj,
 *                           struct Data *data,
 *                           const char *s) 
 *
 * return the pixel width of a line with potential
 * linebreaks. The object is needed to try to get
 * a rastport for it. 
 *******************************************************/
WORD textlength_splitline(APTR obj, struct Data *data, const char *s) {
  int i;
  WORD t;
  WORD max=0;
  struct RastPort *rp;

  DebOut("textlength_splitline(%lx,%lx,%s)\n",obj,data,s);

  DebOut(" more than one line ..\n");

  rp=getrp(obj,data);

  if(nrlines(s)==1) {
    return TextLength(rp,s,strlen(s));
  }

  if(!rp) {
    ErrOut("classes/label.c: no RastPort available!\n");
    return 40;
  }

  /* iterate through all parts */
  for(i=0;data->line[i];i++) {
    DebOut("  wrk[%d]=%s\n",i,data->line[i]);
    //t=strlen(wrk[i]);
    t=TextLength(rp,data->line[i],strlen(data->line[i]));
    if(t>max) {
      max=t;
    }
  }

  DebOut(" strlen_splitline returns: %d\n",max);

  return max;
}

STATIC VOID mSet(struct Data *data, APTR obj, struct opSet *msg);

/*******************************************************
 * Layout
 *******************************************************/
#ifndef __AROS__
HOOKPROTO(LayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm) {
#else 
AROS_UFH3(static ULONG, LayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif
  struct TextFont *font;

  DebOut("label->LayoutHook\n");

  switch (lm->lm_Type) {

    /* ======== MUILM_MINMAX ======= */

    case MUILM_MINMAX: {
      struct Data *data = hook->h_Data;

      DebOut("MUILM_MINMAX\n");

      font = _font(obj);

      DebOut("font: %lx\n",font);

      lm->lm_MinMax.MinWidth  = 5 * font->tf_XSize;
      lm->lm_MinMax.MinHeight = data->nrlines * (font->tf_YSize + 2);

      lm->lm_MinMax.DefWidth  = textlength_splitline(obj,data,data->text)+2;
      lm->lm_MinMax.DefHeight = data->nrlines * (font->tf_YSize + 2);

      lm->lm_MinMax.MaxWidth  = 100 * font->tf_XSize;
      lm->lm_MinMax.MaxHeight = font->tf_YSize + 2;

      DebOut("done\n");

      return 0;
    }

    /* ======== MUILM_LAYOUT ======= */

    case MUILM_LAYOUT: {

      Object *cstate = (Object *)lm->lm_Children->mlh_Head;
      Object *child;

      DebOut("MUILM_LAYOUT\n");

      /* now place all childs */
      while ((child = NextObject(&cstate))) {
        if (!MUI_Layout(child, 0, 0,
              lm->lm_Layout.Width-1,lm->lm_Layout.Height-1,0)) {
          
          return FALSE;
        }
      }
      DebOut("done2\n");
    }
    return TRUE;
  }

  return MUILM_UNKNOWN;
}

STATIC struct TextNode *mNewTag(void)
{
   struct TextNode *node;

   node = mgtk_allocmem(sizeof(*node), MEMF_ANY);

   if (node) {
      node->pangotag = PANGO_NONE;
      node->enable = 0;
      node->text = NULL;
   }

   return node;
}

/* Returns length of text -1 indicating an error */
STATIC LONG mAddText(struct MinList *minlist, struct TextNode *node, CONST_STRPTR start, CONST_STRPTR end)
{
   LONG len, rc;

   len = end - start;
   rc = 0;

   if (len >= 0)
   {
      node->textlen = len;
      node->text = mgtk_allocmem(len + 1, MEMF_ANY);
      rc = -1;

      if (node->text)
      {
         CopyMem((void *)start, node->text, len);
         node->text[len] = '\0';

         ADDTAIL(minlist, node);
         rc = len;
      }

      ADDTAIL(minlist, node);
   }

   return rc;
}

STATIC int mStrcasecmp(CONST_STRPTR str1, CONST_STRPTR str2)
{
   UBYTE a, b;
   int rc = 1;

   do
   {
      a = *str1++;
      b = *str2++;

      if (a >= 'A' && a <= 'Z')
         a += 'a' - 'A';

      if (b >= 'A' && b <= 'Z')
         b += 'a' - 'A';

      if (a != b)
         return rc;
   }
   while (a);

   rc = 0;

   return rc;
}

STATIC mPANGOTAG mGetTag(CONST_STRPTR tag, CONST_STRPTR *next)
{
   mPANGOTAG rc = PANGO_NONE;
   CONST_STRPTR t = tag;
   TEXT c;
   int len = -1;

   do
   {
      c = *t++;
      len++;
   }
   while (c != '\0' || c != '>' || c != ' ');

   if (c)
   {
      CONST struct PangoTag *pt;
      CONST_STRPTR name;

      pt = (CONST struct PangoTag *)&PangoTag;

      do
      {
         mPANGOTAG id;

         name = pt->tagname;
         id = pt->tagnum;

         pt++;

         if (strlen((char *)name) == len)
         {
            if (mStrcasecmp(name, tag) == 0)
            {
               // tag found
               *next = tag + len + 1;
               rc = id;
               return rc;
            }
         }
      }
      while (pt->tagname);
   }

   return rc;
}

/**********************************************************************
   mNewPangoText
**********************************************************************/

STATIC VOID mNewPangoText(struct Data *data, CONST_STRPTR text)
{
   int length;

   if (!text)
      return;

   length = strlen((char *)text) + 1;

   data->text = mgtk_allocvec(length, MEMF_ANY);

   if (data->text)
   {
      struct TextNode *node;

      CopyMem((void *)text, (void *)data->text, length);

      node = mNewTag();

      if (node)
      {
         CONST_STRPTR start;
         TEXT c;

         start = text;

         // main loop

         while ((c = *text))
         {
            text++;

            if (c == '<')   // is it a tag?
            {
               if (*text == '/')
               {
                  // end of tag
                  text++;
               }
               else
               {
                  // start of tag
                  CONST_STRPTR next;
                  mPANGOTAG tag;

                  tag = mGetTag(text, &next);

                  if (tag != PANGO_NONE)
                  {
                     switch (tag)
                     {
                        default:
                        case PANGO_NONE         : break;

                        case PANGO_SPAN         : break;
                        case PANGO_BOLD         : break;
                        case PANGO_BIG          : break;
                        case PANGO_ITALIC       : break;
                        case PANGO_STRIKETHROUGH: break;
                        case PANGO_SUBSCRIPT    : break;
                        case PANGO_SUPERSCRIPT  : break;
                        case PANGO_SMALL        : break;
                        case PANGO_MONOSPACE    : break;
                        case PANGO_UNDERLINE    : break;
                     }

                     if (mAddText(&data->textlist, node, start, text-1) < 0)
                        return;

                     start = text;
                     text = next;
                  }
               }
            }
         }

         mAddText(&data->textlist, node, start, text);
      }
   }
}

STATIC VOID mDisposeText(struct Data *data)
{
   struct TextNode *node;

   if (data->text) {
      mgtk_freevec((void *)data->text);
  }

  if(data->line) {
      g_strfreev(data->line);
  }

   ForeachNode(&data->textlist, node)
   {
      REMOVE(node);

      if (node->text)
         mgtk_freemem(node->text, node->textlen);

      mgtk_freemem(node, sizeof(*node));
   }

   data->text = NULL;
}

/**********************************************************************
   mNewText
**********************************************************************/

STATIC VOID mNewText(struct Data *data, CONST_STRPTR text)
{

   if (text) {
      int length = strlen((char *)text) + 1;

      data->text = mgtk_allocvec(length, MEMF_ANY);
      if (data->text) {
         CopyMem((void *)text, (void *)data->text, length);
      }

    data->nrlines=nrlines(text);
    data->line=g_strsplit_set((gchar *) text,"\n",-1);
   }
}


/**********************************************************************
   Custom class dispatcher functions
**********************************************************************/

STATIC ULONG mNew(struct IClass *cl, APTR obj, APTR msg)
{

  DebOut("label->mNew\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if (obj) {
    GETDATA;
    MGTK_NEWLIST(data->textlist);
    SETUPHOOK(&data->LayoutHook, LayoutHook, data);
    set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
    data->nrlines=1;
    data->rp=NULL; /* will be filled as soon as available */
  }

  DebOut("  obj=%lx\n",obj);

  return (ULONG)obj;
}

STATIC ULONG mAskMinMax(struct Data *data, APTR obj, struct MUIP_AskMinMax *msg, struct IClass *cl) {

   struct MUI_MinMax *mm;
   struct TextFont *font;
#if 0
   LONG minwidth, defwidth, defheight, maxwidth;
#endif
  LONG minheight;

  DoSuperMethodA(cl, obj, (Msg)msg);

  font = _font(obj);
  mm = msg->MinMaxInfo;

  DebOut("mAskMinMax(obj %lx)\n",obj);
  DebOut("  mm->MinWidth,Height: %d,%d\n",mm->MinWidth,mm->MinHeight);
  DebOut("  mm->DefWidth,Height: %d,%d\n",mm->DefWidth,mm->DefHeight);
  DebOut("  mm->MaxWidth,Height: %d,%d\n",mm->MaxWidth,mm->MaxHeight);

  if( !font || (font->tf_YSize < 1) || (font->tf_XSize <1 )) {
    WarnOut("classes/label.c: font error!");
    msg->MinMaxInfo->MinHeight += 10;
    msg->MinMaxInfo->DefHeight += 10;
    msg->MinMaxInfo->MaxHeight  = MUI_MAXMAX;

    msg->MinMaxInfo->MinWidth  += 10;
    msg->MinMaxInfo->DefWidth  += 100;
    msg->MinMaxInfo->MaxWidth  = MUI_MAXMAX;
    return 0;
  }

   minheight = font->tf_YSize + 2;

  if(!data->defwidth) {
    data->defwidth = 4 * font->tf_XSize;
  }

#if 0
   defwidth = data->defwidth;
   defheight = data->defheight;

   minwidth = 8 * font->tf_XSize;
   maxwidth = 8 * font->tf_XSize;

   if (defwidth < 0)
      defwidth = mm->DefWidth;
   else if (defwidth == 0)
      defwidth = mm->MinWidth;
   else {
      if (defwidth < mm->MinWidth)
         defwidth = mm->MinWidth;
      else if (defwidth > mm->MaxWidth)
         defwidth = mm->MaxWidth;
   }

   if (defheight < 0)
      defheight = mm->DefHeight;
   else if (defheight == 0)
      defheight = mm->MinHeight;
   else {
      if (defheight < mm->MinHeight)
         defheight = mm->MinHeight;
      else if (defheight > mm->MaxHeight)
      defheight = mm->MaxHeight;
   }

   mm->MinWidth  += minwidth;
//   mm->MinHeight += minheight;

   mm->DefWidth  += defwidth;
//   mm->DefHeight += defheight;
#endif

  if(data->text) {
    mm->DefWidth  = textlength_splitline(obj,data,data->text)+2;
//    mm->DefWidth  = strlen(data->text)*font->tf_XSize;
    DebOut(" text: >%s<\n",data->text);
  }
  else {
    mm->DefWidth  = 12 * font->tf_XSize;
  }
  mm->DefHeight = mm->MinHeight;

  /* labels are free to grow always..?*/
  mm->MaxHeight=MUI_MAXMAX;
  mm->MaxWidth =MUI_MAXMAX;

  DebOut("result:\n");
  DebOut("  mm->MinWidth,Height: %d,%d\n",mm->MinWidth,mm->MinHeight);
  DebOut("  mm->DefWidth,Height: %d,%d\n",mm->DefWidth,mm->DefHeight);
  DebOut("  mm->MaxWidth,Height: %d,%d\n",mm->MaxWidth,mm->MaxHeight);

  return 0;
}

/*******************************************
 * mSet
 *******************************************/
STATIC VOID mSet(struct Data *data, APTR obj, struct opSet *msg)
{
   struct TagItem *tstate, *tag;
   ULONG update;

   DebOut("label->mSet\n");

   tstate = msg->ops_AttrList;
   update = FALSE;

   while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
   {
      switch (tag->ti_Tag)
      {
         case MUIA_Text_Contents:
            mDisposeText(data);
            tag->ti_Tag = TAG_IGNORE;
            mNewText(data, (CONST_STRPTR)tag->ti_Data);
            update = TRUE;
            break;

         case MA_GtkLabel_PangoText:
            mDisposeText(data);
            mNewPangoText(data, (CONST_STRPTR)tag->ti_Data);
            update = TRUE;
            break;

         case MA_DefWidth : data->defwidth = tag->ti_Data; break;
         case MA_DefHeight: data->defheight = tag->ti_Data; break;
      }
   }

   if (update)
   {
    /* we need to clear it completely, so no UPDATE here */
      MUI_Redraw(obj, MADF_DRAWOBJECT);
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
      case MUIA_Text_Contents: rc = data->text ? (ULONG)data->text : (ULONG)""; break;
      default: return DoSuperMethodA(cl, obj, (Msg)msg);
   }

   *msg->opg_Storage = rc;
   return TRUE;
}

/*******************************************
 * mDraw
 *******************************************/

STATIC ULONG mDraw(struct Data *data, APTR obj, struct MUIP_Draw *msg, struct IClass *cl) {

  DoSuperMethodA(cl, obj, (Msg)msg);

  DebOut("label->mDraw\n");

  if ( !(msg->flags & MADF_DRAWOBJECT) &&
       !(msg->flags & MADF_DRAWUPDATE)) {
    DebOut("  nothing to draw (!MADF_DRAWOBJECT and !MADF_DRAWUPDATE)\n");
    return 0;
  }

  if (data->text) {

    struct RastPort *rp;
    struct TextFont *font;
    ULONG mleft, mtop, mwidth, mheight,c,l;
    gchar *t;
    gchar *t2;
    gchar *part;
    struct TextExtent te;
    
    mleft = _mleft(obj);
    mtop = _mtop(obj);
    mwidth = _mwidth(obj);
    mheight = _mheight(obj);
    rp = _rp(obj);
    font = _font(obj);

    DebOut("left %d top %d (%d) dim %dx%d: >%s<\n",mleft,mtop,mtop + font->tf_Baseline + 1,mwidth,mheight,data->text);

    /* fixme: clipping, render parsed text, add clipping */
    /* fixme: pen */

    #warning fixme
    SetAPen(rp,_dri(obj)->dri_Pens[TEXTPEN]);

    //DoMethod(obj,MUIM_DrawBackground, _mleft(obj),_mtop(obj),_mwidth(obj),_mheight(obj), _mleft(obj), _mtop(obj), 0);

    for(l=1;l<=data->nrlines;l++) {
      if(mheight > (l * (font->tf_Baseline + 2))) { /* height is enough */
        part=data->line[l-1];
        // DebOut("Part %d: %s\n",l,part);
        if(part) { /* "" will cause a part==NULL */
          Move(rp, mleft, mtop + (l * (font->tf_Baseline + 2))); /* FIXME: +2 should be font specific.. */
          
          if(TextLength(rp, part, strlen(part)) <= mwidth) {
            /* enough space */
            Text(rp, part, strlen(part));
          }
          else {
            /* how many chars can we print (if we want to add "..",too) */
            t=g_strdup_printf("..%s",part);
            c=TextFit(rp,t,strlen(t),&te,NULL,1,mwidth,mheight+2);
            g_free(t);
            /* now add ".." at the end */
            t=g_strndup(part,c-2);
            t2=g_strdup_printf("%s..",t);
            Text(rp, t2, strlen(t2));
            g_free(t);
            g_free(t2);
          }
        }
      }
    }
   }

   return 0;
}

/*******************************************
 * mDispose
 *******************************************/
STATIC VOID mDispose(struct Data *data, APTR obj) {
  struct RastPort *rp;

  DebOut("mDispose(%lx)\n",obj);

  mDisposeText(data);

  if(data->rp) {
    mgtk_freevec((void *)sizeof(*rp));
    data->rp=NULL;
  }
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  DebOut("label(%lx)->msg->MethodID: %lx\n",obj,msg->MethodID);

  switch (msg->MethodID) {
    case OM_NEW                : return mNew       (cl, obj, (APTR)msg);
    case OM_SET                :        mSet       (data, obj, (APTR)msg); break;
    case OM_GET                : return mGet       (data, obj, (APTR)msg, cl);
    case MUIM_AskMinMax        : return mAskMinMax (data, obj, (APTR)msg, cl);
    case MUIM_Draw             : return mDraw      (data, obj, (APTR)msg, cl);
    case OM_DISPOSE            :        mDispose   (data, obj); break;
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_label_class(void)
{
  CL_Label = MUI_CreateCustomClass(NULL, NULL, CL_Group, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Label ? 1 : 0;
}

void mgtk_delete_label_class(void) {

  if (CL_Label) {
    MUI_DeleteCustomClass(CL_Label);
    CL_Label=NULL;
  }
}
