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

#include <ignition/msgs/double.pb.h>
#include <ignition/common/Profiler.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gazebo/components/JointForceCmd.hh"
#include "ignition/gazebo/components/JointVelocity.hh"
#include "ignition/gazebo/components/JointVelocityCmd.hh"
#include "ignition/gazebo/Model.hh"

#include "JointController.hh"

using namespace ignition;
using namespace gazebo;
using namespace systems;

class ignition::gazebo::systems::JointControllerPrivate
{
  /// \brief Callback for velocity subscription
  /// \param[in] _msg Velocity message
  public: void OnCmdVel(const ignition::msgs::Double &_msg);

  /// \brief Ignition communication node.
  public: transport::Node node;

  /// \brief Joint Entity
  public: Entity jointEntity;

  /// \brief Joint name
  public: std::string jointName;

  /// \brief Commanded joint velocity
  public: double jointVelCmd;

  /// \brief mutex to protect jointVelCmd
  public: std::mutex jointVelCmdMutex;

  /// \brief Model interface
  public: Model model{kNullEntity};

  /// \brief True if force commands are internally used to keep the target
  /// velocity.
  public: bool useForceCommands{false};

  /// \brief Velocity PID controller.
  public: ignition::math::PID velPid;
};

//////////////////////////////////////////////////
JointController::JointController()
  : dataPtr(std::make_unique<JointControllerPrivate>())
{
}

//////////////////////////////////////////////////
void JointController::Configure(const Entity &_entity,
    const std::shared_ptr<const sdf::Element> &_sdf,
    EntityComponentManager &_ecm,
    EventManager &/*_eventMgr*/)
{
  this->dataPtr->model = Model(_entity);

  if (!this->dataPtr->model.Valid(_ecm))
  {
    ignerr << "JointController plugin should be attached to a model entity. "
           << "Failed to initialize." << std::endl;
    return;
  }

  // Get params from SDF
  this->dataPtr->jointName = _sdf->Get<std::string>("joint_name");

  if (this->dataPtr->jointName == "")
  {
    ignerr << "JointController found an empty jointName parameter. "
           << "Failed to initialize.";
    return;
  }

  if (_sdf->HasElement("use_force_commands"))
  {
    this->dataPtr->useForceCommands =
      _sdf->Get<unsigned int>("use_force_commands");

    // PID parameters
    double p         =  1;
    double i         =  0.0;
    double d         =  0.0;
    double iMax      =  1;
    double iMin      = -1;
    double cmdMax    =  1000;
    double cmdMin    = -1000;
    double cmdOffset =  0;

    if (_sdf->HasElement("p_gain"))
    {
      p = _sdf->Get<double>("p_gain");
    }
    if (_sdf->HasElement("i_gain"))
    {
      i = _sdf->Get<double>("i_gain");
    }
    if (_sdf->HasElement("d_gain"))
    {
      d = _sdf->Get<double>("d_gain");
    }
    if (_sdf->HasElement("i_max"))
    {
      iMax = _sdf->Get<double>("i_max");
    }
    if (_sdf->HasElement("i_min"))
    {
      iMin = _sdf->Get<double>("i_min");
    }
    if (_sdf->HasElement("cmd_max"))
    {
      cmdMax = _sdf->Get<double>("cmd_max");
    }
    if (_sdf->HasElement("cmd_min"))
    {
      cmdMin = _sdf->Get<double>("cmd_min");
    }
    if (_sdf->HasElement("cmd_offset"))
    {
      cmdOffset = _sdf->Get<double>("cmd_offset");
    }

    this->dataPtr->velPid.Init(p, i, d, iMax, iMin, cmdMax, cmdMin, cmdOffset);

    igndbg << "[JointController] Force mode with parameters:" << std::endl;
    igndbg << "p_gain: ["     << p         << "]"             << std::endl;
    igndbg << "i_gain: ["     << i         << "]"             << std::endl;
    igndbg << "d_gain: ["     << d         << "]"             << std::endl;
    igndbg << "i_max: ["      << iMax      << "]"             << std::endl;
    igndbg << "i_min: ["      << iMin      << "]"             << std::endl;
    igndbg << "cmd_max: ["    << cmdMax    << "]"             << std::endl;
    igndbg << "cmd_min: ["    << cmdMin    << "]"             << std::endl;
    igndbg << "cmd_offset: [" << cmdOffset << "]"             << std::endl;
  }
  else
  {
    igndbg << "[JointController] Velocity mode" << std::endl;
  }

  // Subscribe to commands
  std::string topic{"/model/" + this->dataPtr->model.Name(_ecm) + "/joint/" +
                    this->dataPtr->jointName + "/cmd_vel"};
  this->dataPtr->node.Subscribe(topic, &JointControllerPrivate::OnCmdVel,
                                this->dataPtr.get());

  ignmsg << "JointController subscribing to Double messages on [" << topic
         << "]" << std::endl;
}

//////////////////////////////////////////////////
void JointController::PreUpdate(const ignition::gazebo::UpdateInfo &_info,
    ignition::gazebo::EntityComponentManager &_ecm)
{
  IGN_PROFILE("JointController::PreUpdate");

  // \TODO(anyone) Support rewind
  if (_info.dt < std::chrono::steady_clock::duration::zero())
  {
    ignwarn << "Detected jump back in time ["
        << std::chrono::duration_cast<std::chrono::seconds>(_info.dt).count()
        << "s]. System may not work properly." << std::endl;
  }

  // If the joint hasn't been identified yet, look for it
  if (this->dataPtr->jointEntity == kNullEntity)
  {
    this->dataPtr->jointEntity =
        this->dataPtr->model.JointByName(_ecm, this->dataPtr->jointName);
  }

  if (this->dataPtr->jointEntity == kNullEntity)
    return;

  // Nothing left to do if paused.
  if (_info.paused)
    return;

  // Create joint velocity component if one doesn't exist
  auto jointVelComp =
      _ecm.Component<components::JointVelocity>(this->dataPtr->jointEntity);
  if (jointVelComp == nullptr)
  {
    _ecm.CreateComponent(
        this->dataPtr->jointEntity, components::JointVelocity());
  }
  if (jointVelComp == nullptr)
    return;


  // igndbg << "Velocity: " <<  jointVelComp->Data().at(0) << std::endl;

  std::lock_guard<std::mutex> lock(this->dataPtr->jointVelCmdMutex);

  // Force mode.
  if (this->dataPtr->useForceCommands)
  {
    double error = jointVelComp->Data().at(0) - this->dataPtr->jointVelCmd;
    double force = this->dataPtr->velPid.Update(error, _info.dt);

    // igndbg << "Error: " <<  error << std::endl;
    // igndbg << "Force: " <<  force << std::endl<< std::endl;

    auto forceComp =
        _ecm.Component<components::JointForceCmd>(this->dataPtr->jointEntity);
    if (forceComp == nullptr)
    {
      _ecm.CreateComponent(this->dataPtr->jointEntity,
                           components::JointForceCmd({force}));
    }
    else
    {
      forceComp->Data()[0] = force;
    }
  }
  // Velocity mode.
  else
  {
    // Update joint velocity
    auto vel =
      _ecm.Component<components::JointVelocityCmd>(this->dataPtr->jointEntity);

    if (vel == nullptr)
    {
      _ecm.CreateComponent(
          this->dataPtr->jointEntity,
          components::JointVelocityCmd({this->dataPtr->jointVelCmd}));
    }
    else
    {
      vel->Data()[0] = this->dataPtr->jointVelCmd;
    }
  }
}

//////////////////////////////////////////////////
void JointControllerPrivate::OnCmdVel(const msgs::Double &_msg)
{
  std::lock_guard<std::mutex> lock(this->jointVelCmdMutex);
  this->jointVelCmd = _msg.data();
}

IGNITION_ADD_PLUGIN(JointController,
                    ignition::gazebo::System,
                    JointController::ISystemConfigure,
                    JointController::ISystemPreUpdate)

IGNITION_ADD_PLUGIN_ALIAS(JointController,
                          "ignition::gazebo::systems::JointController")
