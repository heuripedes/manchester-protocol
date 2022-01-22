#pragma once
#include "Patient.h"
#include "Util.h"

#include <fstream>
#include <future>
#include <random>

using SimulatorClockType = std::chrono::high_resolution_clock;

class Simulator {
public:
  explicit Simulator(int seed);
  ~Simulator();

  Simulator(const Simulator &) = delete;
  Simulator(const Simulator &&) = delete;
  Simulator &operator=(const Simulator &) = delete;
  Simulator &operator=(const Simulator &&) = delete;

  void advance();
  int ticks() const { return _ticks; }

  void setNumDoctors(int doctors) { _numDoctors = doctors; }
  int numDoctors() const { return _numDoctors; }

  void generatePatientWave();
  const std::vector<Patient> &waitingPatients() const {
    return _waitingPatients;
  }

  const std::vector<std::pair<int, Patient>> &patientsWithDoctors() const {
    return _patientsWithDoctor;
  }

  std::string lastEvent() const { return _lastEvent; }

private:
  int _numDoctors = 5;
  int _deadPatients = 0;
  int _patientsThatLeft = 0;

  std::mutex _mutex;
  std::mt19937 _rng;
  std::ofstream _log;
  std::string _lastEvent;

  // pair is (ticks remaining, patient)
  std::vector<std::pair<int, Patient>> _patientsWithDoctor;
  std::vector<Patient> _waitingPatients;
  std::future<int> _patGen; // _patientGenerator promise
  Event _quitEvent;         // used to tell _patientGenerator to stop

  int _ticks;
  Channel<std::vector<Patient>> _patientCh;

  // this function runs on a separate thread
  int _patientGenerator();

  // helper functions
  void _updateTriagedPatients();
  void _updatePatientCare();

  void _logDeath(const Patient &p, bool waiting);
};