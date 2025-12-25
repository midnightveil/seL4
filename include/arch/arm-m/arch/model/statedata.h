#pragma once

/* Since Arm Cortex-M uses hardware stacking for exceptions, we must stray
   from the other architectures and include a stack so that the hardware can
   unstack the registers (even though we really don't care about them).

   @TODO: Confirm the size of this.
 */
extern char ksArmMIdleThreadStack[50];
