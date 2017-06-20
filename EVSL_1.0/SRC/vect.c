#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "struct.h"
#include "internal_proto.h"
/**
 * @file vect.c
 * @brief Vector operations
 */

void rand_double(int n, double *v) {
  int i;
  double t = ((double) RAND_MAX)/2.0; 
  for (i=0; i<n; i++) {
    v[i] = (rand() -t)/t;
  }
}

/*
 * Generates a normally distributed random vector of length n
 *
 * Uses the Box-Muller transformation
 * @param[out] v Vector to be populated
 * @param[in] n Number of elements to generate (should be the length of v)
 * */
void randn_double(int n, double *v) {
    const double two_pi = 2.0 * 3.1415926535;
    int i;
    for(i = 0; i < n; i++) {
            static double Z0;
            static double Z1;
            static int regen = 0;//A boolean
            regen = !regen;
            if(!regen)
            {
                v[i] = Z1;
            }

            double U1 = rand() * (1.0 / RAND_MAX);
            double U2 = rand() * (1.0 / RAND_MAX);

            Z0 = sqrt(-2.0 * log(U1)) * cos(two_pi  * U2);
            Z1 = sqrt(-2.0 * log(U1)) * sin(two_pi  * U2);
            v[i] = Z0;
            }
}

void vecset(int n, double t, double *v) {
  int i;
  for (i=0; i<n; i++) 
    v[i] = t; 
}

void linspace(double a, double b, int num, double *arr){
  double h;
  h = (num==1? 0: (b-a)/(num-1));
  int i;
 //-------------------- careful at the boundaries!
  arr[0] = a;
  arr[num-1] = b;
  for (i=1; i<num-1; i++)
    arr[i] = a+i*h;

}

int compare1(const void *a, const void *b) {
  double *aa = (double*) a;
  double *bb = (double*) b;
  if (*aa < *bb) {
    return -1;
  } else if (*aa == *bb) {
    return 0;
  } else {
    return 1;
  }
}
typedef struct _doubleint {
  int i;
  double d;
} doubleint;
int compare2(const void *a, const void *b) {
  const doubleint *aa = (doubleint*) a;
  const doubleint *bb = (doubleint*) b;
  if (aa->d < bb->d) {
    return -1;
  } else if (aa->d == bb->d) {
    return 0;
  } else {
    return 1;
  }
}
void sort_double(int n, double *v, int *ind) {
  /* if sorting indices are not wanted */
  if (ind == NULL) {
    qsort(v, n, sizeof(double), compare1);
    return;
  }
  doubleint *vv;
  Malloc(vv, n, doubleint);
  int i;
  for (i=0; i<n; i++) {
    vv[i].d = v[i];
    vv[i].i = i;
  }
  qsort(vv, n, sizeof(doubleint), compare2);
  for (i=0; i<n; i++) {
    v[i] = vv[i].d;
    ind[i] = vv[i].i;
  }
  free(vv);
}

/* @brief y = x(p) */
void vec_perm(int n, int *p, double *x, double *y) {
  if (!p) {
    memcpy(y, x, n*sizeof(double));
  } else {
    int i;
    for (i=0; i<n; i++) {
      y[i] = x[p[i]];
    }
  }
}


/* @brief y(p) = x */
void vec_iperm(int n, int *p, double *x, double *y) {
  if (!p) {
    memcpy(y, x, n*sizeof(double));
  } else {
    int i;
    for (i=0; i<n; i++) {
      y[p[i]] = x[i];
    }
  }
}
