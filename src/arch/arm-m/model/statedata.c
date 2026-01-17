#include <arch/model/statedata.h>
#include <mode/kernel/stack.h>

char ksArmMIdleThreadStack[BIT(EXCEPTION_MAX_STACK_SIZE_BITS)] ALIGN(KERNEL_STACK_ALIGNMENT);
