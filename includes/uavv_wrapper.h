#ifndef uavv_wrapper_h
#define uavv_wrapper_h

#include <uavv_video.h>
#include <uavv_image.h>
#include <uavv_klv.h>
#include <string>
#include "xloader.h"

#ifdef WIN32
#   define LIBNAME "libuavv_video.dll"
#else
#   define WINAPI
#   define LIBNAME "libuavv_video.so"     
#endif


namespace uavv
{
    typedef void* UAVV_USER_DATA;

    using UAVVHANDLE = XLOADER;
    using fnImageDecodeCallback = uavv_video_image_hook;
    using fnAbortCallback = uavv_video_abort_hook;
    
    using fnKlvDataCallback = uavv_video_klv_hook;

    typedef void            (WINAPI *fnSetLicenseDirectory)(const char* dirPath);
    typedef const char*     (WINAPI *fnGetLibraryVersion)();
    typedef UAVV_VIDEO      (WINAPI *fnCreateVideoHandle)(const char* fileName);
    typedef int             (WINAPI *fnIsVideoStreaming)(UAVV_VIDEO handle);
    typedef void            (WINAPI *fnDecodeVideo)(UAVV_VIDEO handle, int timeOutMS); 
    typedef void            (WINAPI *fnDestroyVideoHandle)(UAVV_VIDEO handle);
    typedef void*           (WINAPI *fnGetImageData)(UAVV_IMAGE imgData);
    typedef int             (WINAPI *fnGetImageLength)(UAVV_IMAGE imgData);
    typedef int             (WINAPI *fnGetImageWidth)(UAVV_IMAGE imgData);
    typedef int             (WINAPI *fnGetImageHeight)(UAVV_IMAGE imgData);
    typedef UAVV_IMAGE      (WINAPI *fnCopyImageHandle)(UAVV_IMAGE imgData);
    typedef void            (WINAPI *fnDestroyImageHandle)(UAVV_IMAGE imgData);
    typedef UAVV_IMAGE      (WINAPI *fnCreateImageHandle)(int with, int height);
    typedef UAVV_IMAGE      (WINAPI *fnCreateImageHandle)(int with, int height);
    typedef void            (WINAPI *fnGoToPosition)(UAVV_VIDEO handle, float pos);
    typedef void            (WINAPI *fnSetAbortCallback)(UAVV_VIDEO handle, fnAbortCallback* callbackPtr, UAVV_USER_DATA pUserData);
    typedef void            (WINAPI *fnSetImageDecodeCallback)(UAVV_VIDEO handle,  fnImageDecodeCallback* callbackPtr, UAVV_USER_DATA pUserData);
    typedef void            (WINAPI *fnSetKlvDataCallback)(UAVV_VIDEO handle,  fnKlvDataCallback* callbackPtr, UAVV_USER_DATA pUserData);
    typedef long long       (WINAPI *fnKlvGetTimeStamp)(const UAVV_KLV klv);
    typedef int             (WINAPI *fnKlvSize)(const UAVV_KLV klv);
    typedef uavv_klv_key_t  (WINAPI *fnKlvItem)(const UAVV_KLV klv, int offset);
    typedef const char*     (WINAPI *fnKlvItemName)(uavv_klv_key_t key);
    typedef const char*     (WINAPI *fnKlvGetString)(const UAVV_KLV klv, uavv_klv_key_t key);
    typedef int             (WINAPI *fnKlvGetError)(const UAVV_KLV klv, uavv_klv_key_t key);

    class IUAVVInterface
    {
        private:
        
            static UAVVHANDLE uavvHandle;
            static fnSetLicenseDirectory pfnSetLicenseDirectory;
            static fnGetLibraryVersion pfnGetLibraryVersion; 
            static fnCreateVideoHandle pfnCreateVideoHandle;
            static fnIsVideoStreaming pfnIsVideoStreaming;
            static fnDecodeVideo pfnDecodeVideo; 
            static fnDestroyVideoHandle pfnDestroyVideoHandle;
            static fnGetImageData pfnGetImageData;
            static fnGetImageLength pfnGetImageLength; 
            static fnGetImageWidth pfnGetImageWidth;
            static fnGetImageHeight pfnGetImageHeight;
            static fnSetAbortCallback pfnSetAbortCallback;
            static fnSetImageDecodeCallback pfnSetImageDecodeCallback;
            static fnSetKlvDataCallback pfnSetKlvDataCallback;
            static fnCopyImageHandle pfnCopyImageHandle;
            static fnCreateImageHandle pfnCreateImageHandle;
            static fnDestroyImageHandle pfnDestroyImageHandle;
            static fnGoToPosition pfnGoToPosition;
            static fnKlvGetTimeStamp pfnKlvGetTimeStamp;
            static fnKlvSize pfnKlvSize;
            static fnKlvItem pfnKlvItem;
            static fnKlvItemName pfnKlvItemName;
            static fnKlvGetString pfnKlvGetString;
            static fnKlvGetError pfnKlvGetError;
            static bool InitFunctionDefaultCallbacks();

        public:
            
            // library management routines
            static bool LoadAndInitLibrary();  
            static void UnloadLibrary();

            // library function calls
            static void SetLicenseDirectory(std::string& path);
            static std::string GetLibraryVersion();
            static UAVV_VIDEO CreateVideoHandle(const std::string& filePath);
            static int IsStreamingVideo(UAVV_VIDEO handle);
            static void DestroyVideoHandle(UAVV_VIDEO handle);
            static void DecodeVideo(UAVV_VIDEO handle, int timeOutMS);
            static void* GetImageData(UAVV_IMAGE img);
            static int GetImageWidth(UAVV_IMAGE img);
            static int GetImageHeight(UAVV_IMAGE img);
            static int GetImageLength(UAVV_IMAGE img);
            static UAVV_IMAGE CreateImageHandle(int width, int height);
            static UAVV_IMAGE CopyImageHandle(UAVV_IMAGE img);
            static void DestroyImageHandle(UAVV_IMAGE img);
            static void GoToPosition(UAVV_VIDEO handle, float pos);
            
            static long long KlvGetTimeStamp(const UAVV_KLV klv);
            static int KlvSize(const UAVV_KLV klv);
            static uavv_klv_key_t KlvItem(const UAVV_KLV klv, int offset);
            static std::string KlvItemName(uavv_klv_key_t key);
            static std::string KlvGetString(const UAVV_KLV klv, uavv_klv_key_t key);
            static bool KlvIsInErrorState(const UAVV_KLV klv, uavv_klv_key_t key);

            static void SetAbortCallback(UAVV_VIDEO handle, fnAbortCallback* callbackPtr, UAVV_USER_DATA pUserData);
            static void SetImageDecodeCallback(UAVV_VIDEO handle, fnImageDecodeCallback* callbackPtr, UAVV_USER_DATA pUserData);
            static void SetKlvDataCallback(UAVV_VIDEO handle, fnKlvDataCallback* callbackPtr, UAVV_USER_DATA pUserData);
    }; 
}

#endif


