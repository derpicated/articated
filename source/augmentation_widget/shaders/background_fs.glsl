#ifdef GL_ES
    precision mediump int;
    precision mediump float;
#endif

uniform sampler2D u_tex_background;
flat in int vtf_is_GLRED;
in vec2 vtf_texcoord;

out highp vec4 frag_color;

void main() {
    /* vec4 tex_sample = texture(u_tex_background, vtf_texcoord);
    if (vtf_is_GLRED == 1) {
        frag_color = vec4(tex_sample.r, tex_sample.r, tex_sample.r, 1.0);
    } else {
        frag_color = tex_sample;
    } */
    frag_color = vec4(vtf_texcoord.x, vtf_texcoord.y, 0.0 , 1.0);
}
