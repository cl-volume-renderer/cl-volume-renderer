#pragma once

#include <math.h>

struct Position3D {
   float val[3];
   Position3D(double alpha, double beta, double gamma, Position3D base) {
     val[0] = (cos(alpha)*cos(beta))*base.val[0] + (cos(alpha)*sin(beta)-sin(alpha)*cos(gamma))*base.val[1] + (cos(alpha)*sin(beta)*cos(gamma)+sin(alpha)*sin(gamma))*base.val[2];
     val[1] = (-sin(beta))*base.val[0] + (cos(beta)*sin(gamma))*base.val[1] + (cos(beta)*cos(gamma))*base.val[2];
     val[2] = (sin(alpha)*cos(beta))*base.val[0] + (sin(alpha)*sin(beta)*sin(gamma)+cos(alpha)*cos(gamma))*base.val[1] + (sin(alpha)*sin(beta)*cos(gamma)-cos(alpha)*sin(gamma))*base.val[2];
     (*this).normalize();
   }
   Position3D(double x, double y, double z) {
     val[0] = x;
     val[1] = y;
     val[2] = z;
   }
   Position3D operator+(Position3D other) {
    Position3D ret = {
      val[0] + other.val[0],
      val[1] + other.val[1],
      val[2] + other.val[2]
    };
    return ret;
   }
   Position3D operator-(Position3D other) {
    Position3D ret = {
      val[0] - other.val[0],
      val[1] - other.val[1],
      val[2] - other.val[2]
    };
    return ret;
   }
   Position3D operator*(float other) {
    Position3D ret = {
      val[0] * other,
      val[1] * other,
      val[2] * other
    };
    return ret;
   }
   Position3D operator/(float other) {
    Position3D ret = {
      val[0] / other,
      val[1] / other,
      val[2] / other
    };
    return ret;
   }
   double length() {
    return sqrtf(pow(val[0], 2) + pow(val[1], 2) + pow(val[2], 2));
   }
   void normalize() {
    double len = (*this).length();
    *this = *this / len;
   }
};

inline unsigned long
evenness(const unsigned int g, const unsigned int l)
{
   unsigned int mod = g % l;
   if (g % l == 0) return g;

   return g + l - mod;
}
