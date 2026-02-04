#pragma once

#include "../event.hpp"
#include <sstream>

namespace dvm
{
class KeyPressedEvent : public Event
{
public:
  KeyPressedEvent(int keyCode, bool isRepeat = false)
      : m_keyCode(keyCode), m_isRepeat(isRepeat)
  {
  }

  int getKeyCode() const { return m_keyCode; }
  bool isRepeat() const { return m_isRepeat; }

  EVENT_CLASS_GETTERS(KeyPressed, EventCategoryKeyboard | EventCategoryInput)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << m_keyCode << " (repeat = " << m_isRepeat
       << ")";
    return ss.str();
  }

private:
  int m_keyCode;
  bool m_isRepeat;
};

class KeyReleasedEvent : public Event
{
public:
  KeyReleasedEvent(int keyCode)
      : m_keyCode(keyCode)
  {
  }

  int getKeyCode() const { return m_keyCode; }

  EVENT_CLASS_GETTERS(KeyReleased, EventCategoryKeyboard | EventCategoryInput)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << m_keyCode;
    return ss.str();
  }

private:
  int m_keyCode;
};

class KeyTypedEvent : public Event
{
public:
  KeyTypedEvent(int keyCode)
      : m_keyCode(keyCode)
  {
  }

  int getKeyCode() const { return m_keyCode; }

  EVENT_CLASS_GETTERS(KeyTyped, EventCategoryKeyboard | EventCategoryInput)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "KeyTypedEvent: " << m_keyCode;
    return ss.str();
  }

private:
  int m_keyCode;
};

}  // namespace dvm
