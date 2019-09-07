#ifdef GL_ES
    precision mediump int;
    precision mediump float;
#endif

uniform sampler2D u_tex_background;
uniform float u_threshold;
in vec2 vtf_texcoord;

out highp vec4 frag_color;

void main()
{
    vec4 tex_sample = texture(u_tex_background, vtf_texcoord);
    float binary_value = mix(1.0, 0.0, step(u_threshold, tex_sample.r));
    frag_color = vec4(binary_value, 0.0, 0.0, 1.0);
}
