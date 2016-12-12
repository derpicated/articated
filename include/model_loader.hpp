#ifndef MODEL_OBJ_HPP
#define MODEL_OBJ_HPP

#include <string>
#include <vector>

class model_obj {
    public:
    model_obj ();
    bool load (const char* filename); // Loads the model
    void draw ();                     // Draws the model on the screen
    void release ();                  // Release the model

    private:
    void calculate_normal (float* norm, float* coord1, float* coord2, float* coord3);
    void calculate_scale ();
    bool parse_line (std::string line);
    bool parse_vertex (std::string line);
    bool parse_face (std::string line);
    bool parse_usemtl (std::string line);
    inline std::string trim_str (const std::string& s);

    bool _is_loaded;
    float _scale_factor;
    float _current_rgba[4];
    std::vector<float> _vertices;
    std::vector<float> _faces;
    std::vector<float> _normals;
    std::vector<float> _colors;
};

#endif // MODEL_OBJ_HPP
