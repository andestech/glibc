
#ifndef __NDS32_ABI_2FP_PLUS__
# define ROUNDING_TESTS_float(MODE)	((MODE) == FE_TONEAREST)
# define ROUNDING_TESTS_double(MODE)	((MODE) == FE_TONEAREST)
# define ROUNDING_TESTS_long_double(MODE)	((MODE) == FE_TONEAREST)
# define EXCEPTION_TESTS_float	0
# define EXCEPTION_TESTS_double	0
# define EXCEPTION_TESTS_long_double	0
#endif

#include_next <math-tests.h>
