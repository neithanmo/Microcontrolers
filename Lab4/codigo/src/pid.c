#include <stdlib.h>
#include <math.h>
#include "pid.h"


void pid_init(pid_filter_t *pid)
{
    pid_set_gains(pid, 1., 0., 0.);
    pid->integrator = 0.;
    pid->previous_value = 0.;
    pid->integrator_limit = INFINITY;
    pid->frequency = 1.;
}

void pid_set_gains(pid_filter_t *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

void pid_get_gains(const pid_filter_t *pid, float *kp, float *ki, float *kd)
{
    *kp = pid->kp;
    *ki = pid->ki;
    *kd = pid->kd;
}

float pid_get_integral_limit(const pid_filter_t *pid)
{
    return pid->integrator_limit;
}

float pid_get_integral(const pid_filter_t *pid)
{
    return pid->integrator;
}

float pid_process(pid_filter_t *pid, float value)
{
    float output;
    pid->integrator += value;

    if (pid->integrator > pid->integrator_limit) {
        pid->integrator = pid->integrator_limit;
    } else if (pid->integrator < -pid->integrator_limit) {
        pid->integrator = -pid->integrator_limit;
    }

    output  = pid->kp * value;
    output += pid->ki * pid->integrator / pid->frequency;
    output += pid->kd * (value - pid->previous_value) * pid->frequency;

    pid->previous_value = value;
    return output;
}

void pid_set_integral_limit(pid_filter_t *pid, float max)
{
    pid->integrator_limit = max;
}

void pid_reset_integral(pid_filter_t *pid)
{
    pid->integrator = 0.;
}

void pid_set_frequency(pid_filter_t *pid, float frequency)
{
    pid->frequency = frequency;
}

float pid_get_frequency(const pid_filter_t *pid)
{
    return pid->frequency;
}
