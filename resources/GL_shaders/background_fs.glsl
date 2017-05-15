#ifdef GL_ES
    precision mediump int;
    precision mediump float;
#endif

uniform sampler2D u_tex_background;
in vec2 vtf_texcoord;

out highp vec4 frag_color;

void main()
{
    frag_color = texture(u_tex_background, vtf_texcoord);
}
