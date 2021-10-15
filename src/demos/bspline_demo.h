#ifndef BSPLINEDEMO_H
#define BSPLINEDEMO_H

#include <vector>

#include "demo.h"
#include "../camera.h"
#include "../bsplines/bspline.h"
#include "../bsplines/surface.h"
#include "../utils.h"

class BSplineDemo : public Demo {

public:
    explicit BSplineDemo(int width, int height);
    ~BSplineDemo() override;

    void resize(int width, int height) override;
    void draw() override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void mousewheel(float delta) override;
    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;

    void updateBSpline() override;
    void createBSpline() override;
protected:
    bool _updating;

    std::vector<glm::vec3> _vertices;

    GLuint _vaoLines;
    GLuint _vboLines;

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


#endif // BSPLINEDEMO_H