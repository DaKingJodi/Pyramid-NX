#ifndef CARD_H
#define CARD_H

#include <iosfwd>
#include <string>
#include <vector>

enum class completion_status {
  not_started,
  secondary_not_started,
  failed,
  succeeded,
  succeeded_with_bonus
};

struct card {
  std::string game;
  std::string primary;
  std::string secondary;
  completion_status completed;

  card();

  card(std::string game, std::string primary, std::string secondary,
       completion_status completed);

  void print(std::ostream &os);
};

std::string status_to_string(completion_status status);

std::istream &operator>>(std::istream &is, card &c);
std::ostream &operator<<(std::ostream &os, const card &c);

void save(std::ostream &os, std::vector<card> &v);
void load(std::istream &is, std::vector<card> &v);

#endif