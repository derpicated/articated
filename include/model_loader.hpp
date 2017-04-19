#ifndef MODEL_OBJ_HPP
#define MODEL_OBJ_HPP

#include <array>
#include <set>
#include <string>
#include <vector>

class model_obj {
    public:
    model_obj ();
    const std::vector<float>& load (const std::string filename, bool normalize = true);
    int data_per_vertex ();
    void release ();

    private:
    void calculate_normals (const std::vector<float>& vertices, std::vector<float>& normals);
    float calculate_scale ();
    void normalize_vertices ();
    void interleave ();
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
    std::array<float, 4> _current_rgba;
    std::vector<float> _vertices;
    std::vector<float> _normals;
    std::vector<float> _faces;
    std::vector<float> _faces_normals;
    std::vector<float> _faces_colors;
    std::vector<float> _interleaved_faces;
    std::set<std::string> _unknown_options;
};

#endif // MODEL_OBJ_HPP
