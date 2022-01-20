#include "Simulator.h"
#include "Tui.h"
#include <signal.h>

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

static volatile int gotSignalToQuit = 0;

void signalToQuitHandler(int dummy) { gotSignalToQuit = 1; }

int main() {
  auto tick_delay = 500ms;

  signal(SIGINT, signalToQuitHandler);

  Tui tui;
  Simulator sim(0);
  while (tui.isOpen() && !gotSignalToQuit) {
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

    sim.advance();
    tui.render(sim);

    std::this_thread::sleep_for(tick_delay);
  }
  return 0;
}