/*
Copyright (C) 2014-2015 Thiemar Pty Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <cmath>

#if defined(CONFIG_STM32_STM32F302)
    /* STM32F30X-specific CMSIS defs */
    #include "cmsis_stm32f302.h"
#elif defined(CONFIG_STM32_STM32F446)
    /* STM32F44X-specific CMSIS defs */
    #include "cmsis_stm32f446.h"
#endif /* defined(CONFIG_STM32_STM32F302) */

#include "core_cm4.h"


#ifndef M_PI
#define M_PI 3.141592653589793f
#endif


struct motor_state_t {
    float angular_velocity_rad_per_s;
    float angle_rad; /* 0 .. 2 * pi */
    float i_dq_a[2];
    float v_dq_v[2];
};


struct motor_params_t {
    float rs_r; /* winding resistance in ohms */
    float ls_h; /* winding inductance in ohms */
    float phi_v_s_per_rad; /* speed constant in volt-seconds per radian */

    /* Operating limits */
    float max_current_a; /* RMS current limit in amps */
    float max_voltage_v; /* RMS voltage limit in volts */
    float accel_voltage_v; /* RMS initial open-loop voltage */

    uint32_t num_poles; /* number of poles */
};


struct control_params_t {
    float bandwidth_hz;
    float gain;
    float braking_frac;
};


inline void __attribute__((always_inline))
sin_cos(
    float& sinx,
    float& cosx,
    float x /* x must be in the range [-pi, pi] */
) {
    const float Q = 3.1f;
    const float P = 3.6f;

    float y;

    x *= float(1.0 / M_PI);

    y = x - x * std::abs(x);
    sinx = y * (Q + P * std::abs(y));

    /* Calculate the cosine */
    x += 0.5f;
    if (x > 1.0f) {
        x -= 2.0f;
    }

    y = x - x * std::abs(x);
    cosx = y * (Q + P * std::abs(y));
}


inline float __attribute__((always_inline)) __VSQRTF(float x) {
    float result;
    asm ("vsqrt.f32 %0, %1" : "=w" (result) : "w" (x) );
    return result;
}
