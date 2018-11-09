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
#ifndef IGNITION_GAZEBO_EVENTS_HH_
#define IGNITION_GAZEBO_EVENTS_HH_
#include <ignition/common/Event.hh>

namespace ignition
{
  namespace gazebo
  {
    namespace events
    {

      /// \brief Pause signal
      /// Emited when the simulation is paused/unpaused.
      using Pause = ignition::common::EventT<void(bool)>;

    }  // namespace events
  }  // namespace gazebo
}  // namespace ignition

#endif  // IGNITION_GAZEBO_EVENTS_HH_
