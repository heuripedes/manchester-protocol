#pragma once
#include <string>
#include <utility>

enum class PatientTag { blue = 0, green, yellow, orange, red };

int getTagTicks(PatientTag tag);

class Simulator;
class Patient {
public:
  Patient(const Simulator *sim, PatientTag tag, std::string name);
  Patient(const Patient &) = delete;
  Patient(Patient &&other);

  Patient &operator=(const Patient &) = delete;
  Patient &operator=(Patient &&other);

  ~Patient();

  void update() { _ticks--; }
  int ticks() const { return _ticks; }
  bool isUnconscious() const { return _ticks < 0; }
  bool isDead() const { return _ticks < -5; }

  int id() const { return _id; }
  PatientTag tag() const { return _tag; }
  std::string name() const { return _name; }

private:
  static int _next_patient_id;

  int _id;
  int _ticks;
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
