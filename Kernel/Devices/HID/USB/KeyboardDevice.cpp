/*
 * Copyright (c) 2022, Liav A. <liavalb@hotmail.co.il>
 * Copyright (c) 2024, Olekoop <mlglol360xd@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Try.h>
#include <AK/TypedTransfer.h>
#include <Kernel/API/KeyCode.h>
#include <Kernel/Bus/USB/Drivers/HID/Codes.h>
#include <Kernel/Bus/USB/USBController.h>
#include <Kernel/Bus/USB/USBDescriptors.h>
#include <Kernel/Bus/USB/USBRequest.h>
#include <Kernel/Bus/USB/USBTransfer.h>
#include <Kernel/Devices/DeviceManagement.h>
#include <Kernel/Devices/HID/USB/KeyboardDevice.h>
#include <Kernel/Library/KString.h>

namespace Kernel {

static constexpr KeyCodeEntry KeyCodes[232] = {
    { Key_Invalid, 0xFF }, // No key pressed
    { Key_Invalid, 0xFF }, // Keyboard Error Roll Over - used for all slots if too many keys are pressed ("Phantom key")
    { Key_Invalid, 0xFF }, // Keyboard POST Fail
    { Key_Invalid, 0xFF }, // Keyboard Error Undefined
    { Key_A, 0x1E },
    { Key_B, 0x30 },
    { Key_C, 0x2E },
    { Key_D, 0x20 },
    { Key_E, 0x12 },
    { Key_F, 0x21 },
    { Key_G, 0x22 },
    { Key_H, 0x23 },
    { Key_I, 0x17 },
    { Key_J, 0x24 },
    { Key_K, 0x25 },
    { Key_L, 0x26 },
    { Key_M, 0x32 },
    { Key_N, 0x31 },
    { Key_O, 0x18 },
    { Key_P, 0x19 },
    { Key_Q, 0x10 },
    { Key_R, 0x13 },
    { Key_S, 0x1F },
    { Key_T, 0x14 },
    { Key_U, 0x16 },
    { Key_V, 0x2F },
    { Key_W, 0x11 },
    { Key_X, 0x2D },
    { Key_Y, 0x15 },
    { Key_Z, 0x2C },
    { Key_1, 2 },
    { Key_2, 3 },
    { Key_3, 4 },
    { Key_4, 5 },
    { Key_5, 6 },
    { Key_6, 7 },
    { Key_7, 8 },
    { Key_8, 9 },
    { Key_9, 0x0A },
    { Key_0, 0x0B },
    { Key_Return, 0x1C },
    { Key_Escape, 1 },
    { Key_Backspace, 0x0E },
    { Key_Tab, 0x0F },
    { Key_Space, 0x39 },
    { Key_Minus, 0x0C },
    { Key_Equal, 0x0D },
    { Key_LeftBracket, 0x1A },
    { Key_RightBracket, 0x1B },
    { Key_Backslash, 0x2B },
    { Key_Hashtag, 0x2B }, // Intended for key next to vertical Return key. Keyboard Backslash (0x31) is used instead
    { Key_Semicolon, 0x27 },
    { Key_Apostrophe, 0x28 },
    { Key_Backtick, 0x29 },
    { Key_Comma, 0x33 },
    { Key_Period, 0x34 },
    { Key_Slash, 0x35 },
    { Key_CapsLock, 0x3A },
    { Key_F1, 0x3B },
    { Key_F2, 0x3C },
    { Key_F3, 0x3D },
    { Key_F4, 0x3E },
    { Key_F5, 0x3F },
    { Key_F6, 0x40 },
    { Key_F7, 0x41 },
    { Key_F8, 0x42 },
    { Key_F9, 0x43 },
    { Key_F10, 0x44 },
    { Key_F11, 0xFF },
    { Key_F12, 0xFF },
    { Key_PrintScreen, 0xFF },
    { Key_ScrollLock, 0xFF },
    { Key_PauseBreak, 0xFF },
    { Key_Insert, 0xFF },
    { Key_Home, 0xFF },
    { Key_PageUp, 0xFF },
    { Key_Delete, 0xFF },
    { Key_End, 0xFF },
    { Key_PageDown, 0xFF },
    { Key_Right, 0xFF },
    { Key_Left, 0xFF },
    { Key_Down, 0xFF },
    { Key_Up, 0xFF },
    // The numpad
    { Key_NumLock, 0xFF },
    { Key_Slash, 0x35 },
    { Key_Asterisk, 0x37 },
    { Key_Minus, 0x4A },
    { Key_Plus, 0x4E },
    { Key_Return, 0x1C },
    { Key_1, 2 },
    { Key_2, 3 },
    { Key_3, 4 },
    { Key_4, 5 },
    { Key_5, 6 },
    { Key_6, 7 },
    { Key_7, 8 },
    { Key_8, 9 },
    { Key_9, 0x0A },
    { Key_0, 0x0B },
    { Key_Period, 0x34 },
    { Key_Backslash, 0x2B },
    { Key_Menu, 0xFF },
    { Key_Power, 0xFF },
    { Key_Equal, 0x0D },
    // The following keys utill the modifiers are seldom used and are not supported by SerenityOS or any major OS
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    // The modifier keys
    { Key_Control, 0x1D },
    { Key_LeftShift, 0x2A },
    { Key_Alt, 0x38 },
    { Key_LeftGUI, 0xFF },
    { Key_RightControl, 0xFF },
    { Key_RightShift, 0x36 },
    { Key_RightAlt, 0xFF },
    { Key_RightGUI, 0xFF }
};

static constexpr KeyCodeEntry NumKeyCodes[232] = {
    { Key_Invalid, 0xFF }, // No key pressed
    { Key_Invalid, 0xFF }, // Keyboard Error Roll Over - used for all slots if too many keys are pressed ("Phantom key")
    { Key_Invalid, 0xFF }, // Keyboard POST Fail
    { Key_Invalid, 0xFF }, // Keyboard Error Undefined
    { Key_A, 0x1E },
    { Key_B, 0x30 },
    { Key_C, 0x2E },
    { Key_D, 0x20 },
    { Key_E, 0x12 },
    { Key_F, 0x21 },
    { Key_G, 0x22 },
    { Key_H, 0x23 },
    { Key_I, 0x17 },
    { Key_J, 0x24 },
    { Key_K, 0x25 },
    { Key_L, 0x26 },
    { Key_M, 0x32 },
    { Key_N, 0x31 },
    { Key_O, 0x18 },
    { Key_P, 0x19 },
    { Key_Q, 0x10 },
    { Key_R, 0x13 },
    { Key_S, 0x1F },
    { Key_T, 0x14 },
    { Key_U, 0x16 },
    { Key_V, 0x2F },
    { Key_W, 0x11 },
    { Key_X, 0x2D },
    { Key_Y, 0x15 },
    { Key_Z, 0x2C },
    { Key_1, 2 },
    { Key_2, 3 },
    { Key_3, 4 },
    { Key_4, 5 },
    { Key_5, 6 },
    { Key_6, 7 },
    { Key_7, 8 },
    { Key_8, 9 },
    { Key_9, 0x0A },
    { Key_0, 0x0B },
    { Key_Return, 0x1C },
    { Key_Escape, 1 },
    { Key_Backspace, 0x0E },
    { Key_Tab, 0x0F },
    { Key_Space, 0x39 },
    { Key_Minus, 0x0C },
    { Key_Equal, 0x0D },
    { Key_LeftBracket, 0x1A },
    { Key_RightBracket, 0x1B },
    { Key_Backslash, 0x2B },
    { Key_Hashtag, 0x2B }, // Intended for key next to vertical Return key. Keyboard Backslash (0x31) is used instead
    { Key_Semicolon, 0x27 },
    { Key_Apostrophe, 0x28 },
    { Key_Backtick, 0x29 },
    { Key_Comma, 0x33 },
    { Key_Period, 0x34 },
    { Key_Slash, 0x35 },
    { Key_CapsLock, 0x3A },
    { Key_F1, 0x3B },
    { Key_F2, 0x3C },
    { Key_F3, 0x3D },
    { Key_F4, 0x3E },
    { Key_F5, 0x3F },
    { Key_F6, 0x40 },
    { Key_F7, 0x41 },
    { Key_F8, 0x42 },
    { Key_F9, 0x43 },
    { Key_F10, 0x44 },
    { Key_F11, 0xFF },
    { Key_F12, 0xFF },
    { Key_PrintScreen, 0xFF },
    { Key_ScrollLock, 0xFF },
    { Key_PauseBreak, 0xFF },
    { Key_Insert, 0xFF },
    { Key_Home, 0xFF },
    { Key_PageUp, 0xFF },
    { Key_Delete, 0xFF },
    { Key_End, 0xFF },
    { Key_PageDown, 0xFF },
    { Key_Right, 0xFF },
    { Key_Left, 0xFF },
    { Key_Down, 0xFF },
    { Key_Up, 0xFF },
    // The numpad
    { Key_NumLock, 0xFF },
    { Key_Slash, 0x35 },
    { Key_Asterisk, 0x37 },
    { Key_Minus, 0x4A },
    { Key_Plus, 0x4E },
    { Key_Return, 0x1C },
    { Key_End, 0xFF },
    { Key_Down, 0xFF },
    { Key_PageDown, 0xFF },
    { Key_Left, 0xFF },
    { Key_5, 6 },
    { Key_Right, 0xFF },
    { Key_Home, 0xFF },
    { Key_Up, 0xFF },
    { Key_PageUp, 0xFF },
    { Key_Insert, 0xFF },
    { Key_Delete, 0xFF },
    { Key_Backslash, 0x2B },
    { Key_Menu, 0xFF },
    { Key_Power, 0xFF },
    { Key_Equal, 0x0D },
    // The following keys utill the modifiers are seldom used and are not supported by SerenityOS or any major OS
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    // The modifier keys
    { Key_Control, 0x1D },
    { Key_LeftShift, 0x2A },
    { Key_Alt, 0x38 },
    { Key_LeftGUI, 0xFF },
    { Key_RightControl, 0xFF },
    { Key_RightShift, 0x36 },
    { Key_RightAlt, 0xFF },
    { Key_RightGUI, 0xFF }
};

static constexpr KeyCodeEntry ShiftedKeyCodes[232] = {
    { Key_Invalid, 0xFF }, // No key pressed
    { Key_Invalid, 0xFF }, // Keyboard Error Roll Over - used for all slots if too many keys are pressed ("Phantom key")
    { Key_Invalid, 0xFF }, // Keyboard POST Fail
    { Key_Invalid, 0xFF }, // Keyboard Error Undefined
    { Key_A, 0x1E },
    { Key_B, 0x30 },
    { Key_C, 0x2E },
    { Key_D, 0x20 },
    { Key_E, 0x12 },
    { Key_F, 0x21 },
    { Key_G, 0x22 },
    { Key_H, 0x23 },
    { Key_I, 0x17 },
    { Key_J, 0x24 },
    { Key_K, 0x25 },
    { Key_L, 0x26 },
    { Key_M, 0x32 },
    { Key_N, 0x31 },
    { Key_O, 0x18 },
    { Key_P, 0x19 },
    { Key_Q, 0x10 },
    { Key_R, 0x13 },
    { Key_S, 0x1F },
    { Key_T, 0x14 },
    { Key_U, 0x16 },
    { Key_V, 0x2F },
    { Key_W, 0x11 },
    { Key_X, 0x2D },
    { Key_Y, 0x15 },
    { Key_Z, 0x2C },
    { Key_ExclamationPoint, 2 },
    { Key_AtSign, 3 },
    { Key_Hashtag, 4 },
    { Key_Dollar, 5 },
    { Key_Percent, 6 },
    { Key_Circumflex, 7 },
    { Key_Ampersand, 8 },
    { Key_Asterisk, 9 },
    { Key_LeftParen, 0x0A },
    { Key_RightParen, 0x0B },
    { Key_Return, 0x1C },
    { Key_Escape, 1 },
    { Key_Backspace, 0x0E },
    { Key_Tab, 0x0F },
    { Key_Space, 0x39 },
    { Key_Underscore, 0x0C },
    { Key_Equal, 0x0D },
    { Key_LeftBrace, 0x1A },
    { Key_RightBrace, 0x1B },
    { Key_Pipe, 0x2B },
    { Key_Tilde, 0x2B }, // Intended for key next to vertical Return key. Keyboard Backslash (0x31) is used instead
    { Key_Colon, 0x27 },
    { Key_DoubleQuote, 0x28 },
    { Key_Backtick, 0x29 },
    { Key_Tilde, 0x33 },
    { Key_LessThan, 0x34 },
    { Key_GreaterThan, 0x35 },
    { Key_CapsLock, 0x3A },
    { Key_F1, 0x3B },
    { Key_F2, 0x3C },
    { Key_F3, 0x3D },
    { Key_F4, 0x3E },
    { Key_F5, 0x3F },
    { Key_F6, 0x40 },
    { Key_F7, 0x41 },
    { Key_F8, 0x42 },
    { Key_F9, 0x43 },
    { Key_F10, 0x44 },
    { Key_F11, 0xFF },
    { Key_F12, 0xFF },
    { Key_PrintScreen, 0xFF },
    { Key_ScrollLock, 0xFF },
    { Key_PauseBreak, 0xFF },
    { Key_Insert, 0xFF },
    { Key_Home, 0xFF },
    { Key_PageUp, 0xFF },
    { Key_Delete, 0xFF },
    { Key_End, 0xFF },
    { Key_PageDown, 0xFF },
    { Key_Right, 0xFF },
    { Key_Left, 0xFF },
    { Key_Down, 0xFF },
    { Key_Up, 0xFF },
    // The numpad
    { Key_NumLock, 0xFF },
    { Key_Slash, 0x35 },
    { Key_Asterisk, 0x37 },
    { Key_Minus, 0x4A },
    { Key_Plus, 0x4E },
    { Key_Return, 0x1C },
    { Key_1, 2 },
    { Key_2, 3 },
    { Key_3, 4 },
    { Key_4, 5 },
    { Key_5, 6 },
    { Key_6, 7 },
    { Key_7, 8 },
    { Key_8, 9 },
    { Key_9, 0x0A },
    { Key_0, 0x0B },
    { Key_Period, 0x34 },
    { Key_Pipe, 0x2B },
    { Key_Menu, 0xFF },
    { Key_Power, 0xFF },
    { Key_Plus, 0x0D },
    // The following keys utill the modifiers are seldom used and are not supported by SerenityOS or any major OS
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    // The modifier keys
    { Key_Control, 0x1D },
    { Key_LeftShift, 0x2A },
    { Key_Alt, 0x38 },
    { Key_LeftGUI, 0xFF },
    { Key_RightControl, 0xFF },
    { Key_RightShift, 0x36 },
    { Key_RightAlt, 0xFF },
    { Key_RightGUI, 0xFF }
};

static constexpr KeyCodeEntry ShiftedNumKeyCodes[232] = {
    { Key_Invalid, 0xFF }, // No key pressed
    { Key_Invalid, 0xFF }, // Keyboard Error Roll Over - used for all slots if too many keys are pressed ("Phantom key")
    { Key_Invalid, 0xFF }, // Keyboard POST Fail
    { Key_Invalid, 0xFF }, // Keyboard Error Undefined
    { Key_A, 0x1E },
    { Key_B, 0x30 },
    { Key_C, 0x2E },
    { Key_D, 0x20 },
    { Key_E, 0x12 },
    { Key_F, 0x21 },
    { Key_G, 0x22 },
    { Key_H, 0x23 },
    { Key_I, 0x17 },
    { Key_J, 0x24 },
    { Key_K, 0x25 },
    { Key_L, 0x26 },
    { Key_M, 0x32 },
    { Key_N, 0x31 },
    { Key_O, 0x18 },
    { Key_P, 0x19 },
    { Key_Q, 0x10 },
    { Key_R, 0x13 },
    { Key_S, 0x1F },
    { Key_T, 0x14 },
    { Key_U, 0x16 },
    { Key_V, 0x2F },
    { Key_W, 0x11 },
    { Key_X, 0x2D },
    { Key_Y, 0x15 },
    { Key_Z, 0x2C },
    { Key_ExclamationPoint, 2 },
    { Key_AtSign, 3 },
    { Key_Hashtag, 4 },
    { Key_Dollar, 5 },
    { Key_Percent, 6 },
    { Key_Circumflex, 7 },
    { Key_Ampersand, 8 },
    { Key_Asterisk, 9 },
    { Key_LeftParen, 0x0A },
    { Key_RightParen, 0x0B },
    { Key_Return, 0x1C },
    { Key_Escape, 1 },
    { Key_Backspace, 0x0E },
    { Key_Tab, 0x0F },
    { Key_Space, 0x39 },
    { Key_Underscore, 0x0C },
    { Key_Equal, 0x0D },
    { Key_LeftBrace, 0x1A },
    { Key_RightBrace, 0x1B },
    { Key_Pipe, 0x2B },
    { Key_Tilde, 0x2B }, // Intended for key next to vertical Return key. Keyboard Backslash (0x31) is used instead
    { Key_Colon, 0x27 },
    { Key_DoubleQuote, 0x28 },
    { Key_Backtick, 0x29 },
    { Key_Tilde, 0x33 },
    { Key_LessThan, 0x34 },
    { Key_GreaterThan, 0x35 },
    { Key_CapsLock, 0x3A },
    { Key_F1, 0x3B },
    { Key_F2, 0x3C },
    { Key_F3, 0x3D },
    { Key_F4, 0x3E },
    { Key_F5, 0x3F },
    { Key_F6, 0x40 },
    { Key_F7, 0x41 },
    { Key_F8, 0x42 },
    { Key_F9, 0x43 },
    { Key_F10, 0x44 },
    { Key_F11, 0xFF },
    { Key_F12, 0xFF },
    { Key_PrintScreen, 0xFF },
    { Key_ScrollLock, 0xFF },
    { Key_PauseBreak, 0xFF },
    { Key_Insert, 0xFF },
    { Key_Home, 0xFF },
    { Key_PageUp, 0xFF },
    { Key_Delete, 0xFF },
    { Key_End, 0xFF },
    { Key_PageDown, 0xFF },
    { Key_Right, 0xFF },
    { Key_Left, 0xFF },
    { Key_Down, 0xFF },
    { Key_Up, 0xFF },
    // The numpad
    { Key_NumLock, 0xFF },
    { Key_Slash, 0x35 },
    { Key_Asterisk, 0x37 },
    { Key_Minus, 0x4A },
    { Key_Plus, 0x4E },
    { Key_Return, 0x1C },
    { Key_End, 0xFF },
    { Key_Down, 0xFF },
    { Key_PageDown, 0xFF },
    { Key_Left, 0xFF },
    { Key_5, 6 },
    { Key_Right, 0xFF },
    { Key_Home, 0xFF },
    { Key_Up, 0xFF },
    { Key_PageUp, 0xFF },
    { Key_Insert, 0xFF },
    { Key_Delete, 0xFF },
    { Key_Pipe, 0x2B },
    { Key_Menu, 0xFF },
    { Key_Power, 0xFF },
    { Key_Plus, 0x0D },
    // The following keys utill the modifiers are seldom used and are not supported by SerenityOS or any major OS
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    { Key_Invalid, 0xFF },
    // The modifier keys
    { Key_Control, 0x1D },
    { Key_LeftShift, 0x2A },
    { Key_Alt, 0x38 },
    { Key_LeftGUI, 0xFF },
    { Key_RightControl, 0xFF },
    { Key_RightShift, 0x36 },
    { Key_RightAlt, 0xFF },
    { Key_RightGUI, 0xFF }
};

ErrorOr<NonnullRefPtr<USBKeyboardDevice>> USBKeyboardDevice::try_create_instance(USB::Device const& usb_device, size_t max_packet_size, NonnullOwnPtr<USB::InterruptInPipe> pipe)
{
    if (max_packet_size < 8)
        return Error::from_errno(ENOTSUP);
    auto device = TRY(DeviceManagement::try_create_device<USBKeyboardDevice>(usb_device, move(pipe)));
    TRY(device->create_and_start_polling_process(max_packet_size));
    return *device;
}

ErrorOr<void> USBKeyboardDevice::create_and_start_polling_process(size_t max_packet_size)
{
    VERIFY(max_packet_size >= 8);
    [[maybe_unused]] auto interrupt_in_transfer = TRY(m_interrupt_in_pipe->submit_interrupt_in_transfer(max_packet_size, 10, [this](auto* transfer) {
        USB::HID::KeyboardBootProtocolPacket packet_raw;
        memcpy(&packet_raw, transfer->buffer().as_ptr(), 8);
        KeyEvent event;
        event.flags = 0;
        event.key = Key_Invalid;
        if (packet_raw.modifiers & (1 << 0) || packet_raw.modifiers & (1 << 4))
            update_modifier(Mod_Ctrl, true);
        else
            update_modifier(Mod_Ctrl, false);
        if (packet_raw.modifiers & (1 << 1) || packet_raw.modifiers & (1 << 5))
            update_modifier(Mod_Shift, true);
        else
            update_modifier(Mod_Shift, false);
        if (packet_raw.modifiers & (1 << 2))
            update_modifier(Mod_Alt, true);
        else
            update_modifier(Mod_Alt, false);
        if (packet_raw.modifiers & (1 << 3) || packet_raw.modifiers & (1 << 7))
            update_modifier(Mod_Super, true);
        else
            update_modifier(Mod_Super, false);
        if (packet_raw.modifiers & (1 << 6))
            update_modifier(Mod_AltGr, true);
        else
            update_modifier(Mod_AltGr, false);

        if (packet_raw.keypress1 <= 0xe7 && !!packet_raw.keypress1 && !m_key1_pressed) {
            event.flags |= Is_Press;
            if (m_num_lock_on) {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedKeyCodes[packet_raw.keypress1].key_code;
                    event.map_entry_index = ShiftedKeyCodes[packet_raw.keypress1].map_entry_index;
                    if (packet_raw.keypress1 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = KeyCodes[packet_raw.keypress1].key_code;
                    event.map_entry_index = KeyCodes[packet_raw.keypress1].map_entry_index;
                    if (packet_raw.keypress1 == 0x54)
                        event.code_point = '/';
                }
            } else {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedNumKeyCodes[packet_raw.keypress1].key_code;
                    event.map_entry_index = ShiftedNumKeyCodes[packet_raw.keypress1].map_entry_index;
                    if (packet_raw.keypress1 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = NumKeyCodes[packet_raw.keypress1].key_code;
                    event.map_entry_index = NumKeyCodes[packet_raw.keypress1].map_entry_index;
                    if (packet_raw.keypress1 == 0x54)
                        event.code_point = '/';
                }
            }
            m_key1_pressed = true;
            m_last_event = event;
            handle_input_event(event);
        }
        if (packet_raw.keypress2 <= 0xe7 && !!packet_raw.keypress2 && !m_key2_pressed) {
            event.flags |= Is_Press;
            if (m_num_lock_on) {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedKeyCodes[packet_raw.keypress2].key_code;
                    event.map_entry_index = ShiftedKeyCodes[packet_raw.keypress2].map_entry_index;
                    if (packet_raw.keypress2 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = KeyCodes[packet_raw.keypress2].key_code;
                    event.map_entry_index = KeyCodes[packet_raw.keypress2].map_entry_index;
                    if (packet_raw.keypress2 == 0x54)
                        event.code_point = '/';
                }
            } else {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedNumKeyCodes[packet_raw.keypress2].key_code;
                    event.map_entry_index = ShiftedNumKeyCodes[packet_raw.keypress2].map_entry_index;
                    if (packet_raw.keypress2 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = NumKeyCodes[packet_raw.keypress2].key_code;
                    event.map_entry_index = NumKeyCodes[packet_raw.keypress2].map_entry_index;
                    if (packet_raw.keypress2 == 0x54)
                        event.code_point = '/';
                }
            }
            m_key2_pressed = true;
            m_last_event = event;
            handle_input_event(event);
        }
        if (packet_raw.keypress3 <= 0xe7 && !!packet_raw.keypress3 && !m_key3_pressed) {
            event.flags |= Is_Press;
            if (m_num_lock_on) {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedKeyCodes[packet_raw.keypress3].key_code;
                    event.map_entry_index = ShiftedKeyCodes[packet_raw.keypress3].map_entry_index;
                    if (packet_raw.keypress3 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = KeyCodes[packet_raw.keypress3].key_code;
                    event.map_entry_index = KeyCodes[packet_raw.keypress3].map_entry_index;
                    if (packet_raw.keypress3 == 0x54)
                        event.code_point = '/';
                }
            } else {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedNumKeyCodes[packet_raw.keypress3].key_code;
                    event.map_entry_index = ShiftedNumKeyCodes[packet_raw.keypress3].map_entry_index;
                    if (packet_raw.keypress3 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = NumKeyCodes[packet_raw.keypress3].key_code;
                    event.map_entry_index = NumKeyCodes[packet_raw.keypress3].map_entry_index;
                    if (packet_raw.keypress3 == 0x54)
                        event.code_point = '/';
                }
            }
            m_key3_pressed = true;
            m_last_event = event;
            handle_input_event(event);
        }
        if (packet_raw.keypress4 <= 0xe7 && !!packet_raw.keypress4 && !m_key4_pressed) {
            event.flags |= Is_Press;
            if (m_num_lock_on) {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedKeyCodes[packet_raw.keypress4].key_code;
                    event.map_entry_index = ShiftedKeyCodes[packet_raw.keypress4].map_entry_index;
                    if (packet_raw.keypress4 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = KeyCodes[packet_raw.keypress4].key_code;
                    event.map_entry_index = KeyCodes[packet_raw.keypress4].map_entry_index;
                    if (packet_raw.keypress4 == 0x54)
                        event.code_point = '/';
                }
            } else {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedNumKeyCodes[packet_raw.keypress4].key_code;
                    event.map_entry_index = ShiftedNumKeyCodes[packet_raw.keypress4].map_entry_index;
                    if (packet_raw.keypress4 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = NumKeyCodes[packet_raw.keypress4].key_code;
                    event.map_entry_index = NumKeyCodes[packet_raw.keypress4].map_entry_index;
                    if (packet_raw.keypress4 == 0x54)
                        event.code_point = '/';
                }
            }
            m_key4_pressed = true;
            m_last_event = event;
            handle_input_event(event);
        }
        if (packet_raw.keypress5 <= 0xe7 && !!packet_raw.keypress5 && !m_key5_pressed) {
            event.flags |= Is_Press;
            if (m_num_lock_on) {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedKeyCodes[packet_raw.keypress5].key_code;
                    event.map_entry_index = ShiftedKeyCodes[packet_raw.keypress5].map_entry_index;
                    if (packet_raw.keypress5 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = KeyCodes[packet_raw.keypress5].key_code;
                    event.map_entry_index = KeyCodes[packet_raw.keypress5].map_entry_index;
                    if (packet_raw.keypress5 == 0x54)
                        event.code_point = '/';
                }
            } else {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedNumKeyCodes[packet_raw.keypress5].key_code;
                    event.map_entry_index = ShiftedNumKeyCodes[packet_raw.keypress5].map_entry_index;
                    if (packet_raw.keypress5 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = NumKeyCodes[packet_raw.keypress5].key_code;
                    event.map_entry_index = NumKeyCodes[packet_raw.keypress5].map_entry_index;
                    if (packet_raw.keypress5 == 0x54)
                        event.code_point = '/';
                }
            }
            m_key5_pressed = true;
            m_last_event = event;
            handle_input_event(event);
        }
        if (packet_raw.keypress6 <= 0xe7 && !!packet_raw.keypress6 && !m_key6_pressed) {
            event.flags |= Is_Press;
            if (m_num_lock_on) {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedKeyCodes[packet_raw.keypress6].key_code;
                    event.map_entry_index = ShiftedKeyCodes[packet_raw.keypress6].map_entry_index;
                    if (packet_raw.keypress6 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = KeyCodes[packet_raw.keypress6].key_code;
                    event.map_entry_index = KeyCodes[packet_raw.keypress6].map_entry_index;
                    if (packet_raw.keypress6 == 0x54)
                        event.code_point = '/';
                }
            } else {
                if (m_modifiers & Mod_Shift) {
                    event.key = ShiftedNumKeyCodes[packet_raw.keypress6].key_code;
                    event.map_entry_index = ShiftedNumKeyCodes[packet_raw.keypress6].map_entry_index;
                    if (packet_raw.keypress6 == 0x54)
                        event.code_point = '/';
                } else {
                    event.key = NumKeyCodes[packet_raw.keypress6].key_code;
                    event.map_entry_index = NumKeyCodes[packet_raw.keypress6].map_entry_index;
                    if (packet_raw.keypress6 == 0x54)
                        event.code_point = '/';
                }
            }
            m_key6_pressed = true;
            m_last_event = event;
            handle_input_event(event);
        }

        if (!packet_raw.keypress1)
            m_key1_pressed = false;
        if (!packet_raw.keypress2)
            m_key2_pressed = false;
        if (!packet_raw.keypress3)
            m_key3_pressed = false;
        if (!packet_raw.keypress4)
            m_key4_pressed = false;
        if (!packet_raw.keypress5)
            m_key5_pressed = false;
        if (!packet_raw.keypress6)
            m_key6_pressed = false;

        m_current_num_pressed_keys = m_key1_pressed + m_key2_pressed + m_key3_pressed + m_key4_pressed + m_key5_pressed + m_key6_pressed;
        if (m_last_num_pressed_keys == m_current_num_pressed_keys && m_current_num_pressed_keys != 0 && m_last_event.code_point != '/') // FIXME: Have a better handling of the numpad slash
            handle_input_event(m_last_event);
        m_last_num_pressed_keys = m_current_num_pressed_keys;
    }));
    return {};
}

USBKeyboardDevice::USBKeyboardDevice(USB::Device const& usb_device, NonnullOwnPtr<USB::InterruptInPipe> pipe)
    : KeyboardDevice()
    , m_interrupt_in_pipe(move(pipe))
    , m_attached_usb_device(usb_device)
{
}

}
