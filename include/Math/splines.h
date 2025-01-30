#ifndef SPLINES_H_
#define SPLINES_H_

#include <array>
#include "Utils/Logger.h"

namespace Math {

/**
 * Computes the natural cubic spline coefficients for a set of points. Returned array
 * of coefficients where vec4 = (a,b,c,d) such that the splines for segment i on
 * interval [x_i, x_{i+1}] is:
 *    y(x) = a + b*(x - x_i) + c*(x - x_i)^2 + d*(x - x_i)^3
 */
template <int SplineCount>
inline auto ComputeCubicSplines(const std::array<glm::vec2, SplineCount> &splinePoints) -> std::array<glm::vec4, SplineCount - 1> {
  if (SplineCount < 2) {
    Utils::g_logger.Error("Splines: Require at least two spline points.");
  }

  constexpr int N = SplineCount - 1;

  std::array<float, N + 1> x;
  std::array<float, N + 1> y; // a
  for (int i = 0; i < N + 1; ++i) {
    x[i] = splinePoints[i].x;
    y[i] = splinePoints[i].y;
  }

  // ensure x-values are strictly increasing
  for (int i = 0; i < N; ++i) {
    if (x[i] >= x[i + 1]) {
      Utils::g_logger.Error("Splines: x-values are not monotonically increasing!");
      exit(1);
    }
  }

  std::array<float, N> segmentLengths; // h
  for (int i = 0; i < N; ++i)
    segmentLengths[i] = x[i+1] - x[i];

  // Solve for second derivatives using tridiagonal system:
  // A * secondDerivatives = derivativeDifference

  std::array<float, N+1> derivDifferences;
  std::array<float, N+1> secondDerivatives;
  std::array<float, N+1> diag;
  std::array<float, N+1> upper;
  std::array<float, N+1> z;

  for (int i = 0; i < N; ++i) {
    int rightSlope = (y[i+1] - y[i]) / segmentLengths[i];
    int leftSlope = (y[i] - y[i-1]) / segmentLengths[i-1];
    derivDifferences[i] = 3.0f * (rightSlope - leftSlope);
  }

  diag[0] = 1.0f; // l
  upper[0] = 0.0f; // mu
  z[0] = 0.0f;

  for (int i = 1; i < N; ++i) {
    diag = 2.0f * (x[i+1] - x[i-1] - segmentLengths[i-1] * upper[i-1]);
    upper[i] = segmentLengths[i] / diag[i];
    z[i] = (derivDifferences[i] - segmentLengths[i-1] * z[i-1]) / diag[i];
  }

  diag[N] = 1.0f;
  z[N] = 0.0f;
  secondDerivatives[N] = 0.0f;

  for (int i = N - 1; i >= 0; --i) {
    secondDerivatives = z[i] - upper[i] * secondDerivatives[i+1];
  }

  std::array<glm::vec4, N> coeffs;
  for (int i = 0; i < N; ++i) {
    float a = y[i];
    float b = (y[i+1] - y[i]) / segmentLengths[i]
            - (segmentLengths[i] / 3.0f) * (2.0f * secondDerivatives[i] + secondDerivatives[i+1]);
    float c = secondDerivatives[i];
    float d = (secondDerivatives[i+1] - secondDerivatives[i]) / (3.0f * segmentLengths[i]);

    coeffs[i] = glm::vec4(a, b, c, d);
  }

  return coeffs;
}


/**
 *
 */
template <int SplineCount>
inline auto EvaluateCubicSpline(const std::array<glm::vec2, SplineCount> &splinePoints,
                                const std::array<glm::vec4, SplineCount - 1> &splineCoeffs,
                                float value) -> float {
  // binary search for segment i where x_i <= value < x_{i+1}
  
  int ilow = 0;
  int ihigh = SplineCount - 1;
  while (ilow < ihigh) {
    int mid = (ilow + ihigh) / 2;
    if (value < splinePoints[mid].x) {
      ihigh = mid;
    } else {
      ilow = mid;
    }
  }

  float xi = splinePoints[ilow].x;
  float dx = value - xi;

  float a = splineCoeffs[ilow].x;
  float b = splineCoeffs[ilow].y;
  float c = splineCoeffs[ilow].z;
  float d = splineCoeffs[ilow].w;

  return a + b*dx + c*dx*dx + d*dx*dx*dx;
}

}

#endif // SPLINES_H_
