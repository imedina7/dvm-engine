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
  virtual bool isVisible() { return false; };
  const std::string& getTitle() const { return title; };
  //   const std::string getTitle() const { return title; };
protected:
  std::string title;
};
}  // namespace dvm::gui