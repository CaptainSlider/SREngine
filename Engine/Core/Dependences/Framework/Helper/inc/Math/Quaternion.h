//
// Created by Nikita on 01.03.2021.
//

#ifndef GAMEENGINE_QUATERNION_H
#define GAMEENGINE_QUATERNION_H

#include <Math/Mathematics.h>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <limits>
#include <algorithm>
#include <glm/gtc/quaternion.hpp>

namespace Framework::Helper::Math {
    template<typename T> struct Vector3;

    class Matrix4x4;

    class Quaternion {
        friend Vector3<Unit>;
    private:
        glm::quat self{};
    public:
        [[nodiscard]] inline glm::quat ToGLM() const {
            return self;
        }

        [[nodiscard]] Matrix4x4 ToMat4x4() const;

        [[nodiscard]] inline glm::mat4 ToMat4x4GLM() const {
            return mat4_cast(self);
        }

        [[nodiscard]] Vector3<Unit> EulerAngle(bool degrees = true) const;

        [[nodiscard]] Quaternion Rotate(const Vector3<Unit>& v) const;
        Quaternion(const Quaternion &p_q) {
            this->self = p_q.self;
        }

        Quaternion() {
            self = glm::quat();
        }

        Quaternion(const Vector3<Unit> &p_euler, bool inRads = false);
        explicit Quaternion(const glm::quat &q) {
            self = q;
        }

        [[nodiscard]] Quaternion Inverse() const {
            return Quaternion(glm::inverse(self));
        }

        _FORCE_INLINE_ void operator+=(const Quaternion &p_q) {
            self += p_q.self;
        }
        _FORCE_INLINE_ void operator-=(const Quaternion &p_q) {
            self -= p_q.self;
        }
        _FORCE_INLINE_ void operator*=(const double &s){
            self *= s;
        }
        _FORCE_INLINE_ void operator/=(const double &s) {
            self *= 1.0 / s;
        }
        _FORCE_INLINE_ Quaternion operator+(const Quaternion &q2) const {
            const Quaternion &q1 = *this;
            return Quaternion(q1.self + q2.self);
        }
        _FORCE_INLINE_ Quaternion operator-(const Quaternion &q2) const {
            const Quaternion &q1 = *this;
            return Quaternion(q1.self - q2.self);
        }
        _FORCE_INLINE_ Quaternion operator-() const {
            const Quaternion &q2 = *this;
            return Quaternion(-q2.self);
        }
        _FORCE_INLINE_ Quaternion operator*(const double &s) const {
            glm::quat q = self;
            q *= s;
            return Quaternion(q);
        }
        _FORCE_INLINE_ Quaternion operator/(const double &s) const {
            glm::quat q = self;
            q *= 1.0 / s;
            return Quaternion(q);
        }

        Vector3<Unit> operator*(const Vector3<Unit> &v) const;
        Vector3<Unit> operator/(const Vector3<Unit> &v) const;
        Quaternion operator*(const Quaternion& rhs) const {
            return Quaternion(self * rhs.self);
        }
    };
}

#endif //GAMEENGINE_QUATERNION_H