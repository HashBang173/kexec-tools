#include <stdint.h>
#include <stdio.h>
#include <libfdt.h>

#include "../../kexec.h"
#include "../../fs2dt.h"
#include "mach.h"

// The Shield Portables official dtb doesn't contain enough info to be useful, so no sanity checks can be done.
// However, the roth dtb from the mainline kernel needs to be able to be loaded.
// The assumption is that no other Tegra 4 dtb does either, but this is not confirmed.

static int tegra4_choose_dtb(const char *dtb_img, off_t dtb_len, char **dtb_buf, off_t *dtb_length)
{
    char *dtb = (char*)dtb_img;
    char *dtb_end = dtb + dtb_len;
    FILE *f;
    
    while(dtb + sizeof(struct fdt_header) < dtb_end)
    {
        uint32_t dtb_soc_rev_id;
        struct fdt_header dtb_hdr;
        uint32_t dtb_size;

        /* the DTB could be unaligned, so extract the header,
         * and operate on it separately */
        memcpy(&dtb_hdr, dtb, sizeof(struct fdt_header));
        if (fdt_check_header((const void *)&dtb_hdr) != 0 ||
            (dtb + fdt_totalsize((const void *)&dtb_hdr) > dtb_end))
        {
            fprintf(stderr, "DTB: Invalid dtb header!\n");
            break;
        }
        dtb_size = fdt_totalsize(&dtb_hdr);

        *dtb_buf = xmalloc(dtb_size);
        memcpy(*dtb_buf, dtb, dtb_size);
        *dtb_length = dtb_size;
        printf("DTB: assuming match\n");
        return 1;
    }

    return 0;
}

static int tegra4_add_extra_regs(void *dtb_buf)
{
    FILE *f;
    uint32_t reg;
    int res;
    int off;

    off = fdt_path_offset(dtb_buf, "/memory");
    if (off < 0)
    {
        fprintf(stderr, "DTB: Could not find memory node.\n");
        return -1;
    }

    f = fopen("/proc/device-tree/memory/reg", "r");
    if(!f)
    {
        fprintf(stderr, "DTB: Failed to open /proc/device-tree/memory/reg!\n");
        return -1;
    }

    fdt_delprop(dtb_buf, off, "reg");

    while(fread(&reg, sizeof(reg), 1, f) == 1)
        fdt_appendprop(dtb_buf, off, "reg", &reg, sizeof(reg));

    fclose(f);
    return 0;
}

const struct arm_mach arm_mach_tegra4 = {
    .boardnames = { "roth", NULL },
    .choose_dtb = tegra4_choose_dtb,
    .add_extra_regs = tegra4_add_extra_regs
};
