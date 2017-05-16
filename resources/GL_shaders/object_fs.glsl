#ifdef GL_ES
    precision mediump int;
    precision mediump float;
#endif

in vec3 vtf_normal;
in highp vec4 vtf_color;
out highp vec4 frag_color;

void main()
{
    float dot_product = dot( vtf_normal, vec3(0, 0, 1) );
    float cosTheta = clamp(dot_product, 0.0, 1.0);
    vec4 diffuse = vtf_color * cosTheta;
    vec4 ambient = vtf_color * 0.5;
    frag_color = diffuse + ambient;
}
