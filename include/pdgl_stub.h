#include <pdgl.h>

#ifdef _WIN32
#include <pdgl_stub_w32.h>
#endif

#ifdef linux
#include <pdgl_stub_lnx.h>
#endif

#ifdef NATIVECLIENT
#include <pdgl_stub_nacl.h>
#endif
