#include <thumbnails_controller.h>
#include <ui_controller.h>
#include <cassert>
#include <thumbnail_widget.h>
#include <algorithm>

using namespace cvtool;
/**
 Window for thumbnail
*/

ThumbnailsController::ThumbnailsController()
    : uiController(nullptr)
{
    
}

ThumbnailsController::~ThumbnailsController()
{

}

int  ThumbnailsController::GetThumbnailsCount()
{
    return static_cast<int>(thumbnails.size());
}

ThumbnailsController* ThumbnailsController::CreateInstance(UIController* controller)
{
    ThumbnailsController* instance = new ThumbnailsController();
    instance->SetUIController(controller);
    return instance;
}

void ThumbnailsController::SetUIController(UIController* controller)
{
    uiController = controller;
}

void ThumbnailsController::AddThumbnail(UAVV_IMAGE img, const std::string& name)
{
    ThumbnailData thumbData;
    thumbData.img = img;
    thumbData.imgName = name;
    AddThumbnail(thumbData);
}

void ThumbnailsController::RemoveThumbnail(int index)
{
    if(index >= GetThumbnailsCount())
        return;

    std::lock_guard<std::mutex> lock(imageMutex);

    RemoveThumbnailFromCache(thumbnails[index].img);
    if(thumbnails[index].img)
        uavv::IUAVVInterface::DestroyImageHandle(thumbnails[index].img);

    if(thumbnails[index].imageThumb)
    {
        delete thumbnails[index].imageThumb;
        thumbnails[index].imageThumb = nullptr;
    }

    thumbnails.erase(thumbnails.begin() + index);
}

Fl_RGB_Image* ThumbnailsController::GenerateImageThumb(UAVV_IMAGE img)
{
    Fl_RGB_Image* imgRGB = uiController->ConvertImage(img);
    if(imgRGB)
    {
        // if image dimensions are  larger than thumbnail widget dimensions - scale the image 
        if (imgRGB->w() > THUMBNAIL_WIDTH || imgRGB->h() > THUMBNAIL_HEIGHT ) 
        {
            float iw = (float)THUMBNAIL_WIDTH / (float)imgRGB->w();
            float ih = (float)THUMBNAIL_HEIGHT / (float)imgRGB->h();

            Fl_RGB_Image* temp = (Fl_RGB_Image*)imgRGB->copy(imgRGB->w() * iw, imgRGB->h() * ih);
            
            delete imgRGB;
            imgRGB = temp;
            imgRGB->alloc_array = 1;
        } 

        return imgRGB;
    }   

    return nullptr;
}

void ThumbnailsController::AddThumbnail(const ThumbnailData& data)
{
    if(GetThumbnailsCount() == MAX_THUMBNAILS_COUNT)
        return;            

    std::lock_guard<std::mutex> lock(imageMutex);
    ThumbnailData dataToAdd = data;
    if(!dataToAdd.imageThumb)
        dataToAdd.imageThumb = GenerateImageThumb(data.img);

    thumbnails.push_back(dataToAdd);
}

void ThumbnailsController::SetThumbnail(int index, const ThumbnailData& data)
{
    if(index >= GetThumbnailsCount())
        return;

    std::lock_guard<std::mutex> lock(imageMutex);
    
    RemoveThumbnailFromCache(thumbnails[index].img);
    if(thumbnails[index].img)
        uavv::IUAVVInterface::DestroyImageHandle(thumbnails[index].img);

    thumbnails[index].img = data.img;
    thumbnails[index].imgName = data.imgName;
    if(thumbnails[index].imageThumb)
    {
        delete thumbnails[index].imageThumb;
        thumbnails[index].imageThumb = nullptr;
    }
    thumbnails[index].imageThumb = GenerateImageThumb(data.img);
}

void ThumbnailsController::SetThumbnail(int index, UAVV_IMAGE img, const std::string& name)
{
    ThumbnailData thumbData;
    thumbData.img = img;
    thumbData.imgName = name;
    SetThumbnail(index, thumbData);
}

void ThumbnailsController::ClearAllThumbnails()
{
    std::lock_guard<std::mutex> lock(imageMutex);
    for(int i=0; i<(int)thumbnails.size(); i++)
    {
        if(thumbnails[i].img)
            uavv::IUAVVInterface::DestroyImageHandle(thumbnails[i].img);

        if(thumbnails[i].imageThumb)
        {
            delete thumbnails[i].imageThumb;
            thumbnails[i].imageThumb = nullptr;                
        }
    }
    thumbnails.clear();
    thumbnailImageCache.clear(); 
}

bool ThumbnailsController::GetThumbnailData(int index, std::string& name, Fl_Shared_Image*& shrimg)
{
    std::lock_guard<std::mutex> lock(imageMutex);
    if(index < (int)thumbnails.size())
    {
        name = thumbnails[index].imgName;
        if(thumbnails[index].imageThumb)
            shrimg = Fl_Shared_Image::get(thumbnails[index].imageThumb, 0);
        
        return true;
    }

    return false;
}

void ThumbnailsController::AddThumbnailToCache(UAVV_IMAGE img)
{
    thumbnailImageCache.push_back(img);
    if(thumbnailImageCache.size() == 3)
        thumbnailImageCache.pop_front();
}

void ThumbnailsController::RemoveThumbnailFromCache(UAVV_IMAGE img)
{
    thumbnailImageCache.erase(std::remove_if(thumbnailImageCache.begin(), thumbnailImageCache.end(),
                              [img](UAVV_IMAGE x){ return (x == img); }), thumbnailImageCache.end());
}

void ThumbnailsController::ThumbnailClicked(int index)
{
    assert(uiController);
    if(index < (int)thumbnails.size())
    {   
        uiController->Stop();
        uiController->ShowImage(thumbnails[index].img);
        AddThumbnailToCache(thumbnails[index].img);
    }
}

/*static*/ void ThumbnailsController::OnThumbnailClick(Fl_Widget* widget, void* pUserData)
{
    ThumbnailsController* controller = static_cast<ThumbnailsController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    Thumbnail* thumb = static_cast<Thumbnail*>(widget);
    controller->ThumbnailClicked(thumb->GetCurrentIndex());
}

void ThumbnailsController::SwapLastImages()
{
    if(thumbnailImageCache.size() < 2)
        return;

    assert(uiController);
    swap(thumbnailImageCache[0], thumbnailImageCache[1]);
    uiController->Stop();
    uiController->ShowImage(thumbnailImageCache[1]);
}

/*static*/ void ThumbnailsController::OnSwapImages(Fl_Widget*, void* pUserData)
{
    ThumbnailsController* controller = static_cast<ThumbnailsController*>(pUserData); 
    if(!controller)
    {
        return;
    }

    controller->SwapLastImages();
}