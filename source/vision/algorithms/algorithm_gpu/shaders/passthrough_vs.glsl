in vec2 position;
in vec2 tex;

uniform int is_GLRED;

flat out int vtf_is_GLRED;
out vec2 vtf_texcoord;

void main()
{
    gl_Position = vec4(position.x, -position.y, 0.0, 1.0);
    vtf_texcoord = tex;
    vtf_is_GLRED = is_GLRED;
}
