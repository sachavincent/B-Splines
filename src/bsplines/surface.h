#ifndef SURFACE_H
#define SURFACE_H

#include <vector>
#include "opengl_stuff.h"
#include "bspline.h"

class Surface
{
public:
    Surface(int degre, const std::vector<std::vector<glm::vec3>>& controlPoints, const std::vector<float>& modals) :
        _degre(degre), _controlPoints(controlPoints), _modals(modals) {
    }

    std::vector<glm::vec3> generate(float);
protected:
    int _degre;
    std::vector<std::vector<glm::vec3>> _controlPoints;
    std::vector<float> _modals;
};

#endif // SURFACE_H
