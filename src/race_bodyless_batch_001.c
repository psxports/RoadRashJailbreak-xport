#include "race_bodyless_batch_001.h"

uint32_t sub_80047430(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t result;
    frame->stack_pointer -= 24;
    arguments[0] = 0x80060000;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    frame->return_address = 0x80047444;
    arguments[0] = 0x8005b1b8;
    frame->return_value = call(m, 0x8004664c, frame, arguments);
    arguments[0] = 0x8005b1c4;
    frame->return_address = 0x80047458;
    arguments[1] = 0x80054d74;
    frame->return_value = call(m, 0x80044894, frame, arguments);
    w_u8(runtime_gp + 0x2f1, 0);
    w_u8(runtime_gp + 0x2f0, 0);
    frame->return_address = 0x80047468;
    frame->return_value = call(m, 0x800473f0, frame, arguments);
    frame->return_address = 0x80047470;
    frame->return_value = call(m, 0x800472b4, frame, arguments);
    arguments[0] = 1;
    arguments[1] = 0;
    arguments[2] = 0;
    frame->return_address = 0x80047484;
    arguments[3] = 0;
    frame->return_value = call(m, 0x80047020, frame, arguments);
    frame->return_value = rrj_read32(m, runtime_gp + 0x2e0) & 0x10;
    arguments[0] = 1;
    if (frame->return_value)
    {
        arguments[1] = 0;
        arguments[2] = 0;
        frame->return_address = 0x800474a8;
        arguments[3] = 0;
        frame->return_value = call(m, 0x80047020, frame, arguments);
    }
    arguments[0] = 10;
    arguments[1] = 0;
    arguments[2] = 0;
    frame->return_address = 0x800474bc;
    arguments[3] = 0;
    result = call(m, 0x80047020, frame, arguments);
    frame->return_value = UINT32_MAX;
    if (result)
        goto restore_frame;
    arguments[0] = 12;
    arguments[1] = 0;
    arguments[2] = 0;
    frame->return_address = 0x800474d8;
    arguments[3] = 0;
    frame->return_value = call(m, 0x80047020, frame, arguments);
    arguments[0] = 0;
    if (frame->return_value)
    {
        frame->return_value = UINT32_MAX;
        goto restore_frame;
    }
    frame->return_address = 0x800474e8;
    arguments[1] = arguments[0];
    frame->return_value = call(m, 0x80046dd4, frame, arguments);
    arguments[0] = frame->return_value;
    frame->secondary_result = 2;
    frame->return_value = arguments[0] == frame->secondary_result ? 0 : UINT32_MAX;

restore_frame:
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_800473F0(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    rrj_write32(m, runtime_gp + 0x2d8, 0);
    rrj_write32(m, runtime_gp + 0x2d4, 0);
    rrj_write32(m, runtime_gp + 0x2e4, 0);
    rrj_write32(m, runtime_gp + 0x2e0, 0);
    frame->return_address = 0x80047410;
    frame->return_value = call(m, 0x80047934, frame, arguments);
    arguments[0] = 2;
    frame->return_address = 0x80047420;
    arguments[1] = 0x80046948;
    frame->return_value = call(m, 0x80047964, frame, arguments);
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_800472B4(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    uint32_t address = rrj_read32(m, runtime_gp + 0x434), value;
    w_u8(address, 1);
    address = rrj_read32(m, runtime_gp + 0x440);
    value = r_u8(address) & 7;
    arguments[0] = 1;
    while (value)
    {
        address = rrj_read32(m, runtime_gp + 0x434);
        w_u8(address, arguments[0]);
        address = rrj_read32(m, runtime_gp + 0x440);
        w_u8(address, 7);
        address = rrj_read32(m, runtime_gp + 0x43c);
        w_u8(address, 7);
        address = rrj_read32(m, runtime_gp + 0x440);
        value = r_u8(address) & 7;
    }
    w_u8(runtime_gp + 0x44e, 0);
    value = r_u8(runtime_gp + 0x44e);
    arguments[0] = rrj_read32(m, runtime_gp + 0x434);
    w_u8(runtime_gp + 0x44d, (uint8_t)value);
    w_u8(0x8005b0d8, 2);
    w_u8(arguments[0], 0);
    address = rrj_read32(m, runtime_gp + 0x440);
    w_u8(address, 0);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x444);
    frame->return_value = 0x1325;
    rrj_write32(m, frame->secondary_result, frame->return_value);
    return frame->return_value;
}

uint32_t sub_80047020(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t index, table, address, value, result;
    frame->return_value = rrj_read32(m, runtime_gp + 0x2dc);
    frame->stack_pointer -= 40;
    rrj_write32(m, frame->stack_pointer + 20, frame->preserved_s1);
    frame->preserved_s1 = arguments[1];
    rrj_write32(m, frame->stack_pointer + 28, frame->preserved_s3);
    frame->preserved_s3 = arguments[2];
    rrj_write32(m, frame->stack_pointer + 24, frame->preserved_s2);
    frame->preserved_s2 = arguments[3];
    rrj_write32(m, frame->stack_pointer + 16, frame->preserved_s0);
    frame->preserved_s0 = arguments[0];
    frame->return_value = rrj_s32(frame->return_value) < 2;
    rrj_write32(m, frame->stack_pointer + 32, frame->return_address);
    if (!frame->return_value)
    {
        frame->return_value = 0x80054ad4;
        frame->secondary_result = frame->return_value + ((frame->preserved_s0 & 255) << 2);
        arguments[1] = rrj_read32(m, frame->secondary_result);
        frame->return_address = 0x80047078;
        arguments[0] = 0x8005b160;
        frame->return_value = call(m, 0x80044894, frame, arguments);
    }
    index = (frame->preserved_s0 & 255) << 2;
    frame->secondary_result = index;
    table = 0x80054b74 + index;
    frame->return_value = rrj_read32(m, table + 384);
    if (frame->return_value && !frame->preserved_s1)
    {
        value = rrj_read32(m, runtime_gp + 0x2dc);
        frame->return_value = 0x80050000;
        if (rrj_s32(value) > 0)
        {
            frame->return_value = 0x80054ad4 + frame->secondary_result;
            arguments[1] = rrj_read32(m, frame->return_value);
            frame->return_address = 0x800470cc;
            arguments[0] = 0x8005b168;
            frame->return_value = call(m, 0x80044894, frame, arguments);
        }
        frame->return_value = 0xfffffffe;
        goto restore_command_frame;
    }
    arguments[0] = 0;
    frame->return_address = 0x800470e0;
    arguments[1] = arguments[0];
    frame->return_value = call(m, 0x80046dd4, frame, arguments);
    index = frame->preserved_s0 & 255;
    if (index == 2)
    {
        arguments[0] = 0;
        arguments[1] = 0x8005af78;
        do
        {
            address = arguments[0] + arguments[1];
            value = r_u8(frame->preserved_s1 + arguments[0]);
            ++arguments[0];
            w_u8(address, (uint8_t)value);
        } while (rrj_s32(arguments[0]) < 4);
    }
    if (index == 14)
    {
        value = r_u8(frame->preserved_s1);
        w_u8(runtime_gp + 0x2f0, (uint8_t)value);
    }
    table = 0x80054b74 + (index << 2);
    frame->secondary_result = table;
    w_u8(runtime_gp + 0x44c, 0);
    if (rrj_read32(m, table + 128))
        w_u8(runtime_gp + 0x44d, 0);
    address = rrj_read32(m, runtime_gp + 0x434);
    w_u8(address, 0);
    value = rrj_read32(m, table + 384);
    arguments[0] = 0;
    if (rrj_s32(value) > 0)
    {
        arguments[1] = table;
        do
        {
            frame->secondary_result = rrj_read32(m, runtime_gp + 0x43c);
            value = r_u8(frame->preserved_s1 + arguments[0]);
            w_u8(frame->secondary_result, (uint8_t)value);
            value = rrj_read32(m, arguments[1] + 384);
            ++arguments[0];
        } while (rrj_s32(arguments[0]) < rrj_s32(value));
    }
    address = rrj_read32(m, runtime_gp + 0x438);
    w_u8(runtime_gp + 0x2f1, (uint8_t)frame->preserved_s0);
    w_u8(address, (uint8_t)frame->preserved_s0);
    frame->return_value = 0;
    if (frame->preserved_s2)
        goto restore_command_frame;
    frame->return_address = 0x800471c8;
    arguments[0] = 0x8005b178;
    frame->return_value = call(m, 0x80046844, frame, arguments);
    frame->return_value = r_u8(runtime_gp + 0x44c);
    while (!frame->return_value)
    {
        frame->return_address = 0x800471e0;
        result = call(m, 0x8004687c, frame, arguments);
        frame->return_value = UINT32_MAX;
        if (result)
            goto restore_command_frame;
        frame->return_address = 0x800471f0;
        frame->return_value = call(m, 0x80046a00, frame, arguments);
        frame->return_value = r_u8(runtime_gp + 0x44c);
    }
    arguments[1] = 0x8005b5b8;
    frame->return_address = 0x80047210;
    arguments[0] = frame->preserved_s3;
    frame->return_value = call(m, 0x800467f4, frame, arguments);
    arguments[0] = 0;
    frame->secondary_result = r_u8(runtime_gp + 0x44c);
    frame->return_value = arguments[0];
    if (frame->secondary_result == 5)
        arguments[0] = UINT32_MAX;
    frame->return_value = arguments[0];

restore_command_frame:
    frame->return_address = rrj_read32(m, frame->stack_pointer + 32);
    frame->preserved_s3 = rrj_read32(m, frame->stack_pointer + 28);
    frame->preserved_s2 = rrj_read32(m, frame->stack_pointer + 24);
    frame->preserved_s1 = rrj_read32(m, frame->stack_pointer + 20);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 40;
    return frame->return_value;
}

uint32_t sub_80046DD4(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t result;
    frame->stack_pointer -= 40;
    rrj_write32(m, frame->stack_pointer + 28, frame->preserved_s3);
    frame->preserved_s3 = arguments[0];
    rrj_write32(m, frame->stack_pointer + 32, frame->preserved_s4);
    frame->preserved_s4 = arguments[1];
    arguments[0] = 0x8005b14c;
    rrj_write32(m, frame->stack_pointer + 36, frame->return_address);
    rrj_write32(m, frame->stack_pointer + 24, frame->preserved_s2);
    rrj_write32(m, frame->stack_pointer + 20, frame->preserved_s1);
    frame->return_address = 0x80046e04;
    rrj_write32(m, frame->stack_pointer + 16, frame->preserved_s0);
    frame->return_value = call(m, 0x80046844, frame, arguments);
    frame->preserved_s1 = 2;
    frame->preserved_s2 = 5;
    do
    {
        frame->return_address = 0x80046e14;
        result = call(m, 0x8004687c, frame, arguments);
        frame->return_value = UINT32_MAX;
        if (result)
            goto restore_sync_frame;
        frame->return_address = 0x80046e24;
        frame->return_value = call(m, 0x80046a00, frame, arguments);
        frame->return_value = r_u8(runtime_gp + 0x44c);
        frame->preserved_s0 = frame->return_value & 255;
        if (frame->preserved_s0 == frame->preserved_s1 || frame->preserved_s0 == frame->preserved_s2)
        {
            w_u8(runtime_gp + 0x44c, (uint8_t)frame->preserved_s1);
            arguments[1] = 0x8005b5b8;
            frame->return_address = 0x80046e54;
            arguments[0] = frame->preserved_s4;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            frame->return_value = frame->preserved_s0;
            goto restore_sync_frame;
        }
        frame->return_value = 0;
    } while (!frame->preserved_s3);

restore_sync_frame:
    frame->return_address = rrj_read32(m, frame->stack_pointer + 36);
    frame->preserved_s4 = rrj_read32(m, frame->stack_pointer + 32);
    frame->preserved_s3 = rrj_read32(m, frame->stack_pointer + 28);
    frame->preserved_s2 = rrj_read32(m, frame->stack_pointer + 24);
    frame->preserved_s1 = rrj_read32(m, frame->stack_pointer + 20);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 40;
    return frame->return_value;
}

uint32_t sub_80046844(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->preserved_s0);
    frame->preserved_s0 = arguments[0];
    rrj_write32(m, frame->stack_pointer + 20, frame->return_address);
    frame->return_address = 0x8004685c;
    arguments[0] = UINT32_MAX;
    frame->return_value = call(m, 0x80047724, frame, arguments);
    frame->return_address = rrj_read32(m, frame->stack_pointer + 20);
    rrj_write32(m, runtime_gp + 0x954, frame->preserved_s0);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 16);
    frame->return_value += 960;
    rrj_write32(m, runtime_gp + 0x94c, frame->return_value);
    rrj_write32(m, runtime_gp + 0x950, 0);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_8004687C(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t ready, value;
    frame->stack_pointer -= 32;
    rrj_write32(m, frame->stack_pointer + 24, frame->return_address);
    frame->return_address = 0x8004688c;
    arguments[0] = UINT32_MAX;
    frame->return_value = call(m, 0x80047724, frame, arguments);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x94c);
    frame->secondary_result = rrj_s32(frame->secondary_result) < rrj_s32(frame->return_value);
    if (!frame->secondary_result)
    {
        frame->secondary_result = rrj_read32(m, runtime_gp + 0x950);
        frame->return_value = frame->secondary_result + 1;
        rrj_write32(m, runtime_gp + 0x950, frame->return_value);
        frame->return_value = 0x3c0000 < rrj_s32(frame->secondary_result);
        if (!frame->return_value)
            goto restore_timeout_frame;
    }
    frame->return_address = 0x800468d0;
    arguments[0] = 0x8005b0dc;
    frame->return_value = call(m, 0x8004664c, frame, arguments);
    arguments[1] = 0x80054b54;
    arguments[0] = 0x80060000;
    arguments[3] = r_u8(runtime_gp + 0x44c);
    ready = r_u8(runtime_gp + 0x44d);
    frame->secondary_result = r_u8(runtime_gp + 0x2f1);
    arguments[3] = arguments[1] + (arguments[3] << 2);
    value = rrj_read32(m, arguments[1] + (ready << 2));
    arguments[1] = rrj_read32(m, runtime_gp + 0x954);
    frame->secondary_result <<= 2;
    rrj_write32(m, frame->stack_pointer + 16, value);
    arguments[4] = value;
    frame->return_value = 0x80054ad4;
    frame->secondary_result += frame->return_value;
    arguments[2] = rrj_read32(m, frame->secondary_result);
    arguments[3] = rrj_read32(m, arguments[3]);
    frame->return_address = 0x80046924;
    arguments[0] = 0x8005b0ec;
    frame->return_value = call(m, 0x80044894, frame, arguments);
    frame->return_address = 0x8004692c;
    frame->return_value = call(m, 0x800472b4, frame, arguments);
    frame->return_value = UINT32_MAX;

restore_timeout_frame:
    frame->return_address = rrj_read32(m, frame->stack_pointer + 24);
    frame->stack_pointer += 32;
    return frame->return_value;
}

uint32_t sub_80046A00(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    frame->return_address = 0x80046a10;
    frame->return_value = call(m, 0x80047a88, frame, arguments);
    if (frame->return_value)
    {
        frame->return_address = 0x80046a20;
        frame->return_value = call(m, 0x80046948, frame, arguments);
    }
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_800467F4(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    frame->secondary_result = 7;
    if (arguments[0])
    {
        arguments[2] = UINT32_MAX;
        do
        {
            frame->return_value = r_u8(arguments[1]);
            ++arguments[1];
            --frame->secondary_result;
            w_u8(arguments[0], (uint8_t)frame->return_value);
            ++arguments[0];
        } while (frame->secondary_result != arguments[2]);
    }
    return frame->return_value;
}

uint32_t sub_80046948(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t ready, target;
    frame->return_value = rrj_read32(m, runtime_gp + 0x434);
    frame->stack_pointer -= 32;
    rrj_write32(m, frame->stack_pointer + 24, frame->return_address);
    rrj_write32(m, frame->stack_pointer + 20, frame->preserved_s1);
    rrj_write32(m, frame->stack_pointer + 16, frame->preserved_s0);
    frame->return_value = r_u8(frame->return_value);
    frame->preserved_s1 = frame->return_value & 3;
    for (;;)
    {
        frame->return_address = 0x80046970;
        frame->return_value = call(m, 0x80046a30, frame, arguments);
        frame->preserved_s0 = frame->return_value;
        frame->return_value = frame->preserved_s0 & 4;
        if (!frame->preserved_s0)
            break;
        ready = frame->return_value;
        frame->return_value = frame->preserved_s0 & 2;
        if (ready)
        {
            frame->return_value = rrj_read32(m, runtime_gp + 0x2d8);
            if (frame->return_value)
            {
                target = frame->return_value;
                arguments[0] = r_u8(runtime_gp + 0x44d);
                arguments[1] = 0x8005b5c0;
                frame->return_address = 0x800469a8;
                frame->return_value = call(m, target, frame, arguments);
            }
            frame->return_value = frame->preserved_s0 & 2;
        }
        if (frame->return_value)
        {
            frame->return_value = rrj_read32(m, runtime_gp + 0x2d4);
            if (frame->return_value)
            {
                target = frame->return_value;
                arguments[0] = r_u8(runtime_gp + 0x44c);
                arguments[1] = 0x8005b5b8;
                frame->return_address = 0x800469d8;
                frame->return_value = call(m, target, frame, arguments);
            }
        }
    }
    frame->return_value = rrj_read32(m, runtime_gp + 0x434);
    w_u8(frame->return_value, (uint8_t)frame->preserved_s1);
    frame->return_address = rrj_read32(m, frame->stack_pointer + 24);
    frame->preserved_s1 = rrj_read32(m, frame->stack_pointer + 20);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 32;
    return frame->return_value;
}

uint32_t sub_80046A30(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t interrupt, interrupt_port, observed, value;
    frame->stack_pointer -= 48;
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x434);
    frame->return_value = 1;
    rrj_write32(m, frame->stack_pointer + 40, frame->return_address);
    rrj_write32(m, frame->stack_pointer + 36, frame->preserved_s1);
    rrj_write32(m, frame->stack_pointer + 32, frame->preserved_s0);
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    arguments[0] = rrj_read32(m, runtime_gp + 0x440);
    interrupt_port = arguments[0];
    frame->return_value = r_u8(interrupt_port) & 7;
    w_u8(frame->stack_pointer + 16, (uint8_t)frame->return_value);
    frame->return_value = r_u8(frame->stack_pointer + 16);
    frame->preserved_s1 = 0;
    if (!frame->return_value)
        goto interrupt_zero;
    for (;;)
    {
        observed = r_u8(interrupt_port);
        frame->secondary_result = r_u8(frame->stack_pointer + 16);
        frame->return_value = observed & 7;
        frame->preserved_s0 = 0;
        if (frame->secondary_result == frame->return_value)
            break;
        frame->return_value = r_u8(interrupt_port) & 7;
        w_u8(frame->stack_pointer + 16, (uint8_t)frame->return_value);
    }
    arguments[0] = frame->stack_pointer + 24;
    do
    {
        frame->return_value = rrj_read32(m, runtime_gp + 0x434);
        frame->return_value = r_u8(frame->return_value) & 0x20;
        frame->secondary_result = arguments[0] + frame->preserved_s0;
        if (!frame->return_value)
            break;
        frame->return_value = rrj_read32(m, runtime_gp + 0x438);
        frame->return_value = r_u8(frame->return_value);
        ++frame->preserved_s0;
        w_u8(frame->secondary_result, (uint8_t)frame->return_value);
        frame->return_value = rrj_s32(frame->preserved_s0) < 8;
    } while (frame->return_value);
    frame->return_value = rrj_s32(frame->preserved_s0) < 8;
    frame->secondary_result = frame->preserved_s0;
    if (frame->return_value)
    {
        arguments[0] = frame->stack_pointer + 24;
        frame->return_value = arguments[0] + frame->secondary_result;
        do
        {
            w_u8(frame->return_value, 0);
            ++frame->secondary_result;
            value = rrj_s32(frame->secondary_result) < 8;
            frame->return_value = arguments[0] + frame->secondary_result;
        } while (value);
    }
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x434);
    frame->return_value = 1;
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->return_value = rrj_read32(m, runtime_gp + 0x440);
    frame->secondary_result = 7;
    w_u8(frame->return_value, (uint8_t)frame->secondary_result);
    frame->return_value = rrj_read32(m, runtime_gp + 0x43c);
    w_u8(frame->return_value, (uint8_t)frame->secondary_result);
    frame->secondary_result = r_u8(frame->stack_pointer + 16);
    interrupt = frame->secondary_result;
    frame->return_value = 0x80050000;
    if (interrupt == 3)
    {
        frame->secondary_result = r_u8(runtime_gp + 0x2f1);
        frame->return_value = 0x80054b74;
        frame->secondary_result = frame->return_value + (frame->secondary_result << 2);
        frame->return_value = rrj_read32(m, frame->secondary_result + 256);
    }
    if (frame->return_value)
    {
        frame->return_value = rrj_read32(m, runtime_gp + 0x2e0) & 0x10;
        if (!frame->return_value)
        {
            frame->return_value = r_u8(frame->stack_pointer + 24) & 0x10;
            if (frame->return_value)
            {
                frame->return_value = rrj_read32(m, runtime_gp + 0x2e8) + 1;
                rrj_write32(m, runtime_gp + 0x2e8, frame->return_value);
            }
        }
        frame->return_value = r_u8(frame->stack_pointer + 24);
        frame->secondary_result = r_u8(frame->stack_pointer + 25);
        frame->preserved_s1 = frame->return_value & 0x1d;
        rrj_write32(m, runtime_gp + 0x2e0, frame->return_value);
        rrj_write32(m, runtime_gp + 0x2e4, frame->secondary_result);
    }
    frame->secondary_result = r_u8(frame->stack_pointer + 16);
    frame->return_value = 5;
    if (frame->secondary_result == frame->return_value)
    {
        frame->return_value = rrj_s32(rrj_read32(m, runtime_gp + 0x2dc)) < 3;
        if (!frame->return_value)
        {
            arguments[0] = 0x80060000;
            frame->return_address = 0x80046bdc;
            arguments[0] = 0x8005b108;
            frame->return_value = call(m, 0x80044894, frame, arguments);
            value = rrj_s32(rrj_read32(m, runtime_gp + 0x2dc)) < 3;
            frame->return_value = 0x80050000;
            if (!value)
            {
                frame->return_value = 0x80054ad4;
                arguments[0] = 0x80060000;
                frame->secondary_result = r_u8(runtime_gp + 0x2f1);
                arguments[2] = rrj_read32(m, runtime_gp + 0x2e0);
                arguments[3] = rrj_read32(m, runtime_gp + 0x2e4);
                frame->secondary_result = frame->return_value + (frame->secondary_result << 2);
                arguments[1] = rrj_read32(m, frame->secondary_result);
                frame->return_address = 0x80046c18;
                arguments[0] = 0x8005b114;
                frame->return_value = call(m, 0x80044894, frame, arguments);
            }
        }
    }
    frame->return_value = r_u8(frame->stack_pointer + 16);
    interrupt = frame->return_value;
    frame->secondary_result = interrupt - 1;
    frame->return_value = 0x80010000;
    if (frame->secondary_result < 5)
    {
        frame->return_value = 0x80011004;
        frame->secondary_result = frame->return_value + (frame->secondary_result << 2);
        frame->return_value = rrj_read32(m, frame->secondary_result);
    }
    // Runtime jump table identity still requires evidence
    switch (interrupt)
    {
        case 3:
            frame->return_value = 5;
            if (frame->preserved_s1)
                goto interrupt_complete;
            frame->return_value = 0x80050000;
            frame->secondary_result = r_u8(runtime_gp + 0x2f1);
            frame->return_value = 0x80054b74;
            frame->secondary_result = frame->return_value + (frame->secondary_result << 2);
            frame->return_value = rrj_read32(m, frame->secondary_result);
            if (!frame->return_value)
            {
                frame->return_value = 2;
                goto interrupt_complete;
            }
            frame->return_value = 3;
            w_u8(runtime_gp + 0x44c, (uint8_t)frame->return_value);
            arguments[0] = 0x8005b5b8;
            frame->return_address = 0x80046c90;
            arguments[1] = frame->stack_pointer + 24;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            frame->return_value = 1;
            break;
        case 2:
            frame->return_value = frame->preserved_s1 ? 5 : 2;
        interrupt_complete:
            w_u8(runtime_gp + 0x44c, (uint8_t)frame->return_value);
            arguments[0] = 0x8005b5b8;
            frame->return_address = 0x80046cc0;
            arguments[1] = frame->stack_pointer + 24;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            frame->return_value = 2;
            break;
        case 1:
            if (frame->preserved_s1 && frame->preserved_s0 == 1)
                frame->preserved_s1 = 0;
            frame->return_value = frame->preserved_s1 ? 5 : 1;
            arguments[0] = 0x8005b5c0;
            arguments[1] = frame->stack_pointer + 24;
            w_u8(runtime_gp + 0x44d, (uint8_t)frame->return_value);
            frame->return_address = 0x80046d00;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            frame->return_value = rrj_read32(m, runtime_gp + 0x434);
            w_u8(frame->return_value, 0);
            frame->secondary_result = rrj_read32(m, runtime_gp + 0x440);
            frame->return_value = 4;
            w_u8(frame->secondary_result, 0);
            break;
        case 4:
            arguments[0] = 0x8005b5c8;
            frame->return_value = 4;
            frame->preserved_s0 = frame->stack_pointer + 24;
            w_u8(runtime_gp + 0x44e, (uint8_t)frame->return_value);
            frame->return_value = r_u8(runtime_gp + 0x44e);
            arguments[1] = frame->preserved_s0;
            w_u8(runtime_gp + 0x44d, (uint8_t)frame->return_value);
            frame->return_address = 0x80046d48;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            arguments[0] = 0x8005b5c0;
            frame->return_address = 0x80046d58;
            arguments[1] = frame->preserved_s0;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            frame->return_value = 4;
            break;
        case 5:
            arguments[0] = 0x8005b5b8;
            frame->return_value = 5;
            frame->preserved_s0 = frame->stack_pointer + 24;
            w_u8(runtime_gp + 0x44d, (uint8_t)frame->return_value);
            frame->return_value = r_u8(runtime_gp + 0x44d);
            arguments[1] = frame->preserved_s0;
            w_u8(runtime_gp + 0x44c, (uint8_t)frame->return_value);
            frame->return_address = 0x80046d88;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            arguments[0] = 0x8005b5c0;
            frame->return_address = 0x80046d98;
            arguments[1] = frame->preserved_s0;
            frame->return_value = call(m, 0x800467f4, frame, arguments);
            frame->return_value = 6;
            break;
        default:
            arguments[0] = 0x80060000;
            frame->return_address = 0x80046dac;
            arguments[0] = 0x8005b130;
            frame->return_value = call(m, 0x8004664c, frame, arguments);
            arguments[0] = 0x80060000;
            arguments[1] = r_u8(frame->stack_pointer + 16);
            frame->return_address = 0x80046dbc;
            arguments[0] = 0x8005b144;
            frame->return_value = call(m, 0x80044894, frame, arguments);
        interrupt_zero:
            frame->return_value = 0;
            break;
    }
    frame->return_address = rrj_read32(m, frame->stack_pointer + 40);
    frame->preserved_s1 = rrj_read32(m, frame->stack_pointer + 36);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 32);
    frame->stack_pointer += 48;
    return frame->return_value;
}

uint32_t sub_80047364(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x448);
    frame->stack_pointer -= 32;
    rrj_write32(m, frame->stack_pointer + 24, frame->return_address);
    frame->return_value = r_u16(frame->secondary_result + 0x1b8);
    if (!frame->return_value)
    {
        frame->return_value = r_u16(frame->secondary_result + 0x1ba);
        if (!frame->return_value)
        {
            frame->return_value = 0x3fff;
            w_u16(frame->secondary_result + 0x180, (uint16_t)frame->return_value);
            w_u16(frame->secondary_result + 0x182, (uint16_t)frame->return_value);
        }
    }
    frame->return_value = 0x3fff;
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x448);
    arguments[0] = frame->stack_pointer + 16;
    w_u16(frame->secondary_result + 0x1b0, (uint16_t)frame->return_value);
    w_u16(frame->secondary_result + 0x1b2, (uint16_t)frame->return_value);
    frame->return_value = 0xc001;
    w_u16(frame->secondary_result + 0x1aa, (uint16_t)frame->return_value);
    frame->return_value = 0x80;
    w_u8(frame->stack_pointer + 18, (uint8_t)frame->return_value);
    w_u8(frame->stack_pointer + 16, (uint8_t)frame->return_value);
    w_u8(frame->stack_pointer + 19, 0);
    frame->return_address = 0x800473d0;
    w_u8(frame->stack_pointer + 17, 0);
    frame->return_value = call(m, 0x80047248, frame, arguments);
    frame->secondary_result = frame->return_value;
    frame->return_value = frame->secondary_result ? UINT32_MAX : 0;
    frame->return_address = rrj_read32(m, frame->stack_pointer + 24);
    frame->stack_pointer += 32;
    return frame->return_value;
}

uint32_t sub_80047248(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x434);
    frame->return_value = 2;
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x43c);
    frame->return_value = r_u8(arguments[0]);
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x440);
    frame->return_value = r_u8(arguments[0] + 1);
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x434);
    frame->return_value = 3;
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x438);
    frame->return_value = r_u8(arguments[0] + 2);
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x43c);
    frame->return_value = r_u8(arguments[0] + 3);
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x440);
    frame->return_value = 0x20;
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->return_value = 0;
    return frame->return_value;
}

uint32_t sub_8001578C(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t state;
    frame->stack_pointer -= 24;
    arguments[0] &= 255;
    frame->return_value = 2;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    if (arguments[0] != frame->return_value)
        goto other_event;
    frame->return_value = 0x800d0000;
    arguments[1] = frame->return_value + 0x6710;
    frame->secondary_result = rrj_read32(m, arguments[1] + 24);
    state = frame->secondary_result;
    frame->return_value = state < 3;
    if (state == arguments[0])
    {
        arguments[0] = 0;
        frame->secondary_result = rrj_read32(m, arguments[1] + 16);
        frame->return_value = 5;
        rrj_write32(m, arguments[1] + 24, frame->return_value);
        frame->return_address = 0x80015810;
        rrj_write32(m, arguments[1] + 20, frame->secondary_result);
        frame->return_value = call(m, 0x800457e8, frame, arguments);
        goto event_return;
    }
    frame->return_value = 1;
    if (state < 3)
    {
        if (state != 1)
            goto event_return;
        rrj_write32(m, arguments[1] + 24, arguments[0]);
        arguments[0] = 6;
        goto issue_command;
    }
    frame->return_value = 4;
    if (state == 3)
    {
        rrj_write32(m, arguments[1] + 24, frame->return_value);
        arguments[0] = 6;
    issue_command:
        frame->return_address = 0x80015828;
        arguments[1] = 0;
        frame->return_value = call(m, 0x8004594c, frame, arguments);
        goto event_return;
    }
    arguments[0] = 0;
    if (state == 4)
    {
        frame->return_value = rrj_read32(m, arguments[1] + 16);
        rrj_write32(m, arguments[1] + 24, 0);
        frame->return_address = 0x80015840;
        rrj_write32(m, arguments[1] + 28, frame->return_value);
        frame->return_value = call(m, 0x800457e8, frame, arguments);
    }
    goto event_return;
other_event:
    frame->return_address = 0x80015850;
    frame->return_value = call(m, 0x80015a18, frame, arguments);
event_return:
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_80015860(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t state, current, target;
    frame->stack_pointer -= 40;
    arguments[0] &= 255;
    frame->return_value = 1;
    rrj_write32(m, frame->stack_pointer + 32, frame->return_address);
    rrj_write32(m, frame->stack_pointer + 28, frame->preserved_s1);
    rrj_write32(m, frame->stack_pointer + 24, frame->preserved_s0);
    if (arguments[0] != 1)
    {
        frame->return_value = 5;
        if (arguments[0] == frame->return_value)
        {
            frame->return_address = 0x80015890;
            arguments[0] = frame->return_value;
            frame->return_value = call(m, 0x80015a18, frame, arguments);
        }
        goto sector_return;
    }
    frame->return_value = 0x800d0000;
    frame->preserved_s1 = frame->return_value + 0x6710;
    frame->secondary_result = rrj_read32(m, frame->preserved_s1 + 24);
    state = frame->secondary_result;
    frame->return_value = 5;
    if (state < 5)
    {
        if (state)
            goto sector_return;
        frame->return_value = rrj_read32(m, frame->preserved_s1 + 20) + 1;
        rrj_write32(m, frame->preserved_s1 + 20, frame->return_value);
        goto sector_return;
    }
    frame->preserved_s0 = 0x800d0000;
    if (state != 5)
        goto sector_return;
    frame->preserved_s0 += 0x6730;
    arguments[0] = frame->preserved_s0;
    frame->return_address = 0x800158f0;
    arguments[1] = 3;
    frame->return_value = call(m, 0x80045bec, frame, arguments);
    frame->return_address = 0x800158f8;
    arguments[0] = 0;
    frame->return_value = call(m, 0x80045c50, frame, arguments);
    frame->return_address = 0x80015900;
    arguments[0] = frame->preserved_s0;
    frame->return_value = call(m, 0x80015c08, frame, arguments);
    arguments[0] = rrj_read32(m, frame->preserved_s1 + 16);
    current = arguments[0];
    frame->secondary_result = frame->return_value + 1;
    rrj_write32(m, frame->preserved_s1 + 20, frame->secondary_result);
    if (frame->return_value != current)
    {
        arguments[0] = rrj_read32(m, frame->preserved_s1 + 4);
        arguments[1] = rrj_read32(m, frame->preserved_s1 + 12);
        rrj_write32(m, frame->preserved_s1 + 24, 0);
        frame->return_address = 0x80015924;
        frame->return_value = call(m, 0x8001564c, frame, arguments);
        goto sector_return;
    }
    frame->secondary_result = rrj_read32(m, frame->preserved_s1 + 12);
    frame->return_value = rrj_s32(frame->secondary_result) < 0x801;
    frame->preserved_s0 = 0xfffffffc;
    if (!frame->return_value)
        frame->secondary_result = 0x800;
    frame->preserved_s0 &= frame->secondary_result;
    arguments[0] = rrj_read32(m, frame->preserved_s1 + 4);
    frame->return_address = 0x80015954;
    arguments[1] = (uint32_t)(rrj_s32(frame->preserved_s0) >> 2);
    frame->return_value = call(m, 0x80045bec, frame, arguments);
    frame->return_value = rrj_read32(m, frame->preserved_s1 + 4);
    frame->secondary_result = rrj_read32(m, frame->preserved_s1 + 12);
    frame->return_value += frame->preserved_s0;
    frame->secondary_result -= frame->preserved_s0;
    frame->preserved_s0 = rrj_s32(frame->preserved_s0) < 0x800;
    rrj_write32(m, frame->preserved_s1 + 4, frame->return_value);
    rrj_write32(m, frame->preserved_s1 + 12, frame->secondary_result);
    if (frame->preserved_s0)
    {
        frame->preserved_s0 = frame->stack_pointer + 16;
        if (frame->secondary_result)
        {
            arguments[0] = frame->preserved_s0;
            frame->return_address = 0x80015988;
            arguments[1] = 1;
            frame->return_value = call(m, 0x80045bec, frame, arguments);
            frame->return_address = 0x80015990;
            arguments[0] = 0;
            frame->return_value = call(m, 0x80045c50, frame, arguments);
            arguments[0] = rrj_read32(m, frame->preserved_s1 + 4);
            arguments[2] = rrj_read32(m, frame->preserved_s1 + 12);
            frame->return_address = 0x800159a0;
            arguments[1] = frame->preserved_s0;
            frame->return_value = call(m, 0x8001e08c, frame, arguments);
            frame->return_value = rrj_read32(m, frame->preserved_s1 + 4);
            frame->secondary_result = rrj_read32(m, frame->preserved_s1 + 12);
            rrj_write32(m, frame->preserved_s1 + 12, 0);
            frame->return_value += frame->secondary_result;
            rrj_write32(m, frame->preserved_s1 + 4, frame->return_value);
        }
    }
    frame->preserved_s1 = 0x800d0000;
    frame->preserved_s0 = frame->preserved_s1 + 0x6710;
    frame->return_value = rrj_read32(m, frame->preserved_s0 + 8) - 1;
    rrj_write32(m, frame->preserved_s0 + 8, frame->return_value);
    frame->return_value = rrj_read32(m, frame->preserved_s0 + 16);
    frame->secondary_result = rrj_read32(m, frame->preserved_s0 + 8);
    ++frame->return_value;
    rrj_write32(m, frame->preserved_s0 + 16, frame->return_value);
    if (!frame->secondary_result)
    {
        frame->return_address = 0x800159e8;
        arguments[0] = 0;
        frame->return_value = call(m, 0x80045c50, frame, arguments);
        frame->return_value = rrj_read32(m, frame->preserved_s0 + 20);
        frame->secondary_result = rrj_read32(m, frame->preserved_s1 + 0x6710);
        rrj_write32(m, frame->preserved_s0 + 24, 0);
        target = frame->secondary_result;
        rrj_write32(m, frame->preserved_s0 + 28, frame->return_value);
        if (target)
        {
            frame->return_address = 0x80015a04;
            frame->return_value = call(m, target, frame, arguments);
        }
    }
sector_return:
    frame->return_address = rrj_read32(m, frame->stack_pointer + 32);
    frame->preserved_s1 = rrj_read32(m, frame->stack_pointer + 28);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 24);
    frame->stack_pointer += 40;
    return frame->return_value;
}

uint32_t sub_80045BEC(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    frame->return_address = 0x80045bfc;
    frame->return_value = call(m, 0x80047580, frame, arguments);
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->return_value = frame->return_value < 1;
    frame->stack_pointer += 24;
    return frame->return_value;
}
