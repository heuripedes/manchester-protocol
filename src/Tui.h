#pragma once

#include <memory>

enum class TuiCmd {
  unknown,
  quit,
  addPatients,
  incDoctors,
  decDoctors,
  incSpeed,
  decSpeed,
};

enum class Color { black = 0, white, blue, green, yellow, orange, red, dim };

class Simulator;
class Patient;

class Tui {
public:
  Tui(const Tui &) = delete;
  Tui(Tui &&) = delete;
  Tui &operator=(const Tui &) = delete;
  Tui &operator=(Tui &&) = delete;

  bool isOpen() const { return _open; }
  void setOpen(bool open) { _open = open; }

  virtual TuiCmd readCommand() = 0;
  virtual void render(Simulator &sim) = 0;

  virtual ~Tui();

  static std::unique_ptr<Tui> createInstance();

protected:
  bool _open;
  Tui();
};
