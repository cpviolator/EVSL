#ifndef DEF_H
#define DEF_H

#include <stdlib.h>
#include <assert.h>
#include <math.h>



#ifdef __cplusplus
#include <algorithm>
extern "C" {
#endif

/*! \file def.h
    \brief defs in EVSL
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PI M_PI 
//3.14159265358979323846
#define orthTol 1e-14

#define DBL_EPS_MULT 10

#define CHKERR(ierr) assert(!(ierr))
//#define CHKREQ(ierr) { if (ierr) { return (ierr); } }

/**
 * @brief A malloc wrapper which provides basic error checking
 * @param base The pointer to be allocated
 * @param nmem Number of elements to be allocated
 * @param type Type of elements to be allocated
 */
#define Malloc(base, nmem, type) { \
  size_t nbytes = (nmem) * sizeof(type); \
  (base) = (type*) malloc(nbytes); \
  if ((base) == NULL) { \
    fprintf(stdout, "EVSL Error: out of memory [%zu bytes asked]\n", nbytes); \
    fprintf(stdout, "Malloc at FILE %s, LINE %d, nmem %zu\n", __FILE__, __LINE__, (size_t) nmem); \
    exit(-1); \
  } \
}

/**
 * @brief A calloc wrapper which provides basic error checking
 * @param[in] base The pointer to be allocated
 * @param[in] nmem Number of elements to be allocated
 * @param[in] type Type of elements to be allocated
 */
#define Calloc(base, nmem, type) { \
  size_t nbytes = (nmem) * sizeof(type); \
  (base) = (type*) calloc((nmem), sizeof(type)); \
  if ((base) == NULL) { \
    fprintf(stdout, "EVSL Error: out of memory [%zu bytes asked]\n", nbytes); \
    fprintf(stdout, "Calloc at FILE %s, LINE %d, nmem %zu\n", __FILE__, __LINE__, (size_t) nmem); \
    exit(-1); \
  } \
}

/**
 * @brief A realloc wrapper which provides basic error checking
 * @param base The pointer to be allocated
 * @param nmem Number of elements to be allocated
 * @param type Type of elements to be allocated
 */
#define Realloc(base, nmem, type) {\
  size_t nbytes = (nmem) * sizeof(type); \
  (base) = (type*) realloc((base), nbytes); \
  if ((base) == NULL && nbytes > 0) { \
    fprintf(stdout, "EVSL Error: out of memory [%zu bytes asked]\n", nbytes); \
    fprintf(stdout, "Realloc at FILE %s, LINE %d, nmem %zu\n", __FILE__, __LINE__, (size_t) nmem); \
    exit(-1); \
  } \
}

#ifndef __cplusplus
/*!
  \def max(x,y)
  Computes the maximum of \a x and \a y.
*/
#define evsl_max(a, b) ((a) > (b) ? (a) : (b))

/*!
  \def min(x,y)
  Computes the minimum of \a x and \a y.
*/
#define evsl_min(a, b) ((a) < (b) ? (a) : (b))

#else

/*!
  \def max(x,y)
  Computes the maximum of \a x and \a y.
*/
#define evsl_max(a, b) std::max(a, b)

/*!
  \def min(x,y)
  Computes the minimum of \a x and \a y.
*/
#define evsl_min(a, b) std::min(a, b)

#endif
/*! Fortran interface naming convention
 */
#define EVSLFORT(name) name ## _f90_

/*! max number of Gram–Schmidt process in orthogonalization
 */
#define NGS_MAX 2

/*! integer type of EVSL */
#define EVSL_Int      int
#define EVSL_Unsigned unsigned

#ifdef __cplusplus
}
#endif

#endif
