#include "demo.h"

Demo::Demo(int width, int height) : _width(width), _height(height), _drawfill(true) {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
}

Demo::~Demo() {
}

void Demo::resize(int width, int height) {
    _width = width;
    _height = height;
    glViewport(0, 0, _width, _height);
}

void Demo::draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (_drawfill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Demo::mouseclick(int, float, float) {

}

void Demo::mousemove(float, float) {

}

void Demo::mousewheel(float) {

}

void Demo::keyboardmove(int, double) {

}

void Demo::createBSpline() {

}

void Demo::updateBSpline() {

}

bool Demo::keyboard(unsigned char) {
    return false;
}

void Demo::toggledrawmode() {
    _drawfill = !_drawfill;
}
