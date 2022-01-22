#include "Patient.h"
#include "Simulator.h"

int Patient::_next_patient_id = 0;

int getTagTicks(PatientTag tag) {
  switch (tag) {
  case PatientTag::blue:
    return 240;
  case PatientTag::green:
    return 120;
  case PatientTag::yellow:
    return 60;
  case PatientTag::orange:
    return 10;
  case PatientTag::red:
    return 1;
  default:
    throw std::runtime_error("this shouldn't have happened");
  }
}

Patient::Patient(const Simulator *sim, PatientTag tag, std::string name)
    : _id{++_next_patient_id}, _ticks{getTagTicks(tag)}, _name{std::move(name)},
      _tag{tag} {}

Patient::Patient(Patient &&other)
    : _id{std::move(other._id)}, _ticks{std::move(other._ticks)},
      _name{std::move(other._name)}, _tag{std::move(other._tag)} {
  other._id = 0;
}

Patient &Patient::operator=(Patient &&other) {
  _id = std::move(other._id);
  _ticks = std::move(other._ticks);
  _tag = std::move(other._tag);
  _name = std::move(other._name);

  other._id = 0;

  return *this;
}

Patient::~Patient() { _id = 0; }
