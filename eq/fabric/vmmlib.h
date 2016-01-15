
/* Copyright (c) 2011-2014, Stefan Eilemann <eile@eyescale.ch>
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

#ifndef EQFABRIC_VMMLIB_H
#define EQFABRIC_VMMLIB_H

#include <eq/fabric/api.h>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include <lunchbox/bitOperation.h>

namespace eq
{
namespace fabric
{
typedef glm::tmat3x3< double > Matrix3d; //!< A 3x3 double matrix
typedef glm::tmat4x4< double > Matrix4d; //!< A 4x4 double matrix
typedef glm::tmat3x3< float > Matrix3f; //!< A 3x3 float matrix
typedef glm::tmat4x4< float > Matrix4f; //!< A 4x4 float matrix

typedef glm::tvec2< unsigned > Vector2ui; //!< A two-component unsigned integer vector
typedef glm::tvec2< int > Vector2i; //!< A two-component integer vector
typedef glm::tvec3< unsigned > Vector3ui; //!< A three-component unsigned integer vector
typedef glm::tvec3< int > Vector3i; //!< A three-component integer vector
typedef glm::tvec4< unsigned > Vector4ui; //!< A four-component unsigned integer vector
typedef glm::tvec4< int > Vector4i; //!< A four-component integer vector
typedef glm::tvec3< double > Vector3d; //!< A three-component double vector
typedef glm::tvec4< double > Vector4d; //!< A four-component double vector
typedef glm::tvec2< float > Vector2f; //!< A two-component float vector
typedef glm::tvec3< float > Vector3f; //!< A three-component float vector
typedef glm::tvec4< float > Vector4f; //!< A four-component float vector
typedef glm::tvec3< uint8_t > Vector3ub; //!< A three-component byte vector
typedef glm::tvec4< uint8_t > Vector4ub; //!< A four-component byte vector
//typedef Matrix4f Frustumf; //!< A frustum definition

struct Frustumf
{
    float left, right, bottom, top, near, far;
    Frustumf() : left( 0.f ), right( 0.f ), bottom( 0.f ), top( 0.f ), near( 0.f ), far( 0.f ) {}
    Frustumf( const float left_, const float right_, const float bottom_,
              const float top_, const float near_, const float far_ )
        : left( left_ ), right( right_ ), bottom( bottom_ ), top( top_ ), near( near_ ), far( far_ ) {}

    EQFABRIC_API static const Frustumf DEFAULT;

    void adjust_near( const float new_near )
    {
        if( new_near == near )
            return;

        const float ratio = new_near / near;
        right     *= ratio;
        left     *= ratio;
        top       *= ratio;
        bottom    *= ratio;
        near  = new_near;
    }

    void apply_jitter( const Vector2f jitter_ )
    {
        left += jitter_.x;
        right += jitter_.x;
        bottom += jitter_.y;
        top += jitter_.y;
    }

    float get_width() const
    {
        return fabs( right - left );
    }

    float get_height() const
    {
        return fabs( top - bottom );
    }


//    Frustum& operator=( const Frustum& rhs )
//    {
//        left = rhs.left;
//        right = rhs.right;
//        bottom = rhs.bottom;
//        top = rhs.top;
//        near = rhs.near;
//        far = rhs.far;
//        return *this;
//    }
};

}
}

namespace lunchbox
{
template<> inline void byteswap( eq::fabric::Vector2ui& value )
{
    byteswap( value.x );
    byteswap( value.y );
}

template<> inline void byteswap( eq::fabric::Vector2i& value )
{
    byteswap( value.x );
    byteswap( value.y );
}

template<> inline void byteswap( eq::fabric::Vector2f& value )
{
    byteswap( value.x );
    byteswap( value.y );
}

template<> inline void byteswap( eq::fabric::Vector3f& value )
{
    byteswap( value.x );
    byteswap( value.y );
    byteswap( value.z );
}

template<> inline void byteswap( eq::fabric::Vector4f& value )
{
    byteswap( value.x );
    byteswap( value.y );
    byteswap( value.z );
    byteswap( value.w );
}

template<> inline void byteswap( eq::fabric::Vector4ui& value )
{
    byteswap( value.x );
    byteswap( value.y );
    byteswap( value.z );
    byteswap( value.w );
}

template<> inline void byteswap( eq::fabric::Vector4i& value )
{
    byteswap( value.x );
    byteswap( value.y );
    byteswap( value.z );
    byteswap( value.w );
}

template<> inline void byteswap( eq::fabric::Vector4ub& ) { /*NOP*/ }
template<> inline void byteswap( eq::fabric::Vector3ub& ) { /*NOP*/ }

template<> inline void byteswap( eq::fabric::Matrix4f& value )
{
    for( size_t i = 0; i < 16; ++i )
        byteswap( glm::value_ptr( value )[ i ]);
}

template<> inline void byteswap( eq::fabric::Frustumf& value )
{
    byteswap( value.left );
    byteswap( value.right );
    byteswap( value.bottom );
    byteswap( value.top );
    byteswap( value.near );
    byteswap( value.far );
}
}

#endif // EQFABRIC_VMMLIB_H
