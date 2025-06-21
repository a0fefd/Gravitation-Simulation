#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float mass;
out vec4 colour;

void main()
{
    colour = vec4(mass*mass, pow(mass, 11), pow(mass, 5), 1.f);

    gl_Position = vec4(aPos.xy, 0.0f, 1.0f);
//    gl_PointSize = round(100*mass)/20;
    gl_PointSize = 15*mass;
}