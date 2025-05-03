#include "Kalman.h"


void Kalman::init(float noiseCovariance, float initialEstimateCovariance, float initialEstimatedState)
{
  R = noiseCovariance;
  uHat = initialEstimatedState;
  Q = initialEstimateCovariance;
}

float Kalman::filter(float U)
{
  // update:
  K = P*H / (H*P*H+R); // update kalman gain
  uHat = uHat + K*(U-H*uHat); // update estimate
  // update error covariance:
  P = (1-K*H) * P+Q;

  // return estimate of uHat (the state):
  return uHat;
}
