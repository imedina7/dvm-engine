#pragma once

#include "core.hpp"
#include <functional>
#include <string>

namespace dvm
{
enum EventCategory
{
  None = 0,
  EventCategoryApplication = BIT(0),
  EventCategoryInput = BIT(1),
  EventCategoryKeyboard = BIT(2),
  EventCategoryMouse = BIT(3),
  EventCategoryMouseButton = BIT(4),
  EventCategoryGamepadButton = BIT(5),
  EventCategoryGamepadAnalog = BIT(6)
};
enum class EventType
{
  None = 0,
  WindowClose,
  WindowResize,
  WindowFocus,
  WindowLostFocus,
  WindowMoved,
  AppTick,
  AppUpdate,
  AppRender,
  KeyPressed,
  KeyReleased,
  KeyTyped,
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseScrolled,
  GamepadAnalogChanged,
  GamepadButtonPressed,
  GamepadButtonReleased
};

#define EVENT_CLASS_GETTERS(type, category) \
  static EventType getStaticType() \
  { \
    return EventType::type; \
  } \
  virtual EventType getEventType() const override \
  { \
    return getStaticType(); \
  } \
  virtual const char* getName() const override \
  { \
    return #type; \
  } \
  virtual int getCategories() const override \
  { \
    return category; \
  }

class Event
{
public:
  virtual ~Event() = default;

  bool isHandled = false;

  virtual EventType getEventType() const = 0;
  virtual const char* getName() const = 0;
  virtual int getCategories() const = 0;
  virtual std::string toString() const { return getName(); }

  bool isInCategory(EventCategory category)
  {
    return getCategories() & category;
  }
};

class EventDispatcher
{
public:
  EventDispatcher(Event& event)
      : m_Event(event)
  {
  }

  template<typename T, typename F>
  bool dispatch(const F& func)
  {
    if (m_Event.getEventType() == T::getStaticType()) {
      m_Event.isHandled |= func(static_cast<T&>(m_Event));
      return true;
    }
    return false;
  }

private:
  Event& m_Event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
  return os << e.toString();
}

}  // namespace dvm
