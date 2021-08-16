#include <iostream>

#include "card.h"

card::card() {}

card::card(std::string game, std::string primary, std::string secondary,
           completion_status completed) {
  this->game = game;
  this->primary = primary;
  this->secondary = secondary;
  this->completed = completed;
}

std::string status_to_string(completion_status status) {
  switch (status) {
  case completion_status::failed:
    return "Failed";
  case completion_status::succeeded:
    return "Succeeded";
  case completion_status::succeeded_with_bonus:
    return "Succeeded with bonus";
  case completion_status::not_started:
    return "Not started";
  case completion_status::secondary_not_started:
    return "Secondary not started";
  default:
    return "Erroneous value encountered";
  }
}

void card::print(std::ostream &os) {
  os << "Game: " << this->game << "\nPrimary objective: " << this->primary
     << "\nSecondary objective: " << this->secondary
     << "\nCompleted: " << status_to_string(this->completed) << std::endl;
}

std::istream &operator>>(std::istream &is, card &c) {
  std::getline(is, c.game, '\x1F');
  std::getline(is, c.primary, '\x1F');
  std::getline(is, c.secondary, '\x1F');
  std::string status;
  std::getline(is, status, '\x1E');
  c.completed = static_cast<completion_status>(std::stoi(status));

  return is;
}

std::ostream &operator<<(std::ostream &os, const card &c) {
  return os << c.game << '\x1F' << c.primary << '\x1F' << c.secondary << '\x1F'
            << static_cast<std::underlying_type_t<completion_status>>(
                   c.completed)
            << '\x1E';
}

void save(std::ostream &os, std::vector<card> &v) {
  for (auto &e : v) {
    os << e;
  }
  os << '\x1D';
}

void load(std::istream &is, std::vector<card> &v) {
  card c;

  while (is >> c) {
    v.push_back(c);
    if (is.peek() == '\x1D')
      break;
  }
  is.get();
}