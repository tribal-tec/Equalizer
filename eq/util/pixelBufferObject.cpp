
/* Copyright (c)      2012, Maxim Makhinya <maxmah@gmail.com>
 *               2012-2013, Stefan Eilemann <eile@eyescale.ch>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "pixelBufferObject.h"

#include <eq/gl.h>
#include <lunchbox/debug.h>
#include <lunchbox/scopedMutex.h>

namespace eq
{
namespace util
{
namespace detail
{
class PixelBufferObject
{
public:
    PixelBufferObject( const GLEWContext* glewContext,
                       const bool threadSafe )
        : lock_( threadSafe ? new lunchbox::Lock : 0 )
        , pboID( 0 )
        , size( 0 )
        , _glewContext( glewContext )
        , _type( 0 )
    {
        LBVERB << (void*)this << backtrace << std::endl;
    }

    ~PixelBufferObject()
    {
        LBASSERTINFO( !isInitialized(), (void*)this << backtrace );
        if( isInitialized( ))
            LBWARN << "PBO was not freed" << std::endl;

        delete lock_;
    }

    const GLEWContext* glewGetContext() const { return _glewContext; }

    bool isInitialized() const { return pboID != 0; }

    Error setup( const size_t newSize, const GLuint type )
    {
        LBASSERT( _glewContext );

//        if( !GL_ARB_pixel_buffer_object )
//            return Error( ERROR_PBO_UNSUPPORTED );

        if( newSize == 0 )
        {
            destroy();
            return Error( ERROR_PBO_SIZE_TOO_SMALL );
        }

        if( pboID == 0 )
        {
            EQ_GL_CALL( glGenBuffers( 1, &pboID ));
        }
        if( pboID == 0 )
            return Error( ERROR_PBO_NOT_INITIALIZED );

        if( _type == type && size >= newSize )
        {
            bind();
            return Error( ERROR_NONE );
        }

        _type = type;
        size = newSize;
        bind();

        switch( type )
        {
        case GL_READ_ONLY:
            EQ_GL_CALL(
                glBufferData( GL_PIXEL_PACK_BUFFER, newSize, 0,
                                 GL_STREAM_READ ));
            return Error( ERROR_NONE );

        case GL_WRITE_ONLY:
            EQ_GL_CALL(
                glBufferData( GL_PIXEL_UNPACK_BUFFER, newSize, 0,
                                 GL_STREAM_DRAW ));
            return Error( ERROR_NONE );

        default:
            destroy();
            return Error( ERROR_PBO_TYPE_UNSUPPORTED );
        }
    }

    void destroy()
    {
        if( pboID != 0 )
        {
            unbind();
            EQ_GL_CALL( glDeleteBuffers( 1, &pboID ));
        }
        pboID = 0;
        size = 0;
        _type = 0;
    }

    const void* mapRead() const
    {
        if( !isInitialized() || _type != GL_READ_ONLY )
            return 0;

        bind();
        return glMapBuffer( _getName(), GL_READ_ONLY );
    }

    void* mapWrite()
    {
        if( !isInitialized() || _type != GL_WRITE_ONLY )
            return 0;

        bind();
        // cancel all draw operations on this buffer to prevent stalling
        EQ_GL_CALL( glBufferData( _getName(), size, 0, GL_STREAM_DRAW ));
        return glMapBuffer( _getName(), GL_WRITE_ONLY );
    }

    void unmap() const
    {
        LBASSERT( isInitialized( ));
        EQ_GL_CALL( glUnmapBuffer( _getName( )));
        unbind();
    }

    bool bind() const
    {
        if( !isInitialized( ))
            return false;

        EQ_GL_CALL( glBindBuffer( _getName(), pboID ));
        return true;
    }

    void unbind() const
    {
        LBASSERT( isInitialized( ));
        EQ_GL_CALL( glBindBuffer( _getName(), 0 ));
    }

    void lock()   const { if( lock_ ) lock_->set();   }
    void unlock() const { if( lock_ ) lock_->unset(); }

    mutable lunchbox::Lock* lock_;
    GLuint  pboID; //!< the PBO GL name
    size_t size;  //!< size of the allocated PBO buffer

private:
    const GLEWContext* const _glewContext;
    GLuint _type; //!< GL_READ_ONLY or GL_WRITE_ONLY

    GLuint _getName() const
    {
        return _type == GL_READ_ONLY ? GL_PIXEL_PACK_BUFFER :
                                       GL_PIXEL_UNPACK_BUFFER;
    }
};
}

PixelBufferObject::PixelBufferObject( const GLEWContext* glewContext,
                                      const bool threadSafe )
    : _impl( new detail::PixelBufferObject( glewContext, threadSafe ))
{
}

PixelBufferObject::~PixelBufferObject()
{
    delete _impl;
}

Error PixelBufferObject::setup( const size_t size, const unsigned type )
{
    lunchbox::ScopedWrite mutex( _impl->lock_ );
    return _impl->setup( size, type );
}

void PixelBufferObject::destroy()
{
    lunchbox::ScopedWrite mutex( _impl->lock_ );
    _impl->destroy();
}

const void* PixelBufferObject::mapRead() const
{
    _impl->lock();
    return _impl->mapRead();
}

void* PixelBufferObject::mapWrite()
{
    _impl->lock();
    return _impl->mapWrite();
}

void PixelBufferObject::unmap() const
{
    _impl->unmap();
    _impl->unlock();
}

bool PixelBufferObject::bind() const
{
    _impl->lock();
    return _impl->bind();
}

void PixelBufferObject::unbind() const
{
    _impl->unbind();
    _impl->unlock();
}

size_t PixelBufferObject::getSize() const
{
    return _impl->size;
}

bool PixelBufferObject::isInitialized() const
{
    return _impl->isInitialized();
}

bool PixelBufferObject::isThreadSafe() const
{
    return _impl->lock_ != 0;
}

GLuint PixelBufferObject::getID() const
{
    return _impl->pboID;
}

}
}
