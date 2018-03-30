/** @file uavv_video.h
    @brief  Header file for UAV Vision Video API
    @ingroup client_api
 */


#ifndef UAVV_VIDEO_H
#define UAVV_VIDEO_H


#include <uavv_image.h>
#include <uavv_klv.h>


#ifdef __cplusplus
extern "C" {
#endif


#define UAVV_VIDEO_FALSE    0       ///< Result for false value
#define UAVV_VIDEO_TRUE     1       ///< Result for true value


/// @brief  Handle to a video decoder
typedef void* UAVV_VIDEO;

/** @brief  Callback function to test for end of read operation.
    @details    This is called regularly by uavv_video_decode() while waiting
                for data to allow early termination of read operation.
    @return @p 0 if uavv_video_decode() should continue to wait for data.
    @return Non-zero if uavv_video_decode() should return immediately.
    @param priv Private data registered with this callback function.
  */
typedef int (uavv_video_abort_hook)(
        void* priv);

/** @brief  Callback function for streaming data.
    @details    This is called by uavv_video_decode() when data arrives from
                the source.
    @param data Pointer to the streaming data.  This pointer is valid only for
                the duration of the function call.
    @param len  Number of bytes in @a data.
    @param priv Private data registered with this callback function.
  */
typedef void (uavv_video_stream_hook)(
        const void* data, int len, void* priv);

/** @brief  Callback function for decoded frame.
    @details    This is called by uavv_video_decode() when a frame has been
                successfully decoded.
    @param img      Handle to decoded frame.  This handle is valid only
                    for the duration of the function call.
    @param delay    How many milliseconds to wait since last frame before this
                    frame should be shown.
    @param pos      Percentage position of decoded frame in source file.  This
                    value is always zero for streaming video.
    @param priv     Private data registered with this callback function.
  */
typedef void (uavv_video_image_hook)(
        UAVV_IMAGE img, int delay, float pos, void* priv);

/** @brief  Callback function for KLV data.
    @details    This is called by uavv_video_decode() when KLV data has been
                successfully decoded.
    @param klv  Handle to KLV data.  This handle is valid only for the
                duration of the function call.
    @param priv Private data registered with this callback function.
  */
typedef void (uavv_video_klv_hook)(
        UAVV_KLV klv, void* priv);


/// @name License management
/// @{

/** @brief  Specifies the license directory.
    @details    This function should be used to set the license directory
                before other functions are called.  The @p "uavv_video.lic"
                license file is expected inside this directory.  If this
                function is not called, the current directory is used.
                
                If there is no valid activation file, a @p "uavv_video.hw"
                hardware ID file will be written inside the license directory.
                This file should be sent to UAV Vision when requesting a
                software license.

    @param dir  The license directory to use for UAV Vision Video API.
  */
void
uavv_video_set_license_directory(const char* dir);

/** @brief  Queries the version number.
    @return The version string.
  */
const char*
uavv_video_version();

/// @}


/// @name Memory management
/// @{

/** @brief  Creates a video decoder.
    @return Handle to a new video decoder.  The decoder should be destroyed
            using uavv_video_destroy() after use.
    @return @p NULL if the library does not have a valid license.
    @param src      The video source in the form of a filename or a URL in the
                    form of @b "udp://host:port"
  */
UAVV_VIDEO uavv_video_create(const char* src);

/** @brief  Destroys a video decoder.
    @param vid  Handle to a video decoder.
  */
void uavv_video_destroy(UAVV_VIDEO vid);

/// @}


/// @name Callback function management
/// @{

/** @brief  Sets up an abort callback function.  Only one abort callback
            function can be set at any time.
    @param vid  Handle to a video decoder.
    @param func Callback function for streaming data.
    @param priv Private data for @a func.
  */
void uavv_video_set_abort_hook(UAVV_VIDEO vid,
        uavv_video_abort_hook* func, void* priv);

/** @brief  Sets up a stream callback function.  Only one stream callback
            function can be set at any time.
    @param vid  Handle to a video decoder.
    @param func Callback function for streaming data.
    @param priv Private data for @a func.
  */
void uavv_video_set_stream_hook(UAVV_VIDEO vid,
        uavv_video_stream_hook* func, void* priv);

/** @brief  Sets up an image callback function.  Only one image callback
            function can be set at any time.
    @param vid  Handle to a video decoder.
    @param func Callback function for decoded frame.
    @param priv Private data for @a func.
  */
void uavv_video_set_image_hook(UAVV_VIDEO vid,
        uavv_video_image_hook* func, void* priv);

/** @brief  Sets up a KLV data callback function.  Only one KLV data callback
            function can be set at any time.
    @param vid  Handle to a video decoder.
    @param func Callback function for decoded KLV data.
    @param priv Private data for @a func.
  */
void uavv_video_set_klv_hook(UAVV_VIDEO vid,
        uavv_video_klv_hook* func, void* priv);

/// @}


/// @name Video decoding
/// @{

/** @brief Decodes incoming data with timeout.
    @details    The video decoder will retrieve data from the video source and
                perform decoding.  Callback functions are invoked in the
                following situations:
                    - Whenever streaming data is received, the stream callback
                      function will be invoked.
                    - Whenever a video frame is decoded, the image callback
                      function will be invoked.
                    - Whenever KLV data is decoded, the KLV data callback
                      function will be invoked.

                The function returns immediately when a video frame has been
                successfully decoded;  otherwise it waits until the specified
                timeout duration has elapsed.

    @param vid      Handle to a video decoder.
    @param timeout  Number of milliseconds to wait before timing out.  Zero or
                    negative values mean the decoder should wait indefinitely
                    for data, and should only terminate when the abort hook
                    says so.
  */
void uavv_video_decode(UAVV_VIDEO vid, int timeout);

/** @brief Seeks to an absolute position in a video file.
    @details    This function only works with file-based video source.
    @param vid  Handle to a video decoder.
    @param pos  Absolute file position in percentage.
  */
void uavv_video_seek(UAVV_VIDEO vid, float pos);

/** @brief Checks whether the input is streaming video.
    @return @p UAVV_KLV_TRUE if the video is streaming.
    @return @p UAVV_KLV_FALSE if the video is file-based.
    @param vid  Handle to a video decoder.
  */
int uavv_video_is_streaming(UAVV_VIDEO vid);

/// @}


#ifdef __cplusplus
}
#endif


#endif  /* UAVV_VIDEO_H */


/* vim: set cindent tabstop=8 softtabstop=4 expandtab shiftwidth=4: */
