#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <switch.h>
#include <sys/stat.h>
#include <time.h>
#include <vector>

std::vector<std::string> game_list;

std::vector<std::string> primary_list;

std::vector<std::string> secondary_list;

using json = nlohmann::json;
DIR *dir;
struct dirent *ent;
using namespace std;

void clrscrn() { cout << "\033[2J\033[1;1H"; }

void print_game_list(int i) {
  cout << "                      \n\n\n\n";
  cout << game_list.at(i);
}

void print_primary_list(int i) {
  cout << "                                            \n\n\n\n";
  cout << primary_list.at(i);
}

void print_secondary_list(int i) {
  cout << "                                            \n\n\n\n";
  cout << secondary_list.at(i);
}

void save_game() {
  chdir("/");
  auto f = ofstream("decks/data.txt", std::ios::binary | std::ios::out);
  if (!f) {
    std::cout << "Could not save data" << std::endl;
    return;
  }

  for (auto entry = game_list.cbegin(); entry != game_list.cend(); ++entry) {
    f << *entry << '\0';
  }
}

void read_json(const char *json_name) {
  chdir("/decks");
  // cout << "loading ";
  // cout << json_name;
  // cout << "...";
  std::ifstream file(json_name, std::ifstream::binary);
  json game_json;
  // string game_name;
  file >> game_json;
  // string primary_objectives[] = {game_json["mainObj"]};
  // printf("\n\n saving");
  // cout << game_json["name"];
  // cout << "\n\n";
  game_list.push_back(game_json["name"]);

  int randint = rand() % game_json["mainObj"].size() + 0;
  // cout << distrub(game_json["mainObj"]);
  primary_list.push_back(game_json["mainObj"][randint]);
  randint = rand() % game_json["bonusObj"].size() + 0;
  secondary_list.push_back(game_json["bonusObj"][randint]);
  // cout << CONSOLE_YELLOW;
  // cout << game_list.size();
  // cout << " deck(s) loaded\n\n" CONSOLE_WHITE;
}
void update_list() {
  dir = opendir("/decks");
  chdir("/decks");
  if (dir == NULL) {
    cout << CONSOLE_RED "Failed to open decks directory... creating new "
                        "directory\n" CONSOLE_WHITE;
    cout << CONSOLE_GREEN "decks directory Created!\n\n" CONSOLE_WHITE;
    mkdir("/decks", 0775);
    cout << "Add some decks into the decks directory and restart the "
            "application!";
  } else {
    // cout << "Scanning decks directory..\n\n";
    while ((ent = readdir(dir))) {
      if (std::string(ent->d_name).ends_with(".json")) {
        //    printf("%s %s", ent->d_name, "\n\n");
        read_json(ent->d_name);
      }
    }
    closedir(dir);
  }
}
void intro() {
  cout << "Welcome to the Pyramid!\n\n";
  cout << CONSOLE_YELLOW "what card do you want to view?\n\n" CONSOLE_WHITE;
  if (game_list.size() > 0) {
    cout << "Y = ";
    cout << game_list.at(0);
  }
  if (game_list.size() > 1) {
    cout << "\n\nx = ";
    cout << game_list.at(1);
  }
  if (game_list.size() > 2) {
    cout << "\n\nB = ";
    cout << game_list.at(2);
  }
  if (game_list.size() > 3) {
    cout << "\n\nA = ";
    cout << game_list.at(3);
  }
}

void intro2_electric_boogaloo() {
  cout << CONSOLE_YELLOW "what card do you want to view?\n\n" CONSOLE_WHITE;
  if (game_list.size() > 0) {
    cout << "Y = ";
    cout << game_list.at(0);
  }
  if (game_list.size() > 1) {
    cout << "\n\nx = ";
    cout << game_list.at(1);
  }
  if (game_list.size() > 2) {
    cout << "\n\nB = ";
    cout << game_list.at(2);
  }
  if (game_list.size() > 3) {
    cout << "\n\nA = ";
    cout << game_list.at(3);
  }
  cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
}

int main(int argc, char *argv[]) {
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

  // Initialize the default gamepad (which reads handheld mode inputs as well as
  // the first connected controller)
  PadState pad;
  padInitializeDefault(&pad);
  // Main loop
  srand(time(NULL));
  update_list();
  intro();
  save_game();
  while (appletMainLoop()) {
    // Scan the gamepad. This should be done once for each frame
    padUpdate(&pad);
    // padGetButtonsDown returns the set of buttons that have been
    // newly pressed in this frame compared to the previous one
    u64 kDown = padGetButtonsDown(&pad);

    if (kDown & HidNpadButton_Plus)
      break; // break in order to return to hbmenu

    // Your code goes here
    if (kDown & HidNpadButton_Y && game_list.size() > 0) {
      clrscrn();
      cout << "press - to go back\n";
      intro2_electric_boogaloo();
      cout << CONSOLE_YELLOW;
      print_game_list(0);
      cout << CONSOLE_WHITE;
      print_primary_list(0);
      print_secondary_list(0);
    }
    if (kDown & HidNpadButton_X && game_list.size() > 1) {
      clrscrn();
      cout << "press - to go back\n";
      intro2_electric_boogaloo();
      cout << CONSOLE_YELLOW;
      print_game_list(1);
      cout << CONSOLE_WHITE;
      print_primary_list(1);
      print_secondary_list(1);
    }
    if (kDown & HidNpadButton_B && game_list.size() > 2) {
      clrscrn();
      cout << "press - to go back\n";
      intro2_electric_boogaloo();
      cout << CONSOLE_YELLOW;
      print_game_list(2);
      cout << CONSOLE_WHITE;
      print_primary_list(2);
      print_secondary_list(2);
    }
    if (kDown & HidNpadButton_A && game_list.size() > 3) {
      clrscrn();
      cout << "press - to go back\n";
      intro2_electric_boogaloo();
      cout << CONSOLE_YELLOW;
      print_game_list(3);
      cout << CONSOLE_WHITE;
      print_primary_list(3);
      print_secondary_list(3);
    }
    // Update the console, sending a new frame to the display
    consoleUpdate(NULL);
  }

  // Deinitialize and clean up resources used by the console (important!)
  consoleExit(NULL);
  return 0;
}
