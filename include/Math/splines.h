#ifndef SPLINES_H_
#define SPLINES_H_

#include "Utils/Logger.h"

namespace TinyMinecraft {

  /**
    * Computes the natural cubic spline coefficients for a set of points. Returned array
    * of coefficients where vec4 = (a,b,c,d) such that the splines for segment i on
    * interval [x_i, x_{i+1}] is:
    *    y(x) = a + b*(x - x_i) + c*(x - x_i)^2 + d*(x - x_i)^3
    */
  [[nodiscard]] auto ComputeCubicSplines(const std::vector<glm::vec2> &splinePoints) -> std::vector<glm::vec4>;
  [[nodiscard]] auto ComputeMonotonicCubicSplines(const std::vector<glm::vec2> &splinePoints) -> std::vector<glm::vec4>;


  /**
    * Evalues cubic spline givin input-output of `ComputeCubicSplines` or `ComputeMonotonicCubicSplines` and a point to compute at.
    */
  [[nodiscard]] auto EvaluateCubicSpline(const std::vector<glm::vec2> &splinePoints, const std::vector<glm::vec4> &splineCoeffs, float value) -> float;

}

#endif // SPLINES_H_
