
/* Copyright (c) 2015-2016, Stefan Eilemann <eile@equalizergraphics.com>
 *                          Daniel Nachbaur <danielnachbaur@gmail.com>
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

#include "shader.h"

#include <eq/gl.h>
#include <lunchbox/log.h>

#undef glewGetContext
#define glewGetContext() glewContext

namespace eq
{
namespace util
{
namespace shader
{

bool compile( const GLEWContext* glewContext LB_UNUSED, const unsigned shader,
              const char* source )
{
    EQ_GL_CALL( glShaderSource( shader, 1, &source, 0 ));
    EQ_GL_CALL( glCompileShader( shader ));
    GLint status;
    EQ_GL_CALL( glGetShaderiv( shader, GL_COMPILE_STATUS, &status ));
    if( !status )
    {
        GLchar errorLog[1024] = {0};
        EQ_GL_CALL( glGetShaderInfoLog( shader, 1024, 0, errorLog ));
        LBWARN << "Failed to compile shader " << shader << ": " << errorLog
               << std::endl;
        return false;
    }
    return true;
}

bool linkProgram( const GLEWContext* glewContext LB_UNUSED,
                  const unsigned program, const char* vertexShaderSource,
                  const char* fragmentShaderSource,
                  const char* geometryShaderSource )
{
    if( !program || !vertexShaderSource || !fragmentShaderSource )
    {
        LBWARN << "Failed to link shader program " << program << ": No valid "
                  "shader program, vertex or fragment source." << std::endl;
        return false;
    }

    const GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    if( !compile( glewContext, vertexShader, vertexShaderSource ))
    {
        EQ_GL_CALL( glDeleteShader( vertexShader ));
        return false;
    }

    const GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    if( !compile( glewContext, fragmentShader, fragmentShaderSource ))
    {
        EQ_GL_CALL( glDeleteShader( fragmentShader ));
        return false;
    }

    if( geometryShaderSource )
    {
        const GLuint geometryShader = glCreateShader( GL_GEOMETRY_SHADER_ARB );
        if( !compile( glewContext, geometryShader, geometryShaderSource ))
        {
            EQ_GL_CALL( glDeleteShader( geometryShader ));
            return false;
        }

        EQ_GL_CALL( glAttachShader( program, geometryShader ));
        EQ_GL_CALL( glDeleteShader( geometryShader ));

        EQ_GL_CALL( glProgramParameteriEXT( program, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS ));
        EQ_GL_CALL( glProgramParameteriEXT( program, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP ));


        // Setting output vertices to maximum possible caused large fps drop on linux AMD drivers (catalyst 12.6-4)
        // So has been limited to 4 as that is all we need for now.

        // GLint maxPossibleOutVerts;
        // glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&maxPossibleOutVerts);
        EQ_GL_CALL( glProgramParameteriEXT( program,GL_GEOMETRY_VERTICES_OUT_EXT, 4 ));
    }

    EQ_GL_CALL( glAttachShader( program, vertexShader ));
    EQ_GL_CALL( glAttachShader( program, fragmentShader ));
    EQ_GL_CALL( glDeleteShader( vertexShader ));
    EQ_GL_CALL( glDeleteShader( fragmentShader ));

    EQ_GL_CALL( glLinkProgram( program ));
    GLint status;
    EQ_GL_CALL( glGetProgramiv( program, GL_LINK_STATUS, &status ));
    if( !status )
    {
        GLchar errorLog[1024] = {0};
        EQ_GL_CALL( glGetProgramInfoLog( program, 1024, 0, errorLog ));
        LBWARN << "Failed to link shader program " << program << ": "
               << errorLog << std::endl;
        return false;
    }
    return true;
}

}
}
}
