#include <uavv_wrapper.h>
#include <xloader.h>
#include <cassert>

using namespace uavv;

/*static*/ UAVVHANDLE IUAVVInterface::uavvHandle = nullptr;        
/*static*/ fnSetLicenseDirectory IUAVVInterface::pfnSetLicenseDirectory = nullptr;
/*static*/ fnGetLibraryVersion IUAVVInterface::pfnGetLibraryVersion = nullptr; 
/*static*/ fnCreateVideoHandle IUAVVInterface::pfnCreateVideoHandle = nullptr; 
/*static*/ fnIsVideoStreaming IUAVVInterface::pfnIsVideoStreaming = nullptr;
/*static*/ fnDecodeVideo IUAVVInterface::pfnDecodeVideo = nullptr; 
/*static*/ fnDestroyVideoHandle IUAVVInterface::pfnDestroyVideoHandle = nullptr;
/*static*/ fnGetImageData IUAVVInterface::pfnGetImageData = nullptr;
/*static*/ fnGetImageLength IUAVVInterface::pfnGetImageLength = nullptr; 
/*static*/ fnGetImageWidth IUAVVInterface::pfnGetImageWidth = nullptr;
/*static*/ fnGetImageHeight IUAVVInterface::pfnGetImageHeight = nullptr;
/*static*/ fnSetAbortCallback IUAVVInterface::pfnSetAbortCallback = nullptr;
/*static*/ fnSetImageDecodeCallback IUAVVInterface::pfnSetImageDecodeCallback = nullptr;
/*static*/ fnSetKlvDataCallback IUAVVInterface::pfnSetKlvDataCallback = nullptr; 
/*static*/ fnCopyImageHandle IUAVVInterface::pfnCopyImageHandle = nullptr;
/*static*/ fnCreateImageHandle IUAVVInterface::pfnCreateImageHandle = nullptr;
/*static*/ fnDestroyImageHandle IUAVVInterface::pfnDestroyImageHandle = nullptr;
/*static*/ fnGoToPosition IUAVVInterface::pfnGoToPosition = nullptr;
/*static*/ fnKlvGetTimeStamp IUAVVInterface::pfnKlvGetTimeStamp = nullptr;
/*static*/ fnKlvSize IUAVVInterface::pfnKlvSize = nullptr;
/*static*/ fnKlvItem IUAVVInterface::pfnKlvItem = nullptr;
/*static*/ fnKlvItemName IUAVVInterface::pfnKlvItemName = nullptr;
/*static*/ fnKlvGetString IUAVVInterface::pfnKlvGetString = nullptr;
/*static*/ fnKlvGetError IUAVVInterface::pfnKlvGetError = nullptr;

/*static*/ bool IUAVVInterface::LoadAndInitLibrary()
{
    uavvHandle = xloader_open(LIBNAME);
    if(uavvHandle == nullptr)
    {
        return false;
    }   

    return InitFunctionDefaultCallbacks();
}

/*static*/ bool IUAVVInterface::InitFunctionDefaultCallbacks()
{
    pfnSetLicenseDirectory = (fnSetLicenseDirectory)(xloader_lookup_symbol(uavvHandle, "uavv_video_set_license_directory"));
    if(pfnSetLicenseDirectory == nullptr)
        return false;

    pfnGetLibraryVersion = (fnGetLibraryVersion)(xloader_lookup_symbol(uavvHandle, "uavv_video_version"));
    if(pfnGetLibraryVersion == nullptr)
        return false;

    pfnCreateVideoHandle = (fnCreateVideoHandle)(xloader_lookup_symbol(uavvHandle, "uavv_video_create"));
    if(pfnCreateVideoHandle == nullptr)
        return false;

    pfnIsVideoStreaming = (fnIsVideoStreaming)(xloader_lookup_symbol(uavvHandle, "uavv_video_is_streaming"));
    if(pfnIsVideoStreaming == nullptr)
        return false;

    pfnDecodeVideo = (fnDecodeVideo)(xloader_lookup_symbol(uavvHandle, "uavv_video_decode"));
    if(pfnDecodeVideo == nullptr)
        return false;
        
    pfnDestroyVideoHandle = (fnDestroyVideoHandle)(xloader_lookup_symbol(uavvHandle, "uavv_video_destroy"));
    if(pfnDestroyVideoHandle == nullptr)
        return false;
        
    pfnGetImageData = (fnGetImageData)(xloader_lookup_symbol(uavvHandle, "uavv_image_data"));
    if(pfnGetImageData == nullptr)
        return false;
        
    pfnGetImageLength = (fnGetImageLength)(xloader_lookup_symbol(uavvHandle, "uavv_image_length"));
    if(pfnGetImageLength == nullptr)
        return false;
        
    pfnGetImageWidth = (fnGetImageWidth)(xloader_lookup_symbol(uavvHandle, "uavv_image_width"));
    if(pfnGetImageWidth == nullptr)
        return false;
        
    pfnGetImageHeight = (fnGetImageHeight)(xloader_lookup_symbol(uavvHandle, "uavv_image_height"));
    if(pfnGetImageHeight == nullptr)
        return false;
        
    pfnSetAbortCallback = (fnSetAbortCallback)(xloader_lookup_symbol(uavvHandle, "uavv_video_set_abort_hook"));
    if(pfnSetAbortCallback == nullptr)
        return false;
        
    pfnSetImageDecodeCallback = (fnSetImageDecodeCallback)(xloader_lookup_symbol(uavvHandle, "uavv_video_set_image_hook"));
    if(pfnSetImageDecodeCallback == nullptr)
        return false;

    pfnSetKlvDataCallback = (fnSetKlvDataCallback)(xloader_lookup_symbol(uavvHandle, "uavv_video_set_klv_hook"));
    if(pfnSetKlvDataCallback == nullptr)
        return false;

    pfnCopyImageHandle = (fnCopyImageHandle)(xloader_lookup_symbol(uavvHandle, "uavv_image_copy"));
    if(pfnCopyImageHandle == nullptr)
        return false;
        
    pfnDestroyImageHandle = (fnDestroyImageHandle)(xloader_lookup_symbol(uavvHandle, "uavv_image_destroy"));
    if(pfnDestroyImageHandle == nullptr)
        return false;

    pfnCreateImageHandle = (fnCreateImageHandle)(xloader_lookup_symbol(uavvHandle, "uavv_image_create"));
    if(pfnCreateImageHandle == nullptr)
        return false;

    pfnGoToPosition = (fnGoToPosition)(xloader_lookup_symbol(uavvHandle, "uavv_video_seek"));
    if(pfnGoToPosition == nullptr)
        return false;

    // klv functions
    pfnKlvGetTimeStamp = (fnKlvGetTimeStamp)(xloader_lookup_symbol(uavvHandle, "uavv_klv_get_timestamp"));
    if(pfnKlvGetTimeStamp == nullptr)
        return false;

    pfnKlvSize = (fnKlvSize)(xloader_lookup_symbol(uavvHandle, "uavv_klv_size"));
    if(pfnKlvSize == nullptr)
        return false;

    pfnKlvItem = (fnKlvItem)(xloader_lookup_symbol(uavvHandle, "uavv_klv_item"));
    if(pfnKlvItem == nullptr)
        return false;

    pfnKlvItemName = (fnKlvItemName)(xloader_lookup_symbol(uavvHandle, "uavv_klv_item_name"));
    if(pfnKlvItemName == nullptr)
        return false;
    
    pfnKlvGetString = (fnKlvGetString)(xloader_lookup_symbol(uavvHandle, "uavv_klv_get_str"));
    if(pfnKlvGetString == nullptr)
        return false;
    
    pfnKlvGetError = (fnKlvGetError)(xloader_lookup_symbol(uavvHandle, "uavv_klv_get_error"));
    if(pfnKlvGetError == nullptr)
        return false;

    return true;
}

/*static*/ void IUAVVInterface::UnloadLibrary()
{
    xloader_close(uavvHandle);
}


/*static*/ void IUAVVInterface::SetLicenseDirectory(std::string& path)
{
    assert(pfnSetLicenseDirectory);
    if(!pfnSetLicenseDirectory)
        return;

    pfnSetLicenseDirectory(path.c_str()); 
}
            
/*static*/ std::string IUAVVInterface::GetLibraryVersion()
{
    assert(pfnGetLibraryVersion);
    if(!pfnGetLibraryVersion)
        return "n/a";

    const char* libver = pfnGetLibraryVersion();
    return std::string(libver);    
}

/*static*/ UAVV_VIDEO IUAVVInterface::CreateVideoHandle(const std::string& filePath)
{
    assert(pfnCreateVideoHandle);
    if(!pfnCreateVideoHandle)
        return nullptr;

    return pfnCreateVideoHandle(filePath.c_str());
}

/*static*/ int IUAVVInterface::IsStreamingVideo(UAVV_VIDEO handle)
{
    assert(pfnIsVideoStreaming);
    if(!pfnIsVideoStreaming)
        return 0;
    
    return pfnIsVideoStreaming(handle);
}

/*static*/ void IUAVVInterface::DestroyVideoHandle(UAVV_VIDEO handle)
{
    assert(pfnDestroyVideoHandle);
    if(!pfnDestroyVideoHandle)
        return;

    pfnDestroyVideoHandle(handle);
}

/*static*/ void IUAVVInterface::DecodeVideo(UAVV_VIDEO handle, int timeOutMS)
{
    assert(pfnDecodeVideo);
    if(!pfnDecodeVideo)
        return;

    pfnDecodeVideo(handle, timeOutMS);
}

/*static*/ void* IUAVVInterface::GetImageData(UAVV_IMAGE img)
{
    assert(pfnGetImageData);
    if(!pfnGetImageData)
        return nullptr;

    return pfnGetImageData(img);
}

/*static*/ int IUAVVInterface::GetImageWidth(UAVV_IMAGE img)
{
    assert(pfnGetImageWidth);
    if(!pfnGetImageWidth)
        return 0;

    return pfnGetImageWidth(img);
}

/*static*/ int IUAVVInterface::GetImageHeight(UAVV_IMAGE img)
{
    assert(pfnGetImageHeight);
    if(!pfnGetImageHeight)
        return 0;

    return pfnGetImageHeight(img);
}

/*static*/ int IUAVVInterface::GetImageLength(UAVV_IMAGE img)
{
    assert(pfnGetImageLength);
    if(!pfnGetImageLength)
        return 0;
    
    return pfnGetImageLength(img);
}

/*static*/ void IUAVVInterface::SetAbortCallback(UAVV_VIDEO handle, fnAbortCallback* callbackPtr, UAVV_USER_DATA pUserData)
{
    assert(pfnSetAbortCallback);
    if(!pfnSetAbortCallback)
        return;
    
    pfnSetAbortCallback(handle, callbackPtr, pUserData);
}

/*static*/ void IUAVVInterface::SetImageDecodeCallback(UAVV_VIDEO handle, fnImageDecodeCallback* callbackPtr, UAVV_USER_DATA pUserData)
{
    assert(pfnSetImageDecodeCallback);
    if(!pfnSetImageDecodeCallback)
        return;

    pfnSetImageDecodeCallback(handle, callbackPtr, pUserData);
}

/*static*/ void IUAVVInterface::SetKlvDataCallback(UAVV_VIDEO handle, fnKlvDataCallback* callbackPtr, UAVV_USER_DATA pUserData)
{
    assert(pfnSetKlvDataCallback);
    if(!pfnSetKlvDataCallback)
        return;

    pfnSetKlvDataCallback(handle, callbackPtr, pUserData);
}

/*static*/ UAVV_IMAGE IUAVVInterface::CreateImageHandle(int width, int height)
{
    assert(pfnCreateImageHandle);
    if(!pfnCreateImageHandle)
        return nullptr;

    return pfnCreateImageHandle(width, height);
}

/*static*/ UAVV_IMAGE IUAVVInterface::CopyImageHandle(UAVV_IMAGE img)
{
    assert(pfnCopyImageHandle);
    if(!pfnCopyImageHandle)
        return nullptr;

    return pfnCopyImageHandle(img);
}

/*static*/ void IUAVVInterface::DestroyImageHandle(UAVV_IMAGE img)
{
    assert(pfnDestroyImageHandle);
    if(!pfnDestroyImageHandle)
        return;

    pfnDestroyImageHandle(img);
}

/*static*/ void IUAVVInterface::GoToPosition(UAVV_VIDEO handle, float pos)
{
    assert(pfnGoToPosition);
    if(!pfnGoToPosition)
        return;

    pfnGoToPosition(handle, pos);
}
            
/*static*/ long long IUAVVInterface::KlvGetTimeStamp(const UAVV_KLV klv)
{
    assert(pfnKlvGetTimeStamp);
    if(!pfnKlvGetTimeStamp)
        return 0;

    return pfnKlvGetTimeStamp(klv); 
}

/*static*/ int IUAVVInterface::KlvSize(const UAVV_KLV klv)
{
    assert(pfnKlvSize);
    if(pfnKlvSize == nullptr)
        return 0;

    return pfnKlvSize(klv);
}

/*static*/ uavv_klv_key_t IUAVVInterface::KlvItem(const UAVV_KLV klv, int offset)
{
    assert(pfnKlvItem);
    if(pfnKlvItem == nullptr)
        return ((uavv_klv_key_t)-1);

    return pfnKlvItem(klv, offset);
}

/*static*/ std::string IUAVVInterface::KlvItemName(uavv_klv_key_t key)
{
    assert(pfnKlvItemName);
    if(pfnKlvItemName == nullptr)
        return "";

    const char* itemName = pfnKlvItemName(key);
    return std::string(itemName);
}

/*static*/ std::string IUAVVInterface::KlvGetString(const UAVV_KLV klv, uavv_klv_key_t key)
{
    assert(pfnKlvGetString);
    if(pfnKlvGetString == nullptr)
        return "";

    const char* itemString = pfnKlvGetString(klv, key);
    return std::string(itemString);
}

/*static*/ bool IUAVVInterface::KlvIsInErrorState(const UAVV_KLV klv, uavv_klv_key_t key)
{
    assert(pfnKlvGetError);
    if(pfnKlvGetError == nullptr)
        return false;
    
    return (pfnKlvGetError(klv, key) == UAVV_KLV_TRUE);
}