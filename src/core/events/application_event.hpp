#pragma once

#include "../event.hpp"
#include <sstream>

namespace dvm
{
class WindowCloseEvent : public Event
{
public:
  WindowCloseEvent() = default;

  EVENT_CLASS_GETTERS(WindowClose, EventCategoryApplication)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "WindowCloseEvent";
    return ss.str();
  }
};

class WindowResizeEvent : public Event
{
public:
  WindowResizeEvent(uint32_t width, uint32_t height)
      : m_width(width), m_height(height)
  {
  }

  uint32_t getWidth() const { return m_width; }
  uint32_t getHeight() const { return m_height; }

  EVENT_CLASS_GETTERS(WindowResize, EventCategoryApplication)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "WindowResizeEvent: " << m_width << "x" << m_height;
    return ss.str();
  }

private:
  uint32_t m_width;
  uint32_t m_height;
};

class AppTickEvent : public Event
{
public:
  AppTickEvent(float deltaTime)
      : m_deltaTime(deltaTime)
  {
  }

  float getDeltaTime() const { return m_deltaTime; }

  EVENT_CLASS_GETTERS(AppTick, EventCategoryApplication)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "AppTickEvent: " << m_deltaTime << "s";
    return ss.str();
  }

private:
  float m_deltaTime;
};

class WindowFocusEvent : public Event
{
public:
  WindowFocusEvent() = default;

  EVENT_CLASS_GETTERS(WindowFocus, EventCategoryApplication)

  std::string toString() const override { return "WindowFocusEvent"; }
};

class WindowLostFocusEvent : public Event
{
public:
  WindowLostFocusEvent() = default;

  EVENT_CLASS_GETTERS(WindowLostFocus, EventCategoryApplication)

  std::string toString() const override { return "WindowLostFocusEvent"; }
};

}  // namespace dvm
