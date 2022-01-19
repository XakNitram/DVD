#pragma once

#include "pch.hpp"

struct KeyboardEvent { int key, scancode, mods; };
struct TextEvent { unsigned int codepoint; };
struct MouseMotionEvent { double xpos, ypos; };
struct MouseButtonEvent { int button, mods; };

class UserEvent {};  // Subclass this to create user-defined events.

using AnonymousEvent = \
std::variant<
    KeyboardEvent,
    TextEvent,
    MouseMotionEvent,
    MouseButtonEvent,
    UserEvent *
>;


struct Event {
public:
    enum class Type {
        // KeyboardEvent
        KeyPress,
        KeyRelease,
        KeyRepeat,

        // TextEvent
        TextInput,

        // MouseMotionEvent
        MouseMotion,

        // MouseButtonEvent
        MouseDown,
        MouseUp,

        // UserEvent
        UserEvent
    };

    Type type;
    AnonymousEvent event;

    Event(Type type, AnonymousEvent event);
};
