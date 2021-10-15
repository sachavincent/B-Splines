#include "surface_demo.h"
#include <iostream>
#include <fstream>

using namespace std;

static const GLchar* vertexshader_source = "#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        layout (location = 1) in vec3 inormal;\n\
        uniform mat4 model;\n\
        uniform mat4 view;\n\
        uniform mat4 projection;\n\
        out vec3 normal;\n\
        out vec3 fragPos;\n\
        void main()\n\
        {\n\
            vec4 pos = vec4(position, 1.0f);\n\
            gl_Position = projection * view * model * pos; \n\
            fragPos = vec3(model * pos);\n\
            normal = (model * vec4(normalize(inormal), 0.0)).xyz;\n\
        }\n";

static const GLchar* fragmentshader_source = "#version 410 core\n\
        uniform vec3 viewPos;\n\
        in vec3 fragPos;\n\
        in vec3 normal;\n\
        out vec4 color;\n\
        void main()\n\
        {\n\
            vec3 norm = normalize(normal);\n\
            vec3 lightPos = vec3(0, 0, 2);\n\
            vec3 lightColor = vec3(1, 1, 1);\n\
            vec3 lightDir = normalize(lightPos - fragPos);\n\
            float diff = max(dot(norm, lightDir), 0.0);\n\
            vec3 diffuse = diff * lightColor;\n\
            vec3 viewDir = normalize(viewPos - fragPos);\n\
            vec3 reflectDir = reflect(-lightDir, norm);\n\
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n\
            vec3 specular = 0.5 * spec * lightColor;\n\
            color = vec4(diffuse + specular, 1.0);\n\
        }\n";

//color = vec4(normal * 0.5 + 0.5, 1); 
SurfaceDemo::SurfaceDemo(int width, int height) : Demo(width, height), _camera(new EulerCamera(glm::vec3(0.f, 0.f, 1.f))) {
    _rotation = { 0, 0, 0 };
    _updating = false;
    GLint success;
    GLchar infoLog[512]; // warning fixed size ... request for LOG_LENGTH!!!
    GLuint vertexshader, fragmentshader;

    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexshader, 1, &vertexshader_source, nullptr);
    glCompileShader(vertexshader);
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &fragmentshader_source, NULL);
    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glPointSize(10);
    _program = glCreateProgram();

    glAttachShader(_program, vertexshader);
    glAttachShader(_program, fragmentshader);
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);

    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();

    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

SurfaceDemo::~SurfaceDemo() {
    glDeleteProgram(_program);
    glDeleteVertexArrays(1, &_vaoLines);
    glDeleteVertexArrays(1, &_vaoPoints);
    glDeleteVertexArrays(1, &_ibo);
    glDeleteVertexArrays(1, &_nbo);
}

void SurfaceDemo::resize(int width, int height) {
    Demo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void SurfaceDemo::draw() {
    if (_modalIndex < 0 || _updating)
        return;

    Demo::draw();
    glUseProgram(_program);
    _modalIndex = 0;
    _view = _camera->viewmatrix();

    auto matrix = Utils::createTransformationMatrix({ 0, 0, 0 }, _rotation, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE, (GLfloat*) &matrix);
    glUniformMatrix4fv(glGetUniformLocation(_program, "view"), 1, GL_FALSE, glm::value_ptr(_view));
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));
    glUniformMatrix4fv(glGetUniformLocation(_program, "viewPos"), 1, GL_FALSE, glm::value_ptr(_camera->position()));

    glBindVertexArray(_vaoLines);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);


    glBindVertexArray(_vaoPoints);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, _controlPoints.size());

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

void SurfaceDemo::createBSpline() {
    if (_modalIndex >= 0)
       return;
    _modalIndex = 0;
    std::vector<std::vector<glm::vec3>> controls = {
        { {0, 0, 0}, {2 / 12.0f, 0, 0}, {5 / 12.0f, 0, 0}, {7 / 12.0f, 0, 0}, {10 / 12.0f, 0, 0}, {12 / 12.0f, 0, 0} },
        { {0, 2 / 12.0f, 0}, {2 / 12.0f, 2 / 12.0f, 1 / 12.0f}, {5 / 12.0f, 2 / 12.0f, 2 / 12.0f}, {7 / 12.0f, 2 / 12.0f, 3 / 12.0f}, {10 / 12.0f, 2 / 12.0f, 1 / 12.0f}, {12 / 12.0f, 2 / 12.0f, 0} },
        { {0, 5 / 12.0f, 0}, {2 / 12.0f, 5 / 12.0f, 1 / 12.0f}, {5 / 12.0f, 5 / 12.0f, 2 / 12.0f}, {7 / 12.0f, 5 / 12.0f, 3 / 12.0f}, {10 / 12.0f, 5 / 12.0f, 1 / 12.0f}, {12 / 12.0f, 5 / 12.0f, 0} },
        { {0, 7 / 12.0f, 0}, {2 / 12.0f, 7 / 12.0f, -1 / 12.0f}, {5 / 12.0f, 7 / 12.0f, -2 / 12.0f}, {7 / 12.0f, 7 / 12.0f, -3 / 12.0f}, {10 / 12.0f, 7 / 12.0f, -1 / 12.0f}, {12 / 12.0f, 7 / 12.0f, 0} },
        { {0, 10 / 12.0f, 0}, {2 / 12.0f, 10 / 12.0f, -1 / 12.0f}, {5 / 12.0f, 10 / 12.0f, -2 / 12.0f}, {7 / 12.0f, 10 / 12.0f, -3 / 12.0f}, {10 / 12.0f, 10 / 12.0f, -1 / 12.0f}, {12 / 12.0f, 10 / 12.0f, 0} },
        { {0, 12 / 12.0f, 0}, {2 / 12.0f, 12 / 12.0f, 0}, { 5 / 12.0f, 12 / 12.0f, 0}, {7 / 12.0f, 12 / 12.0f, 0}, {10 / 12.0f, 12 / 12.0f, 0}, {12 / 12.0f, 12 / 12.0f, 0} }
    };

    float myconstant{ -0.5f };
    std::transform(controls.begin(), controls.end(), controls.begin(), [&myconstant](auto& vec) {
        std::transform(vec.begin(), vec.end(), vec.begin(), [&myconstant](auto& c) {
            glm::vec3 res;
            res.x = c.x + myconstant;
            res.y = c.y + myconstant;
            res.z = c.z;
            return res;
        });
        return vec;
    });

    for (auto&& v : controls) {
        _controlPoints.insert(_controlPoints.end(), v.begin(), v.end());
    }
    //Surface* surface = new Surface(1, controls, { 0, 0, 0, 1, 2, 3, 3, 3 });
    Surface* surface = new Surface(2, controls, { 0, 0, 0, 1, 2, 3, 4, 4, 4 });
    //Surface* surface = new Surface(3, controls, { 0, 0, 0, 1, 2, 3, 4, 5, 5, 5 });
    //Surface* surface = new Surface(4, controls, { 0, 0, 0, 1, 2, 3, 4, 5, 6, 6, 6 });
    auto vertices = surface->generate(0.04f);
    /*ofstream output("output2.txt");
    for (auto& v : _vertices) {
        output << v.x << " " << v.y << " " << v.z << std::endl;
    }*/

    handleVertices(vertices);

    glGenVertexArrays(1, &_vaoLines);
    glBindVertexArray(_vaoLines);
    glGenBuffers(1, &_vboLines);
    glBindBuffer(GL_ARRAY_BUFFER, _vboLines);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * 3 * sizeof(GLfloat), &_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &_nbo);
    glBindBuffer(GL_ARRAY_BUFFER, _nbo);
    glBufferData(GL_ARRAY_BUFFER, _normals.size() * 3 * sizeof(GLfloat), &_normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &_ibo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLfloat), _indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glGenVertexArrays(1, &_vaoPoints);
    glBindVertexArray(_vaoPoints);
    glGenBuffers(1, &_vboPoints);
    glBindBuffer(GL_ARRAY_BUFFER, _vboPoints);
    glBufferData(GL_ARRAY_BUFFER, _controlPoints.size() * 3 * sizeof(GLfloat), &_controlPoints[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void SurfaceDemo::handleVertices(std::vector<glm::vec3> vertices) {
    int VERTEX_COUNT = int(sqrt(vertices.size()));

    std::vector<GLuint> indices(6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1));
    
    int pointer = 0;
    for (int gz = 0; gz < VERTEX_COUNT - 1; gz++) {
        for (int gx = 0; gx < VERTEX_COUNT - 1; gx++) {
            int topLeft = (gz * VERTEX_COUNT) + gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
            int bottomRight = bottomLeft + 1;
            indices[pointer++] = topLeft;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = topRight;
            indices[pointer++] = topRight;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = bottomRight;
        }
    }

    _normals.resize(vertices.size());
    
    for (int i = 0; i < indices.size(); i += 3) {
        // Get the face normal
        auto vector1 = vertices[indices[(size_t)i + 1]] - vertices[indices[i]];
        auto vector2 = vertices[indices[(size_t)i + 2]] - vertices[indices[i]];
        glm::vec3 faceNormal;

        faceNormal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
        faceNormal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
        faceNormal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];

        // Add the face normal to the 3 vertices normal touching this face
        _normals[indices[i]] += faceNormal;
        _normals[indices[(size_t)i + 1]] += faceNormal;
        _normals[indices[(size_t)i + 2]] += faceNormal;
    }

    _indices = indices;
    _vertices = vertices;
}

void SurfaceDemo::updateBSpline() {
    _updating = true;
    _updating = false;
}

void SurfaceDemo::mouseclick(int, float, float) {

}

void SurfaceDemo::mousemove(float, float) {

}

void SurfaceDemo::mousewheel(float v) {
    _camera->processmousescroll(v);
}

void SurfaceDemo::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool SurfaceDemo::keyboard(unsigned char k) {
    switch (k) {
    case 'q':
        _rotation.y = fmod(_rotation.y - 10.0f, 360.0f);
        return true;
    case 'd':
        _rotation.y = fmod(_rotation.y + 10.0f, 360.0f);
        return true;
    case 's':
        _rotation.z = fmod(_rotation.z - 10.0f, 360.0f);
        return true;
    case 'z':
        _rotation.z = fmod(_rotation.z + 10.0f, 360.0f);
        return true;
    case 'c':
        _rotation.x = fmod(_rotation.x - 10.0f, 360.0f);
        return true;
    case 'v':
        _rotation.x = fmod(_rotation.x + 10.0f, 360.0f);
        return true;
    case 't':
        toggledrawmode();
        return true;
    }
    return true;
}
