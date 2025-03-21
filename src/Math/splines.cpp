#include "Math/splines.h"

namespace TinyMinecraft {

  /**
    * Computes the natural cubic spline coefficients for a set of points. Returned array
    * of coefficients where vec4 = (a,b,c,d) such that the splines for segment i on
    * interval [x_i, x_{i+1}] is:
    *    y(x) = a + b*(x - x_i) + c*(x - x_i)^2 + d*(x - x_i)^3
    */
  auto ComputeCubicSplines(const std::vector<glm::vec2> &splinePoints) -> std::vector<glm::vec4> {
    const int N = static_cast<int>(splinePoints.size()) - 1;

    if (N < 1) {
      Utils::Logger::Error("Splines: Require at least two spline points.");
      exit(1);
    }

    std::vector<float> x(N + 1);
    std::vector<float> y(N + 1);
    for (int i = 0; i <= N; ++i) {
      x[i] = splinePoints[i].x;
      y[i] = splinePoints[i].y;
    }

    for (int i = 0; i < N; ++i) {
      if (x[i] >= x[i + 1]) {
        Utils::Logger::Error("Splines: x-values are not monotonically increasing!");
        exit(1);
      }
    }

    std::vector<float> segmentLengths(N);
    for (int i = 0; i < N; ++i)
      segmentLengths[i] = x[i+1] - x[i];

    std::vector<float> derivDifferences(N+1);
    for (int i = 1; i < N; ++i) {
      float rightSlope = (y[i+1] - y[i]) / segmentLengths[i];
      float leftSlope = (y[i] - y[i-1]) / segmentLengths[i-1];
      derivDifferences[i] = 3.0f * (rightSlope - leftSlope);
    }

    std::vector<float> secondDerivatives(N+1);
    std::vector<float> diag(N+1);
    std::vector<float> upper(N+1);
    std::vector<float> z(N+1);

    diag[0] = 1.0f;
    upper[0] = 0.0f;
    z[0] = 0.0f;

    for (int i = 1; i < N; ++i) {
      diag[i] = 2.0f * (x[i+1] - x[i-1]) - segmentLengths[i-1] * upper[i-1];
      upper[i] = segmentLengths[i] / diag[i];
      z[i] = (derivDifferences[i] - segmentLengths[i-1] * z[i-1]) / diag[i];
    }

    diag[N] = 1.0f;
    z[N] = 0.0f;
    secondDerivatives[N] = 0.0f;

    for (int i = N - 1; i >= 0; --i) {
      secondDerivatives[i] = z[i] - upper[i] * secondDerivatives[i+1];
    }

    std::vector<glm::vec4> coeffs(N);
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

  auto ComputeMonotonicCubicSplines(const std::vector<glm::vec2> &splinePoints) -> std::vector<glm::vec4> {
    const int N = static_cast<int>(splinePoints.size()) - 1;
    if (N < 1) {
      Utils::Logger::Error("Splines: Require at least two spline points.");
      exit(1);
    }

    std::vector<float> x(N+1), y(N+1);
    for (int i = 0; i <= N; ++i) {
      x[i] = splinePoints[i].x;
      y[i] = splinePoints[i].y;
    }
    for (int i = 0; i < N; ++i) {
      if (x[i] >= x[i+1]) {
        Utils::Logger::Error("Splines: x-values are not monotonically increasing!");
        exit(1);
      }
    }

    std::vector<float> segmentLengths(N);
    std::vector<float> slope(N);
    for (int i = 0; i < N; ++i) {
      segmentLengths[i] = x[i+1] - x[i];
      slope[i] = (y[i+1] - y[i]) / segmentLengths[i];
    }

    std::vector<float> tangent(N+1);
    tangent[0] = slope[0];
    tangent[N] = slope[N-1];

    for (int i = 1; i < N; ++i) {
      if (slope[i-1] * slope[i] <= 0.0f) {
        tangent[i] = 0.0f;
      } else {
        float h0 = segmentLengths[i-1];
        float h1 = segmentLengths[i];
        float w1 = 2.0f * h1 + h0;
        float w2 = h1 + 2.0f * h0;
        tangent[i] = (w1 + w2) / ((w1 / slope[i-1]) + (w2 / slope[i]));
      }
    }

    std::vector<glm::vec4> coeffs(N);
    for (int i = 0; i < N; ++i) {
      float a = y[i];
      float b = tangent[i];
      float c = (3.0f * slope[i] - 2.0f * tangent[i] - tangent[i+1]) / segmentLengths[i];
      float d = (tangent[i] + tangent[i+1] - 2.0f * slope[i]) / (segmentLengths[i] * segmentLengths[i]);
      coeffs[i] = glm::vec4(a, b, c, d);
    }

    return coeffs;
  }


  /**
    * Evalues cubic spline givin input-output of `ComputeCubicSplines` or `ComputeMonotonicCubicSplines` and a point to compute at.
    */
  auto EvaluateCubicSpline(const std::vector<glm::vec2> &splinePoints,
                                  const std::vector<glm::vec4> &splineCoeffs,
                                  float value) -> float {
    // binary search for segment i where x_i <= value < x_{i+1}
    
    int ilow = 0;
    int ihigh = static_cast<int>(splinePoints.size()) - 1;
    while (ilow < ihigh - 1) {
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
