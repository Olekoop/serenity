/*
 * Copyright (c) 2023, Liav A. <liavalb@hotmail.co.il>
 * Copyright (c) 2024, Olekoop <mlglol360xd@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/StdLibExtraDetails.h>
#include <AK/StringView.h>
#include <AK/Types.h>

namespace Kernel::USB::HID {

enum class SubclassCode : u8 {
    BootProtocol = 0x01,
};

struct [[gnu::packed]] MouseBootProtocolPacket {
    u8 buttons;
    i8 x;
    i8 y;
    i8 z;
    i8 reserved1;
    i8 reserved2;
};

static_assert(AssertSize<MouseBootProtocolPacket, 6>());

struct [[gnu::packed]] KeyboardBootProtocolPacket {
    u8 modifiers;
    u8 reverved;
    u8 keypress1;
    u8 keypress2;
    u8 keypress3;
    u8 keypress4;
    u8 keypress5;
    u8 keypress6;
};

static_assert(AssertSize<KeyboardBootProtocolPacket, 8>());

constexpr StringView subclass_string(SubclassCode code)
{
    switch (code) {
    case SubclassCode::BootProtocol:
        return "Boot Protocol"sv;
    }

    return "Reserved"sv;
}

enum class InterfaceProtocol : u8 {
    Mouse = 0x02,
    Keyboard = 0x01,
};

}
