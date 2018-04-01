#ifndef video_player_h
#define video_player_h

#include <uavv_video.h>
#include <uavv_image.h>
#include <string>

#define LICENSE_DIR     "."
#define TIMEOUT_MS      60000
#define IMAGE_DUMP      "image.rgba"

namespace cvtool
{
    typedef void (*fnImageCallback) (UAVV_IMAGE img, int delay, float pos, void* pUserData);

    class VideoPlayer 
    {
        UAVV_VIDEO uavvHandler;
        bool isPaused;
        bool isDecoding;
        std::string sourceFile;
        
        fnImageCallback pfnImageCallback;
        void* pUserData;

        static int abort_cb(void* v);
        
        public:
            VideoPlayer();
            ~VideoPlayer();

            void InitPlayback(const std::string& file);
            void ResetPlayer();
            
            void Play();
            void Stop();
            void Pause();
            void GoToStart();
            void GoToEnd();
            void StepBackward();
            void StepForward();
            void GoTo(double pos);

            void SetImageDecodingCallback(fnImageCallback callback, void* userData);

            static volatile int cancelDecoding;
            
            //static void image_cb(UAVV_IMAGE img, int delay, float pos, void* v);
    };
}

#endif