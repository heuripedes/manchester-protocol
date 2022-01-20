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

class TuiImpl {
public:
  TuiImpl() = default;
  TuiImpl(const TuiImpl &) = delete;
  TuiImpl(TuiImpl &&other) = delete;
  TuiImpl &operator=(const TuiImpl &) = delete;
  TuiImpl &operator=(TuiImpl &&other) = delete;
  virtual ~TuiImpl() = default;

  virtual TuiCmd readCommand() = 0;
  virtual void clear() = 0;
  virtual void flip() = 0;
  virtual void print(const std::string &str) = 0;
};

class Tui {
public:
  Tui();
  Tui(const Tui &) = delete;
  Tui(Tui &&other);
  Tui &operator=(const Tui &) = delete;
  Tui &operator=(Tui &&other);

  bool isOpen() const { return _open; }
  void setOpen(bool open) { _open = open; }

  TuiCmd readCommand();
  void render(unsigned tick);

  ~Tui();

private:
  std::unique_ptr<TuiImpl> impl;
  bool _open;
};
