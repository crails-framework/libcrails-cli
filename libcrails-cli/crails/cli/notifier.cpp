#include "notifier.hpp"
#include "terminal.hpp"
#include <sstream>
#include <map>
#include <list>
#include <memory>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace Crails;

static const map<string, string> notify_send_icons{
  {"success", "face-cool"},
  {"failure", "face-crying"},
  {"pending", "face-sad"}
};

static const map<string, const char*> stdout_moods{
  {"success", Terminal::Color::green},
  {"failure", Terminal::Color::red},
  {"pending", Terminal::Color::yellow}
};

static const list<Notifier*> notifiers{
  new NotifySendNotifier(),
  new KDialogNotifier(),
  new TerminalNotifier()
};

void KDialogNotifier::notify(const string& title, const string& message, const string& mood) const
{
  stringstream command;

  command << bin << " --title \"" << title << "\" --passivepopup \"" << message << '"';
  run_command(command.str());
}

void NotifySendNotifier::notify(const string& title, const string& message, const string& mood) const
{
  stringstream command;
  unsigned short timeout = 4000;
  auto icon = notify_send_icons.find(mood);

  command << bin << " \"" << message << "\" -t " << timeout;
  if (icon != notify_send_icons.end())
    command << " -i " << icon->second;
  run_command(command.str());
}

void TerminalNotifier::notify(const string& title, const string& message, const string& mood) const
{
  stringstream command;

  command << bin << " -title \"" << title << "\" -message \"" << message << '"';
  run_command(command.str());
}

static const Notifier* get_notifier()
{
  auto compare = [](const Notifier* notifier) { return notifier->is_available(); };
  auto iterator = find_if(notifiers.begin(), notifiers.end(), compare);

  if (iterator != notifiers.end())
    return *iterator;
  return nullptr;
}

static void stdout_notification(const string& title, const string& message, const string& mood)
{
  cout << '[' << title << ']' << ' ';
  if (stdout_moods.find(mood) != stdout_moods.end())
    cout << stdout_moods.at(mood);
  cout << message << Terminal::reset_color << endl;
}

namespace Crails
{
  void cli_notification(const string& title, const string& message, const string& mood)
  {
    const Notifier* notifier = get_notifier();

    if (notifier)
      notifier->notify(title, message, mood);
    stdout_notification(title, message, mood);
  }
}
