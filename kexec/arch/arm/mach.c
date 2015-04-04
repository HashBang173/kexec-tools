#include <string.h>
#include "mach.h"

extern const struct arch_mach arm_mach_flounder;
extern const struct arch_mach arm_mach_hammerhead;
extern const struct arch_mach arm_mach_shamu;
extern const struct arch_mach arm_mach_tegra;
extern const struct arch_mach arm_mach_tegra4;
static const struct arm_mach *const arm_machs[] = {
    &arm_mach_flounder,
    &arm_mach_hammerhead,
    &arm_mach_shamu,
    &arm_mach_tegra,
    &arm_mach_tegra4,
    NULL
};
// update zImage_arm_usage when modifying this.

struct arm_mach *arm_mach_choose(const char *boardname)
{
    int i, x;
    for(i = 0; arm_machs[i]; ++i)
    {
        struct arm_mach *m = arm_machs[i];
        for(x = 0; m->boardnames[x]; ++x)
        {
            if(strcmp(m->boardnames[x], boardname) == 0)
                return m;
        }
    }

    return NULL;
}
