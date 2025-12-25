#pragma once

#include <arch/types.h>

void handleMemManageFault(word_t CFSR);

/* This isn't technically part of the MPU, but this is as good a place as any */
void handleBusFault(word_t CFSR);
