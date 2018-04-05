#ifndef video_player_h
#define video_player_h

#include <uavv_wrapper.h>
#include <string>
#include <atomic>
#include <thread>

namespace cvtool
{
    typedef void (fnPositionChangedNotification)(float pos, void* pUserData);
    using fnImageCallback = uavv::fnImageDecodeCallback;
    using fnCancelCallback = uavv::fnAbortCallback;

    using USER_DATA = uavv::UAVV_USER_DATA;

    struct VideoPlayerCallbackInfo
    {
        fnImageCallback* pfnImageCallback;
        fnPositionChangedNotification* pfnPositionChangedNotification;
        USER_DATA pUserData;

        void Reset()
        {
            pfnImageCallback = nullptr;
            pfnPositionChangedNotification = nullptr;
            pUserData = nullptr;   
        }
    };

    class VideoPlayer 
    {
        private:
            // class data 
            volatile int cancelDecoding;
            UAVV_VIDEO uavvHandler;
            std::atomic<bool> isPaused;
            std::atomic<bool> isDecoding;
            std::string sourceFile;
            float currentPosition;

            std::thread decodingThread;
            VideoPlayerCallbackInfo callbackInfo;

        private:
            // private callbacks
            static int abort_cb(USER_DATA v);

            // video iterface managemange routines
            void StartDecoding();
            void StopDecoding();
            void PauseDecoding();
            bool GoToPos(float pos, bool notifyObserver = true);

            void ResetPlayer();
            void ResetCancelState();

            // player state query functions
            bool IsDecodeInitialized() const;
            int IsDecodingCancelled() const;

        public:
            VideoPlayer();
            ~VideoPlayer();
            
            void Play();
            void Stop();
            void Pause();
            bool GoToStart();
            bool GoToEnd();
            bool StepBackward();
            bool StepForward();
            bool SlideToPosition(float pos);

            // player state query functions
            bool IsVideoStreaming() const;
            bool IsPlaying() const;
            bool IsPaused() const;
            float GetCurrentPosition() const;
            void SetCurrentPosition(float);

            void SetCallbackInfo(const VideoPlayerCallbackInfo& info);
            bool InitPlayback(const std::string& file);

            static bool InitUAVVLibrary();
            static std::string GetUAVVVersion();
    };
}

#endif