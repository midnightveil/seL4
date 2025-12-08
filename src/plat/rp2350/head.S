.syntax unified
.thumb

/*
 * Ref: RP2350 Datasheet
 * build-version: d126e9e-clean
 * https://pip.raspberrypi.com/documents/RP-008373-DS-2-rp2350-datasheet.pdf
 *
 * Section §5.9 of the datasheet specifies the details of this format.
 * We use verbatim the minimum viable ARM IMAGE_DEF from §5.9.5.1
 * It must appear within the first 4kB of a flash image.
 *
 * > Since the above block does not specify an explicit entry point, the bootrom
 * > will assume the binary starts with a Cortex-M vector table, and enter via the
 * > reset handler and initial stack pointer specified in that table.
 *
 */
.section .picobin_blocks, "a"
.global picobin_image_def
picobin_image_def:
    .4byte 0xffffded3 /* PICOBIN_BLOCK_MARKER_START */

    .byte  0x42       /* item_type == PICOBIN_BLOCK_ITEM_1BS_IMAGE_TYPE */
    .byte  0x01       /* size = 1 word */
    .2byte 0x1021     /* ARM, RP2350, EXE */

    .byte  0xff       /* size_type == 1, item_type = PICOBIN_BLOCK_ITEM_2BS_LAST */
    .2byte 0x0001     /* size = 1 */
    .byte  0x00       /* pad */

    .4byte 0x00000000 /* next in loop is self */

    .4byte 0xab123579 /* PICOBIN_BLOCK_MARKER_END */
.size picobin_image_def, .-picobin_image_def
