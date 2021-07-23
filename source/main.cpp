#define TESLA_INIT_IMPL // If you have more than one file using the tesla
                        // header, only define this in the main one
#include <cmath>
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <switch.h>
#include <sys/stat.h>
#include <tesla.hpp> // The Tesla Header
#include <vector>

int i = 3;
std::vector<std::string> game_list;
using json = nlohmann::json;
DIR *dir;
struct dirent *ent;
using namespace std;

void read_json(const char *json_name) {
  game_list.push_back("welp");
  chdir("/decks");
  std::ifstream file(json_name, std::ifstream::binary);
  json game_json;
  string game_name;
  file >> game_json;
  file >> game_name;
  // printf("\n\n saving");
  cout << game_json["name"];
  printf("\n\n");
  game_list.push_back(game_name);
  game_list.push_back("welp");
  // cout << game_list.size();
  // cout << game_list.at(0);
}

void update_list() {
  tsl::hlp::doWithSDCardHandle([&](){
  auto dir = opendir("/decks");
  if (dir) {
    while (true) {
      auto entry = readdir(dir);
      if (entry == nullptr) {
        break;
      }
      auto json_path = "/decks/" + entry->d_name;
      read_json(json_path);
    }
  }
  closedir(dir);
    }
});

/*
void update_li1st() {
  tsl::hlp::doWithSDCardHandle([&](){
    dir = opendir("/decks");
    chdir("decks");
  )}
  // printf("changed directory\n\n");
  if (dir == NULL) {
    // printf("Failed to open decks directory... creating new directory\n");
    mkdir("/decks", 0775);
  } else {
    while ((ent = readdir(dir))) {
      game_list.push_back("welp");
      game_list.push_back("welp");
      printf("%s %s", ent->d_name, "\n\n");
      read_json(ent->d_name);
    }
    closedir(dir);
  }
}
*/
class GuiSecondary : public tsl::Gui {
public:
  GuiSecondary() {}

  virtual tsl::elm::Element *createUI() override {
    auto *rootFrame =
        new tsl::elm::OverlayFrame("Tesla Example", "v1.3.2 - Secondary Gui");

    rootFrame->setContent(
        new tsl::elm::DebugRectangle(tsl::Color{0x8, 0x3, 0x8, 0xF}));

    return rootFrame;
  }
};

class GuiTest : public tsl::Gui {
public:
  GuiTest(u8 arg1, u8 arg2, bool arg3) {}

  // Called when this Gui gets loaded to create the UI
  // Allocate all elements on the heap. libtesla will make sure to clean them up
  // when not needed anymore
  virtual tsl::elm::Element *createUI() override {
    // A OverlayFrame is the base element every overlay consists of. This will
    // draw the default Title and Subtitle. If you need more information in the
    // header or want to change it's look, use a HeaderOverlayFrame.
    auto frame = new tsl::elm::OverlayFrame("Tesla Example", "v1.3.2");

    // A list that can contain sub elements and handles scrolling
    auto list = new tsl::elm::List();

    // List Items
    list->addItem(new tsl::elm::CategoryHeader("List items"));

    auto *clickableListItem =
        new tsl::elm::ListItem("Clickable List Item", "...");
    clickableListItem->setClickListener([](u64 keys) {
      if (keys & HidNpadButton_A) {
        tsl::changeTo<GuiSecondary>();
        return true;
      }

      return false;
    });

    list->addItem(clickableListItem);
    list->addItem(new tsl::elm::ListItem("Default List Item"));
    list->addItem(
        new tsl::elm::ListItem("Default List Item with an extra long name to "
                               "trigger truncation and scrolling"));
    list->addItem(new tsl::elm::ToggleListItem(to_string(game_list.size()),
                                               false, "won", "lost"));

    // Custom Drawer, a element that gives direct access to the renderer
    // list->addItem(new tsl::elm::CategoryHeader("Custom Drawer", true));
    // list->addItem(
    // new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, s32 x,
    //                            s32 y, s32 w, s32 h) {
    // renderer->drawCircle(x + 40, y + 40, 20, true, renderer->a(0xF00F));
    // renderer->drawCircle(x + 50, y + 50, 20, true, renderer->a(0xF0F0));
    // renderer->drawRect(x + 130, y + 30, 60, 40, renderer->a(0xFF00));
    // renderer->drawString("Hello :)", false, x + 250, y + 70, 20,
    //                   renderer->a(0xFF0F));
    // renderer->drawRect(x + 40, y + 90, 300, 10, renderer->a(0xF0FF));
    // }),
    // 100);

    // Track bars
    // list->addItem(new tsl::elm::CategoryHeader("Did you Win or Lose?"));
    // list->addItem(new tsl::elm::TrackBar("\u2600"));
    // list->addItem(new tsl::elm::StepTrackBar("\uE13C", 20));
    // list->addItem(new tsl::elm::NamedStepTrackBar("\uE132", {"Won",
    // "Lose"}));

    // Add the list to the frame for it to be drawn
    frame->setContent(list);

    // Return the frame to have it become the top level element of this Gui
    return frame;
  }

  // Called once every frame to update values
  virtual void update() override {}

  // Called once every frame to handle inputs not handled by other UI elements
  virtual bool handleInput(u64 keysDown, u64 keysHeld,
                           const HidTouchState &touchPos,
                           HidAnalogStickState joyStickPosLeft,
                           HidAnalogStickState joyStickPosRight) override {
    return false; // Return true here to signal the inputs have been consumed
  }
};

class OverlayTest : public tsl::Overlay {
public:
  // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
  virtual void initServices() override {
  } // Called at the start to initialize all services necessary for this Overlay
  virtual void exitServices() override {
  } // Called at the end to clean up all services previously initialized

  virtual void onShow() override {
  } // Called before overlay wants to change from invisible to visible state
  virtual void onHide() override {
  } // Called before overlay wants to change from visible to invisible state

  virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
    return initially<GuiTest>(
        1, 2, true); // Initial Gui to load. It's possible to pass arguments to
                     // it's constructor like this
  }
};

int main(int argc, char **argv) {
  update_list();
  game_list.push_back("welp");
  return tsl::loop<OverlayTest>(argc, argv);
}