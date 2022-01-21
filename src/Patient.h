#pragma once
#include <string>
#include <utility>

enum class PatientTag { blue = 0, green, yellow, orange, red };

class Simulator;
class Patient {
public:
  Patient(const Simulator *sim, PatientTag tag, std::string name);
  Patient(const Patient &) = delete;
  Patient(Patient &&other);

  Patient &operator=(const Patient &) = delete;
  Patient &operator=(Patient &&other);

  ~Patient();

  bool isAlive() const { return _alive; }

  void setArrivalTick(int tick) { _arrivalTick = tick; }
  int arrivalTick() const { return _arrivalTick; }
  int waitTime() const;
  int remainingTime() const;

  int id() const { return _id; }
  PatientTag tag() const { return _tag; }
  std::string name() const { return _name; }

private:
  static int _next_patient_id;

  int _id;
  bool _alive;
  int _arrivalTick;
  const Simulator *_sim;
  std::string _name;
  PatientTag _tag;
};

// needed for std::sort
namespace std {
inline void swap(Patient &a, Patient &b) {
  Patient tmp(std::move(a));
  a = std::move(b);
  b = std::move(tmp);
}
} // namespace std
