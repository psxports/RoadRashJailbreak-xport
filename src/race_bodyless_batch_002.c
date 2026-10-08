#include "race_bodyless_batch_002.h"

uint32_t sub_80047580(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    uint32_t busy;
    frame->return_value = rrj_read32(m, runtime_gp + 0x434);
    arguments[2] = 0x20000;
    w_u8(frame->return_value, 0);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x440);
    frame->return_value = 0x80;
    w_u8(frame->secondary_result, (uint8_t)frame->return_value);
    frame->return_value = rrj_read32(m, runtime_gp + 0x544);
    arguments[2] |= 0x943;
    rrj_write32(m, frame->return_value, arguments[2]);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x444);
    frame->return_value = 0x1323;
    rrj_write32(m, frame->secondary_result, frame->return_value);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x548);
    frame->return_value = rrj_read32(m, frame->secondary_result);
    frame->return_value |= 0x8000;
    rrj_write32(m, frame->secondary_result, frame->return_value);
    frame->return_value = rrj_read32(m, runtime_gp + 0x54c);
    rrj_write32(m, frame->return_value, arguments[0]);
    frame->return_value = 0x10000;
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x550);
    arguments[1] |= frame->return_value;
    rrj_write32(m, frame->secondary_result, arguments[1]);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x434);
    do
    {
        frame->return_value = r_u8(frame->secondary_result);
        frame->return_value &= 0x40;
        busy = frame->return_value == 0;
        frame->return_value = 0x11000000;
    } while (busy);
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x554);
    rrj_write32(m, frame->secondary_result, frame->return_value);
    arguments[0] = rrj_read32(m, runtime_gp + 0x554);
    frame->return_value = rrj_read32(m, arguments[0]);
    frame->secondary_result = 0x01000000;
    frame->return_value &= frame->secondary_result;
    busy = frame->return_value != 0;
    frame->secondary_result = arguments[0];
    if (busy)
    {
        arguments[0] = 0x01000000;
        do
        {
            frame->return_value = rrj_read32(m, frame->secondary_result);
            frame->return_value &= arguments[0];
        } while (frame->return_value);
    }
    frame->secondary_result = rrj_read32(m, runtime_gp + 0x444);
    frame->return_value = 0x1325;
    rrj_write32(m, frame->secondary_result, frame->return_value);
    frame->return_value = 0;
    return frame->return_value;
}

uint32_t sub_80047514(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t stop;
    frame->stack_pointer -= 32;
    rrj_write32(m, frame->stack_pointer + 20, frame->preserved_s1);
    frame->preserved_s1 = arguments[0];
    arguments[0] = 0x8005b1e4;
    rrj_write32(m, frame->stack_pointer + 24, frame->return_address);
    frame->return_address = 0x80047534;
    rrj_write32(m, frame->stack_pointer + 16, frame->preserved_s0);
    frame->return_value = call(m, 0x80046844, frame, arguments);
    frame->preserved_s0 = 0x01000000;
    for (;;)
    {
        frame->return_address = 0x80047540;
        frame->return_value = call(m, 0x8004687c, frame, arguments);
        stop = frame->return_value != 0;
        frame->return_value = 0xffffffff;
        if (stop)
            break;
        frame->return_value = rrj_read32(m, runtime_gp + 0x554);
        frame->return_value = rrj_read32(m, frame->return_value);
        frame->return_value &= frame->preserved_s0;
        stop = frame->return_value == 0;
        frame->return_value = 0;
        if (stop)
            break;
        stop = frame->preserved_s1 != 0;
        frame->return_value = 1;
        if (stop)
            break;
    }
    frame->return_address = rrj_read32(m, frame->stack_pointer + 24);
    frame->preserved_s1 = rrj_read32(m, frame->stack_pointer + 20);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 32;
    return frame->return_value;
}

uint32_t sub_80015724(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame)
{
    frame->return_value = 1;
    rrj_write32(m, runtime_gp + 0x740, frame->return_value);
    return frame->return_value;
}

uint32_t sub_80044FE4(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeBiosCall call)
{
    frame->return_value = call(m, 0xb0, 0x4a, frame, arguments);
    return frame->return_value;
}

uint32_t sub_80044FF4(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeBiosCall call)
{
    frame->return_value = call(m, 0xb0, 0x4b, frame, arguments);
    return frame->return_value;
}

uint32_t sub_80040CB4(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t target;
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 20, frame->return_address);
    rrj_write32(m, frame->stack_pointer + 16, frame->preserved_s0);
    frame->return_address = 0x80040ccc;
    rrj_write32(m, 0x80054934, 0);
    frame->return_value = call(m, 0x80043da4, frame, arguments);
    arguments[0] = 2;
    frame->preserved_s0 = 0x800d9cb0;
    frame->return_address = 0x80040ce0;
    arguments[1] = frame->preserved_s0;
    frame->return_value = call(m, 0x80043e44, frame, arguments);
    arguments[0] = 2;
    frame->return_address = 0x80040cec;
    arguments[1] = frame->preserved_s0;
    frame->return_value = call(m, 0x80043e34, frame, arguments);
    arguments[0] = 3;
    frame->secondary_result = rrj_read32(m, 0x8005495c);
    frame->return_value = 0xfffffffe;
    rrj_write32(m, frame->secondary_result, frame->return_value);
    frame->return_value = rrj_read32(m, frame->secondary_result + 4);
    arguments[1] = 0;
    frame->return_value |= 1;
    frame->return_address = 0x80040d14;
    rrj_write32(m, frame->secondary_result + 4, frame->return_value);
    frame->return_value = call(m, 0x80043e54, frame, arguments);
    frame->return_address = 0x80040d1c;
    frame->return_value = call(m, 0x80043db4, frame, arguments);
    arguments[0] = rrj_read32(m, 0x80054930);
    frame->return_value = rrj_read32(m, 0x80054900);
    target = frame->return_value;
    frame->return_address = 0x80040d38;
    frame->return_value = call(m, target, frame, arguments);
    arguments[0] = rrj_read32(m, 0x80054930);
    frame->return_value = rrj_read32(m, 0x80054900);
    target = frame->return_value;
    frame->return_address = 0x80040d54;
    arguments[0] += 240;
    frame->return_value = call(m, target, frame, arguments);
    frame->return_value = 0x800d9cc0;
    rrj_write32(m, frame->return_value + 4, 0);
    rrj_write32(m, frame->return_value, 0);
    frame->return_value = 1;
    rrj_write32(m, 0x80054934, frame->return_value);
    frame->return_address = rrj_read32(m, frame->stack_pointer + 20);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_80042334(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t target;
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->preserved_s0);
    frame->return_value = rrj_read32(m, 0x80054914);
    rrj_write32(m, frame->stack_pointer + 20, frame->return_address);
    target = frame->return_value;
    frame->return_address = 0x80042350;
    frame->preserved_s0 = arguments[0];
    frame->return_value = call(m, target, frame, arguments);
    arguments[0] = frame->preserved_s0;
    frame->secondary_result = rrj_read32(m, frame->preserved_s0 + 60);
    arguments[1] = 0xfffffffe;
    rrj_write32(m, 0x8005497c, frame->return_value);
    frame->return_address = 0x8004236c;
    w_u8(frame->secondary_result, 0);
    frame->return_value = call(m, 0x800411e8, frame, arguments);
    frame->return_address = rrj_read32(m, frame->stack_pointer + 20);
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_8001B500(RRJMemory *m, uint32_t *runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t target;
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    rrj_write32(m, 0x80052654, *runtime_gp);
    *runtime_gp = 0x80050000;
    *runtime_gp = rrj_read32(m, *runtime_gp + 0x253c);
    frame->return_value = rrj_read32(m, *runtime_gp + 0x7bc);
    if (frame->return_value)
    {
        target = frame->return_value;
        frame->return_address = 0x8001b538;
        frame->return_value = call(m, target, frame, arguments);
    }
    frame->secondary_result = rrj_read32(m, *runtime_gp + 0x7c0);
    frame->return_value = 1;
    frame->return_value -= frame->secondary_result;
    rrj_write32(m, *runtime_gp + 0x7c0, frame->return_value);
    *runtime_gp = 0x80050000;
    *runtime_gp = rrj_read32(m, *runtime_gp + 0x2654);
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_8001DF64(RRJMemory *m, uint32_t *runtime_gp, RRJNativeCallFrame *frame)
{
    rrj_write32(m, 0x800527d0, *runtime_gp);
    *runtime_gp = 0x80050000;
    *runtime_gp = rrj_read32(m, *runtime_gp + 0x253c);
    frame->secondary_result = rrj_read32(m, 0x800527d8);
    frame->return_value = 0xffff;
    if (frame->secondary_result == frame->return_value)
    {
        rrj_write32(m, 0x800527d8, 0);
        frame->return_value = rrj_read32(m, 0x800527dc);
        ++frame->return_value;
        rrj_write32(m, 0x800527dc, frame->return_value);
    }
    else
    {
        frame->return_value = rrj_read32(m, 0x800527d8);
        ++frame->return_value;
        rrj_write32(m, 0x800527d8, frame->return_value);
    }
    *runtime_gp = 0x80050000;
    *runtime_gp = rrj_read32(m, *runtime_gp + 0x27d0);
    frame->return_value = 0;
    return frame->return_value;
}

uint32_t sub_8001E0DC(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    frame->return_value = arguments[0];
    frame->secondary_result = frame->return_value;
    while (arguments[2])
    {
        w_u8(frame->secondary_result, (uint8_t)arguments[1]);
        --arguments[2];
        ++frame->secondary_result;
    }
    return frame->return_value;
}

uint32_t sub_8001B6A8(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame)
{
    frame->return_value = rrj_read32(m, 0x8005b2f8);
    rrj_write32(m, runtime_gp + 0x7cc, 0);
    rrj_write32(m, runtime_gp + 0x100, 0);
    rrj_write32(m, frame->return_value + 12, 0);
    rrj_write32(m, frame->return_value + 16, 0);
    return frame->return_value;
}

uint32_t sub_800201C0(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    uint32_t failed;
    frame->return_value = rrj_read32(m, runtime_gp + 0x130);
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    if (!frame->return_value)
    {
        arguments[0] = 0x6000;
        frame->return_address = 0x800201dc;
        arguments[1] = 0;
        frame->return_value = call(m, 0x8001447c, frame, arguments);
        arguments[0] = 0x600;
        rrj_write32(m, runtime_gp + 0x128, frame->return_value);
        frame->return_address = 0x800201ec;
        arguments[1] = 0;
        frame->return_value = call(m, 0x8001447c, frame, arguments);
        frame->secondary_result = rrj_read32(m, runtime_gp + 0x128);
        rrj_write32(m, runtime_gp + 0x12c, frame->return_value);
        failed = frame->secondary_result == 0;
        if (!failed)
        {
            failed = frame->return_value == 0;
            frame->return_value = 1;
        }
        if (failed)
        {
            frame->return_address = 0x80020218;
            frame->return_value = call(m, 0x80020228, frame, arguments);
        }
        else
            rrj_write32(m, runtime_gp + 0x130, frame->return_value);
    }
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_8002026C(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call)
{
    arguments[2] = arguments[0];
    arguments[0] = rrj_read32(m, runtime_gp + 0x128);
    frame->stack_pointer -= 24;
    rrj_write32(m, frame->stack_pointer + 16, frame->return_address);
    if (arguments[0])
    {
        arguments[1] = rrj_read32(m, runtime_gp + 0x12c);
        if (arguments[1])
        {
            frame->return_value = rrj_read32(m, runtime_gp + 0x130);
            if (frame->return_value)
            {
                frame->return_address = 0x800202a8;
                frame->return_value = call(m, 0x800202b8, frame, arguments);
            }
        }
    }
    frame->return_address = rrj_read32(m, frame->stack_pointer + 16);
    frame->stack_pointer += 24;
    return frame->return_value;
}

uint32_t sub_800202B8(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    uint32_t row, values, table_offset, table_cursor, value, bits, prefix, copies, repeat;
    frame->stack_pointer -= 8;
    rrj_write32(m, frame->stack_pointer, frame->preserved_s0);
    rrj_write32(m, runtime_gp + 0x120, arguments[0]);
    rrj_write32(m, runtime_gp + 0x124, arguments[1]);
    frame->return_value = 0xffffffff;
    if (arguments[0] && arguments[1])
    {
        values = arguments[0] + 0x2000;
        if (!arguments[2])
        {
            frame->return_value = 0x80050000;
            arguments[2] = frame->return_value + 0x2c20;
        }
        row = 0;
        frame->preserved_s0 = 13;
        table_offset = 2;
        frame->return_value = 0x80050000;
        table_cursor = frame->return_value + 0x28a0;
        do
        {
            value = r_u16(arguments[2]);
            arguments[2] += 2;
            arguments[3] = 0;
            frame->secondary_result = 0x800528a0 + table_offset;
            bits = r_u8(table_cursor);
            frame->secondary_result = r_u16(frame->secondary_result);
            frame->return_value = frame->preserved_s0 - bits;
            copies = UINT32_C(1) << (frame->return_value & 31);
            prefix = frame->secondary_result >> 3;
            if (rrj_s32(copies) > 0)
            {
                frame->return_value = prefix | arguments[3];
                do
                {
                    ++arguments[3];
                    frame->secondary_result = arguments[0] + frame->return_value;
                    frame->return_value = values + (frame->return_value << 1);
                    w_u8(frame->secondary_result, (uint8_t)bits);
                    w_u16(frame->return_value, (uint16_t)value);
                    frame->return_value = rrj_s32(arguments[3]) < rrj_s32(copies);
                    repeat = frame->return_value;
                    frame->return_value = prefix | arguments[3];
                } while (repeat);
            }
            table_offset += 4;
            ++row;
            frame->return_value = rrj_s32(row) < 96;
            table_cursor += 4;
        } while (frame->return_value);
        arguments[0] = arguments[1];
        values = arguments[0] + 512;
        row = 0;
        frame->preserved_s0 = 9;
        table_offset = 2;
        frame->return_value = 0x80050000;
        table_cursor = frame->return_value + 0x2a20;
        do
        {
            value = r_u16(arguments[2]);
            arguments[2] += 2;
            arguments[1] = 0;
            frame->secondary_result = 0x80052a20 + table_offset;
            bits = r_u8(table_cursor);
            frame->secondary_result = r_u16(frame->secondary_result);
            frame->return_value = frame->preserved_s0 - bits;
            arguments[3] = UINT32_C(1) << (frame->return_value & 31);
            prefix = frame->secondary_result >> 7;
            if (rrj_s32(arguments[3]) > 0)
            {
                frame->return_value = prefix | arguments[1];
                do
                {
                    ++arguments[1];
                    frame->secondary_result = arguments[0] + frame->return_value;
                    frame->return_value = values + (frame->return_value << 1);
                    w_u8(frame->secondary_result, (uint8_t)bits);
                    w_u16(frame->return_value, (uint16_t)value);
                    frame->return_value = rrj_s32(arguments[1]) < rrj_s32(arguments[3]);
                    repeat = frame->return_value;
                    frame->return_value = prefix | arguments[1];
                } while (repeat);
            }
            table_offset += 4;
            ++row;
            frame->return_value = rrj_s32(row) < 128;
            table_cursor += 4;
        } while (frame->return_value);
        frame->return_value = 0;
    }
    frame->preserved_s0 = rrj_read32(m, frame->stack_pointer);
    frame->stack_pointer += 8;
    return frame->return_value;
}

uint32_t sub_8001FC84(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    rrj_write32(m, runtime_gp + 0x81c, arguments[0]);
    return frame->return_value;
}

uint32_t sub_8001EFDC(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    frame->return_value = 0x800d0000;
    rrj_write32(m, frame->return_value + 0x69f4, arguments[0]);
    return frame->return_value;
}
