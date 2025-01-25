#include "Utils/utils.h"

namespace Utils {

float ScaleValue(float min1, float max1, float min2, float max2, float value) {
  float slope = (max2 - min2) / (max1 - min1);
  float intercept = min2 - slope * min1;
  return slope * value + intercept;
}

}
