#pragma once
#include "Patient.h"
#include "ThreadUtil.h"

#include <future>
#include <random>

using SimulatorClockType = std::chrono::high_resolution_clock;

class Simulator {
public:
  explicit Simulator(int seed);
  ~Simulator();

  void advance();
  int ticks() const { return _ticks; }

  void generatePatientWave();
  const std::vector<Patient> &waitingPatients() const {
    return _waitingPatients;
  }

private:
  std::mutex _mutex;
  std::mt19937 _rng;

  std::vector<Patient> _waitingPatients;
  std::future<int> _patGen; // _patientGenerator promise
  int _ticks;
  Event _quitEvent;
  Channel<std::vector<Patient>> _patientCh;

  int _patientGenerator();
};