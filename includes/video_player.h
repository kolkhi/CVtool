#ifndef video_player_h
#define video_player_h

#include <uavv_wrapper.h>
#include <string>
#include <atomic>
#include <thread>

#define LICENSE_DIR     "."
#define TIMEOUT_MS      60000
#define IMAGE_DUMP      "image.rgba"

namespace cvtool
{
    using fnImageCallback = uavv::fnImageDecodeCallback;
    using fnCancelCallback = uavv::fnAbortCallback;

    using USER_DATA = uavv::UAVV_USER_DATA;

    class VideoPlayer 
    {
        private:
            // class data 
            volatile int cancelDecoding;
            UAVV_VIDEO uavvHandler;
            std::atomic<bool> isPaused;
            std::atomic<bool> isDecoding;
            std::string sourceFile;

            fnImageCallback* pfnImageCallback;
            void* pUserData;

            std::thread decodingThread;
        private:
            // private callbacks
            static int abort_cb(USER_DATA v);

            // video iterface managemange routines
            void StartDecoding();
            void StopDecoding();
            void PauseDecoding();
            bool MoveToPos(double pos);

            bool InitPlayback(const std::string& file);
            void ResetPlayer();

            bool IsDecodeInitialized();
            void ResetCancelState();
            int IsDecodingCancelled();

        public:
            VideoPlayer();
            ~VideoPlayer();
            
            void Play();
            void Stop();
            void Pause();
            bool GoToStart();
            bool GoToEnd();
            bool StepBackward(double curPos);
            bool StepForward(double curPos);
            bool GoTo(double pos);

            bool IsVideoStreaming();
            bool IsPlaying();

            void SetImageDecodingCallback(fnImageCallback callback, USER_DATA userData);
            void SetVideoSource(std::string& filePath);

            static bool InitUAVVLibrary();
            static std::string GetUAVVVersion();
    };
}

#endif