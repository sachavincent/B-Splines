#ifndef DEMO_H
#define DEMO_H

#include <vector>

#include <QOpenGLFunctions_4_1_Core>
#include "../opengl_stuff.h"

class Demo : public QOpenGLFunctions_4_1_Core {

public:
    explicit Demo(int width, int height);
    virtual ~Demo();

    virtual void resize(int width, int height);
    virtual void draw();

    virtual void mouseclick(int button, float xpos, float ypos);
    virtual void mousemove(float xpos, float ypos);
    virtual void mousewheel(float delta);
    virtual void keyboardmove(int key, double time);
    virtual bool keyboard(unsigned char k);

    virtual void createBSpline();
    virtual void updateBSpline();

    void toggledrawmode();
public:
    int _degre;
    std::vector<glm::vec3> _controlPoints;
    std::vector<float> _modals;
    int _modalIndex = -1;
protected:
    int _width;
    int _height;
private:
    bool _drawfill;
};


#endif // DEMO_H