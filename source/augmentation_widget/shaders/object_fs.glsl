#ifdef GL_ES
    precision mediump int;
    precision mediump float;
#endif

in vec3 vtf_normal;
in highp vec4 vtf_color;
out highp vec4 frag_color;

void main()
{
    float dot_product = dot( vtf_normal, vec3(0.17, 0.98, 0) );
    float cosTheta = clamp(dot_product, 0.0, 1.0);
    vec3 diffuse = vtf_color.rgb * cosTheta;
    vec3 ambient = vtf_color.rgb * 0.5;
    frag_color = vec4(vec3(diffuse + ambient), vtf_color.a);
}
