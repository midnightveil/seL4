-- this file contains types shared between libsel4 and the kernel

tagged_union seL4_Fault seL4_FaultType {
    -- generic faults
    tag NullFault 0
    tag CapFault 1
    tag UnknownSyscall 2
    tag UserException 3
#ifdef CONFIG_KERNEL_MCS
    tag Timeout 5
#endif
    -- arch specific faults
    tag BusFault 6
    tag MemManageFault 7
}
