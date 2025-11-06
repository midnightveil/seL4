#pragma once

/**
 * Whilst ARMv7-M can support a 4-byte aligned stack, it "deprecates
 * "implementation or use of 4-byte SP alignment" (ARMv7-M ARM DDI 0403E.e, B1.5.7)
 *
 * Whether or not an exception entry supports CCR.STKALIGN='0' for 4-byte
 * alignment is ɪᴍᴘʟᴇᴍᴇɴᴛᴀᴛɪᴏɴ ᴅᴇꜰɪɴᴇᴅ. Also, the AAPCS call standard
 * "requires 8-byte stack pointer alignment on entry to a conforming function."
 *
 * Using a double-word aligned stack does not really pose additional constraints,
 * so let's do so.
 **/
#define KERNEL_STACK_ALIGNMENT 8
