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

  Simulator sim(0);
  auto tui = Tui::createInstance();

  signal(SIGINT, signalToQuitHandler);

  while (tui->isOpen() && !gotSignalToQuit) {
    switch (tui->readCommand()) {
    case TuiCmd::quit:
      tui->setOpen(false);
      break;
    case TuiCmd::incSpeed:
      tick_delay -= 50ms;
      break;
    case TuiCmd::decSpeed:
      tick_delay += 50ms;
      break;
    case TuiCmd::incDoctors:
      if (sim.numDoctors() < 20)
        sim.setNumDoctors(sim.numDoctors() + 1);
      break;
    case TuiCmd::decDoctors:
      if (sim.numDoctors() > 0)
        sim.setNumDoctors(sim.numDoctors() - 1);
      break;
    case TuiCmd::addPatients:
      sim.generatePatientWave();
      break;
    default:
      break;
    }

    if (tick_delay < 50ms)
      tick_delay = 50ms;
    else if (tick_delay > 1000ms)
      tick_delay = 1000ms;

    sim.advance();
    tui->render(sim);

    std::this_thread::sleep_for(tick_delay);
  }
  return 0;
}