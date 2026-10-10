#include "race_native_dispatch.h"
#include "race_bodyless_batch_007.h"
#include "race_bodyless_batch_006.h"
#include "race_bodyless_batch_004.h"
#include "race_bodyless_batch_003.h"
#include "race_bodyless_batch_005.h"
#include "wip.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t native_bios_call(RRJMemory *m, uint32_t table, uint32_t selector, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    RRJNativeDispatchContext *context = m->native_dispatch;
    if (context->bios_call)
        return context->bios_call(m, table, selector, frame, arguments);
    {
        uint32_t journal[8] = {table, selector, frame->return_address, frame->stack_pointer, arguments[0], arguments[1], arguments[2], arguments[3]};
        rrj_wip_handoff(m, table, "native_bios", __func__, __FILE__, __LINE__, "bind_bios_frame_service", journal, 8);
        rrj_wip_stop(__func__, __FILE__, __LINE__);
    }
}

uint32_t rrj_native_frame_call(RRJMemory *m, uint32_t target, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    RRJNativeDispatchContext *context;
    if (!m || !frame || !arguments || !m->native_dispatch || !m->native_dispatch->runtime_gp)
    {
        fprintf(stderr, "Missing native frame dispatch context for %08X\n", target);
        exit(21);
    }
    context = m->native_dispatch;
    switch (target)
    {
        case 0x80024610:
            return sub_80024610(frame, arguments);
        case 0x80033E5C:
            return sub_80033E5C(frame, arguments);
        case 0x80020F7C:
            return sub_80020F7C(frame, arguments);
        case 0x8002D2E0:
            return sub_8002D2E0(*context->runtime_gp, frame);
        case 0x80023DA4:
            return sub_80023DA4(arguments[0], frame);
        case 0x8004DE34:
            frame->return_value = sub_8004DE34(arguments[0]);
            return frame->return_value;
        case 0x8001C498:
            frame->return_value = sub_8001C498();
            return frame->return_value;
        case 0x800210BC:
            frame->return_value = sub_800210BC();
            return frame->return_value;
        case 0x80047430:
            return sub_80047430(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x800473F0:
            return sub_800473F0(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x800472B4:
            return sub_800472B4(*context->runtime_gp, frame, arguments);
        case 0x80047020:
            return sub_80047020(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x80046DD4:
            return sub_80046DD4(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x80046844:
            return sub_80046844(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x8004687C:
            return sub_8004687C(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x80046A00:
            return sub_80046A00(frame, arguments, rrj_native_frame_call);
        case 0x800467F4:
            return sub_800467F4(frame, arguments);
        case 0x80046948:
            return sub_80046948(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x80046A30:
            return sub_80046A30(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x80047364:
            return sub_80047364(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x80047248:
            return sub_80047248(*context->runtime_gp, frame, arguments);
        case 0x8001578C:
            return sub_8001578C(frame, arguments, rrj_native_frame_call);
        case 0x80015860:
            return sub_80015860(frame, arguments, rrj_native_frame_call);
        case 0x80045BEC:
            return sub_80045BEC(frame, arguments, rrj_native_frame_call);
        case 0x80047580:
            return sub_80047580(*context->runtime_gp, frame, arguments);
        case 0x80047514:
            return sub_80047514(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x80015724:
            return sub_80015724(*context->runtime_gp, frame);
        case 0x80044FE4:
            return sub_80044FE4(frame, arguments, native_bios_call);
        case 0x80044FF4:
            return sub_80044FF4(frame, arguments, native_bios_call);
        case 0x80040CB4:
            return sub_80040CB4(frame, arguments, rrj_native_frame_call);
        case 0x80042334:
            return sub_80042334(frame, arguments, rrj_native_frame_call);
        case 0x8001B500:
            return sub_8001B500(context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x8001DF64:
            return sub_8001DF64(context->runtime_gp, frame);
        case 0x8001E0DC:
            return sub_8001E0DC(frame, arguments);
        case 0x8001B6A8:
            return sub_8001B6A8(*context->runtime_gp, frame);
        case 0x800201C0:
            return sub_800201C0(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x8002026C:
            return sub_8002026C(*context->runtime_gp, frame, arguments, rrj_native_frame_call);
        case 0x800202B8:
            return sub_800202B8(*context->runtime_gp, frame, arguments);
        case 0x8001FC84:
            return sub_8001FC84(*context->runtime_gp, frame, arguments);
        case 0x8001EFDC:
            return sub_8001EFDC(frame, arguments);
        case 0x8004664C:
        case 0x80044894:
        case 0x80047934:
        case 0x80047964:
        case 0x80047724:
        case 0x80047A88:
            if (context->library_call)
                return context->library_call(m, target, frame, arguments);
            break;
    }
    rrj_wip_handoff(m, target, "native_frame", __func__, __FILE__, __LINE__, "bind_proven_frame_callee", arguments, 8);
    rrj_wip_stop(__func__, __FILE__, __LINE__);
}
