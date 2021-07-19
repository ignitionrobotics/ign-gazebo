/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

/*
 * \author Nick Lamprianidis <nlamprian@gmail.com>
 * \date January 2021
 */

namespace ignition
{
namespace gazebo
{
// Inline bracket to help doxygen filtering
inline namespace IGNITION_GAZEBO_VERSION_NAMESPACE
{
namespace systems
{
/// \brief Event that signifies there is a new target that needs to be enqueued.
class EnqueueNewTargetEvent
{
  /// \brief Constructor
  /// \param[in] _target New target
  public: EnqueueNewTargetEvent(double _target) : target(_target) {}

  /// \brief target New target
  public: double target;
};

/// \brief Event that signifies a new target will be processed.
class NewTargetEvent
{
};

/// \brief Event that signifies the door at the target floor level has been
/// opened.
class DoorOpenEvent
{
};

/// \brief Event that signifies the door at the target floor level has been
/// closed.
class DoorClosedEvent
{
};

/// \brief Event that signifies the door at the target floor level has remained
/// open for the required amount of time.
class TimeoutEvent
{
};

/// \brief Event that signifies the cabin has reached the target floor level.
class CabinAtTargetEvent
{
};

}  // namespace systems
}  // namespace IGNITION_GAZEBO_VERSION_NAMESPACE
}  // namespace gazebo
}  // namespace ignition
