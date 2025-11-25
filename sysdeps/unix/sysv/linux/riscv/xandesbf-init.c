#include <stdint.h>
#include <sys/hwprobe.h>
#include <libc-symbols.h>

#define RISCV_HWPROBE_EXT_XANDESBF (1ULL << 60)

/* Extern global variable checked by assembly */
int __has_xandesbf_support = 0;

/* Auto-initializing function */
int
__init_xandesbf_support(void)
{
  struct riscv_hwprobe pair = {
    .key = RISCV_HWPROBE_KEY_IMA_EXT_0,
    .value = 0
  };

  int ret = __riscv_hwprobe(&pair, 1, 0, NULL, 0);

  if (ret >= 0 && (pair.value & RISCV_HWPROBE_EXT_XANDESBF))
    __has_xandesbf_support = 1;
  else
    __has_xandesbf_support = 0;

  return __has_xandesbf_support;
}

/* Keep automatic init at startup via .init_array */
__attribute__((section(".init_array")))
__attribute__((used))
static void (*__init_xandesbf_ptr)(void) = (void (*)(void)) __init_xandesbf_support;

