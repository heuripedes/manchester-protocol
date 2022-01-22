#include "Simulator.h"
#include <algorithm>
#include <iostream>
using namespace std::chrono_literals;

Simulator::Simulator(int seed)
    : _rng(seed), _log("log.txt"), _ticks(0), _patientCh(2) {
  _patGen = std::async(&Simulator::_patientGenerator, this);
}

Simulator::~Simulator() {
  _quitEvent.set();
  _patGen.wait();
  auto generated_patients = _patGen.get();

  std::cout << "Generated patients: " << generated_patients << "\n";
  std::cout << _patientsThatLeft << " patients got tired and left the ER.\n";
  std::cout << _deadPatients << " patients died. See log.txt for details.\n";
}

void Simulator::advance() {

  _ticks++;
  _updateTriagedPatients();

  _updatePatientCare();

  if (_ticks % 2 == 0)
    generatePatientWave();
}

void Simulator::generatePatientWave() {
  auto wave = _patientCh.receive();

  for (auto &p : wave) {
    _waitingPatients.emplace_back(std::move(p));
  }

  auto cur_tick = _ticks;
  std::sort(_waitingPatients.begin(), _waitingPatients.end(),
            [cur_tick](const Patient &a, const Patient &b) {
              if (a.tag() > b.tag())
                return true;

              return a.tag() == b.tag() && a.ticks() < b.ticks();
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

void Simulator::_updateTriagedPatients() {
  for (auto &p : _waitingPatients) {
    p.update();
  }

  // remove patient that died while waiting
  _waitingPatients.erase(
      std::remove_if(_waitingPatients.begin(), _waitingPatients.end(),
                     [&](const Patient &p) {
                       if (p.isDead()) {
                         _logDeath(p, true);
                         _deadPatients++;
                         return true;
                       }

                       if ((p.tag() == PatientTag::blue ||
                            p.tag() == PatientTag::green) &&
                           p.ticks() <= getTagTicks(p.tag()) / 2) {

                         _lastEvent = p.name() + " got tired and left.";
                         _patientsThatLeft++;
                       }
                       return false;
                     }),
      _waitingPatients.end());
}

void Simulator::_updatePatientCare() {
  static std::uniform_int_distribution<int> treatment_time_dist(0, 6);

  // update patients and remaining treatment care
  for (auto &p : _patientsWithDoctor) {
    p.first--;
    p.second.update();
  }

  // remove patients treatment
  _patientsWithDoctor.erase(
      std::remove_if(_patientsWithDoctor.begin(), _patientsWithDoctor.end(),
                     [&](const std::pair<int, Patient> &p) {
                       if (p.first <= 0) {
                         if (p.second.isDead()) {
                           _logDeath(p.second, false);
                           _deadPatients++;
                         }
                         return true;
                       }

                       return false;
                     }),
      _patientsWithDoctor.end());

  // treat patient if we have enough space
  while (_patientsWithDoctor.size() < (unsigned)_numDoctors &&
         !_waitingPatients.empty()) {
    auto it = _waitingPatients.begin();
    auto p = std::move(*it);
    _waitingPatients.erase(it);

    // we're about to change _rng
    std::lock_guard lock(_mutex);
    _patientsWithDoctor.emplace_back(treatment_time_dist(_rng), std::move(p));
  }
}

void Simulator::_logDeath(const Patient &p, bool waiting) {
  static std::uniform_int_distribution<int> dice(1, 4);
  std::lock_guard lock(_mutex);

  _lastEvent = p.name() + " died ";

  if (waiting)
    _lastEvent += "waiting for treatment.";
  else
    _lastEvent += "during treatment.";

  _log << "On tick " << _ticks << ", " << p.name() << " passed away.\n";

  if (waiting) {
    _log << "They died waiting for treatment.\n";
  } else {
    switch (dice(_rng)) {
    case 1:
      _log << "They did not resist the treatment.\n";
      break;
    case 2:
      _log << "The treatment was too hard on their body.\n";
      break;
    case 3:
      _log << "The doctors tried to reanimate them but it was all in vain.\n";
      break;
    case 4:
      _log << "There was a medical error.\n";
      break;
    }
  }

  auto roll = dice(_rng);

  switch (roll) {
  case 1:
    _log << "They went to a better place.\n";
    break;
  case 2:
    _log << "May the Angels receive their soul and present them to God.\n";
    break;
  case 3:
    _log << "They went too soon.\n";
    break;
  case 4:
    _log << "They'll be missed.\n";
  }

  _log << "----\n\n";

  _log.flush();
}