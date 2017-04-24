uniform mat4 view_matrix;
in vec3 position;
in vec3 normal;
in vec4 color;

out vec4 vtf_color;

void main()
{
    vtf_color = color;
    gl_Position = view_matrix * vec4(position, 1.0);
}
