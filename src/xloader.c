// Source file for libxloader


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#  include <windows.h>
#  define dlsym(h,s)    GetProcAddress((HINSTANCE)(h),s)
#  define dlopen(n,f)   LoadLibrary(n)
#  define dlclose(h)    FreeLibrary((HINSTANCE)(h))
#else
#  include <dlfcn.h>
#endif

#include "xloader.h"


XLOADER
xloader_open(const char* path)
{
    if (!path)  return NULL;
    if (!strlen(path))  return NULL;
    return dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
}


void
xloader_close(XLOADER loader)
{
    if (loader)  dlclose(loader);
}


FUNCRET
xloader_lookup_symbol(XLOADER loader, const char* symbol)
{
    if (!loader)  return NULL;
    return dlsym(loader, symbol);
}


/* vim: set cindent tabstop=8 softtabstop=4 expandtab shiftwidth=4: */
