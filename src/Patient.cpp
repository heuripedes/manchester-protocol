#include "Patient.h"
#include "Simulator.h"

int Patient::_next_patient_id = 0;

Patient::Patient(const Simulator *sim, PatientTag tag, std::string name)
    : _id{++_next_patient_id}, _alive{true}, _arrivalTick{0},
      _sim(sim), _name{std::move(name)}, _tag{tag} {}

Patient::Patient(Patient &&other)
    : _id{std::move(other._id)}, _alive{std::move(other._alive)},
      _arrivalTick{std::move(other._arrivalTick)}, _sim{std::move(other._sim)},
      _name{std::move(other._name)}, _tag{std::move(other._tag)} {
  other._id = 0;
}

Patient &Patient::operator=(Patient &&other) {
  _id = std::move(other._id);
  _alive = std::move(other._alive);
  _arrivalTick = std::move(other._arrivalTick);
  _sim = std::move(other._sim);
  _tag = std::move(other._tag);
  _name = std::move(other._name);

  other._id = 0;

  return *this;
}

Patient::~Patient() {
  _id = 0;
  _sim = nullptr;
}

int Patient::waitTime() const { return _sim->ticks() - _arrivalTick; }
int Patient::remainingTime() const {
  auto elapsed = waitTime();
  switch (_tag) {
  case PatientTag::blue:
    return 240 - elapsed;
  case PatientTag::green:
    return 120 - elapsed;
  case PatientTag::yellow:
    return 60 - elapsed;
  case PatientTag::orange:
    return 10 - elapsed;
  case PatientTag::red:
    return 1 - elapsed;
  default:
    throw std::runtime_error("this shouldn't have happened");
  }
}