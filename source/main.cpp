#include "card.h"
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <random>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <switch.h>
#include <sys/stat.h>
#include <system_error>
#include <time.h>
#include <vector>
// saving/loading code courtesy of Famicom Guy (a pog person)
namespace fs = std::filesystem;
using json = nlohmann::json;
DIR *dir;
struct dirent *ent;
using namespace std;

/*
struct card {
  std::string game;
  std::string primary;
  std::string secondary;
  bool completed;

  card(std::string game, std::string primary, std::string secondary,
       bool completed) {
    this->game = game;
    this->primary = primary;
    this->secondary = secondary;
    this->completed = completed;
  }
};
*/
void add_points(bool is_bonus, double score) {
  if (is_bonus) {
    score += 125;
  }
  score += 175;
}
void finish_game(int game_number) {}
/*
bool _is_empty(std::ifstream &pFile) {
  return pFile.peek() == std::ifstream::traits_type::eof();
}
*/
void clrscrn() { cout << "\033[2J\033[1;1H"; }
/*
void print_game_list(int i, std::vector<std::string> game_list) {
  cout << "                      \n\n\n\n";
  cout << game_list.at(i);
}
*/
void print_list(int i, std::vector<card> card_list) {
  cout << "                                            \n\n\n\n";
  cout << CONSOLE_YELLOW;
  cout << card_list[i].game << "\n\n\n\n";
  cout << CONSOLE_WHITE;
  cout << card_list[i].primary << "\n\n\n\n";
  cout << card_list[i].secondary << "\n\n\n\n";
}
/*
void print_primary_list(int i), std::vector<std::string> primary_list){
  cout << "                                            \n\n\n\n";
  cout << primary_list.at(i);
}

void print_secondary_list(int i), std::vector<std::string> secondary_list {
  cout << "                                            \n\n\n\n";
  cout << secondary_list.at(i);
}
*/

void read_json(const fs::path &json_name, std::vector<card> &card_list) {
  std::ifstream file(json_name, std::ifstream::binary);
  json game_json;
  file >> game_json;
  // std::cout << json_name;
  std::string game = game_json["name"];

  auto randint = rand() % game_json["mainObj"].size() + 0;
  std::string primary = game_json["mainObj"][randint];

  randint = rand() % game_json["bonusObj"].size() + 0;
  std::string secondary = game_json["bonusObj"][randint];

  card_list.emplace_back(game, primary, secondary, false);
}
void update_list(std::vector<card> &card_list) {
  // chdir("/");
  fs::path dir_path("../decks");
  std::error_code errc;

  fs::directory_iterator dir(dir_path, errc);

  if (errc) {
    std::cout << "Could not open directory" << std::endl;
    fs::create_directory(dir_path);
    fs::permissions(dir_path, fs::perms::all & ~fs::perms::group_write &
                                  ~fs::perms::others_write);
    return;
  }

  for (auto &ent : dir) {
    // std::cout << ent.path() << std::endl << std::endl;
    if (std::string(ent.path()).ends_with(".json")) {
      read_json(ent.path(), card_list);
    }
  }
  // closedir(dir_path);
}
void intro(std::vector<card> card_list) {
  cout << "press + to exit\n\n\n";
  cout << "Welcome to the Pyramid!\n\n";
  cout << "a deck has been generated for you to complete!\n\n\n";
  cout << CONSOLE_YELLOW "what card do you want to view?\n\n" CONSOLE_WHITE;
  if (card_list.size() > 0) {
    cout << "Y = ";
    cout << card_list[0].game;
  }
  if (card_list.size() > 1) {
    cout << "\n\nx = ";
    cout << card_list[1].game;
  }
  if (card_list.size() > 2) {
    cout << "\n\nB = ";
    cout << card_list[2].game;
  }
  if (card_list.size() > 3) {
    cout << "\n\nA = ";
    cout << card_list[3].game;
  }
}

void intro2_electric_boogaloo(std::vector<card> card_list) {
  cout << CONSOLE_YELLOW "what card do you want to view?\n\n" CONSOLE_WHITE;
  if (card_list.size() > 0) {
    cout << "Y = ";
    cout << card_list[0].game;
  }
  if (card_list.size() > 1) {
    cout << "\n\nx = ";
    cout << card_list[1].game;
  }
  if (card_list.size() > 2) {
    cout << "\n\nB = ";
    cout << card_list[2].game;
  }
  if (card_list.size() > 3) {
    cout << "\n\nA = ";
    cout << card_list[3].game;
  }
  cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
}
/*
void save(std::ostream &os, std::vector<card> &v) {
  for (auto &entry : v) {
    os << entry << '\x1F';
  }
  os << '\x1D';
}

void load(std::istream &is, std::vector<card> &v) {
  std::string buf;

  while (std::getline(is, buf, '\x1F')) {
    v.emplace_back(buf);
    if (is.peek() == '\x1D')
      break;
  }

  is.get();
}
*/
void finish_run(double score) {
  cout << "You finished the run with ";
  cout << score;
  cout << "points!\n\n";
  cout << "please press + to close the program and start it again for a new "
          "run!";
}

int main(int argc, char *argv[]) {
  std::vector<card> card_list;

  double score;

  bool text_loaded = false;

  // std::vector<std::string> game_list;

  // std::vector<std::string> primary_list;

  // std::vector<std::string> secondary_list;

  // std::vector<std::string> completed_list;
  // This example uses a text console, as a simple way to output text to the
  // screen. If you want to write a software-rendered graphics application,
  //   take a look at the graphics/simplegfx example, which uses the libnx
  //   Framebuffer API instead.
  // If on the other hand you want to write an OpenGL based application,
  //   take a look at the graphics/opengl set of examples, which uses EGL
  //   instead.
  consoleInit(NULL);

  // Configure our supported input layout: a single player with standard
  // controller styles
  padConfigureInput(1, HidNpadStyleSet_NpadStandard);

  // Initialize the default gamepad (which reads handheld mode inputs as well
  // as the first connected controller)
  PadState pad;
  padInitializeDefault(&pad);
  // Main loop
  srand(time(NULL));
  bool menu1 = false;
  // chdir("/");
  std::ifstream save_file("../decks/data.txt");

  if (!save_file) {
    // cout << "save file doesnt exist";
    std::ifstream outfile("../decks/data.txt");
    outfile.close();
  }
  // std::vector<std::string> load_v1{};
  // std::vector<std::string> load_v2{};
  // bool text_loaded = false;
  {
    auto o_f =
        std::ofstream("../decks/data.txt", std::ios::out | std::ios::binary);
    if (!o_f) {
      // std::cout << CONSOLE_RED "Could not save data" CONSOLE_WHITE <<
      // std::endl;
      // cout << "\n\nCreating save file...\n\n";
      // cout << CONSOLE_GREEN "Save file created!\n\n";
      // cout << CONSOLE_WHITE "Restart the application";
    }

    // std::ifstream save_file("../decks/data.txt");
    if (save_file.peek() == std::ifstream::traits_type::eof() && o_f) {
      // std::ostream data_file("../decks/data.txt");
      update_list(card_list);
      // card_list = {{"monolith", "die", "bad", false},
      //                 {"isek", "cry", "pee", true}};
      save(o_f, card_list);
      intro(card_list);
      text_loaded = true;

      // cout << "\n\nwelp";
    }
  }

  {
    // chdir("/");
    auto i_f =
        std::ifstream("../decks/data.txt", std::ios::in | std::ios::binary);
    if (!i_f) {
      // std::cout << "Could not load data" << std::endl;
      // std::cout << "Creating data.txt...";
      // std::ofstream outfile("data.txt");
      //  outfile.close();
      //    cout << "save file created, restart the application!";
      // return 1;
    }
    //  chdir("/");
    load(i_f, card_list);
    // cout << game_list.size();
    //  cout << std::endl;
    // load(i_f, primary_list);
    // cout << primary_list.size();
    // load(i_f, secondary_list);
    if (!text_loaded) {
      cout << "press + to exit\n";
      intro2_electric_boogaloo(card_list);
      // cout << secondary_list.size();
    }
  }
  /*
    for (auto &c : game_list) {
      std::cout << c << std::endl;
    }

    for (auto &c : primary_list) {
      std::cout << c << std::endl;
    }

    for (auto &c : secondary_list) {
      std::cout << c << std::endl;
    }
  */

  while (appletMainLoop()) {
    // Scan the gamepad. This should be done once for each frame
    padUpdate(&pad);
    // padGetButtonsDown returns the set of buttons that have been
    // newly pressed in this frame compared to the previous one
    u64 kDown = padGetButtonsDown(&pad);

    if (kDown & HidNpadButton_Plus && !menu1)
      break; // break in order to return to hbmenu

    // Your code goes here
    if (kDown & HidNpadButton_Y && card_list.size() > 0 && !menu1) {
      clrscrn();
      cout << "press + to exit\n\n\n";
      intro2_electric_boogaloo(card_list);
      cout << CONSOLE_YELLOW;
      print_list(0, card_list);
      cout << CONSOLE_WHITE;
      // print_list(0, card_list);
      //   print_list(0, card_list);
    }
    if (kDown & HidNpadButton_X && card_list.size() > 1 && !menu1) {
      clrscrn();
      cout << "press + to exit\n\n\n";
      intro2_electric_boogaloo(card_list);
      cout << CONSOLE_YELLOW;
      print_list(1, card_list);
      cout << CONSOLE_WHITE;
      // print_list(1, primary_list);
      // print_list(1, secondary_list);
    }
    if (kDown & HidNpadButton_B && card_list.size() > 2 && !menu1) {
      clrscrn();
      cout << "press + to exit\n\n\n";
      intro2_electric_boogaloo(card_list);
      cout << CONSOLE_YELLOW;
      print_list(2, card_list);
      cout << CONSOLE_WHITE;
      //  print_list(2, card_list);
      // print_list(2, card_list);
    }
    if (kDown & HidNpadButton_A && card_list.size() > 3 && !menu1) {
      clrscrn();
      cout << "press + to exit\n\n\n";
      intro2_electric_boogaloo(card_list);
      cout << CONSOLE_YELLOW;
      print_list(3, card_list);
      cout << CONSOLE_WHITE;
      // print_list(3, primary_list);
      // print_list(3, secondary_list);
    }

    if (kDown & HidNpadButton_Minus) {
      clrscrn();
      menu1 = true;
      cout << "you sure you want to delete the file??\n\n";
      cout << "press - again to confirm or press + to cancel\n\n";
    }
    if (menu1 && kDown & HidNpadButton_Y) {
      menu1 = false;
      if (text_loaded) {
        intro(card_list);
      } else {
        intro2_electric_boogaloo(card_list);
      }
    }
    if (menu1 && kDown & HidNpadButton_Plus) {
      clrscrn();
      fs::current_path("/");
      /*
      std::fstream file("decks/data.txt");
      if (file) {
        cout << "save file found uwu \n\n";
      } else {
        cout << "no save file found, ya stinker \n\n";
      }
      menu1 = false;
    }
    */
      bool rm = std::filesystem::remove("../decks/data.txt");
      if (!rm) {
        cout << "failed removing save file";
        menu1 = false;
        // perror("error removing file: \n\n");
        // cout << "press any button to continue\n\n";
        // std::cin.ignore(10000, '\n');

        if (text_loaded) {
          intro(card_list);
        } else {
          intro2_electric_boogaloo(card_list);
        }
      } /*else {
        clrscrn();
        menu1 = false;
        cout << "press + to exit\n\n";
        cout << "file deleted, restart the application";
      }
      */
    }

    // Update the console, sending a new frame to the display
    consoleUpdate(NULL);
  }

  // Deinitialize and clean up resources used by the console (important!)
  consoleExit(NULL);
  return 0;
}
