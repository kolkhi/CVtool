#include <video_player.h>
#include <FL/fl_ask.H>
#include <signal.h>
#include <algorithm>
#include <cmath>

using namespace cvtool;
using namespace uavv;


#include "uavv_video.h"


#define LICENSE_DIR         "."
#define TIMEOUT_MS          1000
#define SCROLL_TIMEOUT_MS   500
#define MOVE_STEP           1.0
#define IMAGE_CACHE_LIMIT   5

VideoPlayer::VideoPlayer()
    : cancelDecoding(0)
    , uavvHandler(nullptr)
    , isPaused(false)
    , isDecoding(false)
    , stopWhenFrameReceived(false)
    , updatePositionOnDecode(true)
{
    callbackInfo.Reset();
    positionsHistory.resize(1, 0.0);
} 
    
VideoPlayer::~VideoPlayer()
{
    StopDecoding();

    ClearStoredImages();
    IUAVVInterface::DestroyVideoHandle(uavvHandler);
    IUAVVInterface::UnloadLibrary();
}

/*static*/ bool VideoPlayer::InitUAVVLibrary()
{
    if(!IUAVVInterface::LoadAndInitLibrary())
    {
        return false;
    }
    
    std::string currentDir(LICENSE_DIR);
    IUAVVInterface::SetLicenseDirectory(currentDir);
    return true;
}

/*static*/ std::string VideoPlayer::GetUAVVVersion()
{
    return IUAVVInterface::GetLibraryVersion();
}

bool VideoPlayer::InitPlayback(const std::string& file)
{
    sourceFile = file;
    if(sourceFile.length() == 0)
    {
        fl_message("Source path is empty.");
        return false;
    }

    ResetPlayer();

    uavvHandler = IUAVVInterface::CreateVideoHandle(sourceFile);
    if (!uavvHandler)
    {
        fl_message("Library needs to be activated.");
        return false;
    }

    IUAVVInterface::SetAbortCallback(uavvHandler, VideoPlayer::abort_cb, static_cast<USER_DATA>(this));
    IUAVVInterface::SetImageDecodeCallback(uavvHandler, VideoPlayer::image_cb, static_cast<USER_DATA>(this));
    IUAVVInterface::SetKlvDataCallback(uavvHandler, VideoPlayer::klv_cb, static_cast<USER_DATA>(this));
    
    return true;
}

void VideoPlayer::ResetPlayer()
{
    StopDecoding();

    ClearStoredImages();
    IUAVVInterface::DestroyVideoHandle(uavvHandler);
    ResetCancelState();
    uavvHandler = nullptr;
    isPaused = false;
    positionsHistory[0] = 0;
    
    if(callbackInfo.pfnPositionChangedNotification)
        callbackInfo.pfnPositionChangedNotification(GetCurrentPosition(), callbackInfo.pUserData);

}

void VideoPlayer::Play()
{
    if(isPaused)
    {
        isPaused = false;
    }
    else
    {
        if(!IsDecodeInitialized())
        {
            if(!InitPlayback(sourceFile))
                return;
        }
    }

    StartDecoding();
}
    
void VideoPlayer::Stop()
{
    ResetPlayer();
}
    
void VideoPlayer::Pause()
{
    PauseDecoding(); 
}

int VideoPlayer::IsDecodingCancelled() const
{
    return cancelDecoding;
}

/*static*/ int VideoPlayer::abort_cb(USER_DATA v)
{
    VideoPlayer* player = static_cast<VideoPlayer*>(v);
    if(!player)
        return 0;

    return player->IsDecodingCancelled();
}

void VideoPlayer::FrameReceived(UAVV_IMAGE img, int delay, float pos)
{
    stopWhenFrameReceived = true;
    positionsHistory[0] = pos;
    
    AddImageToCache(img, pos);

    if(callbackInfo.pfnImageCallbackNotification)
        callbackInfo.pfnImageCallbackNotification(img, delay, pos, callbackInfo.pUserData);
}

/*static*/ void VideoPlayer::image_cb(UAVV_IMAGE img, int delay, float pos, USER_DATA v)
{
    VideoPlayer* player = static_cast<VideoPlayer*>(v);
    if(!player)
        return;

    player->FrameReceived(img, delay, pos);
}

void VideoPlayer::KlvDataReceived(UAVV_KLV klv)
{
    if(callbackInfo.pfnGetKlvDataNotification)
        callbackInfo.pfnGetKlvDataNotification(klv, callbackInfo.pUserData);
}

/*static*/ void VideoPlayer::klv_cb(UAVV_KLV klv, USER_DATA v)
{
    VideoPlayer* player = static_cast<VideoPlayer*>(v);
    if(!player)
        return;

    player->KlvDataReceived(klv);
}

bool VideoPlayer::IsPlaying() const
{
    return isDecoding && !isPaused;
}

bool VideoPlayer::IsPaused() const
{
    return isPaused;
}

bool VideoPlayer::IsDecodeInitialized() const
{
    return (uavvHandler != nullptr);
}

void VideoPlayer::StartDecoding()
{
    if(!IsDecodeInitialized())
    {
        fl_message("Decoder is not initialized.");
        return;
    }

    if(!isDecoding)
    { 
        isDecoding = true;
        decodingThread = std::thread([this]()
        {
            while(!IsDecodingCancelled())
            {
                if(isPaused)
                    continue;

                IUAVVInterface::DecodeVideo(uavvHandler, TIMEOUT_MS);
            }

            isDecoding = false;
            ResetCancelState();
        });
    }
}

void VideoPlayer::StopDecoding()
{
    cancelDecoding = 1;  
    if(decodingThread.joinable())
        decodingThread.join();
}
    
void VideoPlayer::ResetCancelState()
{
    cancelDecoding = 0;
}

void VideoPlayer::PauseDecoding()
{
    isPaused = true;
    //StopDecoding();
}

bool VideoPlayer::SlideToPosition(float pos)
{
    ClearStoredImages();
    return GoToPos(pos, SCROLL_TIMEOUT_MS, 10, false);
}

bool VideoPlayer::GoToStart()
{
    ClearStoredImages();
    return GoToPos(0.0, TIMEOUT_MS, 60);
}
    
bool VideoPlayer::GoToEnd()
{
    ClearStoredImages();
    return GoToPos(100.0, TIMEOUT_MS, 60);
}
    
bool VideoPlayer::StepBackward()
{
    if(previousImages.size() > 0)
    {
        // remove current frame first
        auto data = previousImages.back();
        previousImages.pop_back();
        IUAVVInterface::DestroyImageHandle(data.first);
    }

    if(previousImages.size() == 0)
    {
        float newPos = 0.0;
        if(positionsHistory.size() > 0)
        {
            newPos = positionsHistory.back() - 2 * MOVE_STEP;
            newPos = std::trunc(newPos); 
        }

        return GoToPos(newPos, TIMEOUT_MS, 60);
    }

    return ShowPreviousImage();    
}

bool VideoPlayer::StepForward()
{
    if(!IsDecodeInitialized())
    {
        if(!InitPlayback(sourceFile))
            return false;
    }
    else if(isDecoding)
    {
        StopDecoding();
    }

    if(GetCurrentPosition() == 100)
        return false;

    DecodeNextImage(TIMEOUT_MS, 60);

    if(callbackInfo.pfnPositionChangedNotification)
        callbackInfo.pfnPositionChangedNotification(GetCurrentPosition(), callbackInfo.pUserData);
   
    return true;
}

bool VideoPlayer::ShowPreviousImage()
{
    if(!IsDecodeInitialized())
    {
        return false;
    }

    if(previousImages.size() == 0)
        return false;

    // can't move to position for streaming video
    if(IsVideoStreaming())
        return false;

    if(isDecoding)
    {
        StopDecoding();
    }

    auto data = previousImages.back();

    UAVV_IMAGE img = data.first;
    float pos = data.second;

    positionsHistory[0] = pos;
    
    if(callbackInfo.pfnImageCallbackNotification)
        callbackInfo.pfnImageCallbackNotification(img, 0.0, pos, callbackInfo.pUserData);

    return true;
}

bool VideoPlayer::GoToPos(float pos, int timeOut, int decodeLimit, bool notifyObserver /*=true*/)
{
    if(!IsDecodeInitialized())
    {
        return false;
    }

    // can't move to position for streaming video
    if(IsVideoStreaming())
        return false;

    if(isDecoding)
    {
        StopDecoding();
    }

    positionsHistory[0] = pos;
    IUAVVInterface::GoToPosition(uavvHandler, pos);

    DecodeNextImage(timeOut, decodeLimit);
    
    if(notifyObserver && callbackInfo.pfnPositionChangedNotification)
        callbackInfo.pfnPositionChangedNotification(GetCurrentPosition(), callbackInfo.pUserData);

    return true;
}

void VideoPlayer::DecodeNextImage(int timeOut, int decodeLimit)
{
    stopWhenFrameReceived = false;
    std::thread nextFrameThread = std::thread([this, timeOut, decodeLimit]()
    {
        for(auto i=1; i <= decodeLimit; i++)
        {
            IUAVVInterface::DecodeVideo(uavvHandler, timeOut);
            if(stopWhenFrameReceived)
                break;
        }
    });

    if(nextFrameThread.joinable())
        nextFrameThread.join();
}

bool VideoPlayer::IsVideoStreaming() const
{
    return (IUAVVInterface::IsStreamingVideo(uavvHandler) != 0);
}

float VideoPlayer::GetCurrentPosition() const
{
    if(positionsHistory.size() == 0)
        return 0.0;

    return positionsHistory.back();
}

void VideoPlayer::SetCallbackInfo(const VideoPlayerCallbackInfo& info)
{
    callbackInfo.pfnImageCallbackNotification = info.pfnImageCallbackNotification;
    callbackInfo.pfnPositionChangedNotification = info.pfnPositionChangedNotification;
    callbackInfo.pfnGetKlvDataNotification = info.pfnGetKlvDataNotification;
    callbackInfo.pUserData = info.pUserData;
}

void VideoPlayer::ClearStoredImages()
{
    for(auto i=0; i<(int)previousImages.size(); i++)
    {
        IUAVVInterface::DestroyImageHandle(previousImages[i].first);
    }
    previousImages.clear();
}

void VideoPlayer::AddImageToCache(UAVV_IMAGE img, float pos)
{
    UAVV_IMAGE newImage = IUAVVInterface::CopyImageHandle(img);
    previousImages.push_back(std::make_pair(newImage, pos));
    if((int)previousImages.size() > IMAGE_CACHE_LIMIT)
    {
        auto data = previousImages.front();
        IUAVVInterface::DestroyImageHandle(data.first);
        previousImages.pop_front();
    }
}

UAVV_IMAGE VideoPlayer::GetLastLastCachedImageCopy()
{
    if(previousImages.size() > 0)
    {
        UAVV_IMAGE newImage = IUAVVInterface::CopyImageHandle(previousImages.back().first);
        return newImage;
    }

    return nullptr;
}