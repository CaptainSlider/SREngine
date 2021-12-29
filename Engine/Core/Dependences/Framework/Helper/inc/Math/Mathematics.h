//
// Created by Nikita on 17.11.2020.
//

#ifndef GAMEENGINE_MATHEMATICS_H
#define GAMEENGINE_MATHEMATICS_H

#ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
#endif

#ifndef SR_PI
    #define SR_PI M_PI
#endif

#define RAD(x) (x * SR_PI / 180.0)
#define DEG(x) (x / SR_PI * 180.0)

//#define _FORCE_INLINE_ __attribute__((always_inline))
//#define _ALWAYS_INLINE_ inline __attribute__((always_inline))

#define _FORCE_INLINE_ __forceinline
#define _ALWAYS_INLINE_ inline

#define DegreesToRad(angle) angle*M_PI/180
#define RadToDegrees(angle) angle*180/M_PI

#define CMP_EPSILON 0.00001
#define CMP_BIG_EPSILON 0.001
#define CMP_EPSILON2 (CMP_EPSILON * CMP_EPSILON)
#define UNIT_EPSILON 0.00001

#define CMP_NORMALIZE_TOLERANCE 0.000001
#define CMP_POINT_IN_PLANE_EPSILON 0.00001

#define Math_SQRT12 0.7071067811865475244008443621048490
#define Math_SQRT2 1.4142135623730950488016887242
#define Math_LN2 0.6931471805599453094172321215
#define Math_TAU 6.2831853071795864769252867666
#define Math_PI 3.1415926535897932384626433833
#define Math_E 2.7182818284590452353602874714
#define Math_INF INFINITY
#define Math_NAN NAN

#define SR_MAX(a, b) (a > b ? a : b)
#define SR_MIN(a, b) (a < b ? a : b)

#define RAD3(v) glm::vec3(RAD(v.x), RAD(v.y), RAD(v.z))
#define DEG3(v) glm::vec3(DEG(v.x), DEG(v.y), DEG(v.z))

#include <cmath>
#include <cfloat>
#include <cstdint>
#include <limits>

namespace Framework::Helper::Math {
    typedef double Unit; //! can broken render

    const double_t DoubleMAX = DBL_MAX;
    const float_t  FloatMAX  = FLT_MAX;
    const int32_t  Int32MAX  = INT32_MAX;

    const Unit UnitMAX = DoubleMAX;

    static _ALWAYS_INLINE_ bool is_equal_approx(Unit a, Unit b) noexcept {
        // Check for exact equality first, required to handle "infinity" values.
        if (a == b) {
            return true;
        }
        // Then check for approximate equality.
        double tolerance = CMP_EPSILON * abs(a);
        if (tolerance < CMP_EPSILON) {
            tolerance = CMP_EPSILON;
        }
        return abs(a - b) < tolerance;
    }
    static _ALWAYS_INLINE_ bool is_equal_approx(Unit a, Unit b, Unit tolerance) {
        // Check for exact equality first, required to handle "infinity" values.
        if (a == b) {
            return true;
        }
        // Then check for approximate equality.
        return abs(a - b) < tolerance;
    }
}

#define Deg180InRad ((Framework::Helper::Math::Unit)M_PI)
#define Deg90InRad  ((Framework::Helper::Math::Unit)RAD(90.0))

#endif //GAMEENGINE_MATHEMATICS_H