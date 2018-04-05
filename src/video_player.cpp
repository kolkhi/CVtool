#include <video_player.h>
#include <FL/fl_ask.H>
#include <signal.h>

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
    , currentPosition(0.0)
{
    callbackInfo.Reset();
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
    IUAVVInterface::SetImageDecodeCallback(uavvHandler, callbackInfo.pfnImageCallback, callbackInfo.pUserData);
    return true;
}

void VideoPlayer::ResetPlayer()
{
    StopDecoding();

    IUAVVInterface::DestroyVideoHandle(uavvHandler);
    ResetCancelState();
    uavvHandler = nullptr;
    isPaused = false;
    SetCurrentPosition(0.0);

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
    
bool VideoPlayer::GoToStart()
{
    return GoToPos(0.0);
}
    
bool VideoPlayer::GoToEnd()
{
    return GoToPos(100.0);
}
    
bool VideoPlayer::StepBackward()
{
    float newPos = currentPosition - MOVE_STEP;
    return GoToPos(newPos);
}

bool VideoPlayer::StepForward()
{
    float newPos = currentPosition + MOVE_STEP;
    return GoToPos(newPos);
}

bool VideoPlayer::SlideToPosition(float pos)
{
    return GoToPos(pos, false);
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
    
bool VideoPlayer::GoToPos(float pos, bool notifyObserver /*=true*/)
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

    
    SetCurrentPosition(pos);
    IUAVVInterface::GoToPosition(uavvHandler, static_cast<float>(GetCurrentPosition()));    
    IUAVVInterface::DecodeVideo(uavvHandler, SCROLL_TIMEOUT_MS);
    
    if(notifyObserver && callbackInfo.pfnPositionChangedNotification)
        callbackInfo.pfnPositionChangedNotification(GetCurrentPosition(), callbackInfo.pUserData);

    return true;
}

bool VideoPlayer::IsVideoStreaming() const
{
    return (IUAVVInterface::IsStreamingVideo(uavvHandler) != 0);
}

float VideoPlayer::GetCurrentPosition() const
{
    return currentPosition;
}

void VideoPlayer::SetCallbackInfo(const VideoPlayerCallbackInfo& info)
{
    callbackInfo.pfnImageCallback = info.pfnImageCallback;
    callbackInfo.pfnPositionChangedNotification = info.pfnPositionChangedNotification;
    callbackInfo.pUserData = info.pUserData;
}

void VideoPlayer::SetCurrentPosition(float pos)
{
    currentPosition = pos;
    if(currentPosition < 0.0)
        currentPosition = 0.0;
    
    if(currentPosition > 100.0)
        currentPosition = 100.0;
}