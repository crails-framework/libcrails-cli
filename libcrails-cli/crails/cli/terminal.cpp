#include "terminal.hpp"

using namespace Crails;

const char* Terminal::reset_color         = "\x1B[0m";
const char* Terminal::Color::black        = "\x1B[30m";
const char* Terminal::Color::red          = "\x1B[31m";
const char* Terminal::Color::green        = "\x1B[32m";
const char* Terminal::Color::yellow       = "\x1B[33m";
const char* Terminal::Color::blue         = "\x1B[34m";
const char* Terminal::Color::magenta      = "\x1B[35m";
const char* Terminal::Color::cyan         = "\x1B[36m";
const char* Terminal::Color::gray         = "\x1B[37m";
const char* Terminal::Background::black   = "\x1B[40m";
const char* Terminal::Background::red     = "\x1B[41m";
const char* Terminal::Background::green   = "\x1B[42m";
const char* Terminal::Background::yellow  = "\x1B[43m";
const char* Terminal::Background::blue    = "\x1B[44m";
const char* Terminal::Background::magenta = "\x1B[45m";
const char* Terminal::Background::cyan    = "\x1B[46m";
const char* Terminal::Background::gray    = "\x1B[47m";
