//
// Created by robgrzel on 11.06.17.
//

#ifndef UTILS_REAL_H
#define UTILS_REAL_H

/*! \brief Double precision accuracy */
#define GMX_DOUBLE_EPS   2.2204460492503131e-16

/*! \brief Maximum double precision value - reduced 1 unit in last digit for MSVC */
#define GMX_DOUBLE_MAX   1.7976931348623157e+308

/*! \brief Minimum double precision value */
#define GMX_DOUBLE_MIN   2.2250738585072014e-308

/*! \brief Single precision accuracy */
#define GMX_FLOAT_EPS    1.19209290e-07F

/*! \brief Maximum single precision value - reduced 1 unit in last digit for MSVC */
#define GMX_FLOAT_MAX    3.40282346E+38F

/*! \brief Minimum single precision value */
#define GMX_FLOAT_MIN    1.175494351E-38F

#ifdef __PGI
/* The portland group x86 C/C++ compilers do not treat negative zero initializers
 * correctly, but "optimizes" them to positive zero, so we implement it explicitly.
 * These constructs are optimized to simple loads at compile time. If you want to
 * use them on other compilers those have to support gcc preprocessor extensions.
 * Note: These initializers might be sensitive to the endianness (which can
 * be different for byte and word order), so check that it works for your platform
 * and add a separate section if necessary before adding to the ifdef above.
 */
#    define GMX_DOUBLE_NEGZERO  ({ const union { int  di[2]; double d; } _gmx_dzero = {0, -2147483648}; _gmx_dzero.d; })
#    define GMX_FLOAT_NEGZERO   ({ const union { int  fi; float f; } _gmx_fzero = {-2147483648}; _gmx_fzero.f; })
#else
/*! \brief Negative zero in double */
#    define GMX_DOUBLE_NEGZERO  (-0.0)

/*! \brief Negative zero in float */
#    define GMX_FLOAT_NEGZERO   (-0.0f)
#endif

/*! \typedef real
 * \brief Precision-dependent \Gromacs floating-point type.
 */
/*! \def HAVE_REAL
 * \brief Used to check whether `real` is already defined.
 */
/*! \def GMX_MPI_REAL
 * \brief MPI data type for `real`.
 */
/*! \def GMX_REAL_EPS
 * \brief Accuracy for `real`.
 */
/*! \def GMX_REAL_MIN
 * \brief Smallest non-zero value for `real`.
 */
/*! \def GMX_REAL_MAX
 * \brief Largest finite value for `real`.
 */
/*! \def GMX_REAL_NEGZERO
 * \brief Negative zero for `real`.
 */
/*! \def gmx_real_fullprecision_pfmt
 * \brief Format string for full `real` precision.
 */
#ifdef GMX_DOUBLE

#ifndef HAVE_REAL
typedef double      real;
#define HAVE_REAL
#endif

#define GMX_MPI_REAL      MPI_DOUBLE
#define GMX_REAL_EPS      GMX_DOUBLE_EPS
#define GMX_REAL_MIN      GMX_DOUBLE_MIN
#define GMX_REAL_MAX      GMX_DOUBLE_MAX
#define GMX_REAL_NEGZERO  GMX_DOUBLE_NEGZERO
#define gmx_real_fullprecision_pfmt "%21.14e"

#else /* GMX_DOUBLE */

#ifndef HAVE_REAL
typedef float           real;
#define HAVE_REAL
#endif

#define GMX_MPI_REAL      MPI_FLOAT
#define GMX_REAL_EPS      GMX_FLOAT_EPS
#define GMX_REAL_MIN      GMX_FLOAT_MIN
#define GMX_REAL_MAX      GMX_FLOAT_MAX
#define GMX_REAL_NEGZERO  GMX_FLOAT_NEGZERO
#define gmx_real_fullprecision_pfmt "%14.7e"

#endif /* GMX_DOUBLE */

#endif //UTILS_REAL_H
