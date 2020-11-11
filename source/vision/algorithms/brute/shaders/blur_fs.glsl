#ifdef GL_ES
    precision mediump int;
    precision mediump float;
#endif

uniform sampler2D u_tex_background;
flat in int vtf_is_GLRED;
in vec2 vtf_texcoord;

out highp vec4 frag_color;

void main()
{
    vec4 tex_sample = texture(u_tex_background, vtf_texcoord);
    frag_color = vec4(tex_sample.r, tex_sample.r, tex_sample.r, 1.0);
}
