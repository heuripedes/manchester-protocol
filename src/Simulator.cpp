#include "Simulator.h"
#include <algorithm>
#include <iostream>
using namespace std::chrono_literals;

Simulator::Simulator(int seed) : _rng(seed), _ticks(0), _patientCh(2) {
  _patGen = std::async(&Simulator::_patientGenerator, this);
}

Simulator::~Simulator() {
  _quitEvent.set();
  _patGen.wait();
  auto generated_patients = _patGen.get();

  std::cout << "Generated patients: " << generated_patients << "\n";
}

void Simulator::advance() {
  _ticks++;
  if (_ticks % 2 == 0)
    generatePatientWave();

  // clear patients
  auto to_clear = std::remove_if(
      _patientsWithDoctor.begin(), _patientsWithDoctor.end(),
      [&](const std::pair<int, Patient> &p) { return p.first < _ticks; });
  _patientsWithDoctor.erase(to_clear, _patientsWithDoctor.end());

  while (_patientsWithDoctor.size() < (unsigned)_numDoctors &&
         !_waitingPatients.empty()) {
    auto it = _waitingPatients.begin();
    auto p = std::move(*it);

    _patientsWithDoctor.emplace_back(_ticks + (_rng() % (int(p.tag()) + 2)),
                                     std::move(p));
    _waitingPatients.erase(it);
  }
}

void Simulator::generatePatientWave() {
  auto wave = _patientCh.receive();

  for (auto &p : wave) {
    p.setArrivalTick(_ticks);
    _waitingPatients.emplace_back(std::move(p));
  }

  auto cur_tick = _ticks;
  std::sort(_waitingPatients.begin(), _waitingPatients.end(),
            [cur_tick](const Patient &a, const Patient &b) {
              if (a.tag() > b.tag())
                return true;

              return a.tag() == b.tag() &&
                     a.remainingTime() < b.remainingTime();
            });
}

int Simulator::_patientGenerator() {
  std::uniform_int_distribution<int> tag_dist(int(PatientTag::blue),
                                              int(PatientTag::red));

  int generated = 0;

  while (!_quitEvent.isSet()) {
    std::vector<Patient> wave;

    // lock needed because we change _rng and read numDoctors
    std::unique_lock lock(_mutex);

    std::uniform_int_distribution<int> amount_dist(0, _numDoctors * 1.5);

    int amount = amount_dist(_rng);
    for (int i = 0; i < amount; ++i) {
      wave.emplace_back(Patient(this, static_cast<PatientTag>(tag_dist(_rng)),
                                generateName(_rng)));
    }

    lock.unlock();

    generated += amount;
    _patientCh.send(std::move(wave), 1000ms);
    std::this_thread::sleep_for(10ms);
  }

  return generated;
}