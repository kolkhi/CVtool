#include <video_player.h>
#include <FL/fl_ask.H>
#include <signal.h>

using namespace cvtool;


#include "uavv_video.h"


#define LICENSE_DIR     "."
#define TIMEOUT_MS      60000

#define STREAM_DUMP     "stream.ts"
#define IMAGE_DUMP      "image.rgba"
#define KLV_DUMP        "klv.txt"

volatile int VideoPlayer::cancelDecoding = 0;

VideoPlayer::VideoPlayer()
    : uavvHandler(nullptr)
    , isPaused(false)
    , isDecoding(false)
{

} 
    
VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::InitPlayback(const std::string& file)
{
    if(sourceFile == file)
        return;

    ResetPlayer();

#if TODO
    // Setup licensing
    uavv_video_set_license_directory(LICENSE_DIR);

    // Create video decoder and setup callback functions
    uavvHandler = uavv_video_create(sourceFile.c_str());
    
    if (!uavvHandler)
    {
        printf("Library needs to be activated\n");
        return EXIT_FAILURE;
    }

    uavv_video_set_abort_hook(uavvHandler, VideoPlayer::abort_cb, nullptr);
    uavv_video_set_image_hook(uavvHandler, pfnImageCallback, pUserData);

#endif
}

void VideoPlayer::ResetPlayer()
{

}

void VideoPlayer::Play()
{
    fl_message("Play video");
}
    
void VideoPlayer::Stop()
{
    fl_message("Stop video playback");
}
    
void VideoPlayer::Pause()
{
    fl_message("Pause video playback");
}
    
void VideoPlayer::GoToStart()
{
    fl_message("Go to start");
}
    
void VideoPlayer::GoToEnd()
{
    fl_message("Go to end");
}
    
void VideoPlayer::StepBackward()
{
    fl_message("Step backward");
}

void VideoPlayer::StepForward()
{
    fl_message("Step forward");
}

void VideoPlayer::GoTo(double pos)
{
    fl_message("Slider pos changed %.2f", pos);
}

/*static*/ int VideoPlayer::abort_cb(void* v)
{
    return cancelDecoding;
}

void VideoPlayer::SetImageDecodingCallback(fnImageCallback callback, void* userData)
{
    pfnImageCallback = callback;
    pUserData = userData;
}
