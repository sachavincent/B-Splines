#include "bspline_demo.h"
#include <iostream>
#include <fstream>

using namespace std;

static const GLchar* vertexshader_source = "#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        uniform mat4 model; \n\
        uniform mat4 view; \n\
        uniform mat4 projection; \n\
        void main()\n\
        {\n\
            gl_Position = projection * view * model * vec4(position, 1.0f); \n\
        }\n";

//normal = (model * vec4(normalize(inormal), 0.0)).xyz; \n\*/
static const GLchar* fragmentshader_source = "#version 410 core\n\
        out vec4 color;\n\
        void main()\n\
        {\n\
            color = vec4(1.0f, 0.5f, 0.2f, 1.0f); \n\
        }\n";

BSplineDemo::BSplineDemo(int width, int height): Demo(width, height), _camera(new EulerCamera(glm::vec3(0.f, 0.f, 1.f))) {
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

BSplineDemo::~BSplineDemo() {
    glDeleteProgram(_program);
    glDeleteVertexArrays(1, &_vaoLines);
    glDeleteVertexArrays(1, &_vaoPoints);
}

void BSplineDemo::resize(int width, int height) {
    Demo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _projection = glm::perspective(_camera->zoom(), float(_width) / _height, 0.1f, 100.0f);
}

void BSplineDemo::draw() {
    if (_modalIndex < 0 || _updating)
        return;

    Demo::draw();

    glUseProgram(_program);

    _view = _camera->viewmatrix();

    auto matrix = Utils::createTransformationMatrix({ 0, 0, 0 }, _rotation, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE, (GLfloat*) &matrix);
    glUniformMatrix4fv(glGetUniformLocation(_program, "view"), 1, GL_FALSE, glm::value_ptr(_view));
    glUniformMatrix4fv(glGetUniformLocation(_program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

    glBindVertexArray(_vaoLines);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINE_STRIP, 0, _vertices.size());
    glDisableVertexAttribArray(0);

    glBindVertexArray(_vaoPoints);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, _controlPoints.size());

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

void BSplineDemo::createBSpline() {
     if (_modalIndex >= 0)
        return;

    _controlPoints = { glm::vec3(-1, -1, 0), glm::vec3(-0.5f, 1, 0), glm::vec3(0.5f, -1, 0), glm::vec3(1, 1, 0) };
    _degre = 1;
    _modalIndex = 0;
    _modals = { 0, 1, 2, 3, 4, 5 };

    BSpline* bSpline = new BSpline(_degre, _controlPoints, _modals);

    _vertices = bSpline->generate(0.01f);

    glGenVertexArrays(1, &_vaoLines);
    glBindVertexArray(_vaoLines);
    glGenBuffers(1, &_vboLines);
    glBindBuffer(GL_ARRAY_BUFFER, _vboLines);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * 3 * sizeof(GLfloat), &_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

void BSplineDemo::updateBSpline() {
    _updating = true;
    std::vector<glm::vec3> controlPoints = { glm::vec3(-1, -1, 0), glm::vec3(-0.5f, 1, 0),
                                        glm::vec3(0.5f, -1, 0), glm::vec3(1, 1, 0) };

    BSpline* bSpline = new BSpline(_degre, _controlPoints, _modals);
    
    _vertices = bSpline->generate(0.01f);

    glBindBuffer(GL_ARRAY_BUFFER, _vboLines);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * 3 * sizeof(GLfloat), &_vertices[0], GL_STATIC_DRAW);
   
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _vboPoints);
    glBufferData(GL_ARRAY_BUFFER, _controlPoints.size() * 3 * sizeof(GLfloat), &_controlPoints[0], GL_STATIC_DRAW);
   
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _updating = false;
}

void BSplineDemo::mouseclick(int, float, float) {

}

void BSplineDemo::mousemove(float, float) {

}

void BSplineDemo::mousewheel(float v) {
    _camera->processmousescroll(v);
}

void BSplineDemo::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool BSplineDemo::keyboard(unsigned char k) {
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
    }
    return true;
}
