#include <frbcigl.h>

#ifdef _WIN32
#include <frbcigl_main_w32.h>
#endif

#ifdef linux
#include <frbcigl_main_lnx.h>
#endif

#ifdef NATIVECLIENT
#include <frbcigl_main_nacl.h>
#endif
