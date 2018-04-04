/** @file xloader.h
    @brief Header file for xloader library
  */


#ifndef XLOADER_H
#define XLOADER_H


#ifdef WIN32
#include <windows.h>
typedef FARPROC FUNCRET;
#else
typedef void* FUNCRET;
#endif


#ifdef __cplusplus
extern "C" {
#endif


/// @brief Handle to a dynamic library
typedef void* XLOADER;


/** @brief Opens a dynamic library.
    @param path Full path of the dynamic library.
  */
XLOADER xloader_open(const char* path);

/// @brief Closes a dynamic library.
void xloader_close(XLOADER loader);

/** @brief Finds a the address of a symbol in a dynamic library.
    @return Address of the symbol if successful.
    @return @p NULL if the symbol does not exist in the dynamic library.
    @param loader   Handle to a dynamic library.
    @param symbol   The name of the symbol to lookup.
  */
FUNCRET xloader_lookup_symbol(XLOADER loader, const char* symbol);


#ifdef __cplusplus
}
#endif


#endif /* XLOADER_H */


/* vim: set cindent tabstop=8 softtabstop=4 expandtab shiftwidth=4: */
