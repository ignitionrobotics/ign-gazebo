/*
 * Copyright (C) 2019 Open Source Robotics Foundation
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

#ifndef IGNITION_GAZEBO_SYSTEMS_DETACHABLEJOINT_HH_
#define IGNITION_GAZEBO_SYSTEMS_DETACHABLEJOINT_HH_

#include <string>
#include <ignition/transport/Node.hh>
#include <ignition/msgs/boolean.pb.h>

#include "ignition/gazebo/System.hh"

namespace ignition
{
namespace gazebo
{
// Inline bracket to help doxygen filtering.
inline namespace IGNITION_GAZEBO_VERSION_NAMESPACE {
namespace systems
{
  /// \brief A system that initially attaches two models via a fixed joint and
  /// allows for the models to get detached during simulation via a topic.
  ///
  /// Parameters:
  ///
  /// <parent_model_link>: Name of the link in the parent model to be used in
  /// creating a fixed joint with a link in the child model.
  ///
  /// <child_model>: Name of the model to which this model will be connected
  ///
  /// <child_model_link>: Name of the link in the child model to be used in
  /// creating a fixed joint with a link in the parent model.
  ///
  /// <topic> (optional): Topic name to be used for detaching connections

  class IGNITION_GAZEBO_VISIBLE DetachableJoint
      : public System,
        public ISystemConfigure,
        public ISystemPreUpdate
  {
    /// Documentation inherited
    public: void Configure(const Entity &_entity,
                           const std::shared_ptr<const sdf::Element> &_sdf,
                           EntityComponentManager &_ecm,
                           EventManager &_eventMgr) final;

    /// Documentation inherited
    public: void PreUpdate(
                const ignition::gazebo::UpdateInfo &_info,
                ignition::gazebo::EntityComponentManager &_ecm) final;

    /// \brief Callback for detach request topic
    /// \todo(addisu) This could simply be msgs::Empty
    private: void OnDetachRequest(const msgs::Boolean &_msg);

    /// \brief Name of attachment link in the parent model
    private: std::string parentModelLinkName;

    /// \brief Name of child model
    private: std::string childModelName;

    /// \brief Name of attachment link in the child model
    private: std::string childModelLinkName;

    /// \brief Topic to be used for detaching connections
    private: std::string topic;

    /// \brief Entity of attachment link in the parent model
    private: Entity parentModelLinkEntity{kNullEntity};

    /// \brief Entity of attachment link in the child model
    private: Entity childModelLinkEntity{kNullEntity};

    /// \brief Entity of the detachable joint created by this system
    private: Entity detachableJointEntity{kNullEntity};

    /// \brief Whether detachment has been requested
    private: std::atomic<bool> detachRequested{false};

    /// \brief Ignition communication node.
    public: transport::Node node;

    /// \brief Whether all parameters are valid and the system can proceed
    private: bool validConfig{false};

    /// \brief Whether the system has been initialized
    private: bool initialized{false};
  };
  }
}
}
}

#endif
