#ifndef SURFACEDEMO_H
#define SURFACEDEMO_H

#include <vector>

#include "demo.h"
#include "../camera.h"
#include "../bsplines/bspline.h"
#include "../bsplines/surface.h"
#include "../utils.h"

class SurfaceDemo : public Demo {

public:
    explicit SurfaceDemo(int width, int height);
    ~SurfaceDemo() override;

    void draw() override;
    void resize(int width, int height) override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void mousewheel(float delta) override;
    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;

    void handleVertices(std::vector<glm::vec3>);

    void updateBSpline() override;
    void createBSpline() override;
protected:
    bool _updating;

    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _normals;
    std::vector<GLuint> _indices;

    GLuint _vaoLines;
    GLuint _vboLines;
    GLuint _ibo;
    GLuint _nbo;

    GLuint _vaoPoints;
    GLuint _vboPoints;

    GLuint _program;

    std::unique_ptr<Camera> _camera;

    // matrices
    glm::vec3 _rotation;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
};


#endif // SURFACEDEMO_H