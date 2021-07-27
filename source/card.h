#ifndef CARD_H
#define CARD_H

#include <iosfwd>
#include <string>
#include <vector>

struct card {
  std::string game;
  std::string primary;
  std::string secondary;
  bool completed;

  card();

  card(std::string game, std::string primary, std::string secondary,
       bool completed);

  void print(std::ostream &os);
};

std::istream &operator>>(std::istream &is, card &c);
std::ostream &operator<<(std::ostream &os, const card &c);

void save(std::ostream &os, std::vector<card> &v);
void load(std::istream &is, std::vector<card> &v);

#endif