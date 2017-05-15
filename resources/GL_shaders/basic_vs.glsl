uniform mat4 view_matrix;
uniform mat4 normal_matrix;
in vec3 position;
in vec3 normal;
in vec4 color;

out vec4 vtf_color;
out vec3 vtf_normal;

void main()
{
    vtf_color = color;
    vtf_normal = vec3(normal_matrix * vec4(normal, 0.0));
    gl_Position = view_matrix * vec4(position, 1.0);
}
