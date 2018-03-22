/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#include <signal.h>
#include <gflags/gflags.h>
#include <ignition/common/Console.hh>

#include <iostream>

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
  #include <ignition/gui/Iface.hh>
  #include <ignition/gui/MainWindow.hh>
#endif

#include "ignition/gazebo/config.hh"
#include "ignition/gazebo/Server.hh"

// Gflag command line argument definitions
// This flag is an abbreviation for the longer gflags built-in help flag.
DEFINE_bool(h, false, "");
DEFINE_int32(verbose, 1, "");
DEFINE_int32(v, 1, "");
DEFINE_uint64(iterations, 0, "Number of iterations to execute.");
DEFINE_bool(s, false, "Run only the server (headless mode).");
DEFINE_bool(g, false, "Run only the GUI.");

//////////////////////////////////////////////////
void Help()
{
  std::cout
  << "gazebo -- Run the Gazebo server and GUI." << std::endl
  << std::endl
  << "`gazebo` [options] <world_file>" << std::endl
  << std::endl
  << std::endl
  << "Options:" << std::endl
  << "  -h [ --help ]          Print help message."
  << std::endl
  << "  --version              Print version information."
  << std::endl
  << "  -v [--verbose] arg     Adjust the level of console output (0~4)."
  << std::endl
  << "  --iterations arg       Number of iterations to execute."
  << std::endl
  << "  -s                     Run only the server (headless mode). This will "
  << " override -g, if it is also present."
  << std::endl
  << "  -g                     Run only the GUI."
  << std::endl
  << std::endl;
}

//////////////////////////////////////////////////
void Version()
{
  std::cout << IGNITION_GAZEBO_VERSION_HEADER << std::endl;
}

//////////////////////////////////////////////////
static bool VerbosityValidator(const char */*_flagname*/, int _value)
{
  return _value >= 0 && _value <= 4;
}

//////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  // Register validators
  gflags::RegisterFlagValidator(&FLAGS_verbose, &VerbosityValidator);
  gflags::RegisterFlagValidator(&FLAGS_v, &VerbosityValidator);

  // Parse command line
  gflags::ParseCommandLineNonHelpFlags(&_argc, &_argv, true);

  // Hold info as we parse it
  gflags::CommandLineFlagInfo info;

  // Help
  // Parse out the help flag in such a way that the full help text
  // is suppressed: if --help* or -h is specified, override the default
  // help behavior and turn on --helpmatch, to only shows help for the
  // current executable (instead of showing a huge list of gflags built-ins).
  gflags::GetCommandLineFlagInfo("help", &info);
  bool showHelp = FLAGS_h || (info.current_value == "true");

  // Version
  gflags::GetCommandLineFlagInfo("version", &info);
  bool showVersion = (info.current_value == "true");

  // Verbosity
  gflags::GetCommandLineFlagInfo("verbose", &info);
  if (info.is_default)
  {
    gflags::GetCommandLineFlagInfo("v", &info);
    if (!info.is_default)
      FLAGS_verbose = FLAGS_v;
  }

  // If help message is requested, substitute in the override help function.
  if (showHelp)
  {
    gflags::SetCommandLineOption("help", "false");
    gflags::SetCommandLineOption("helpshort", "false");
    gflags::SetCommandLineOption("helpfull", "false");
    gflags::SetCommandLineOption("helpmatch", "");
    Help();
  }
  // If version is requested, override with custom version print function.
  else if (showVersion)
  {
    gflags::SetCommandLineOption("version", "false");
    Version();
  }
  // Run Gazebo
  else
  {
    // Set verbosity
    ignition::common::Console::SetVerbosity(FLAGS_verbose);

    // Run only the server (headless)
    if (FLAGS_s)
    {
      // Create the Gazebo server
      ignition::gazebo::Server server;

      // Run the server, and block.
      server.Run(FLAGS_iterations, true);
    }
    // Run the GUI, or GUI+server
    else
    {
      std::unique_ptr<ignition::gazebo::Server> server;

      // Run the server along with the GUI
      if (!FLAGS_g)
      {
        // Create the server
        server.reset(new ignition::gazebo::Server());

        // Run the server, and don't block.
        server->Run(FLAGS_iterations, false);
      }

      // Initialize app
      ignition::gui::initApp();

      // Look for all plugins in the same place
      ignition::gui::setPluginPathEnv("GAZEBO_PLUGIN_PATH");

      // Create main window
      ignition::gui::createMainWindow();

      // Customize window
      ignition::gui::MainWindow *win = ignition::gui::mainWindow();
      win->setWindowTitle("Gazebo");

      // Then look for plugins on compile-time defined path.
      // Plugins installed by gazebo end up here
      // ignition::gui::addPluginPath(GAZEBO_PLUGIN_INSTALL_PATH);

      // Run main window - this blocks until the window is closed or we
      // receive a SIGINT
      ignition::gui::runMainWindow();

      // Cleanup once main window is closed
      ignition::gui::stop();
    }
  }

  igndbg << "Shutting down" << std::endl;
  return 0;
}
