#ifndef video_player_h
#define video_player_h

#include <uavv_wrapper.h>
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <deque>

namespace cvtool
{
    typedef void (fnPositionChangedNotification)(float pos, void* pUserData);
    using fnImageCallback = uavv::fnImageDecodeCallback;
    using fnCancelCallback = uavv::fnAbortCallback;

    using fnKlvDataCallback = uavv::fnKlvDataCallback;

    using USER_DATA = uavv::UAVV_USER_DATA;

    struct VideoPlayerCallbackInfo
    {
        fnImageCallback* pfnImageCallbackNotification;
        fnKlvDataCallback* pfnGetKlvDataNotification;
        fnPositionChangedNotification* pfnPositionChangedNotification;
        USER_DATA pUserData;

        void Reset()
        {
            pfnImageCallbackNotification = nullptr;
            pfnGetKlvDataNotification = nullptr;
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
            std::atomic<bool> stopWhenFrameReceived;
            std::string sourceFile;
            std::vector<float> positionsHistory;
            std::deque<std::pair<UAVV_IMAGE, float>> previousImages;
            bool updatePositionOnDecode;
            
            std::thread decodingThread;
            std::thread nextFrameThread;
            VideoPlayerCallbackInfo callbackInfo;

        private:
            // private callbacks
            static int abort_cb(USER_DATA v);
            static void image_cb(UAVV_IMAGE img, int delay, float pos, USER_DATA v);
            static void klv_cb(UAVV_KLV klv, USER_DATA v);

            // video interface managemange routines
            void StartDecoding();
            void StopDecoding();
            void PauseDecoding();
            bool GoToPos(float pos, int timeOut, int decodeLimit, bool notifyObserver = true);
            void DecodeNextImage(int timeOut, int decodeLimit);

            void ResetPlayer();
            void ResetCancelState();

            // player state query functions
            bool IsDecodeInitialized() const;
            int IsDecodingCancelled() const;
            int IsImageDecoded() const;

            void SetCurrentPosition(float);
            void FrameReceived(UAVV_IMAGE img, int delay, float pos);
            void KlvDataReceived(UAVV_KLV klv);

            void ClearStoredImages();
            void AddImageToCache(UAVV_IMAGE img, float pos);
            bool ShowPreviousImage();
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

            void SetCallbackInfo(const VideoPlayerCallbackInfo& info);
            bool InitPlayback(const std::string& file);

            static bool InitUAVVLibrary();
            static std::string GetUAVVVersion();
    };
}

#endif