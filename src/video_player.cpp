#include <video_player.h>
#include <FL/fl_ask.H>
#include <signal.h>
#include <algorithm>
#include <cmath>

using namespace cvtool;
using namespace uavv;


#include "uavv_video.h"


#define LICENSE_DIR         "."
#define TIMEOUT_MS          10000
#define SCROLL_TIMEOUT_MS   500
#define MOVE_STEP           1.0

VideoPlayer::VideoPlayer()
    : cancelDecoding(0)
    , uavvHandler(nullptr)
    , isPaused(false)
    , isDecoding(false)
    , updatePositionOnDecode(true)
    , imageDecoded(false)
{
    callbackInfo.Reset();
    positionsHistory.resize(1, 0.0);
} 
    
VideoPlayer::~VideoPlayer()
{
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
    imageDecoded = true;
    if(updatePositionOnDecode)
    {
        positionsHistory[0] = pos;
    }

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
                IUAVVInterface::DecodeVideo(uavvHandler, TIMEOUT_MS);
            }

            isDecoding = false;
            ResetCancelState();
            IUAVVInterface::DecodeVideo(uavvHandler, TIMEOUT_MS);
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
    StopDecoding();
}

bool VideoPlayer::SlideToPosition(float pos)
{
    return GoToPos(pos, SCROLL_TIMEOUT_MS, false);
}

bool VideoPlayer::GoToStart()
{
    return GoToPos(0.0, TIMEOUT_MS);
}
    
bool VideoPlayer::GoToEnd()
{
    return GoToPos(100.0, TIMEOUT_MS);
}
    
bool VideoPlayer::StepBackward()
{
    float newPos = 0.0;
    if(positionsHistory.size() > 0)
    {
        newPos = positionsHistory.back() - MOVE_STEP;
    }

    /*if(positionsHistory.size() > 2) // more than 2 frame
    {   
        // make 2 step back to get previous image when decoded  
        for(auto i=1; i <= 2; i++)
        {
            float lastPosition = positionsHistory.back();
            positionsHistory.pop_back();
        
            auto diff = fabs(positionsHistory.back() - lastPosition);
            if(diff <= MOVE_STEP)
            {
                // if previous postion is less than default position step - get previous position
                newPos = positionsHistory.back();
            }
            else
            {
                // else make one default step back
                newPos = lastPosition - MOVE_STEP;
            }
        }
    }*/  

    return GoToPos(newPos, TIMEOUT_MS);
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

    DecodeNextImage(TIMEOUT_MS);
   
    return true;
}

bool VideoPlayer::GoToPos(float pos, int timeOut, bool notifyObserver /*=true*/)
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

    // remove all positions from history that are greate than target position
    /*auto it = std::remove_if(positionsHistory.begin(), 
                                positionsHistory.end(), 
                                [pos](float val) {  return val >= pos; } );

    positionsHistory.erase(it, positionsHistory.end());*/

    positionsHistory[0] = pos;
    IUAVVInterface::GoToPosition(uavvHandler, pos);

    updatePositionOnDecode = false;
    DecodeNextImage(timeOut, notifyObserver);
    updatePositionOnDecode = true;

    return true;
}

void VideoPlayer::DecodeNextImage(int timeOut, bool notifyObserver /*=true*/ )
{
    
    IUAVVInterface::DecodeVideo(uavvHandler, timeOut);
    
    if(notifyObserver && callbackInfo.pfnPositionChangedNotification)
        callbackInfo.pfnPositionChangedNotification(GetCurrentPosition(), callbackInfo.pUserData);
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
