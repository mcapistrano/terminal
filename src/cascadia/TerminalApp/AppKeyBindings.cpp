﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "AppKeyBindings.h"
#include "KeyChordSerialization.h"

#include "AppKeyBindings.g.cpp"

using namespace winrt::Microsoft::Terminal;
using namespace winrt::TerminalApp;
using namespace winrt::Microsoft::Terminal::TerminalControl;

namespace winrt::TerminalApp::implementation
{
    void AppKeyBindings::SetKeyBinding(const TerminalApp::ActionAndArgs& actionAndArgs,
                                       const KeyChord& chord)
    {
        _keyShortcuts[chord] = actionAndArgs;
    }

    // Method Description:
    // - Remove the action that's bound to a particular KeyChord.
    // Arguments:
    // - chord: the keystroke to remove the action for.
    // Return Value:
    // - <none>
    void AppKeyBindings::ClearKeyBinding(const KeyChord& chord)
    {
        _keyShortcuts.erase(chord);
    }

    KeyChord AppKeyBindings::GetKeyBindingForAction(TerminalApp::ShortcutAction const& action)
    {
        for (auto& kv : _keyShortcuts)
        {
            if (kv.second.Action() == action)
            {
                return kv.first;
            }
        }
        return { nullptr };
    }

    // Method Description:
    // - Lookup the keychord bound to a particular combination of ShortcutAction
    //   and IActionArgs. This enables searching no only for the binding of a
    //   particular ShortcutAction, but also a particular set of values for
    //   arguments to that action.
    // Arguments:
    // - actionAndArgs: The ActionAndArgs to lookup the keybinding for.
    // Return Value:
    // - The bound keychord, if this ActionAndArgs is bound to a key, otherwise nullptr.
    KeyChord AppKeyBindings::GetKeyBindingForActionWithArgs(TerminalApp::ActionAndArgs const& actionAndArgs)
    {
        for (auto& kv : _keyShortcuts)
        {
            const auto action = kv.second.Action();
            const auto args = kv.second.Args();
            const auto actionMatched = action == actionAndArgs.Action();
            const auto argsMatched = args ? args.Equals(actionAndArgs.Args()) : args == actionAndArgs.Args();
            if (actionMatched && argsMatched)
            {
                return kv.first;
            }
        }
        return { nullptr };
    }

    bool AppKeyBindings::TryKeyChord(const KeyChord& kc)
    {
        const auto keyIter = _keyShortcuts.find(kc);
        if (keyIter != _keyShortcuts.end())
        {
            const auto actionAndArgs = keyIter->second;
            return _dispatch.DoAction(actionAndArgs);
        }
        return false;
    }

    void AppKeyBindings::SetDispatch(const winrt::TerminalApp::ShortcutActionDispatch& dispatch)
    {
        _dispatch = dispatch;
    }

    // Method Description:
    // - Takes the KeyModifier flags from Terminal and maps them to the WinRT types which are used by XAML
    // Return Value:
    // - a Windows::System::VirtualKeyModifiers object with the flags of which modifiers used.
    Windows::System::VirtualKeyModifiers AppKeyBindings::ConvertVKModifiers(KeyModifiers modifiers)
    {
        Windows::System::VirtualKeyModifiers keyModifiers = Windows::System::VirtualKeyModifiers::None;

        if (WI_IsFlagSet(modifiers, KeyModifiers::Ctrl))
        {
            keyModifiers |= Windows::System::VirtualKeyModifiers::Control;
        }
        if (WI_IsFlagSet(modifiers, KeyModifiers::Shift))
        {
            keyModifiers |= Windows::System::VirtualKeyModifiers::Shift;
        }
        if (WI_IsFlagSet(modifiers, KeyModifiers::Alt))
        {
            // note: Menu is the Alt VK_MENU
            keyModifiers |= Windows::System::VirtualKeyModifiers::Menu;
        }

        return keyModifiers;
    }
}
