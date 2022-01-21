#include "Tui.h"
#include "Simulator.h"
#include "fmt/core.h"
#include <ncurses.h>

class NcursesTuiImpl : public TuiImpl {
public:
  NcursesTuiImpl() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    // curs_set(false);

    start_color();

    init_pair(int(Color::black), COLOR_WHITE, COLOR_BLACK);
    init_pair(int(Color::white), COLOR_BLACK, COLOR_WHITE);
    init_pair(int(Color::blue), COLOR_BLACK, COLOR_BLUE);
    init_pair(int(Color::green), COLOR_BLACK, COLOR_GREEN);
    init_pair(int(Color::yellow), COLOR_BLACK, COLOR_YELLOW);
    init_pair(int(Color::orange), COLOR_BLACK, COLOR_MAGENTA);
    init_pair(int(Color::red), COLOR_BLACK, COLOR_RED);
  }

  NcursesTuiImpl(const NcursesTuiImpl &) = delete;
  NcursesTuiImpl(NcursesTuiImpl &&other) = delete;
  NcursesTuiImpl &operator=(const NcursesTuiImpl &) = delete;
  NcursesTuiImpl &operator=(NcursesTuiImpl &&other) = delete;

  ~NcursesTuiImpl() override { endwin(); }

  TuiCmd readCommand() override {
    switch (getch()) {
    case KEY_LEFT:
      return TuiCmd::decSpeed;
    case KEY_RIGHT:
      return TuiCmd::incSpeed;
    case KEY_UP:
      return TuiCmd::incDoctors;
    case KEY_DOWN:
      return TuiCmd::decDoctors;
    case 'q':
      return TuiCmd::quit;
    default:
      return TuiCmd::unknown;
    }
  }

  void clear() override { ::clear(); }

  void flip() override { refresh(); }

  void print(const std::string &str) override { printw("%s", str.c_str()); }

  std::tuple<int, int> geometry() override {
    int w, h;
    getmaxyx(stdscr, h, w);

    return {w, h};
  }

  void goTo(int x, int y) override {
    if (x < 0)
      x = getcurx(stdscr);
    if (y < 0)
      y = getcury(stdscr);

    move(y, x);
  }

  void color(Color color) override { attron(COLOR_PAIR(int(color))); }

private:
};

// This class implements a very basic Tui that just prints things on the screen
// and has no interactivity. It exists for debug purposes
class SimpleTuiImpl : public TuiImpl {
public:
  SimpleTuiImpl() {}

  SimpleTuiImpl(const SimpleTuiImpl &) = delete;
  SimpleTuiImpl(SimpleTuiImpl &&other) = delete;
  SimpleTuiImpl &operator=(const SimpleTuiImpl &) = delete;
  SimpleTuiImpl &operator=(SimpleTuiImpl &&other) = delete;

  ~SimpleTuiImpl() override { endwin(); }

  TuiCmd readCommand() override { return TuiCmd::unknown; }

  void clear() override {}

  void flip() override {}

  void print(const std::string &str) override { printf("%s", str.c_str()); }

  std::tuple<int, int> geometry() override { return {80, 25}; }
  void goTo(int x, int y) override {}

  void color(Color color) override {}

private:
};

Tui::Tui() : impl{new NcursesTuiImpl()}, _open(true) {}

Tui::Tui(Tui &&other) : impl{std::move(other.impl)} {}

Tui &Tui::operator=(Tui &&other) {
  impl = std::move(other.impl);
  return *this;
}

Tui::~Tui() {}

TuiCmd Tui::readCommand() { return impl->readCommand(); }

void Tui::render(Simulator &sim) {
  impl->resetColor();
  impl->clear();

  int width, height;
  std::tie(width, height) = impl->geometry();

  impl->print(fmt::format("Tick {}\n", sim.ticks()));

  impl->goTo(0, 2);
  impl->color(Color::white);
  impl->print(
      fmt::format(" With a doctor: {} \n", sim.patientsWithDoctors().size()));
  impl->goTo(0, 3);
  impl->print(fmt::format(" #ID     Time\n"));

  auto &withDoctor = sim.patientsWithDoctors();
  for (int i = 0; i < 10 && (unsigned)i < withDoctor.size(); ++i) {
    impl->goTo(0, -1);
    _drawPatient(withDoctor[i].second);
  }

  impl->goTo(width / 2, 2);
  impl->color(Color::white);
  impl->print(fmt::format(" Waiting: {} \n", sim.waitingPatients().size()));
  impl->goTo(width / 2, 3);
  impl->print(fmt::format(" #ID     Time\n"));

  auto &waiting = sim.waitingPatients();
  for (int i = 0; i < 10 && (unsigned)i < waiting.size(); ++i) {
    impl->goTo(width / 2, -1);
    _drawPatient(waiting[i]);
  }
  impl->flip();
}

void Tui::_drawPatient(const Patient &p) {
  std::string tag_string;

  impl->color(Color(int(p.tag()) + 2));
  impl->print(
      fmt::format(" #{:05} {:5} {}\n", p.id(), p.remainingTime(), p.name()));
}