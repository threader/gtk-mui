#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <constructor.h>

#define VERSION 1

void _INIT_4_GTKMUIPPCBase(void) __attribute__((alias("__CSTP_init_GTKMUIPPCBase")));
void _EXIT_4_GTKMUIPPCBase(void) __attribute__((alias("__DSTP_cleanup_GTKMUIPPCBase")));

struct Library *GTKMUIPPCBase;

STATIC void openliberror(ULONG version, const char *name)
{
	struct Library *MUIMasterBase;

	MUIMasterBase = OpenLibrary("muimaster.library", 0);
	if (MUIMasterBase)
	{
		MUI_Request(NULL, NULL, 0, "gtk-mui startup error", "Quit", "Need version %.10ld of gtkmuippc.library", VERSION);
		CloseLibrary(MUIMasterBase);
	}
}

STATIC CONST TEXT libname[] = "gtkmuippc.library";

STATIC CONSTRUCTOR_P(init_GTKMUIPPCBase, 100)
{
	GTKMUIPPCBase = (void *) OpenLibrary((STRPTR)libname, VERSION);

	if (!GTKMUIPPCBase)
	{
		openliberror(VERSION, libname);
	}

	return (GTKMUIPPCBase == NULL);
}

STATIC DESTRUCTOR_P(cleanup_GTKMUIPPCBase, 100)
{
	CloseLibrary(GTKMUIPPCBase);
}
