#pragma once

#include "../event.hpp"
#include <glm/glm.hpp>
#include <sstream>

namespace dvm
{
class MouseMovedEvent : public Event
{
public:
  MouseMovedEvent(double x, double y, double deltaX, double deltaY)
      : m_mouseX(x), m_mouseY(y), m_deltaX(deltaX), m_deltaY(deltaY)
  {
  }

  double getX() const { return m_mouseX; }
  double getY() const { return m_mouseY; }
  double getDeltaX() const { return m_deltaX; }
  double getDeltaY() const { return m_deltaY; }

  glm::vec2 getPosition() const { return glm::vec2(m_mouseX, m_mouseY); }
  glm::vec2 getDelta() const { return glm::vec2(m_deltaX, m_deltaY); }

  EVENT_CLASS_GETTERS(MouseMoved, EventCategoryMouse | EventCategoryInput)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY
       << " (delta: " << m_deltaX << ", " << m_deltaY << ")";
    return ss.str();
  }

private:
  double m_mouseX;
  double m_mouseY;
  double m_deltaX;
  double m_deltaY;
};

class MouseScrolledEvent : public Event
{
public:
  MouseScrolledEvent(double xOffset, double yOffset)
      : m_xOffset(xOffset), m_yOffset(yOffset)
  {
  }

  double getXOffset() const { return m_xOffset; }
  double getYOffset() const { return m_yOffset; }

  EVENT_CLASS_GETTERS(MouseScrolled, EventCategoryMouse | EventCategoryInput)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "MouseScrolledEvent: " << m_xOffset << ", " << m_yOffset;
    return ss.str();
  }

private:
  double m_xOffset;
  double m_yOffset;
};

class MouseButtonPressedEvent : public Event
{
public:
  MouseButtonPressedEvent(int button)
      : m_button(button)
  {
  }

  int getButton() const { return m_button; }

  EVENT_CLASS_GETTERS(MouseButtonPressed,
                      EventCategoryMouseButton | EventCategoryInput)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << m_button;
    return ss.str();
  }

private:
  int m_button;
};

class MouseButtonReleasedEvent : public Event
{
public:
  MouseButtonReleasedEvent(int button)
      : m_button(button)
  {
  }

  int getButton() const { return m_button; }

  EVENT_CLASS_GETTERS(MouseButtonReleased,
                      EventCategoryMouseButton | EventCategoryInput)

  std::string toString() const override
  {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << m_button;
    return ss.str();
  }

private:
  int m_button;
};

}  // namespace dvm
