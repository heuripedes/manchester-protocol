#include "Tui.h"
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

private:
};

Tui::Tui() : impl{new NcursesTuiImpl()} {}

Tui::Tui(Tui &&other) : impl{std::move(other.impl)} {}

Tui &Tui::operator=(Tui &&other) {
  impl = std::move(other.impl);
  return *this;
}

Tui::~Tui() {}

TuiCmd Tui::readCommand() { return impl->readCommand(); }

void Tui::render(unsigned tick) {
  impl->clear();
  impl->print(fmt::format("Tick {}\n", tick));
  impl->flip();
}