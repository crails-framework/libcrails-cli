#pragma once
#include "process.hpp"
#include <string_view>

namespace Crails
{
  void cli_notification(const std::string& title, const std::string& message, const std::string& mood = "");

  struct Notifier
  {
  protected:
    std::string bin;
  public:
    Notifier(const char* bin_name) { bin = Crails::which(bin_name); }
    virtual ~Notifier() {}
    virtual bool is_available() const { return bin.length() > 0; }
    virtual void notify(const std::string& title, const std::string& message, const std::string& mood) const = 0;
  };

  class KDialogNotifier : public Notifier
  {
  public:
    KDialogNotifier() : Notifier("kdialog") {}
    virtual void notify(const std::string& title, const std::string& message, const std::string& mood) const override;
  };

  class NotifySendNotifier : public Notifier
  {
  public:
    NotifySendNotifier() : Notifier("notify-send") {}
    virtual void notify(const std::string& title, const std::string& message, const std::string& mood) const override;
  };

  class TerminalNotifier : public Notifier
  {
  public:
    TerminalNotifier() : Notifier("terminal-notifier") {}
    virtual void notify(const std::string& title, const std::string& message, const std::string& mood) const override;
  };
}
