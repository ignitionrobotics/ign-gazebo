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
#ifndef IGNITION_GAZEBO_ISYSTEMMODEL_HH_
#define IGNITION_GAZEBO_ISYSTEMMODEL_HH_

#include <memory>

#include <ignition/gazebo/config.hh>
#include <ignition/gazebo/EntityComponentManager.hh>
#include <ignition/gazebo/Export.hh>
#include <ignition/gazebo/Types.hh>

namespace ignition
{
  namespace gazebo
  {
    // Inline bracket to help doxygen filtering.
    inline namespace IGNITION_GAZEBO_VERSION_NAMESPACE {
    //
    /// \class ISystemModel ISystemModel.hh ignition/gazebo/ISystemModel.hh
    /// \brief
    class IGNITION_GAZEBO_VISIBLE ISystemModel {
      /// \brief
      /// \param[in]
      public: EntityId LinkByName(const std::string &_name,
          EntityComponentManager &_ecm);

      /// \brief
      /// \param[in]
      public: EntityId JointByName(const std::string &_name,
          EntityComponentManager &_ecm);

      // TODO(louise): Move this to a place common to all interfaces.
      protected: EntityId modelId{kNullEntity};
    };
    }
  }
}
#endif
