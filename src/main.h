#ifndef MAIN_H
#define MAIN_H
#include "Console.h"
#include "Help.h"
#include "config/Config.h"
#include "config/ConfigFile.h"
#include "config/ConfigFile.h"
#include "filesystem/FileSystemWorker.h"
#include <chrono>
#include <string>

using namespace std;

extern inline bool RETURN [[noreturn]] (const string &message);
//inline bool RETURN [[noreturn]] (const string &message);
inline bool isSet(const string &arg);
void prepare(DataContainer<pair<const string, const string> > &args);
bool parseArguments(DataContainer<string> &, DataContainer<pair<const string, const string> > &);
bool isSupportedArgument(const string &arg);

#ifdef WIN
#define TIME_UNIT "us"
#else
#define TIME_UNIT "μs"
#endif

#endif // MAIN_H
