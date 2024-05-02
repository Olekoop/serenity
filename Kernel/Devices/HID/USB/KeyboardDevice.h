/*
 * Copyright (c) 2022-2023, Liav A. <liavalb@hotmail.co.il>
 * Copyright (c) 2024, Olekoop <mlglol360xd@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Badge.h>
#include <AK/CircularQueue.h>
#include <AK/OwnPtr.h>
#include <AK/RefPtr.h>
#include <Kernel/Bus/USB/USBDevice.h>
#include <Kernel/Bus/USB/USBPipe.h>
#include <Kernel/Devices/HID/KeyboardDevice.h>
#include <Kernel/Library/KString.h>
#include <Kernel/Security/Random.h>

namespace Kernel {

class USBKeyboardDevice final : public KeyboardDevice {
    friend class DeviceManagement;

public:
    static ErrorOr<NonnullRefPtr<USBKeyboardDevice>> try_create_instance(USB::Device const&, size_t max_packet_size, NonnullOwnPtr<USB::InterruptInPipe> pipe);
    virtual ~USBKeyboardDevice() override {};

    USB::Device const& device() const { return *m_attached_usb_device; }

private:
    ErrorOr<void> create_and_start_polling_process(size_t max_packet_size);

    USBKeyboardDevice(USB::Device const&, NonnullOwnPtr<USB::InterruptInPipe> pipe);
    NonnullOwnPtr<USB::InterruptInPipe> m_interrupt_in_pipe;
    NonnullRefPtr<USB::Device> m_attached_usb_device;

    IntrusiveListNode<USBKeyboardDevice, NonnullRefPtr<USBKeyboardDevice>> m_list_node;
    bool m_key1_pressed = false;
    bool m_key2_pressed = false;
    bool m_key3_pressed = false;
    bool m_key4_pressed = false;
    bool m_key5_pressed = false;
    bool m_key6_pressed = false;
    KeyEvent m_last_event = {
        .key = Key_Invalid,
        .map_entry_index = 0xFF
    };
    u8 m_current_num_pressed_keys = 0;
    u8 m_last_num_pressed_keys = 0;

public:
    using List = IntrusiveList<&USBKeyboardDevice::m_list_node>;
};

}
