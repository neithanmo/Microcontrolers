#ifndef PID_H_
#define PID_H_

/** Instance of a PID controller.
 *
 * @note This structure is only public to be able to do static allocation of it.
 * Do not access its fields directly.
 */
typedef struct {
    float kp;
    float ki;
    float kd;
    float integrator;
    float previous_value;
    float integrator_limit;
    float frequency;
} pid_filter_t;

/** Initializes a PID controller. */
void pid_init(pid_filter_t *pid);

/** Sets the gains of the given PID. */
void pid_set_gains(pid_filter_t *pid, float kp, float ki, float kd);

/** Returns the proportional gains of the controller. */
void pid_get_gains(const pid_filter_t *pid, float *kp, float *ki, float *kd);

/** Returns the limit of the PID integrator. */
float pid_get_integral_limit(const pid_filter_t *pid);

/** Returns the value of the PID integrator. */
float pid_get_integral(const pid_filter_t *pid);

/** Process one step if the PID algorithm. */
float pid_process(pid_filter_t *pid, float value);

/** Sets a maximum value for the PID integrator. */
void pid_set_integral_limit(pid_filter_t *pid, float max);

/** Resets the PID integrator to zero. */
void pid_reset_integral(pid_filter_t *pid);

/** Sets the PID frequency for gain compensation. */
void pid_set_frequency(pid_filter_t *pid, float frequency);

/** Gets the PID frequency for gain compensation. */
float pid_get_frequency(const pid_filter_t *pid);

#endif
