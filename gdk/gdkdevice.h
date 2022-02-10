/* just a dummy for now */

/* $Id: */

#ifndef __GDK_DEVICE_H__
#define __GDK_DEVICE_H__

#include <glib/gtypes.h>
#include <glib-object.h>

typedef struct _GdkDevice GdkDevice;
struct _GdkDevice {

  /*    GObject parent_instance;*/
  /* All fields are read-only */

  gchar *name;
  /* don't care about the rest for now ;) */
};

typedef enum {

  GDK_DRAG_PROTO_MOTIF,
  GDK_DRAG_PROTO_XDND,
  GDK_DRAG_PROTO_ROOTWIN,         /* A root window with nobody claiming drags */
  GDK_DRAG_PROTO_NONE,            /* Not a valid drag window */
  GDK_DRAG_PROTO_WIN32_DROPFILES, /* The simple WM_DROPFILES dnd */
  GDK_DRAG_PROTO_OLE2,            /* The complex OLE2 dnd (not implemented) */
  GDK_DRAG_PROTO_LOCAL            /* Intra-app */
} GdkDragProtocol;

typedef enum {

  GDK_ACTION_DEFAULT = 1 << 0,
  GDK_ACTION_COPY    = 1 << 1,
  GDK_ACTION_MOVE    = 1 << 2,
  GDK_ACTION_LINK    = 1 << 3,
  GDK_ACTION_PRIVATE = 1 << 4,
  GDK_ACTION_ASK     = 1 << 5
} GdkDragAction;

typedef struct _GdkDragContext        GdkDragContext;
struct _GdkDragContext {
   /* GObject parent_instance;*/

  /*< public >*/

  GdkDragProtocol protocol;

  gboolean is_source;

  GdkWindow *source_window;
  GdkWindow *dest_window;

  GList *targets;
  GdkDragAction actions;
  GdkDragAction suggested_action;
  GdkDragAction action;

  guint32 start_time;

  /*< private >*/

  gpointer windowing_data;
};

#endif

