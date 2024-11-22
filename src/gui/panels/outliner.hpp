#pragma once
#include <string>
#include <iostream>
#include "panel.hpp"
#include <imgui.h>

namespace dvm::gui
{
struct OutlinerState
{
  bool isVisible = false;
};
class Outliner : public Panel
{
public:
  Outliner(const OutlinerState& outlinerState)
      : state(outlinerState)
      , Panel {"Outliner"}
  {
  }
  virtual void draw() override;
  virtual bool isVisible() override;

private:
  OutlinerState state;
};
}  // namespace dvm::gui