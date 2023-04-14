#include "definitions.h"
Point3 operator+(const Point3& p1, const Point3& p2) {
        return Point3(p1.x() + p2.x(), p1.y() + p2.y(), p1.z() + p2.z());
            };
Point3 operator/(const Point3& p1, const double& scale) {
    return Point3(p1.x()/scale, p1.y()/scale, p1.z()/scale);
            };
