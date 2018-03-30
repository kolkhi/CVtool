/** @file gl_uavv_image.h
    @brief Header file for GLFrame class
  */


#ifndef GL_UAVV_IMAGE_H
#define GL_UAVV_IMAGE_H

#include <uavv_image.h>
#include <stdint.h>
#include <FL/gl.h>
#include <FL/glu.h>


#ifndef GL_GENERATE_MIPMAP_SGIS
#  define GL_GENERATE_MIPMAP_SGIS       0x8191
#endif


/** @class GLUavvImage
    @brief OpenGL image renderer.
    @ingroup utility
  */
class GLUavvImage
{
private:
    size_t             mFrameW;     ///< Frame width
    size_t             mFrameH;     ///< Frame height
    GLuint*            mTiles;      ///< Texture tiles arranged by row
    int                mRow;        ///< Number of tiles vertically
    int                mCol;        ///< Number of tiles horizontally
    UAVV_IMAGE         mRgba;      ///< RGBA buffer for tile generation

    /** @brief Creates a single tile from the frame buffer.
        @param buf  The frame buffer to copy from.  @p NULL is supported.
      */
    void createSingleTile(const UAVV_IMAGE buf)
    {
        // Resize RGBA buffer to frame buffer size
        size_t w = uavv_image_width(buf);
        size_t h = uavv_image_height(buf);
        if (w != (size_t) uavv_image_width(mRgba) || h != (size_t) uavv_image_height(mRgba))
        {
            uavv_image_destroy(mRgba);
            mRgba = uavv_image_create(w, h);
        }

        // Create OpenGL texture ID
        if (!mTiles)
        {
            mTiles = new GLuint[1];
            memset(mTiles, 0, sizeof(GLuint));
            mRow   = 1;
            mCol   = 1;
            glGenTextures(1, mTiles);
        }

        // Setup source buffer
        uint8_t* data = (uint8_t*) uavv_image_data(buf);

        // Create OpenGL texture
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, mTiles[0]);
        glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0,
                GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // Clean up after an error
        if (glGetError() != GL_NO_ERROR)
        {
            glDeleteTextures(1, mTiles);
            delete [] mTiles;
            mTiles = NULL;
        }
    }

    /// @brief Draws a single tile.
    void drawSingleTile() const
    {
        int x1 = 0;
        int y1 = mFrameH;
        int x2 = mFrameW;
        int y2 = 0;

        glBindTexture(GL_TEXTURE_2D, mTiles[0]);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2d(1, 1); glVertex2i(x2, y2);
        glTexCoord2d(1, 0); glVertex2i(x2, y1);
        glTexCoord2d(0, 1); glVertex2i(x1, y2);
        glTexCoord2d(0, 0); glVertex2i(x1, y1);
        glEnd();
    }

public:
    /** @brief Default constructor.
        @param id   Object identifier.
      */
    GLUavvImage() : mFrameW(0), mFrameH(0), mTiles(NULL), mRow(0), mCol(0)
    {
    }

    /// @brief Destructor.
    virtual ~GLUavvImage()
    {
        // Clean up
        copy(NULL);
        uavv_image_destroy(mRgba);
    }

    /// @brief Checks whether a GLUavvImage is valid
    inline bool isValid() const  { return (mTiles != NULL); }

    /// @brief Obtains the frame width
    inline int width() const  { return mFrameW; }
    
    /// @brief Obtains the frame height
    inline int height() const  { return mFrameH; }

    /** @brief Copies from a frame buffer.
        @details    This method should be called from Fl_Gl_Window::draw().
        @param buf  The frame buffer to copy from.  @p NULL is supported.
      */
    void copy(const UAVV_IMAGE buf)
    {
        // Reset variables if framebuffer has a different size
        size_t w = buf ? uavv_image_width(buf) : 0;
        size_t h = buf ? uavv_image_height(buf) : 0;
        if ((mFrameW != w) || (mFrameH != h))
        {
            if (mTiles)
            {
                glDeleteTextures(mRow * mCol, mTiles);
                delete [] mTiles;
                mTiles = NULL;
            }
            mRow = 0;
            mCol = 0;
        }

        // Do nothing if the source buffer is NULL
        if (!buf)
        {
            mFrameW = 0;
            mFrameH = 0;
            return;
        }

        // Save frame attributes
        mFrameW = w;
        mFrameH = h;

        // Try to create a single tile
        if (!mTiles || (((size_t) uavv_image_width(mRgba) == w) && ((size_t)uavv_image_height(mRgba) == h)))
            createSingleTile(buf);
    }

    /** @brief Draws the GL frame with it's bottom-left corner at origin.
        @details    This method should be called from Fl_Gl_Window::draw().
      */
    void draw() const
    {
        // Make sure we have prepared textures
        if (!mTiles)  return;

        // Save OpenGL states
        glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
        glEnable(GL_TEXTURE_2D);

        size_t w = uavv_image_width(mRgba);
        size_t h = uavv_image_height(mRgba);
        if ((w == mFrameW) && (h == mFrameH))
            drawSingleTile();

#ifdef DEBUG_GL_FRAME
        glDisable(GL_TEXTURE_2D);
        glColor4f(1, 1, 1, 0.5);
        glRecti(0, 0, mFrameW, mFrameH);
#endif

        // Restore OpenGL states
        glPopAttrib();
    }
};


#endif  /* GL_UAVV_IMAGE_H */


/* vim: set cindent tabstop=8 softtabstop=4 expandtab shiftwidth=4: */
