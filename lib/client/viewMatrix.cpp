
/* Copyright (c) 2006, Stefan Eilemann <eile@equalizergraphics.com> 
   All rights reserved. */

#include "viewMatrix.h"

#include "projection.h"
#include "wall.h"

#include <math.h>

using namespace eq;

#define DEG2RAD( angle ) ( (angle) * M_PI / 180.f )

void ViewMatrix::applyWall( const Wall& wall )
{
    float u[3] = { wall.bottomRight[0] - wall.bottomLeft[0],
                   wall.bottomRight[1] - wall.bottomLeft[1],
                   wall.bottomRight[2] - wall.bottomLeft[2] };

    float v[3] = { wall.topLeft[0] - wall.bottomLeft[0],
                   wall.topLeft[1] - wall.bottomLeft[1],
                   wall.topLeft[2] - wall.bottomLeft[2] };

    float w[3] = { u[1]*v[2] - u[2]*v[1],
                   u[2]*v[0] - u[0]*v[2],
                   u[0]*v[1] - u[1]*v[0] };

    float length = sqrt( u[0]*u[0] + u[1]*u[1] + u[2]*u[2] );
    u[0] /= length;
    u[1] /= length;
    u[2] /= length;
    width = length;

    length = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
    height = length;

    length = sqrt( w[0]*w[0] + w[1]*w[1] + w[2]*w[2] );
    w[0] /= length;
    w[1] /= length;
    w[2] /= length;

    xfm[0]  = u[0];
    xfm[1]  = v[0];
    xfm[2]  = w[0];
    xfm[3]  = 0.;
             
    xfm[4]  = u[1];
    xfm[5]  = v[1];
    xfm[6]  = w[1];
    xfm[7]  = 0.;
             
    xfm[8]  = u[2];
    xfm[9]  = v[2];
    xfm[10] = w[2];
    xfm[11] = 0.;

    const float center[3] = { (wall.bottomRight[0] + wall.topLeft[0]) / 2.,
                              (wall.bottomRight[1] + wall.topLeft[1]) / 2.,
                              (wall.bottomRight[2] + wall.topLeft[2]) / 2. };

    xfm[12] = -(u[0]*center[0] + u[1]*center[1] + u[2]*center[2]);
    xfm[13] = -(v[0]*center[0] + v[1]*center[1] + v[2]*center[2]);
    xfm[14] = -(w[0]*center[0] + w[1]*center[1] + w[2]*center[2]);
    xfm[15] = 1.;
}

void ViewMatrix::applyProjection( const Projection& projection )
{
    const float cosH = cosf( DEG2RAD( projection.hpr[0] ));
    const float sinH = sinf( DEG2RAD( projection.hpr[0] ));
    const float cosP = cosf( DEG2RAD( projection.hpr[1] ));
    const float sinP = sinf( DEG2RAD( projection.hpr[1] ));
    const float cosR = cosf( DEG2RAD( projection.hpr[2] ));
    const float sinR = sinf( DEG2RAD( projection.hpr[2] ));

    // HPR Matrix = -roll[z-axis] * -pitch[x-axis] * -head[y-axis]
    const float rot[9] =
        {
            sinR*sinP*sinH + cosR*cosH,  cosR*sinP*sinH - sinR*cosH,  cosP*sinH,
            cosP*sinR,                   cosP*cosR,                  -sinP,
            sinR*sinP*cosH - cosR*sinH,  cosR*sinP*cosH + sinR*sinH,  cosP*cosH 
        };

    // translation = HPR x -origin
    const float* origin   = projection.origin;
    const float  distance = projection.distance;
    const float  trans[3] = 
        {
            -( rot[0]*origin[0] + rot[3]*origin[1] + rot[6]*origin[2] ),
            -( rot[1]*origin[0] + rot[4]*origin[1] + rot[7]*origin[2] ),
            -( rot[2]*origin[0] + rot[5]*origin[1] + rot[8]*origin[2] )
        };

    xfm[0]  = rot[0];
    xfm[1]  = rot[1];
    xfm[2]  = rot[2];
    xfm[3]  = 0.;

    xfm[4]  = rot[3];
    xfm[5]  = rot[4];
    xfm[6]  = rot[5];
    xfm[7]  = 0.;
            
    xfm[8]  = rot[6];                
    xfm[9]  = rot[7];
    xfm[10] = rot[8];
    xfm[11] = 0.;

    xfm[12] = trans[0];
    xfm[13] = trans[1];
    xfm[14] = trans[2] + distance;
    xfm[15] = 1.;

    width  = distance * tan(DEG2RAD( projection.fov[0] ));
    height = distance * tan(DEG2RAD( projection.fov[1] ));
}
