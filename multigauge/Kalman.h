#ifndef KALMAN_H
#define KALMAN_H
#include <Arduino.h>


class Kalman
{
  private:
    float H = 1; // measurement map scalar
    float P = 0; // initial error covariance (must be 0)
    float K = 1; // initial kalman gain

  public:
    float R; // measurement noise coveriance, represents uncertainty or inaccuracy in the sensor, higher R means the measurement is less trustworthy.
    float Q = 1; // initial estimated covariance, process noise covariance, higher Q means system is more dynamic/less predictable. Filter will rely more on the measurement
    float uHat = 0; // initial estimated state
    void init(float R, float Q, float uHat);

    float filter(float U);
};

#endif
