#ifndef _NDS32_BITS_ATOMIC_H
#define _NDS32_BITS_ATOMIC_H


#include <stdint.h>

typedef int8_t atomic8_t;
typedef uint8_t uatomic8_t;
typedef int_fast8_t atomic_fast8_t;
typedef uint_fast8_t uatomic_fast8_t;

typedef int16_t atomic16_t;
typedef uint16_t uatomic16_t;
typedef int_fast16_t atomic_fast16_t;
typedef uint_fast16_t uatomic_fast16_t;

typedef int32_t atomic32_t;
typedef uint32_t uatomic32_t;
typedef int_fast32_t atomic_fast32_t;
typedef uint_fast32_t uatomic_fast32_t;

typedef int64_t atomic64_t;
typedef uint64_t uatomic64_t;
typedef int_fast64_t atomic_fast64_t;
typedef uint_fast64_t uatomic_fast64_t;

typedef intptr_t atomicptr_t;
typedef uintptr_t uatomicptr_t;
typedef intmax_t atomic_max_t;
typedef uintmax_t uatomic_max_t;


#ifndef atomic_full_barrier
# define atomic_full_barrier() __asm ("" ::: "memory")
#endif

#ifndef atomic_read_barrier
# define atomic_read_barrier() atomic_full_barrier ()
#endif

#ifndef atomic_write_barrier
# define atomic_write_barrier() atomic_full_barrier ()
#endif

/* We have by default no support for atomic operations.  So define
   them non-atomic.  If this is a problem somebody will have to come
   up with real definitions.  */

/* The only basic operation needed is compare and exchange.  */
#define atomic_compare_and_exchange_val_acq(mem, newval, oldval) \
  ({ __typeof (mem) __gmemp = (mem);                                  \
     __typeof (*mem) __gret = *__gmemp;                               \
     __typeof (*mem) __gnewval = (newval);                            \
                                                                      \
     if (__gret == (oldval))                                          \
       *__gmemp = __gnewval;                                          \
     __gret; })

#define atomic_compare_and_exchange_bool_acq(mem, newval, oldval) \
  ({ __typeof (mem) __gmemp = (mem);                                  \
     __typeof (*mem) __gnewval = (newval);                            \
                                                                      \
     *__gmemp == (oldval) ? (*__gmemp = __gnewval, 0) : 1; })


#endif
