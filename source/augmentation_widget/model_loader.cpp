#include "model_loader.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define DATA_PER_VERTEX 10 // 3 coords + 3 normal + 4 color
#define MAX_COORDINATE 1

ModelLoader::ModelLoader ()
: is_loaded_ (false)
, scale_factor_ (1.0f)
, current_rgba_{ { 1, 1, 1, 1 } } {
}

void ModelLoader::Unload () {
    is_loaded_    = false;
    scale_factor_ = 1.0f;
    vertices_.clear ();
    normals_.clear ();
    faces_.clear ();
    faces_normals_.clear ();
    faces_colors_.clear ();
    interleaved_faces_.clear ();
    current_rgba_ = { { 1, 1, 1, 1 } };
}

int ModelLoader::DataPerVertex () {
    return DATA_PER_VERTEX;
}

void ModelLoader::CalculateNormals (const std::vector<float>& vertices,
std::vector<float>& normals) {
    // calculate Vector1 and Vector2
    float norm[3], va[3], vb[3], vr[3], val;
    va[0] = vertices[0] - vertices[3];
    va[1] = vertices[1] - vertices[4];
    va[2] = vertices[2] - vertices[5];

    vb[0] = vertices[0] - vertices[6];
    vb[1] = vertices[1] - vertices[7];
    vb[2] = vertices[2] - vertices[8];

    // cross product
    vr[0] = va[1] * vb[2] - vb[1] * va[2];
    vr[1] = vb[0] * va[2] - va[0] * vb[2];
    vr[2] = va[0] * vb[1] - vb[0] * va[1];

    // normalization factor
    val = sqrt (vr[0] * vr[0] + vr[1] * vr[1] + vr[2] * vr[2]);

    norm[0] = vr[0] / val;
    norm[1] = vr[1] / val;
    norm[2] = vr[2] / val;

    // push back the norms for all 3 vertices
    normals.push_back (norm[0]);
    normals.push_back (norm[1]);
    normals.push_back (norm[2]);

    normals.push_back (norm[0]);
    normals.push_back (norm[1]);
    normals.push_back (norm[2]);

    normals.push_back (norm[0]);
    normals.push_back (norm[1]);
    normals.push_back (norm[2]);
}

float ModelLoader::CalculateScale () {
    // ensure that every vertex fits into range -1 to 1
    float max_val = 0.0f;
    for (float val : faces_) {
        float abs_val = std::fabs (val);
        if (max_val < abs_val) {
            max_val = abs_val;
        }
    }
    return (MAX_COORDINATE / max_val);
}

void ModelLoader::NormalizeVertices () {
    float scale_factor = CalculateScale ();

    for (float& val : faces_) {
        val = val * scale_factor;
    }

    scale_factor_ = scale_factor;
}

void ModelLoader::Interleave () {
    int vertex_count = faces_.size () / 3;

    for (int vert_idx = 0; vert_idx < vertex_count; vert_idx++) {
        int position_idx = vert_idx * 3;
        int normal_idx   = vert_idx * 3;
        int color_idx    = vert_idx * 4;

        interleaved_faces_.push_back (faces_.at (position_idx));
        interleaved_faces_.push_back (faces_.at (position_idx + 1));
        interleaved_faces_.push_back (faces_.at (position_idx + 2));

        interleaved_faces_.push_back (faces_normals_.at (normal_idx));
        interleaved_faces_.push_back (faces_normals_.at (normal_idx + 1));
        interleaved_faces_.push_back (faces_normals_.at (normal_idx + 2));

        interleaved_faces_.push_back (faces_colors_.at (color_idx));
        interleaved_faces_.push_back (faces_colors_.at (color_idx + 1));
        interleaved_faces_.push_back (faces_colors_.at (color_idx + 2));
        interleaved_faces_.push_back (faces_colors_.at (color_idx + 3));
    }
}

const std::vector<float>& ModelLoader::Load (const std::string filename, bool normalize) {
    bool status = true;
    std::string line;

    if (is_loaded_) {
        Unload ();
    }

    std::ifstream objFile (filename);
    if (objFile.is_open ()) // If obj file is open, continue
    {
        while ((!objFile.eof ()) && status) {
            getline (objFile, line);
            if (!line.empty ()) {
                status = ParseLine (line);
            }
        }
        objFile.close (); // Close OBJ file
    } else {
        status = false;
    }


    if (status == true) {
        if (normalize) {
            NormalizeVertices ();
        }

        Interleave ();
        is_loaded_ = true;
    }

    return interleaved_faces_;
}

bool ModelLoader::ParseLine (const std::string& line) {
    bool status = true;

    std::string value;
    std::string keyword;
    size_t split_pos = line.find (' ');
    if (split_pos != std::string::npos) {
        keyword = line.substr (0, split_pos);
        value   = line.substr (split_pos);
    } else {
        keyword = line;
        value   = "";
    }

    if (keyword == "#" || keyword.empty ()) {
        ; // comment line, ignore
    } else if (keyword == "v") {
        status = ParseVertex (value);
    } else if (keyword == "vn") {
        status = ParseNormal (value);
    } else if (keyword == "f") {
        status = ParseFace (value);
    } else if (keyword == "usemtl") {
        status = ParseUseMTL (value);
    } else {
        if (unknown_options_.find (keyword) == unknown_options_.end ()) {
            // std::cout << "unsupporterd keyword: " << keyword << std::endl;
            unknown_options_.insert (keyword);
        }
    }
    return status;
}

bool ModelLoader::ParseVertex (const std::string& line) {
    bool status = true;

    std::vector<std::string> values = TokenizeString (line, " ");

    if (values.size () == 3) {
        std::array<float, 3> vertex;

        try {
            vertex[0] = std::atof (values[0].c_str ());
            vertex[1] = std::atof (values[1].c_str ());
            vertex[2] = std::atof (values[2].c_str ());
        } catch (std::invalid_argument) {
            status = false;
            // std::cout << "failed line" << line << std::endl;
        }
        if (status == true) {
            vertices_.insert (std::end (vertices_), std::begin (vertex), std::end (vertex));
        }
    } else {
        status = false;
    }
    return status;
}

bool ModelLoader::ParseNormal (const std::string& line) {
    bool status = true;

    std::vector<std::string> values = TokenizeString (line, " ");

    if (values.size () == 3) {
        std::array<float, 3> normal;

        try {
            normal[0] = std::atof (values[0].c_str ());
            normal[1] = std::atof (values[1].c_str ());
            normal[2] = std::atof (values[2].c_str ());
        } catch (std::invalid_argument) {
            status = false;
            // std::cout << "failed line" << line << std::endl;
        }
        if (status == true) {
            normals_.insert (std::end (normals_), std::begin (normal), std::end (normal));
        }
    } else {
        status = false;
    }
    return status;
}


bool ModelLoader::ParseFace (const std::string& line) {
    bool status = true;

    std::vector<std::string> values = TokenizeString (line, " ");

    if (values.size () == 3) {
        // create triangle ABC
        status = ParseTriangle (values);
    } else if (values.size () == 4) {
        // create triangles ABC and ACD from quad ABCD
        std::vector<std::string> triangle_2 = { values[0], values[2], values[3] };
        values.pop_back ();
        status = ParseTriangle (values); // ABC
        if (status == true) {
            status = ParseTriangle (triangle_2); // ACD
        }
    } else {
        status = false;
    }

    return status;
}

bool ModelLoader::ParseTriangle (const std::vector<std::string>& vertices_str) {
    bool status           = true;
    bool normals_provided = true;
    int vert_idx          = 0; // vertex indices
    int text_idx          = 0; // texture point indices
    int norm_idx          = 0; // normal indices
    std::vector<float> vertices;
    std::vector<float> normals;

    for (auto vertex_str : vertices_str) {
        // each vertex can be in format v or v//vn or v/vt/vn
        // TODO: use regex instead
        std::vector<std::string> values = TokenizeString (vertex_str, "/");

        if (values.size () == 1) {
            normals_provided = false;
            vert_idx         = std::atoi (values[0].c_str ());
        } else if (values.size () == 2) {
            vert_idx = std::atoi (values[0].c_str ());
            norm_idx = std::atoi (values[1].c_str ());
        } else if (values.size () == 3) {
            vert_idx = std::atoi (values[0].c_str ());
            text_idx = std::atoi (values[1].c_str ());
            norm_idx = std::atoi (values[2].c_str ());
        } else {
            status = false;
        }

        if (status) {
            // OBJ index starts at 1, and 3 floats per vertex
            vert_idx = (vert_idx - 1) * 3;
            text_idx = (text_idx - 1) * 3;
            norm_idx = (norm_idx - 1) * 3;

            // retrieve the XYZ coords
            vertices.push_back (vertices_.at (vert_idx));
            vertices.push_back (vertices_.at (vert_idx + 1));
            vertices.push_back (vertices_.at (vert_idx + 2));

            if (normals_provided == true) { // retrieve the XYZ angles
                normals.push_back (normals_.at (norm_idx));
                normals.push_back (normals_.at (norm_idx + 1));
                normals.push_back (normals_.at (norm_idx + 2));
            }

            // push back color per vertex
            faces_colors_.insert (std::end (faces_colors_),
            std::begin (current_rgba_), std::end (current_rgba_));
        }
    }

    // push back all vertices
    faces_.insert (std::end (faces_), std::begin (vertices), std::end (vertices));

    // if any of the normals are missing, recalculate all
    if (normals_provided == false) {
        normals.clear ();
        CalculateNormals (vertices, normals);
    }

    // push back all normals
    faces_normals_.insert (
    std::end (faces_normals_), std::begin (normals), std::end (normals));

    return status;
}

bool ModelLoader::ParseUseMTL (const std::string& value) {
    bool status = true; // TODO: check status

    std::string mat = value.substr (value.find (" ")); // get from space
    mat             = TrimString (mat);

    if (mat == "black") { // shuttle materials
        current_rgba_ = { { 0.0, 0.0, 0.0, 1.0 } };
    } else if (mat == "glass") {
        current_rgba_ = { { 0.5, 0.65, 0.75, 1.0 } };
    } else if (mat == "bone") {
        current_rgba_ = { { 0.75, 0.75, 0.65, 1.0 } };
    } else if (mat == "brass") {
        current_rgba_ = { { 0.45, 0.35, 0.12, 1.0 } };
    } else if (mat == "dkdkgrey") {
        current_rgba_ = { { 0.30, 0.35, 0.35, 1.0 } };
    } else if (mat == "fldkdkgrey") {
        current_rgba_ = { { 0.30, 0.35, 0.35, 1.0 } };
    } else if (mat == "redbrick") {
        current_rgba_ = { { 0.61, 0.16, 0.0, 1.0 } };
    } else if (mat == "Mat_1_-1") { // articated materials
        current_rgba_ = { { 0.0, 0.0, 1.0, 1.0 } };
    } else if (mat == "Mat_2_-1") {
        current_rgba_ = { { 0.2, 1.0, 1.0, 0.4 } };
    } else if (mat == "Mat_3_-1") {
        current_rgba_ = { { 1.0, 0.0, 0.0, 1.0 } };
    } else if (mat == "Mat_4_-1") {
        current_rgba_ = { { 0.0, 1.0, 0.0, 1.0 } };
    } else if (mat == "red") { // general collors
        current_rgba_ = { { 1.0, 0.0, 0.0, 1.0 } };
    } else if (mat == "green") {
        current_rgba_ = { { 0.0, 1.0, 0.0, 1.0 } };
    } else if (mat == "blue") {
        current_rgba_ = { { 0.0, 0.0, 1.0, 1.0 } };
    } else if (mat == "cyan") {
        current_rgba_ = { { 0.0, 1.0, 1.0, 1.0 } };
    } else if (mat == "yellow") {
        current_rgba_ = { { 1.0, 1.0, 0.0, 1.0 } };
    } else {
        // default to dark purple
        current_rgba_ = { { 0.2, 0, 0.2, 1 } };

        if (unknown_options_.find (mat) == unknown_options_.end ()) {
            // std::cout << "unknown material: " << mat << std::endl;
            unknown_options_.insert (mat);
        }
    }
    return status;
}

inline std::vector<std::string>
ModelLoader::TokenizeString (const std::string& in, const std::string& delim) {
    size_t split_pos;
    std::string right = in;
    std::string left;
    std::vector<std::string> ret;

    do {
        split_pos = right.find (delim);
        if (split_pos != std::string::npos) {
            left  = right.substr (0, split_pos);
            right = right.substr (split_pos + 1);
            left  = TrimString (left);
            right = TrimString (right);
            if (!left.empty ()) {
                ret.push_back (left);
            }
        }
    } while (split_pos != std::string::npos);

    // the last token
    if (!right.empty ()) {
        ret.push_back (right);
    }

    return ret;
}

inline std::string ModelLoader::TrimString (const std::string& s) {
    auto wsfront = std::find_if_not (
    s.begin (), s.end (), [](int c) { return std::isspace (c); });
    auto wsback = std::find_if_not (
    s.rbegin (), s.rend (), [](int c) { return std::isspace (c); })
                  .base ();
    return (wsback <= wsfront ? std::string () : std::string (wsfront, wsback));
}
