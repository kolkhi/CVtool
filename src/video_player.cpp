#include <video_player.h>
#include <FL/fl_ask.H>
#include <signal.h>

using namespace cvtool;
using namespace uavv;


#include "uavv_video.h"


#define LICENSE_DIR     "."
#define TIMEOUT_MS      60000

#define STREAM_DUMP     "stream.ts"
#define IMAGE_DUMP      "image.rgba"
#define KLV_DUMP        "klv.txt"

VideoPlayer::VideoPlayer()
    : cancelDecoding(0)
    , uavvHandler(nullptr)
    , isPaused(false)
    , isDecoding(false)
    , pfnImageCallback(nullptr)
    , pUserData(nullptr)
{

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
    if(sourceFile.length() == 0)
    {
        fl_message("Source path is empty.");
        return false;
    }

    ResetPlayer();

    uavvHandler = IUAVVInterface::CreateVideoHandle(file);
    if (!uavvHandler)
    {
        fl_message("Library needs to be activated.");
        return false;
    }

    IUAVVInterface::SetAbortCallback(uavvHandler, VideoPlayer::abort_cb, static_cast<UAVV_USER_DATA>(this));
    IUAVVInterface::SetImageDecodeCallback(uavvHandler, pfnImageCallback, pUserData);
    return true;
}

void VideoPlayer::SetVideoSource(std::string& filePath)
{
    sourceFile = filePath;
}

void VideoPlayer::ResetPlayer()
{
    StopDecoding();
}

void VideoPlayer::Play()
{
    if(isPaused)
    {
        isPaused = false;
    }
    else
    {
        if(!InitPlayback(sourceFile))
            return;

        StartDecoding();
    }

    //fl_message("Play video");
}
    
void VideoPlayer::Stop()
{
    StopDecoding();
    //fl_message("Stop video playback");
}
    
void VideoPlayer::Pause()
{
    PauseDecoding();
    //fl_message("Pause video playback");
}
    
bool VideoPlayer::GoToStart()
{
    return GoTo(0.0);
    //fl_message("Go to start");
}
    
bool VideoPlayer::GoToEnd()
{
    return GoTo(100.0);
    //fl_message("Go to end");
}
    
bool VideoPlayer::StepBackward(double curPos)
{
    double newPos = curPos - 0.1;
    if(newPos < 0)
        newPos = 0;
        
    return GoTo(newPos);
}

bool VideoPlayer::StepForward(double curPos)
{
    double newPos = curPos + 0.1;
    if(newPos > 100.0)
        newPos = 100.0;
    
    return GoTo(newPos);
}

bool VideoPlayer::GoTo(double pos)
{
    return MoveToPos(pos);
}

int VideoPlayer::IsDecodingCancelled()
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

void VideoPlayer::SetImageDecodingCallback(fnImageCallback callback, USER_DATA userData)
{
    pfnImageCallback = callback;
    pUserData = userData;
}

bool VideoPlayer::IsPlaying()
{
    return isDecoding && !isPaused;
}

bool VideoPlayer::IsDecodeInitialized()
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

    IUAVVInterface::DestroyVideoHandle(uavvHandler);
    ResetCancelState();
    uavvHandler = nullptr;
    isPaused = false;
}
    
void VideoPlayer::ResetCancelState()
{
    cancelDecoding = 0;
}

void VideoPlayer::PauseDecoding()
{
    isPaused = true;
}
    
bool VideoPlayer::MoveToPos(double pos)
{
    if(!IsDecodeInitialized())
        return false;

    // can't move to position for streaming video
    if(IsVideoStreaming())
        return false;

    if(isDecoding)
    {
        isPaused = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    IUAVVInterface::GoToPosition(uavvHandler, static_cast<float>(pos));
    return true;
}

bool VideoPlayer::IsVideoStreaming()
{
    return (IUAVVInterface::IsStreamingVideo(uavvHandler) != 0);
}