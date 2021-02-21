// model_loader.hpp
#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include <array>
#include <set>
#include <string>
#include <vector>

class ModelLoader {
    public:
    ModelLoader ();
    const std::vector<float>& Load (const std::string& filename, bool normalize = true);
    int DataPerVertex ();
    void Unload ();

    private:
    void CalculateNormals (const std::vector<float>& vertices, std::vector<float>& normals);
    float CalculateScale ();
    void NormalizeVertices ();
    void Interleave ();
    bool ParseLine (const std::string& line);
    bool ParseVertex (const std::string& line);
    bool ParseNormal (const std::string& line);
    bool ParseFace (const std::string& line);
    bool ParseTriangle (const std::vector<std::string>& values);
    bool ParseUseMTL (const std::string& line);
    inline std::string TrimString (const std::string& s);
    inline std::vector<std::string>
    TokenizeString (const std::string& in, const std::string& delim);

    bool is_loaded_;
    float scale_factor_;
    std::array<float, 4> current_rgba_;
    std::vector<float> vertices_;
    std::vector<float> normals_;
    std::vector<float> faces_;
    std::vector<float> faces_normals_;
    std::vector<float> faces_colors_;
    std::vector<float> interleaved_faces_;
    std::set<std::string> unknown_options_;
};

#endif // MODEL_LOADER_HPP
