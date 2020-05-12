// Copyright 2020 SICK AG. All rights reserved.
#include "atlstr.h"
#include "windows.h"
#include <algorithm>
#include <chrono>
#include <codecvt>
#include <iostream>
#include <locale>
#include <string>
#include <thread>

#include "CP210xRuntimeDLL.h"

class RelayBox
{
private:
  HANDLE deviceHandle;

private:
  RelayBox(const RelayBox&) = delete;
  RelayBox& operator=(const RelayBox&) = delete;

  bool probe()
  {
    if (deviceHandle != INVALID_HANDLE_VALUE)
    {
      unsigned char deviceType = !CP210x_CP2104_VERSION;
      if (CP210xRT_GetPartNumber(deviceHandle, &deviceType) == CP210x_SUCCESS)
      {
        if (deviceType == CP210x_CP2104_VERSION)
        {
          return true;
        }
      }
    }
    return false;
  }

public:
  RelayBox()
    : deviceHandle(INVALID_HANDLE_VALUE)
  {
  }

  bool open(std::string port)
  {
    CString comString;
    // COM port numbers from COM1 to COM9 are reserved by the system, COM10 and
    // above need the additional slashes below.
    comString.Format(_T("\\\\.\\%hs"), port.c_str());
    deviceHandle = CreateFile(comString,
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              0,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                              0);
    return probe();
  }

  bool open(int portNumber) { return open("COM" + std::to_string(portNumber)); }

  bool set(int relayNumber, bool state)
  {
    if (deviceHandle == INVALID_HANDLE_VALUE)
    {
      std::cerr << "Device handle invalid." << std::endl;
      return false;
    }

    WORD mask = 0x1 << relayNumber;
    WORD latch = state ? 0x0 : 0xf;
    return CP210xRT_WriteLatch(deviceHandle, mask, latch) != CP210x_SUCCESS;
  }

  ~RelayBox()
  {
    if (deviceHandle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(deviceHandle);
    }
  }
};

bool scanForBox(int& foundBoxPort)
{
  bool boxFound = false;
  for (int portNumber = 0; portNumber < 256; portNumber++)
  {
    RelayBox box;
    if (box.open(portNumber))
    {
      if (boxFound)
      {
        std::cerr << "Multiple RelayBoxes found. Use --port to choose which "
                     "one to use."
                  << std::endl;
        return false;
      }
      foundBoxPort = portNumber;
      boxFound = true;
    }
  }
  if (!boxFound)
  {
    std::cerr << "No RelayBox found." << std::endl;
  }
  return boxFound;
}

bool getNamedArg(int argc,
                 char* argv[],
                 const std::string argName,
                 std::string& argValue)
{
  char** itr = std::find(argv, argv + argc, argName);
  if (itr != (argv + argc) && ++itr != (argv + argc))
  {
    argValue = *itr;
    return true;
  }
  return false;
}

bool cmdOptionExists(int argc, char* argv[], const std::string& option)
{
  return std::find(argv, argv + argc, option) != argv + argc;
}

void cycleAllRelaysOnAndOff(RelayBox& box)
{
  auto delay = std::chrono::milliseconds(100);
  for (int i = 0; i < 4; i++)
  {
    box.set(i, true);
    std::this_thread::sleep_for(delay);
  }
  for (int i = 0; i < 4; i++)
  {
    box.set(i, false);
    std::this_thread::sleep_for(delay);
  }
}

void printHelp()
{
  std::cout
    << "Usage: CP210xRelay.exe [OPTION]... RELAY_NUMBER NEW_STATE\n"
       "Control the state of a CP210x GPIO based relay board.\n"
       "\n"
       "RELAY_NUMBER  Which relay to control. (0 to 3)\n"
       "NEW_STATE     1 turns on the relay, 0 turns it off.\n"
       "\n"
       "All COM ports are probed by default to find the relay board. If\n"
       "exactly one relay board is found, it is used without the need to\n"
       "specify it explicitly.\n"
       "\n"
       " -p, --port   COM port for the relay board to control\n"
       "              Example: \"COM4\"\n"
       " --test       Cycle all relays on and off\n";
}

int main(int argc, char* argv[])
{
  if (cmdOptionExists(argc, argv, "-h")
      || cmdOptionExists(argc, argv, "--help"))
  {
    printHelp();
    return 1;
  }

  RelayBox box;

  int namedArgOffset = 0;
  std::string portArg;
  if (getNamedArg(argc, argv, "-p", portArg)
      || getNamedArg(argc, argv, "--port", portArg))
  {
    namedArgOffset += 2;
    if (!box.open(portArg))
    {
      std::cerr << "Could not find RelayBox at " << portArg << "." << std::endl;
      return 1;
    }
  }
  else
  {
    int port;
    if (!scanForBox(port))
    {
      return 1;
    }
    box.open(port);
  }

  if (cmdOptionExists(argc, argv, "--test"))
  {
    cycleAllRelaysOnAndOff(box);
    return 0;
  }

  if (argc <= (2 + namedArgOffset))
  {
    printHelp();
    return 1;
  }

  int relayNr;
  bool newState;
  try
  {
    relayNr = std::stoi(argv[namedArgOffset + 1]);
    newState = std::stoi(argv[namedArgOffset + 2]) != 0;
  }
  catch (std::exception& e)
  {
    (void)e;
    std::cerr << "RELAY_NUMBER and NEW_STATE must be numeric." << std::endl;
    return 1;
  }
  if ((relayNr < 0) || (relayNr > 3))
  {
    std::cerr << "RELAY_NUMBER must be in range 0-3." << std::endl;
    return false;
  }

  box.set(relayNr, newState);
  return 0;
}
