
/* Copyright (c) 2006-2014, Stefan Eilemann <eile@equalizergraphics.com>
 *               2011-2014, Daniel Nachbaur <danielnachbaur@gmail.com>
 *                    2010, Cedric Stalder <cedric.stalder@gmail.com>
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

#include "pixelData.h"

#include <lunchbox/plugins/compressor.h>

#include <co/dataIStream.h>
#include <co/dataOStream.h>

#include <boost/foreach.hpp>

namespace eq
{
PixelData::PixelData()
{
    reset();
}

PixelData::~PixelData()
{
    reset();
}

void PixelData::reset()
{
    internalFormat = EQ_COMPRESSOR_DATATYPE_NONE;
    externalFormat = EQ_COMPRESSOR_DATATYPE_NONE;
    pixelSize = 0;
    pixels = 0;
    compressedData = lunchbox::CompressorResult();
    compressorName = EQ_COMPRESSOR_INVALID;
    compressorFlags = 0;
}

void PixelData::serialize( co::DataOStream& os ) const
{
    LBASSERT( compressorName != EQ_COMPRESSOR_INVALID );

    os << compressorFlags << externalFormat << internalFormat << pixelSize
       << pvp << compressorName << compressedData.compressor;

    if( compressedData.compressor <= EQ_COMPRESSOR_NONE )
    {
        const uint64_t nBytes = pvp.getArea() * pixelSize;
        os << co::Array< void >( pixels, nBytes );
        return;
    }

    os << uint64_t( compressedData.chunks.size( ));
    BOOST_FOREACH( const lunchbox::CompressorChunk& chunk,
                   compressedData.chunks )
    {
        os << uint64_t(chunk.getNumBytes());
        os << chunk;
    }
}

void PixelData::deserialize( co::DataIStream& is )
{
    is >> compressorFlags >> externalFormat >> internalFormat >> pixelSize
       >> pvp >> compressorName >> compressedData.compressor;

    if( compressedData.compressor <= EQ_COMPRESSOR_NONE )
    {
        const uint64_t nBytes = pvp.getArea() * pixelSize;
        pixels = const_cast< void* >( is.getRemainingBuffer( nBytes ));
        return;
    }

    uint64_t nChunks = 0;
    is >> nChunks;

    compressedData.chunks.clear();
    compressedData.chunks.reserve( nChunks );

    for( uint32_t i = 0; i < nChunks; ++i )
    {
        const uint64_t nBytes = is.read< uint64_t >();

        // works if stream is retained until setPixelData() in eq::Image...
        void* data = const_cast< void* >( is.getRemainingBuffer( nBytes ));
        const lunchbox::CompressorChunk chunk( data, nBytes );
        compressedData.chunks.push_back( chunk );
    }
}

}
