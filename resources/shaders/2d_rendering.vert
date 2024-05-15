#version 460 core

layout(location = 0) in vec2 i_vertex_position;
layout(location = 1) in vec4 i_vertex_color;

uniform uvec2 u_view_dimensions;

out vec2 fragment_position;
out vec4 fragment_color;

// convert from rect (0,0,vp.w,vp.h) to (-1,1,1,-1)

void main()
{
    fragment_position.x =
        (i_vertex_position.x / u_view_dimensions.x // convert (0,vp.w) -> (0,1)
         * 2                                       // convert (0,1) -> (0,2)
         ) -                                       // convert (0,2) -> (-1,1)
        1;
    fragment_position.y = 1 - (i_vertex_position.y / // convert (0,2) -> (1,-1)
                               u_view_dimensions.y // convert (0,vp.h) -> (0,1)
                               * 2                 // convert (0,1) -> (0,2)
                              );
    fragment_color = i_vertex_color;
    gl_Position = vec4(fragment_position, 0, 1);
}
