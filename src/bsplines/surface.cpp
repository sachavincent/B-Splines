#include "surface.h"

std::vector<glm::vec3> Surface::generate(float step)
{
	std::vector<glm::vec3> points;
	for (float u = _modals[_degre]; u <= _modals[_controlPoints.size()]; u += step) {
		for (float v = _modals[_degre]; v <= _modals[_controlPoints.size()]; v += step) {
			std::vector<glm::vec3> controlPoints;
			for (int i = 0; i < _controlPoints.size(); i++) {
				controlPoints.push_back(BSpline(_degre, _controlPoints[i], _modals).generateFor(u));
			}
			points.push_back(BSpline(_degre, controlPoints, _modals).generateFor(v));
		}
	}
	return points;
}