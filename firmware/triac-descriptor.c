#include "triac-control.h"

#include <fx2types.h>

// Make sure that this descriptor is put in the DSCR_AREA memory area.
// This are has to be kept even-aligned.

// This pragma is per-file so all this data has to be kept in a separate file.
#pragma constseg DSCR_AREA

__code struct light_set_descriptor light_set_descriptor = {
    8
};

__code BYTE padding = 1;

__code struct light_descriptor light_descriptors[8] = {
    { FALSE },
    { FALSE },
    { FALSE },
    { FALSE },
    { FALSE },
    { FALSE },
    { FALSE },
    { FALSE } 
};
