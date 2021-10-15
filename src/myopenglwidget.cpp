#include "myopenglwidget.h"

#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :QOpenGLWidget(parent)/*, QOpenGLFunctions_4_1_Core()*/, _openglDemo(nullptr), _lastime(0) {

}

MyOpenGLWidget::~MyOpenGLWidget() {

}

QSize MyOpenGLWidget::minimumSizeHint() const
{
    return QSize(640, 600);
}

QSize MyOpenGLWidget::sizeHint() const
{
    return QSize(640, 600);
}

void MyOpenGLWidget::cleanup() {
    _openglDemo.reset(nullptr);
}

void MyOpenGLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &MyOpenGLWidget::cleanup);

    if (!initializeOpenGLFunctions()) {
        QMessageBox::critical(this, "OpenGL initialization error", "MyOpenGLWidget::initializeGL() : Unable to initialize OpenGL functions");
        exit(1);
    }

    //_openglDemo.reset(new BSplineDemo(width(), height()));
    _openglDemo.reset(new SurfaceDemo(width(), height()));
}

void MyOpenGLWidget::setModalIndex(int index) {
    size_t size = _openglDemo->_degre + _openglDemo->_controlPoints.size() + 1;
    _openglDemo->_modalIndex = index;
    _openglDemo->_modals.clear();
    _openglDemo->_modals.resize(size);

    srand(time(NULL));
    int k, step, start;
    std::vector<float> t;
    switch (index) {
    case 0: // Uniforme
        std::iota(_openglDemo->_modals.begin(), _openglDemo->_modals.end(), 0);
        break;
    case 1: // Ouvert Uniforme
        k = int(size / 3);
        step = rand() % 4 + 1;
        start = rand() % 10;
        for (size_t i = 0; i < k; i++)
            _openglDemo->_modals[i] = start;
        for (size_t i = k; i < size - k; i++)
            _openglDemo->_modals[i] = _openglDemo->_modals[i - 1] + step;
        for (size_t i = size - k; i < size; i++)
            _openglDemo->_modals[i] = _openglDemo->_modals[size - k - 1] + step;

        t = _openglDemo->_modals;
        break;
    case 2: // Quelconque
        _openglDemo->_modals[0] = 0;
        for (size_t i = 1; i < size; i++) {
            _openglDemo->_modals[i] = _openglDemo->_modals[i - 1] + 1 + (rand() % 6);
        }

        break;
    }

    _openglDemo->updateBSpline();
    update();
}

std::vector<glm::vec3> MyOpenGLWidget::getControlPoints() {
    return _openglDemo->_controlPoints;
}

void MyOpenGLWidget::setControlPoints(std::vector<glm::vec3> controlPoints) {
    _openglDemo->_controlPoints = controlPoints;

    setModalIndex(_openglDemo->_modalIndex);
}

void MyOpenGLWidget::setDegreIndex(int index) {
    _openglDemo->_degre = index + 1;
    if (_openglDemo->_controlPoints.size() <= _openglDemo->_degre) {
        size_t nb = _openglDemo->_degre - _openglDemo->_controlPoints.size();
        for (int i = 0; i <= nb; i++)
            _openglDemo->_controlPoints.push_back({ 0, 0, 0 });
    }
    setModalIndex(_openglDemo->_modalIndex);
}

void MyOpenGLWidget::createBSpline() {
    _openglDemo->createBSpline();
}

void MyOpenGLWidget::paintGL() {
    std::int64_t starttime = QDateTime::currentMSecsSinceEpoch();
    _openglDemo->draw();
    glFinish();
    std::int64_t endtime = QDateTime::currentMSecsSinceEpoch();
    _lastime = endtime-starttime;
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    _openglDemo->resize(width, height);
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    // buttons are 0(left), 1(right) to 2(middle)
    int b;
    Qt::MouseButton button=event->button();
    if (button & Qt::LeftButton) {
        if ((event->modifiers() & Qt::ControlModifier))
            b = 2;
        else
            b = 0;
    } else if (button & Qt::RightButton)
        b = 1;
    else if (button & Qt::MiddleButton)
        b = 2;
    else
        b=3;
    _openglDemo->mouseclick(b, event->x(), event->y());
    _lastime = QDateTime::currentMSecsSinceEpoch();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    _openglDemo->mousemove(event->x(), event->y());
    update();
}

void MyOpenGLWidget::wheelEvent(QWheelEvent* event) {
    _openglDemo->mousewheel(event->angleDelta().y());
    update();
}
void MyOpenGLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        // Demo keys
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        break;
        // Move keys
        case Qt::Key_Left:
        case Qt::Key_Up:
        case Qt::Key_Right:
        case Qt::Key_Down:
            _openglDemo->keyboardmove(event->key()-Qt::Key_Left, 1./100/*double(_lastime)/10.*/);
            update();
        break;
        // Wireframe key
        case Qt::Key_W:
            _openglDemo->toggledrawmode();
            update();
        break;
        // Other keys are transmitted to the scene
        default :
            if (_openglDemo->keyboard(event->text().toStdString()[0]))
                update();
        break;
    }
}
