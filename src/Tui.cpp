#include "Tui.h"
#include "Simulator.h"
#include "fmt/core.h"
#include <ncurses.h>

class NcursesTui : public Tui {
public:
  NcursesTui();
  NcursesTui(const NcursesTui &) = delete;
  NcursesTui(NcursesTui &&other) = delete;
  NcursesTui &operator=(const NcursesTui &) = delete;
  NcursesTui &operator=(NcursesTui &&other) = delete;

  ~NcursesTui() override;

  TuiCmd readCommand() override;
  void render(Simulator &sim) override;

private:
  void _drawPatient(const Patient &p);
};

NcursesTui::NcursesTui() {
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

NcursesTui::~NcursesTui() { endwin(); }

TuiCmd NcursesTui::readCommand() {
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

void NcursesTui::render(Simulator &sim) {
  clear();

  int width, height;
  getmaxyx(stdscr, height, width);

  printw("Tick %d\n", sim.ticks());
  printw("Last event: %s", sim.lastEvent().c_str());

  height -= 5; // 3 + 2 header lines

  move(3, 0);
  attron(COLOR_PAIR(int(Color::white)));
  printw(" Undergoing treatment: %d ", sim.patientsWithDoctors().size());

  move(4, 0);
  printw(" %-6s %-18s \n", "#ID", "Name");
  attroff(COLOR_PAIR(int(Color::white)));

  auto &withDoctor = sim.patientsWithDoctors();
  for (int i = 0; i < height && (unsigned)i < withDoctor.size(); ++i) {
    move(getcury(stdscr), 0);
    _drawPatient(withDoctor[i].second);
  }

  for (int i = withDoctor.size(); i < sim.numDoctors(); ++i) {
    move(getcury(stdscr), 0);
    printw("-- idle --");
  }

  move(3, 28);
  attron(COLOR_PAIR(int(Color::white)));
  printw(" Waiting: %d \n", sim.waitingPatients().size());
  move(4, 28);
  printw(" %-6s %-18s \n", "#ID", "Name");
  attroff(COLOR_PAIR(int(Color::white)));

  auto &waiting = sim.waitingPatients();
  for (int i = 0; i < height && (unsigned)i < waiting.size(); ++i) {
    move(getcury(stdscr), 28);
    _drawPatient(waiting[i]);
  }

  refresh();
}

void NcursesTui::_drawPatient(const Patient &p) {
  std::string tag_string;

  attron(COLOR_PAIR(int(Color(int(p.tag()) + 2))));
  // printw(" #%05d %05d %s\n", p.id(), p.ticks(), p.name().c_str());
  printw(" #%05d %-18s \n", p.id(), p.name().c_str());
  attroff(COLOR_PAIR(int(Color(int(p.tag()) + 2))));
}

Tui::Tui() : _open(true) {}

Tui::~Tui() {}

std::unique_ptr<Tui> Tui::createInstance() {
  return std::make_unique<NcursesTui>();
}
