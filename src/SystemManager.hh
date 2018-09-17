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
#ifndef IGNITION_GAZEBO_SYSTEM_MANAGER_HH_
#define IGNITION_GAZEBO_SYSTEM_MANAGER_HH_

#include <memory>
#include <string>

#include <sdf/Element.hh>

#include <ignition/gazebo/Export.hh>
#include <ignition/gazebo/System.hh>
#include <ignition/gazebo/SystemPluginPtr.hh>

namespace ignition
{
  namespace gazebo
  {
    // Inline bracket to help doxygen filtering.
    inline namespace IGNITION_GAZEBO_VERSION_NAMESPACE {
    // Forward declarations.
    class SystemManagerPrivate;

    /// \class SystemManager SystemManager.hh ignition/gazebo/SystemManager.hh
    /// \brief Class for loading/unloading System plugins.
    class IGNITION_GAZEBO_VISIBLE SystemManager
    {
      /// \brief Constructor
      public: explicit SystemManager();

      /// \brief Destructor
      public: ~SystemManager();

      /// \brief Add path to search for plugins.
      /// \param[in] _path New path to be added.
      public: void AddSystemPluginPath(const std::string &_path);

      /// \brief Load and instantiate system plugin from an SDF element.
      /// \param[in] _sdf SDF Element describing plugin instance to be loaded.
      /// \returns Shared pointer to system instance or nullptr.
      public: std::optional<SystemPluginPtr> LoadPlugin(sdf::ElementPtr _sdf);

      /// \brief Load and instantiate system plugin from name/filename.
      /// \param[in] _filename Shared library filename to load plugin from.
      /// \param[in] _name Class name to be instantiated.
      /// \param[in] _sdf SDF Element describing plugin instance to be loaded.
      /// \returns Shared pointer to system instance or nullptr.
      public: std::optional<SystemPluginPtr> LoadPlugin(const std::string &_filename,
                                   const std::string &_name,
                                   sdf::ElementPtr _sdf);

      /// \brief Makes a printable string with info about systems
      /// \returns A pretty string
      public: std::string PrettyStr() const;

      /// \brief Pointer to private data.
      private: std::unique_ptr<SystemManagerPrivate> dataPtr;
    };
    }
  }
}
#endif

