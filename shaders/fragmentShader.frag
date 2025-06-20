#version 330 core
in vec4 colour;
out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    if (length(coord) > 0.5)
        discard;

//    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    FragColor = colour;
}