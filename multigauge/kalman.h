#ifndef kalman_H
#define kalman_H

static const float R = 60; // noise covariance
static const float H = 1; // measurement map scalar
static float Q = 10; // initial estimated covariance
static float P = 0; // initial error covariance (must be 0)
static float uHat = 0; // initial estimated state
static float K = 0; // initial kalman gain

float kalmanFilter(float U)
{
  // update:
  K = P*H / (H*P*H+R); // update kalman gain
  uHat = uHat + K*(U-H*uHat); // update estimate
  // update error covariance:
  P = (1-K*H) * P+Q;

  // return estimate of uHat (the state):
  return uHat;
};
#endif