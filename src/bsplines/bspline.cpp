#include "bspline.h"

glm::vec3 BSpline::generateFor(float u)
{
    int k = _degre + 1;
    int dec = 0;

    while (_modals[k + dec] < u) {
        dec++;
    }
    auto first = _controlPoints.begin() + dec;
    auto last = _controlPoints.begin() + dec + k;

    std::vector<glm::vec3> vec = { first, last };

    for (int j = 0; j < _degre; ++j) {
        for (int i = 0; i < k - 1; ++i) {
            float denom = (_modals[dec + k + i + j] - _modals[dec + 1 + i + j]);
            vec[i] = ((_modals[dec + k + i + j] - u) / denom) * vec[i]
                + ((u - _modals[dec + 1 + i + j]) / denom) * vec[i + 1];
        }
        k--;
    }

    return vec[0];
}

std::vector<glm::vec3> BSpline::generate(float step) {
    std::vector<glm::vec3> vector;

    for (float u = _modals[_degre]; u <= _modals[_controlPoints.size()]; u += step) {
        glm::vec3 p = this->generateFor(u);
        vector.push_back(p);
    }

    return vector;
}

