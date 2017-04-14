#include "model_loader.hpp"

#include <GLES3/gl31.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9
#define MAX_COORDINATE 2

model_obj::model_obj ()
: _is_loaded (false)
, _scale_factor (1.0f)
, _current_rgba{ 1, 1, 1, 1 } {
}

void model_obj::release () {
    _is_loaded    = false;
    _scale_factor = 1.0f;
    _vertices.clear ();
    _normals.clear ();
    _faces.clear ();
    _faces_normals.clear ();
    _faces_colors.clear ();
    _current_rgba = { 1, 1, 1, 1 };
}

void model_obj::calculate_normals (const std::vector<float>& vertices,
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

void model_obj::calculate_scale () {
    // ensure that every vertex fits into range -1 to 1
    float max_val = 0.0f;
    for (float val : _vertices) {
        float abs_val = std::fabs (val);
        if (max_val < abs_val) {
            max_val = abs_val;
        }
    }
    _scale_factor = (MAX_COORDINATE / max_val);
}

void model_obj::draw () {
    if (_is_loaded) {
        GLsizei face_count = _faces.size () / 3; // 3 points per face

        glScalef (_scale_factor, _scale_factor, _scale_factor);

        glBindVertexArray (_model_vao);
        glDrawArrays (GL_TRIANGLES, 0, 3);
    }
}

void model_obj::upload_to_gpu () {
    GLuint vertices_vbo = 0;
    GLuint colours_vbo  = 0;
    GLuint normals_vbo  = 0;

    glGenBuffers (1, &vertices_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vertices_vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), _faces.data (), GL_STATIC_DRAW);

    glGenBuffers (1, &colours_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), _faces_colors.data (), GL_STATIC_DRAW);

    glGenBuffers (1, &normals_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), _faces_normals.data (), GL_STATIC_DRAW);


    _model_vao = 0;
    glGenVertexArrays (1, &_model_vao);
    glBindVertexArray (_model_vao);
    glBindBuffer (GL_ARRAY_BUFFER, vertices_vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
    glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
    glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);
    glEnableVertexAttribArray (2);
    /* Data is stored like:
    glVertexPointer (3, GL_FLOAT, 0, _faces.data ());
    glNormalPointer (GL_FLOAT, 0, _faces_normals.data ());
    glColorPointer (4, GL_FLOAT, 0, _faces_colors.data ());
    glDrawArrays (GL_TRIANGLES, 0, face_count);*/
}

bool model_obj::load (const std::string filename) {
    bool status = true;
    std::string line;

    if (_is_loaded) {
        release ();
    }

    std::ifstream objFile (filename);
    if (objFile.is_open ()) // If obj file is open, continue
    {
        while ((!objFile.eof ()) && status) {
            getline (objFile, line);
            if (!line.empty ()) {
                status = parse_line (line);
            }
        }
        objFile.close (); // Close OBJ file
    } else {
        status = false;
    }


    if (status == true) {
        calculate_scale ();
        _is_loaded = true;
    }

    return status;
}

bool model_obj::parse_line (const std::string& line) {
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
        status = parse_vertex (value);
    } else if (keyword == "vn") {
        status = parse_normal (value);
    } else if (keyword == "f") {
        status = parse_face (value);
    } else if (keyword == "usemtl") {
        status = parse_usemtl (value);
    } else {
        if (_unknown_options.find (keyword) == _unknown_options.end ()) {
            std::cout << "unsupporterd keyword: " << keyword << std::endl;
            _unknown_options.insert (keyword);
        }
    }
    return status;
}

bool model_obj::parse_vertex (const std::string& line) {
    bool status = true;

    std::vector<std::string> values = tokenize_str (line, " ");

    if (values.size () == 3) {
        std::array<float, 3> vertex;

        try {
            vertex[0] = std::atof (values[0].c_str ());
            vertex[1] = std::atof (values[1].c_str ());
            vertex[2] = std::atof (values[2].c_str ());
        } catch (std::invalid_argument) {
            status = false;
            std::cout << "failed line" << line << std::endl;
        }
        if (status == true) {
            _vertices.insert (std::end (_vertices), std::begin (vertex), std::end (vertex));
        }
    } else {
        status = false;
    }
    return status;
}

bool model_obj::parse_normal (const std::string& line) {
    bool status = true;

    std::vector<std::string> values = tokenize_str (line, " ");

    if (values.size () == 3) {
        std::array<float, 3> normal;

        try {
            normal[0] = std::atof (values[0].c_str ());
            normal[1] = std::atof (values[1].c_str ());
            normal[2] = std::atof (values[2].c_str ());
        } catch (std::invalid_argument) {
            status = false;
            std::cout << "failed line" << line << std::endl;
        }
        if (status == true) {
            _normals.insert (std::end (_normals), std::begin (normal), std::end (normal));
        }
    } else {
        status = false;
    }
    return status;
}


bool model_obj::parse_face (const std::string& line) {
    bool status = true;

    std::vector<std::string> values = tokenize_str (line, " ");

    if (values.size () == 3) {
        // create triangle ABC
        status = parse_triangle (values);
    } else if (values.size () == 4) {
        // create triangles ABC and ACD from quad ABCD
        std::vector<std::string> triangle_2 = { values[0], values[2], values[3] };
        values.pop_back ();
        status = parse_triangle (values); // ABC
        if (status == true) {
            status = parse_triangle (triangle_2); // ACD
        }
    } else {
        status = false;
    }

    return status;
}

bool model_obj::parse_triangle (const std::vector<std::string>& vertices_str) {
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
        std::vector<std::string> values = tokenize_str (vertex_str, "/");

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
            vertices.push_back (_vertices.at (vert_idx));
            vertices.push_back (_vertices.at (vert_idx + 1));
            vertices.push_back (_vertices.at (vert_idx + 2));

            if (normals_provided == true) { // retrieve the XYZ angles
                normals.push_back (_normals.at (norm_idx));
                normals.push_back (_normals.at (norm_idx + 1));
                normals.push_back (_normals.at (norm_idx + 2));
            }

            // push back color per vertex
            _faces_colors.insert (std::end (_faces_colors),
            std::begin (_current_rgba), std::end (_current_rgba));
        }
    }

    // push back all vertices
    _faces.insert (std::end (_faces), std::begin (vertices), std::end (vertices));

    // if any of the normals are missing, recalculate all
    if (normals_provided == false) {
        normals.clear ();
        calculate_normals (vertices, normals);
    }

    // push back all normals
    _faces_normals.insert (
    std::end (_faces_normals), std::begin (normals), std::end (normals));

    return status;
}

bool model_obj::parse_usemtl (const std::string& value) {
    bool status = true; // TODO: check status

    std::string mat = value.substr (value.find (" ")); // get from space
    mat             = trim_str (mat);

    if (mat == "black") { // shuttle materials
        _current_rgba = { 0.0, 0.0, 0.0, 1.0 };
    } else if (mat == "glass") {
        _current_rgba = { 0.5, 0.65, 0.75, 1.0 };
    } else if (mat == "bone") {
        _current_rgba = { 0.75, 0.75, 0.65, 1.0 };
    } else if (mat == "brass") {
        _current_rgba = { 0.45, 0.35, 0.12, 1.0 };
    } else if (mat == "dkdkgrey") {
        _current_rgba = { 0.30, 0.35, 0.35, 1.0 };
    } else if (mat == "fldkdkgrey") {
        _current_rgba = { 0.30, 0.35, 0.35, 1.0 };
    } else if (mat == "redbrick") {
        _current_rgba = { 0.61, 0.16, 0.0, 1.0 };
    } else if (mat == "Mat_1_-1") { // articated materials
        _current_rgba = { 0.0, 0.0, 1.0, 1.0 };
    } else if (mat == "Mat_2_-1") {
        _current_rgba = { 0.2, 1.0, 1.0, 0.4 };
    } else if (mat == "Mat_3_-1") {
        _current_rgba = { 1.0, 0.0, 0.0, 1.0 };
    } else if (mat == "Mat_4_-1") {
        _current_rgba = { 0.0, 1.0, 0.0, 1.0 };
    } else if (mat == "red") { // general collors
        _current_rgba = { 1.0, 0.0, 0.0, 1.0 };
    } else if (mat == "green") {
        _current_rgba = { 0.0, 1.0, 0.0, 1.0 };
    } else if (mat == "blue") {
        _current_rgba = { 0.0, 0.0, 1.0, 1.0 };
    } else if (mat == "cyan") {
        _current_rgba = { 0.0, 1.0, 1.0, 1.0 };
    } else if (mat == "yellow") {
        _current_rgba = { 1.0, 1.0, 0.0, 1.0 };
    } else {
        // default to dark purple
        _current_rgba = { 0.2, 0, 0.2, 1 };

        if (_unknown_options.find (mat) == _unknown_options.end ()) {
            std::cout << "unknown material: " << mat << std::endl;
            _unknown_options.insert (mat);
        }
    }
    return status;
}

inline std::vector<std::string>
model_obj::tokenize_str (const std::string& in, const std::string& delim) {
    size_t split_pos;
    std::string right = in;
    std::string left;
    std::vector<std::string> ret;

    do {
        split_pos = right.find (delim);
        if (split_pos != std::string::npos) {
            left  = right.substr (0, split_pos);
            right = right.substr (split_pos + 1);
            left  = trim_str (left);
            right = trim_str (right);
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

inline std::string model_obj::trim_str (const std::string& s) {
    auto wsfront = std::find_if_not (
    s.begin (), s.end (), [](int c) { return std::isspace (c); });
    auto wsback = std::find_if_not (s.rbegin (), s.rend (), [](int c) {
        return std::isspace (c);
    }).base ();
    return (wsback <= wsfront ? std::string () : std::string (wsfront, wsback));
}
