#pragma once
#include <string>
namespace dvm::gui
{
class Panel
{
public:
  Panel(const std::string& _title)
      : title {_title} {};

  virtual ~Panel() = default;
  virtual void draw() {};
  virtual bool isVisible() { return visibility; };

  void show() { visibility = true; };
  void hide() { visibility = false; };
  bool toggle() { return visibility = !visibility; };

  const std::string& getTitle() const { return title; };

protected:
  std::string title;
  bool visibility = true;
};
}  // namespace dvm::gui