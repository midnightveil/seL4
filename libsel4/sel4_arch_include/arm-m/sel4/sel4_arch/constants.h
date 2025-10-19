#include <sel4/config.h>
#include <sel4/macros.h>

#define seL4_WordBits           32
/* log 2 bits in a word */
#define seL4_WordSizeBits       2

#define seL4_SlotBits           4
#ifdef CONFIG_KERNEL_MCS
#define seL4_NotificationBits   5
#define seL4_ReplyBits          4
#else
#define seL4_NotificationBits   4
#endif
#define seL4_EndpointBits       4
#define seL4_IPCBufferSizeBits  9
#define seL4_TCBBits            9

/* TODO: ??? */
#define seL4_VSpaceBits 0
#define seL4_PageTableBits 0
