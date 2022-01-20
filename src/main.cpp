#include "Tui.h"

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

int main() {
  Tui tui;
  unsigned ticks = 0;
  auto tick_delay = 500ms;

  while (tui.isOpen()) {
    switch (tui.readCommand()) {
    case TuiCmd::quit:
      tui.setOpen(false);
      break;
    case TuiCmd::incSpeed:
      tick_delay -= 50ms;
      break;
    case TuiCmd::decSpeed:
      tick_delay += 50ms;
      break;
    default:
      break;
    }

    if (tick_delay < 50ms)
      tick_delay = 50ms;
    else if (tick_delay > 1000ms)
      tick_delay = 1000ms;

    ticks++;

    tui.render(ticks);

    std::this_thread::sleep_for(tick_delay);
  }
  return 0;
}