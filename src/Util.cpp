#include "Util.h"

// Top 10 Baby Names of 2020
// https://www.ssa.gov/oact/babynames/
static const std::vector<const char *> FIRST_NAMES = {
    "Liam",  "Olivia", "Noah",      "Emma",      "Oliver",
    "Ava",   "Elijah", "Charlotte", "William",   "Sophia",
    "James", "Amelia", "Benjamin",  "Isabella",  "Lucas",
    "Mia",   "Henry",  "Evelyn",    "Alexander", "Harper",
};

// 50 most common last names in America
// https://www.al.com/news/2019/10/50-most-common-last-names-in-america.html
static const std::vector<const char *> LAST_NAMES = {
    "Smith",     "Johnson", "Williams", "Brown",     "Jones",
    "Garcia",    "Miller",  "Davis",    "Rodriguez", "Martinez",
    "Hernandez", "Lopez",   "Gonzalez", "Wilson",    "Anderson",
    "Thomas",    "Taylor",  "Moore",    "Jackson",   "Martin",
};

std::string generateName(std::mt19937 &rng) {
  static std::uniform_int_distribution<int> first_name_dist(
      0, FIRST_NAMES.size() - 1);
  static std::uniform_int_distribution<int> last_name_dist(
      0, LAST_NAMES.size() - 1);

  std::string first_name(FIRST_NAMES[first_name_dist(rng)]);
  std::string last_name(LAST_NAMES[last_name_dist(rng)]);
  return first_name + " " + last_name;
}