#pragma once
//------------------------------------------------------------------------------
/**
    @class Input::Key

    Define standard key codes. Note that these are NOT localized character
    codes, but are similar to Windows virtual keys.

    @copyright
    (C) 2006 Radon Labs GmbH
    (C) 2013-2022 Individual contributors, see AUTHORS file
*/

//------------------------------------------------------------------------------
namespace Input
{

class Key
{
public:
    /// key codes
    enum Code
    {
        Back,
        Tab,
        Clear,
        Return,
        Shift,
        Control,
        Alt,
        Menu,
        Pause,
        Capital,
        Escape,
        Convert,
        NonConvert,
        Accept,
        ModeChange,
        Space,
        Prior,
        Next,
        End,
        Home,
        PageUp,
        PageDown,
        Left,
        Right,
        Up,
        Down,
        Select,
        Print,
        Execute,
        Snapshot,
        Insert,
        Delete,
        Help,
        LeftWindows,
        RightWindows,
        Apps,
        Sleep,
        NumPad0,
        NumPad1,
        NumPad2,
        NumPad3,
        NumPad4,
        NumPad5,
        NumPad6,
        NumPad7,
        NumPad8,
        NumPad9,
        NumPadEnter,
        Multiply,
        Add,
        Subtract,
        Separator,
        Decimal,
        Divide,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        NumLock,
        Scroll,
        Semicolon,
        Slash,
        Tilde,
        LeftBracket,
        RightBracket,
        BackSlash,
        Quote,
        Comma,
        Underbar,
        Period,
        Equality,
        LeftShift,
        RightShift,
        LeftControl,
        RightControl,
        LeftAlt,
        RightAlt,
        LeftMenu,
        RightMenu,
        BrowserBack,
        BrowserForward,
        BrowserRefresh,
        BrowserStop,
        BrowserSearch,
        BrowserFavorites,
        BrowserHome,
        VolumeMute,
        VolumeDown,
        VolumeUp,
        MediaNextTrack,
        MediaPrevTrack,
        MediaStop,
        MediaPlayPause,
        LaunchMail,
        LaunchMediaSelect,
        LaunchApp1,
        LaunchApp2,
        Key0,
        Key1,
        Key2,
        Key3,
        Key4,
        Key5,
        Key6,
        Key7,
        Key8,
        Key9,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        NumKeyCodes,
        InvalidKey,
    };

    static Key::Code FromGLFW(int32 key);
};

};
//------------------------------------------------------------------------------
