#ifndef ROTATION_H
#define ROTATION_H


#include <math.h>
#include <windows.h>
#include <ctime>
#include <QDebug>

#include "glm.hpp"
#include "gtx/quaternion.hpp"

const float gravity = 256;

const float accelXScale = gravity / 250.f;
const float accelYScale = gravity / 250.f;
const float accelZScale = gravity / 250.f;

const float magXScale = .16666f;
const float magYScale = .16666f;
const float magZScale = .16666f;

const float pRollPitch = .02f;
const float iRollPitch = .00002f;
const float pYaw = 1.2f;
const float iYaw = .00002f;

struct Rotation {
    glm::vec3 accel, mag, gyro, omega, omegaP, omegaI;
    glm::mat3 DCM;
    float roll, pitch, yaw, magneticHeading;
    double lastTime;
    Rotation()
        : accel(glm::vec3()), mag(glm::vec3()), gyro(glm::vec3()), DCM(glm::mat3(1.0)) {
    }
    void setData(const float&ax, const float&ay, const float&az, const float&gx, const float&gy, const float&gz, const float&mx, const float&my, const float&mz) {
        accel.x = ax;
        accel.y = ay;
        accel.z = az;
        gyro.x = gx;
        gyro.y = gy;
        gyro.z = gz;
        mag.x = mx;
        mag.y = my;
        mag.z = mz;
    }
    void compensateSensorErrors() {
        accel.x *= accelXScale;
        accel.y *= accelYScale;
        accel.z *= accelZScale;

        mag.x *= magXScale;
        mag.y *= magYScale;
        mag.z *= magZScale;
    }
    void getHeading() {
      float a, b;

      // Tilt compensated magnetic field X
      a = (mag.x * cos(pitch)) + (mag.y * sin(roll) * sin(pitch)) + (mag.z * cos(roll) * sin(roll));
      // Tilt compensated magnetic field Y
      b = (mag.y * cos(roll)) - (mag.z * sin(roll));
      // Magnetic Heading
      magneticHeading = atan2(-b, a);
    }
    void driftCorrection() {
        float a = glm::length(accel);
        a /= gravity;
        float w = std::min(std::max(1 - 2 * std::abs(1 - a), 0.f), 1.f);

        //Roll and Pitch
        glm::vec3 errorPitchRoll = glm::cross(accel, DCM[2]);
        omegaP = errorPitchRoll * (pRollPitch * w);
        omegaI = omegaI + (errorPitchRoll * (iRollPitch * w));

        //Yaw
        const float errorCourse = DCM[0][0] * sin(magneticHeading) - DCM[1][0] * cos(magneticHeading);
        glm::vec3 errorYaw = DCM[2] * errorCourse;

        omegaP = omegaP + errorYaw * pYaw;
        omegaI = omegaI + errorYaw * iYaw;
    }

    void matUpdate() {
        SYSTEMTIME nowTime;
        GetSystemTime(&nowTime);
        double t = std::time(NULL) + ((double)nowTime.wMilliseconds/1000.0);
        double timeDif = t - lastTime;
        qDebug() << t << "\n";
        lastTime = t;

        const float gyroGain = .06957f;
        glm::vec3 g = gyro * ((float)M_PI / 180.f) * gyroGain;
        glm::vec3 a = accel;

        omega = g + omegaI + omegaP;

        glm::mat3 update = glm::mat3(0, -timeDif * omega.z, timeDif * omega.y,
                                     timeDif * omega.z, 0, -timeDif * omega.x,
                                     -timeDif * omega.y, timeDif * omega.x, 0);
        glm::mat3 temp;
        temp = DCM * update;
        DCM = DCM + temp;

        //Normalize
        float err = -glm::dot(DCM[0], DCM[1]) * .5;
        temp = glm::mat3(0);
        temp[0] = DCM[1] * err;
        temp[1] = DCM[0] * err;

        temp[0] = temp[0] + DCM[0];
        temp[1] = temp[1] + DCM[1];

        temp[2] = glm::cross(temp[0], temp[1]);

        for (unsigned int i = 0; i < 3; i++) {
            float rn = .5 * (3 - glm::dot(temp[i], temp[i]));
            DCM[i] = temp[i] * rn;
        }
    }
};
#endif // ROTATION_H
