#ifndef QMI8658_H
#define QMI8658_H

#ifdef __cplusplus
extern "C" {
#endif

void qmi8658_init();
void qmi8658_read_accel(float *x, float *y, float *z);

#ifdef __cplusplus
}
#endif

#endif
