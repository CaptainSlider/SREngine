//
// Created by Nikita on 01.03.2021.
//

#ifndef GAMEENGINE_VECTOR3_H
#define GAMEENGINE_VECTOR3_H

#include <string>
#include "Mathematics.h"
#include <glm/glm.hpp>

namespace Framework::Helper::Math {
    class Quaternion;

    struct  Vector2 {
    public:
        union {
            struct {
                double x;
                double y;
            };

            double coord[2] = {0};
        };

        _FORCE_INLINE_ const double &operator[](int p_axis) const {
            return coord[p_axis];
        }

        _FORCE_INLINE_ double &operator[](int p_axis) {
            return coord[p_axis];
        }
        _FORCE_INLINE_ Vector2(double p_x, double p_y) {
            x = p_x;
            y = p_y;
        }
    };

    struct  Vector3 {
    public:
        enum Axis {
            AXIS_X,
            AXIS_Y,
            AXIS_Z,
        };

        union {
            struct {
                double x;
                double y;
                double z;
            };

            double coord[3] = { 0 };
        };

        [[nodiscard]] inline bool Empty() const {
            return (x == 0 && y == 0 && z == 0);
        }

        [[nodiscard]] inline Vector3 Radians() const noexcept {
            return { RAD(x), RAD(y), RAD(z) };
        }
        [[nodiscard]] inline Vector3 Degrees() const noexcept {
            return { DEG(x), DEG(y), DEG(z) };
        }

        [[nodiscard]] inline glm::vec3 ToGLM() const noexcept {
            return glm::vec3(x,y,z);
        }
         Vector3(glm::vec3 v) {
            x = v.x;
            y = v.y;
            z = v.z;
        }

        inline std::string ToString(){
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
        }

        //Vector3(const Quaternion& q);
        _FORCE_INLINE_ Vector3() {
            x = 0;
            y = 0;
            z = 0;
        }
        _FORCE_INLINE_ Vector3(double p_x, double p_y, double p_z) {
            x = p_x;
            y = p_y;
            z = p_z;
        }

        _FORCE_INLINE_ const double &operator[](int p_axis) const {
            return coord[p_axis];
        }

        _FORCE_INLINE_ double &operator[](int p_axis) {
            return coord[p_axis];
        }

        static inline double Dot(Vector3 lhs, Vector3 rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }
        [[nodiscard]] double Dot(Vector3 p_b) const { return x * p_b.x + y * p_b.y + z * p_b.z; }

        [[nodiscard]] inline Vector3 Cross(const Vector3 &p_b) const {
            Vector3 ret(
                    (y * p_b.z) - (z * p_b.y),
                    (z * p_b.x) - (x * p_b.z),
                    (x * p_b.y) - (y * p_b.x));

            return ret;
        }
        static inline Vector3 Cross(const Vector3 &p_a, const Vector3 &p_b) {
            Vector3 ret(
                    (p_a.y * p_b.z) - (p_a.z * p_b.y),
                    (p_a.z * p_b.x) - (p_a.x * p_b.z),
                    (p_a.x * p_b.y) - (p_a.y * p_b.x));
            return ret;
        }

        _FORCE_INLINE_ Vector3 &operator+=(const Vector3 &p_v){
            x += p_v.x;
            y += p_v.y;
            z += p_v.z;
            return *this;
        }
        _FORCE_INLINE_ Vector3 operator+(const Vector3 &p_v) const {
            return Vector3(x + p_v.x, y + p_v.y, z + p_v.z); }
        _FORCE_INLINE_ Vector3 &operator-=(const Vector3 &p_v) {
            x -= p_v.x;
            y -= p_v.y;
            z -= p_v.z;
            return *this;
        }
        _FORCE_INLINE_ Vector3 operator-(const Vector3 &p_v) const {
            return Vector3(x - p_v.x, y - p_v.y, z - p_v.z);
        }
        _FORCE_INLINE_ Vector3 &operator*=(const Vector3 &p_v) {
            x *= p_v.x;
            y *= p_v.y;
            z *= p_v.z;
            return *this;
        }
        _FORCE_INLINE_ Vector3 operator*(const Vector3 &p_v) const {
            return Vector3(x * p_v.x, y * p_v.y, z * p_v.z);
        }

        _FORCE_INLINE_ Vector3 &operator/=(const Vector3 &p_v) {
            x /= p_v.x;
            y /= p_v.y;
            z /= p_v.z;
            return *this;
        }
        _FORCE_INLINE_ Vector3 operator/(const Vector3 &p_v) const {
            return Vector3(x / p_v.x, y / p_v.y, z / p_v.z);
        }

        _FORCE_INLINE_ Vector3 &operator*=(double p_scalar) {
            x *= p_scalar;
            y *= p_scalar;
            z *= p_scalar;
            return *this;
        }
        _FORCE_INLINE_ Vector3 operator*(double p_scalar) const {
            return Vector3(x * p_scalar, y * p_scalar, z * p_scalar);
        }
        _FORCE_INLINE_ Vector3 &operator/=(double p_scalar) {
            x /= p_scalar;
            y /= p_scalar;
            z /= p_scalar;
            return *this;
        }
        _FORCE_INLINE_ Vector3 operator/(double p_scalar) const {
            return Vector3(x / p_scalar, y / p_scalar, z / p_scalar);
        }

        _FORCE_INLINE_ Vector3 operator-() const { return Vector3(-x, -y, -z); }

        _FORCE_INLINE_ bool operator==(const Vector3 &p_v) const { return x == p_v.x && y == p_v.y && z == p_v.z; }
        _FORCE_INLINE_ bool operator!=(const Vector3 &p_v) const {return x != p_v.x || y != p_v.y || z != p_v.z; }
        _FORCE_INLINE_ bool operator<(const Vector3 &p_v) const {
            if (x == p_v.x) {
                if (y == p_v.y) {
                    return z < p_v.z;
                }
                return y < p_v.y;
            }
            return x < p_v.x;
        }
        _FORCE_INLINE_ bool operator<=(const Vector3 &p_v) const {
            if (x == p_v.x) {
                if (y == p_v.y) {
                    return z > p_v.z;
                }
                return y > p_v.y;
            }
            return x > p_v.x;
        }
        _FORCE_INLINE_ bool operator>(const Vector3 &p_v) const {
            if (x == p_v.x) {
                if (y == p_v.y) {
                    return z <= p_v.z;
                }
                return y < p_v.y;
            }
            return x < p_v.x;
        }
        _FORCE_INLINE_ bool operator>=(const Vector3 &p_v) const {
            if (x == p_v.x) {
                if (y == p_v.y) {
                    return z >= p_v.z;
                }
                return y > p_v.y;
            }
            return x > p_v.x;
        }

        Vector3 Rotate(const Quaternion& q) const;
    };
}

#endif //GAMEENGINE_VECTOR3_H
