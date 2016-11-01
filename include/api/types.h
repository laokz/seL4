/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __API_TYPES_H
#define __API_TYPES_H

#include <config.h>
#include <stdint.h>
#include <util.h>
#include <sel4/shared_types_gen.h>
#include <arch/api/types.h>
#include <arch/types.h>
#include <sel4/macros.h>
#include <sel4/constants.h>
#include <sel4/shared_types.h>
#include <machine/io.h>

/* seL4_CapRights_t defined in mode/api/shared_types.bf */

typedef word_t prio_t;
typedef uint64_t ticks_t;
typedef uint64_t time_t;

enum domainConstants {
    minDom = 0,
    maxDom = CONFIG_NUM_DOMAINS - 1
};

struct cap_transfer {
    cptr_t ctReceiveRoot;
    cptr_t ctReceiveIndex;
    word_t ctReceiveDepth;
};
typedef struct cap_transfer cap_transfer_t;

enum ctLimits {
    capTransferDataSize = 3
};

static inline seL4_CapRights_t CONST rightsFromWord(word_t w)
{
    seL4_CapRights_t seL4_CapRights;

    seL4_CapRights.words[0] = w;
    return seL4_CapRights;
}

static inline word_t CONST wordFromRights(seL4_CapRights_t seL4_CapRights)
{
    return seL4_CapRights.words[0] & MASK(seL4_CapRightsBits);
}

static inline cap_transfer_t PURE capTransferFromWords(word_t *wptr)
{
    cap_transfer_t transfer;

    transfer.ctReceiveRoot  = (cptr_t)wptr[0];
    transfer.ctReceiveIndex = (cptr_t)wptr[1];
    transfer.ctReceiveDepth = wptr[2];
    return transfer;
}

static inline seL4_MessageInfo_t CONST messageInfoFromWord_raw(word_t w)
{
    seL4_MessageInfo_t mi;

    mi.words[0] = w;
    return mi;
}

static inline seL4_MessageInfo_t CONST messageInfoFromWord(word_t w)
{
    seL4_MessageInfo_t mi;
    word_t len;

    mi.words[0] = w;

    len = seL4_MessageInfo_get_length(mi);
    if (len > seL4_MsgMaxLength) {
        mi = seL4_MessageInfo_set_length(mi, seL4_MsgMaxLength);
    }

    return mi;
}

static inline word_t CONST wordFromMessageInfo(seL4_MessageInfo_t mi)
{
    return mi.words[0];
}

#ifdef CONFIG_PRINTING
#ifdef CONFIG_COLOUR_PRINTING
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN ANSI_RESET "\033[32m"
#define ANSI_DARK  ANSI_RESET "\033[30;1m"
#else
#define ANSI_RESET ""
#define ANSI_GREEN ANSI_RESET ""
#define ANSI_DARK  ANSI_RESET ""
#endif

/*
 * thread name is only available if the kernel is built in debug mode.
 */
#ifdef CONFIG_DEBUG_BUILD
#define THREAD_NAME NODE_STATE(ksCurThread)->tcbName
#else
#define THREAD_NAME ""
#endif

/*
 * Print to serial a message helping userspace programmers to determine why the
 * kernel is not performing their requested operation.
 */
#define userError(...) \
    do {                                                                     \
        printf(ANSI_DARK "<<" ANSI_GREEN "seL4(CPU %lu)" ANSI_DARK           \
                " [%s/%d T%p \"%s\" @%lx]: ",                                \
                SMP_TERNARY(getCurrentCPUIndex(), 0lu),                      \
                __func__, __LINE__, NODE_STATE(ksCurThread),                 \
                THREAD_NAME,                                                 \
                (word_t)getRestartPC(NODE_STATE(ksCurThread)));              \
        printf(__VA_ARGS__);                                                 \
        printf(">>" ANSI_RESET "\n");                                        \
    } while (0)
#else /* !CONFIG_PRINTING */
#define userError(...)
#endif

#endif
