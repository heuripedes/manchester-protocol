#include "Tui.h"
#include "Simulator.h"
#include "fmt/core.h"
#include <ncurses.h>

class NcursesTuiImpl : public TuiImpl {
public:
  NcursesTuiImpl() {
    initscr();
    raw();
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

  impl->goTo(width / 2, -1);
  impl->color(Color::white);
  impl->print(fmt::format("Waiting: {}\n", sim.waitingPatients().size()));

  auto &waiting = sim.waitingPatients();
  for (int i = 0; i < 10 && i < waiting.size(); ++i) {
    impl->goTo(width / 2, -1);

    std::string tag_string;

    switch (waiting[i].tag()) {
    case PatientTag::blue:
      tag_string = "blue  ";
      break;
    case PatientTag::green:
      tag_string = "green ";
      break;
    case PatientTag::yellow:
      tag_string = "yellow";
      break;
    case PatientTag::orange:
      tag_string = "orange";
      break;
    case PatientTag::red:
      tag_string = "red   ";
      break;
    }

    impl->color(Color(int(waiting[i].tag()) + 2));
    impl->print(fmt::format("    {} {} {}\n", tag_string,
                            waiting[i].remainingTime(), int(waiting[i].tag())));
  }
  impl->flip();
}