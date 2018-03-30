/** @file uavv_image.h
    @brief Header file for UAVV Image
    @ingroup client_api
 */


#ifndef UAVV_IMAGE_H
#define UAVV_IMAGE_H


#ifdef __cplusplus
extern "C" {
#endif


/// @brief Handle to an RGBA image
typedef void* UAVV_IMAGE;


/// @name Memory manager
/// @{

/** @brief Creates a 32-bit RGBA image.
    @return Handle to a new RGBA image.  This should be destroyed using
            uavv_image_destroy() after use.
    @param width    Image width.
    @param height   Image height.
  */
UAVV_IMAGE uavv_image_create(int width, int height);

/** @brief Copies an RGBA image.
    @return Handle to a new RGBA image.  This should be destroyed using
            uavv_image_destroy() after use.
    @param src  Handle to the source image.
  */
UAVV_IMAGE uavv_image_copy(const UAVV_IMAGE src);

/** @brief Destroys an RGBA image.
    @param img  Handle to an RGBA image.
  */
void uavv_image_destroy(UAVV_IMAGE img);

/// @}


/// @name Attribute getters
/// @{

/** @brief Obtains image width.
    @return The image width if the image is valid.
    @return @p 0 if the image is not valid.
    @param img  Handle to an RGBA image.
 */
int uavv_image_width(const UAVV_IMAGE img);

/** @brief Obtains image height.
    @return The image height if the image is valid.
    @return @p 0 if the image is not valid.
    @param img  Handle to an RGBA image.
 */
int uavv_image_height(const UAVV_IMAGE img);

/** @brief Obtains image data.
    @return Pointer to image data if the image is valid.  The pixel components
            are stored in R-G-B-A ordering.  There is no padding at the end
            of each row.
    @return @p NULL if the image is not valid.
    @param img  Handle to an RGBA image.
  */
void* uavv_image_data(const UAVV_IMAGE img);

/** @brief Obtains data length.
    @return The data length.
    @return @p 0 if the image is not valid.
    @param img  Handle to an RGBA image.
  */
int uavv_image_length(const UAVV_IMAGE img);

/// @}


#ifdef __cplusplus
}
#endif


#endif  /* UAVV_IMAGE_H */


/* vim: set cindent tabstop=8 softtabstop=4 expandtab shiftwidth=4: */
