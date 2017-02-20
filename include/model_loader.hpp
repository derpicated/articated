#ifndef MODEL_OBJ_HPP
#define MODEL_OBJ_HPP

#include <array>
#include <set>
#include <string>
#include <vector>

#include <GLES3/gl3.h>

class model_obj {
    public:
    model_obj ();
    bool load (const std::string filename); // Loads the model
    void draw ();                           // Draws the model on the screen
    void release ();                        // Release the model

    private:
    void calculate_normals (const std::vector<float>& vertices, std::vector<float>& normals);
    void calculate_scale ();
    void upload_to_gpu ();
    bool parse_line (const std::string& line);
    bool parse_vertex (const std::string& line);
    bool parse_normal (const std::string& line);
    bool parse_face (const std::string& line);
    bool parse_triangle (const std::vector<std::string>& values);
    bool parse_usemtl (const std::string& line);
    inline std::string trim_str (const std::string& s);
    inline std::vector<std::string>
    tokenize_str (const std::string& in, const std::string& delim);

    bool _is_loaded;
    float _scale_factor;
    GLuint _model_vao;
    std::array<float, 4> _current_rgba;
    std::vector<float> _vertices;
    std::vector<float> _normals;
    std::vector<float> _faces;
    std::vector<float> _faces_normals;
    std::vector<float> _faces_colors;
    std::set<std::string> _unknown_options;
};

#endif // MODEL_OBJ_HPP
