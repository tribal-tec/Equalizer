#version 330

uniform mat4 MVP;
uniform float inBrightness[10];
uniform float inSmoothingLength[10];
uniform float inRadialMod;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in float inRadius;
layout(location = 3) in float inType;

void main()
{

    // Multiply color by type-dependant brightness
    vec3 col = inColor;
    float thisbrightness = inBrightness[int(inType)];
    float thissmooth = inSmoothingLength[int(inType)];

    col = inColor * thisbrightness;

    float radius;

    // Pass through radius to geom shader
    if(thissmooth==0)
    {
        radius = inRadius * inRadialMod;
    }
    else
    {
        radius = thissmooth * inRadialMod;
    }

    // Pass radius to geometry shader in color because inconsistencies in amd/nvidia
    // compilers handling of varying array syntax in #120 are confusing

    gl_FrontColor = vec4(col, radius);
    gl_Position = MVP * vec4(inPosition,1.0);
}


