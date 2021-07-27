#include <iostream>

#include "card.h"

card::card() {}

card::card(std::string game, std::string primary, std::string secondary,
           bool completed) {
  this->game = game;
  this->primary = primary;
  this->secondary = secondary;
  this->completed = completed;
}

void card::print(std::ostream &os) {
  os << "Game: " << this->game << "\nPrimary objective: " << this->primary
     << "\nSecondary objective: " << this->secondary
     << "\nCompleted: " << (this->completed ? "true" : "false") << std::endl;
}

std::istream &operator>>(std::istream &is, card &c) {
  std::getline(is, c.game, '\x1F');
  std::getline(is, c.primary, '\x1F');
  std::getline(is, c.secondary, '\x1F');
  c.completed = is.get() - 0x30;
  is.get();

  return is;
}

std::ostream &operator<<(std::ostream &os, const card &c) {
  return os << c.game << '\x1F' << c.primary << '\x1F' << c.secondary << '\x1F'
            << c.completed << '\x1E';
}

void save(std::ostream &os, std::vector<card> &v) {
  if (!os) {
    // std::cout << "directory existn't lmao";
  }
  // std::cout << "saving\n\n";
  for (auto &e : v) {
    // std::cout << "still saving\n\n";
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