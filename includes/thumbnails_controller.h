#ifndef thumbnails_controller_h
#define thumbnails_controller_h

#include <string>
#include <data_structures.h> 
#include <uavv_image.h>
#include <vector>
#include <deque>
#include <mutex>

#define MAX_THUMBNAILS_COUNT 10

using namespace std;

namespace cvtool
{
    class UIController;
    class ThumbnailsController
    {
        UIController* uiController;
        std::vector<ThumbnailData> thumbnails;
        std::deque<UAVV_IMAGE> thumbnailImageCache;

        std::mutex imageMutex;

        protected:
            ThumbnailsController(); 

            void SetUIController(UIController* controller);
            Fl_RGB_Image* GenerateImageThumb(UAVV_IMAGE img);
            void AddThumbnail(const ThumbnailData& data);
            void SetThumbnail(int index, const ThumbnailData& data);
            void RemoveThumbnailFromCache(UAVV_IMAGE);
            void AddThumbnailToCache(UAVV_IMAGE);
        public:

            ~ThumbnailsController();
            
            void ClearAllThumbnails();

            void AddThumbnail(UAVV_IMAGE img, const std::string& name);
            void SetThumbnail(int index, UAVV_IMAGE img, const std::string& name);
            void RemoveThumbnail(int index);
            int GetThumbnailsCount();
            
            bool GetThumbnailData(int index, std::string& name, Fl_Shared_Image*& shrimg);
            
            // contoller management functions
            static ThumbnailsController* CreateInstance(UIController* controller);

            void ThumbnailClicked(int index);
            void SwapLastImages();

            // widget event static handlers
            static void OnThumbnailClick(Fl_Widget*, void*);
            static void OnSwapImages(Fl_Widget*, void*);
    };
}

#endif

