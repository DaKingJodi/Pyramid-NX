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
// using namespace std;

void change_color(std::vector<card> card_list, completion_status result,
                  int obj) {
  if (obj == 1) {
    if (result == completion_status::failed)
      std::cout << CONSOLE_RED;
    else if ((result == completion_status::succeeded) |
             (result == completion_status::succeeded_with_bonus) |
             (result == completion_status::secondary_not_started))
      std::cout << CONSOLE_GREEN;
    else
      std::cout << CONSOLE_WHITE;
  } else if (obj == 2) {
    if ((result == completion_status::failed) |
            (result == completion_status::succeeded) &&
        result != completion_status::secondary_not_started)
      std::cout << CONSOLE_RED;
    else if (result == completion_status::succeeded_with_bonus)
      std::cout << CONSOLE_GREEN;
    else
      std::cout << CONSOLE_WHITE;
  }
}

void print_score(double score) { std::cout << "\x1b[1;55HScore: " << score; }

void add_points(bool is_bonus, double &score) {
  if (is_bonus) {
    score += 125;
  }
  score += 175;
}

void remove_points(bool is_bonus, double &score) {
  if (is_bonus) {
    score -= 125;
  }
  score -= 175;
}

void clrscrn() { std::cout << "\033[2J\033[1;1H"; }

void print_list(int i, std::vector<card> card_list) {
  completion_status result = card_list[i].completed;
  std::cout << "                                            \n\n\n\n";

  std::cout << CONSOLE_YELLOW;
  std::cout << card_list[i].game << "\n\n\n\n";
  std::cout << CONSOLE_WHITE;

  change_color(card_list, result, 1);
  std::cout << card_list[i].primary << "\n\n\n\n";
  std::cout << CONSOLE_WHITE;

  change_color(card_list, result, 2);
  std::cout << card_list[i].secondary << "\n\n\n\n";
  std::cout << CONSOLE_WHITE;
}

void print_list(unsigned int i, std::vector<card> card_list) {
  completion_status result = card_list[i].completed;
  std::cout << "                                            \n\n\n\n";

  std::cout << CONSOLE_YELLOW;
  std::cout << card_list[i].game << "\n\n\n\n";
  std::cout << CONSOLE_WHITE;

  change_color(card_list, result, 1);
  std::cout << card_list[i].primary << "\n\n\n\n";
  std::cout << CONSOLE_WHITE;

  change_color(card_list, result, 2);
  std::cout << card_list[i].secondary << "\n\n\n\n";
  std::cout << CONSOLE_WHITE;
}

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

  card_list.emplace_back(game, primary, secondary,
                         completion_status::not_started);
  file.close();
}
void update_list(std::vector<card> &card_list) {
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
    if (std::string(ent.path()).ends_with(".json") |
        std::string(ent.path()).ends_with(".JSON")) {
      read_json(ent.path(), card_list);
    }
  }
  // closedir(dir_path);
}

void end_game(double score) {
  clrscrn();
  std::cout << "youve finished the run with " << score << " points!\n\n";
  std::cout << "press " << CONSOLE_YELLOW << "+ " << CONSOLE_WHITE
            << "to exit and go back into the game to start a new run!";
  std::remove("../decks/data.txt");
}

void select_menu(std::vector<card> &card_list, u64 kDown,
                 unsigned int &selector, unsigned int &checker,
                 bool &declare_menu, bool &declare_menu_confirmation,
                 bool &no_quit, bool &no_exit_screen,
                 double &score) { // dont ask.
  bool already_pressed = false;

  completion_status result = completion_status::not_started;

  // if left + right stick are pressed, end the run
  if ((kDown & HidNpadButton_Down) && !declare_menu &&
      !declare_menu_confirmation) {

    // im lazy so im reusing this variable from earlier
    declare_menu_confirmation = true;
    declare_menu = false;

    // if all the cards any cards dont have a final result, set all_filled to
    // false
    bool all_filled /* uwu */ = true;
    for (unsigned int i = 0; i < card_list.size(); i++) {
      if (card_list[i].completed == completion_status::not_started) {
        all_filled = false;
      }
    }
    if (!all_filled) {
      clrscrn();
      no_exit_screen = true;
      no_quit = true;
    } else {
      clrscrn();
      end_game(score);
      no_exit_screen = true;
    }
  }

  else if (kDown & HidNpadButton_StickR && !declare_menu_confirmation) {
    declare_menu = true;
    no_quit = true;
    checker = selector + 1;
  }
  // changes what card you are viewing
  if (kDown & HidNpadButton_R && !no_exit_screen) {
    if (card_list.size() > selector + 1) {
      selector++;
    } else {
      selector = 0;
    }
    result = card_list[selector].completed;
  }
  if (kDown & HidNpadButton_L && !no_exit_screen) {
    if (selector == 0) {

      selector = card_list.size() - 1;
    } else {
      selector--;
    }
    result = card_list[selector].completed;
  }
  // if in declare menu
  if (declare_menu && !declare_menu_confirmation) {
    checker = selector;
    no_quit = true;
    clrscrn();

    std::cout << "\x1b[25;1HPress " << CONSOLE_YELLOW << "B " << CONSOLE_WHITE
              << "to cancel\n\n"
              << CONSOLE_WHITE;

    std::cout << "Press " << CONSOLE_YELLOW << "- " << CONSOLE_WHITE
              << "if you failed the main objective\n\n";
    std::cout << "Press " << CONSOLE_YELLOW << "+ " << CONSOLE_WHITE
              << "if you successfully completed the main objective";
    std::cout << "\x1b[20;1HNavigate the selection menu with " << CONSOLE_YELLOW
              << "L" << CONSOLE_WHITE << " and " << CONSOLE_YELLOW << "R"
              << CONSOLE_WHITE;
    std::cout << "\x1b[;1H";
    print_list(selector, card_list);
    std::cout << "\x1b[1;30HCard #" << selector + 1 << " of "
              << card_list.size();
    // if won
    if (kDown & HidNpadButton_Plus &&
        card_list[selector].completed == completion_status::not_started) {
      card_list[selector].completed = completion_status::secondary_not_started;
      declare_menu_confirmation = true;
      no_quit = true;
      already_pressed = true;
      no_exit_screen = true;
      checker = selector + 1;
    }
    // if lost
    if (kDown & HidNpadButton_Minus &&
        card_list[selector].completed == completion_status::not_started) {
      card_list[selector].completed = completion_status::failed;
      declare_menu = false;
      declare_menu_confirmation = false;
      no_quit = false;
      checker = selector + 1;
    }
    // if cancel
    if (kDown & HidNpadButton_B) {
      clrscrn();
      if (result == completion_status::succeeded) {
        remove_points(false, score);
      } else if (result == completion_status::succeeded_with_bonus) {
        remove_points(true, score);
      }
      card_list[selector].completed = completion_status::not_started;
      declare_menu = false;
      declare_menu_confirmation = false;
      no_quit = false;
      no_exit_screen = false;
      declare_menu_confirmation = false;
      checker = selector + 1;
    }
    auto o_f =
        std::ofstream("../decks/data.txt", std::ios::out | std::ios::binary);
    save(o_f, card_list);
    o_f.close();
  }
  // declare menu 2: electric boogaloo
  if (declare_menu && declare_menu_confirmation) {
    checker = selector;
    clrscrn();

    std::cout << "\x1b[25;1HPress " << CONSOLE_YELLOW << "B " << CONSOLE_WHITE
              << "to cancel\n\n"
              << CONSOLE_WHITE;
    std::cout << "Press " << CONSOLE_YELLOW << "- " << CONSOLE_WHITE
              << "if you failed the main objective\n\n";
    std::cout << "Press " << CONSOLE_YELLOW << "+ " << CONSOLE_WHITE
              << "if you successfully completed the main objective";
    std::cout << "\x1b[20;1HNavigate the selection menu with " << CONSOLE_YELLOW
              << "L" << CONSOLE_WHITE << " and " << CONSOLE_YELLOW << "R"
              << CONSOLE_WHITE;
    std::cout << "\x1b[;1H";
    print_list(selector, card_list);
    std::cout << "\x1b[1;30HCard #" << selector + 1 << " of "
              << card_list.size();
    if (kDown & HidNpadButton_B && result == completion_status::not_started) {
      // cancel
      clrscrn();

      card_list[selector].completed = completion_status::not_started;

      declare_menu = false;
      declare_menu_confirmation = false;
      no_quit = false;
      no_exit_screen = false;
      declare_menu_confirmation = false;

      checker = selector + 1;
    }

    if (kDown & HidNpadButton_Plus && !already_pressed) {
      card_list[checker].completed = completion_status::succeeded_with_bonus;
      declare_menu = false;
      declare_menu_confirmation = false;
      no_quit = false;
      no_exit_screen = false;
      checker = selector + 1;
      already_pressed = false;
      add_points(true, score);
    }
    if (kDown & HidNpadButton_Minus) {
      card_list[selector].completed = completion_status::succeeded;
      declare_menu = false;
      declare_menu_confirmation = false;
      no_quit = false;
      no_exit_screen = false;
      checker = selector + 1;
      already_pressed = false;
      add_points(false, score);
    }
    // save new information to the save file for loading completion status later
    auto o_f =
        std::ofstream("../decks/data.txt", std::ios::out | std::ios::binary);
    save(o_f, card_list);
    o_f.close();
  }

  // update the screen with new information each time it changes
  if (selector != checker) {
    checker = selector;
    std::cout << "\x1b[1;1H";
    clrscrn();
    if (!declare_menu && !no_exit_screen && !declare_menu_confirmation) {
      std::cout << "press " << CONSOLE_YELLOW << "+ " << CONSOLE_WHITE
                << "to exit";
    }
    print_list(selector, card_list);
    if (!no_exit_screen) {
      std::cout << "\x1b[20;1HNavigate the selection menu with "
                << CONSOLE_YELLOW << "L" << CONSOLE_WHITE << " and "
                << CONSOLE_YELLOW << "R" << CONSOLE_WHITE;
    }
    if (result == completion_status::not_started) {
      std::cout << "\n\nPress the " << CONSOLE_YELLOW << "Right Stick "
                << CONSOLE_WHITE << "to declare the result of your run!\n\n";
    }

    std::cout << "\x1b[1;34HCard #" << selector + 1 << " of "
              << card_list.size();
  }
  // if in the end_game() confirm menu
  if (declare_menu_confirmation && no_exit_screen && no_quit && !declare_menu) {
    no_quit = true;
    clrscrn();
    std::cout << CONSOLE_RED << "are you sure?\n\n" << CONSOLE_WHITE;
    std::cout << "press " << CONSOLE_YELLOW << "+ " << CONSOLE_WHITE
              << "to confirm\n\n";
    std::cout << "press " << CONSOLE_YELLOW << "- " << CONSOLE_WHITE
              << "to cancel";
    if (kDown & HidNpadButton_Plus) {
      end_game(score);
      no_exit_screen = false;
    } else if (kDown & HidNpadButton_Minus) {
      no_exit_screen = false;
      no_quit = false;
      declare_menu_confirmation = false;
      checker = selector + 1;
      print_score(score);
    }
  }
}

int main(int argc, char *argv[]) {
  std::vector<card> card_list;
  bool yes_quit = false;
  bool no_quit = false;

  unsigned int selector = 0;
  unsigned int checker = 1;

  double score = 0;

  bool text_loaded = false;

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

  // Initialize the default gamepad (which reads handheld mode inputs as
  // well as the first connected controller)
  PadState pad;
  padInitializeDefault(&pad);
  // Main loop
  srand(time(NULL));
  bool declare_menu_confirmation = false;
  bool declare_menu = false;
  // bool deletion_menu = false;
  bool no_exit_screen = false;

  // chdir("/");
  std::ifstream save_file("../decks/data.txt");

  if (!save_file) {
    // cout << "save file doesnt exist";
    std::ifstream outfile("../decks/data.txt");
    outfile.close();
  }
  {
    auto o_f =
        std::ofstream("../decks/data.txt", std::ios::out | std::ios::binary);

    if (save_file.peek() == std::ifstream::traits_type::eof() && o_f) {

      update_list(card_list);
      save(o_f, card_list);
      text_loaded = true;
    }
    save_file.close();
    o_f.close();
  }

  {
    if (!text_loaded) {
      auto i_f =
          std::ifstream("../decks/data.txt", std::ios::in | std::ios::binary);
      load(i_f, card_list);

      for (unsigned int i = 0; i < card_list.size(); i++) {
        completion_status result = card_list[i].completed;
        if (result == completion_status::succeeded) {
          add_points(false, score);
        } else if (result == completion_status::succeeded_with_bonus) {
          add_points(true, score);
        }
      }
    }
  }
  while (appletMainLoop()) {
    // Scan the gamepad. This should be done once for each frame
    padUpdate(&pad);
    // padGetButtonsDown returns the set of buttons that have been
    // newly pressed in this frame compared to the previous one
    u64 kDown = padGetButtonsDown(&pad);

    if ((kDown & HidNpadButton_Plus && !no_quit | yes_quit)) {
      break; // break in order to return to hbmenu
    }
    // Your code goes here

    select_menu(card_list, kDown, selector, checker, declare_menu,
                declare_menu_confirmation, no_quit, no_exit_screen, score);

    // Update the console, sending a new frame to the display
    consoleUpdate(NULL);
  }

  // Deinitialize and clean up resources used by the console (important!)
  consoleExit(NULL);
  return 0;
}
