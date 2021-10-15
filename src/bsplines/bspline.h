#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>
#include "opengl_stuff.h"

class BSpline
{
public:
    BSpline(int degre, std::vector<glm::vec3> controlPoints, std::vector<float> modals) :
        _degre(degre), _controlPoints(controlPoints), _modals(modals) {
    }

    glm::vec3 generateFor(float);
    std::vector<glm::vec3> generate(float);

protected:
    int _degre;
    std::vector<glm::vec3> _controlPoints;
    std::vector<float> _modals;
};

#endif // BSPLINE_H
