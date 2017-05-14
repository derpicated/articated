#ifdef GL_ES
    precision mediump int;
    precision mediump float;
#endif

in highp vec4 vtf_color;
out highp vec4 frag_color;

void main()
{
      frag_color = vtf_color;
}
