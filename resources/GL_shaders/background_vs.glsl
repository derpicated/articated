in vec2 position;
in vec2 tex;

out vec2 vtf_texcoord;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    vtf_texcoord = tex;
}
