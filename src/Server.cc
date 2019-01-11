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
#include "ignition/gazebo/Server.hh"

#include <sdf/Root.hh>
#include <sdf/Error.hh>
#include "ServerPrivate.hh"
#include "SimulationRunner.hh"

using namespace ignition::gazebo;

static const char kDefaultWorld[] =
  "<?xml version='1.0'?>"
  "<sdf version='1.6'>"
    "<world name='default'>"
      "<plugin filename='libignition-gazebo-physics-system.so'"
      "        name='ignition::gazebo::systems::v0::Physics'>"
      "</plugin>"
      "<plugin filename='libignition-gazebo-systems.so'"
      "        name='ignition::gazebo::systems::v0::SceneBroadcaster'>"
      "</plugin>"
      "<plugin filename='libignition-gazebo-systems.so'"
      "        name='ignition::gazebo::systems::v0::LogRecord'>"
      "</plugin>"
      "<gui fullscreen='0'>"
      "  <plugin filename='Scene3D' name='3D View'>"
      "    <ignition-gui>"
      "      <title>3D View</title>"
      "      <property type='bool' key='showTitleBar'>false</property>"
      "      <property type='string' key='state'>docked</property>"
      "    </ignition-gui>"
      "    <engine>ogre</engine>"
      "    <scene>scene</scene>"
      "    <ambient_light>0.4 0.4 0.4</ambient_light>"
      "    <background_color>0.8 0.8 0.8</background_color>"
      "    <camera_pose>-6 0 6 0 0.5 0</camera_pose>"
      "    <service>/world/default/scene/info</service>"
      "    <pose_topic>/world/default/pose/info</pose_topic>"
      "  </plugin>"
      "  <plugin filename='WorldControl' name='World control'>"
      "    <ignition-gui>"
      "      <title>World control</title>"
      "      <property type='bool' key='showTitleBar'>false</property>"
      "      <property type='bool' key='resizable'>false</property>"
      "      <property type='double' key='height'>72</property>"
      "      <property type='double' key='width'>121</property>"
      "      <property type='double' key='z'>1</property>"
      "      <property type='string' key='state'>floating</property>"
      "      <anchors target='3D View'>"
      "        <line own='left' target='left'/>"
      "        <line own='bottom' target='bottom'/>"
      "      </anchors>"
      "    </ignition-gui>"
      "    <play_pause>true</play_pause>"
      "    <step>true</step>"
      "    <start_paused>true</start_paused>"
      "    <service>/world/default/control</service>"
      "    <stats_topic>/world/default/stats</stats_topic>"
      "  </plugin>"
      "  <plugin filename='WorldStats' name='World stats'>"
      "    <ignition-gui>"
      "      <title>World stats</title>"
      "      <property type='bool' key='showTitleBar'>false</property>"
      "      <property type='bool' key='resizable'>false</property>"
      "      <property type='double' key='height'>110</property>"
      "      <property type='double' key='width'>290</property>"
      "      <property type='double' key='z'>1</property>"
      "      <property type='string' key='state'>floating</property>"
      "      <anchors target='3D View'>"
      "        <line own='right' target='right'/>"
      "        <line own='bottom' target='bottom'/>"
      "      </anchors>"
      "    </ignition-gui>"
      "    <sim_time>true</sim_time>"
      "    <real_time>true</real_time>"
      "    <real_time_factor>true</real_time_factor>"
      "    <iterations>true</iterations>"
      "    <topic>/world/default/stats</topic>"
      "  </plugin>"
      "</gui>"
    "</world>"
  "</sdf>";

/////////////////////////////////////////////////
Server::Server(const ServerConfig &_config)
  : dataPtr(new ServerPrivate)
{
  sdf::Root root;
  sdf::Errors errors;

  // Load a world if specified.
  if (!_config.SdfFile().empty())
  {
    errors = root.Load(_config.SdfFile());
  }
  else
  {
    // Load an empty world.
    /// \todo(nkoenig) Add a "AddWorld" function to sdf::Root.
    errors = root.LoadSdfString(kDefaultWorld);
  }

  if (!errors.empty())
  {
    for (auto &err : errors)
      ignerr << err << "\n";
    return;
  }

  this->dataPtr->CreateEntities(root);

  // Set the desired update period, this will override the desired RTF given in
  // the world file which was parsed by CreateEntities.
  if (_config.UpdatePeriod())
  {
    this->SetUpdatePeriod(_config.UpdatePeriod().value());
  }

  // Establish publishers and subscribers.
  this->dataPtr->SetupTransport();
}

/////////////////////////////////////////////////
Server::~Server()
{
}

/////////////////////////////////////////////////
bool Server::Run(const bool _blocking, const uint64_t _iterations,
    const bool _paused)
{
  // Set the initial pause state of each simulation runner.
  for (std::unique_ptr<SimulationRunner> &runner : this->dataPtr->simRunners)
    runner->SetPaused(_paused);

  // Check the current state, and return early if preconditions are not met.
  {
    std::lock_guard<std::mutex> lock(this->dataPtr->runMutex);
    if (!this->dataPtr->sigHandler.Initialized())
    {
      ignerr << "Signal handlers were not created. The server won't run.\n";
      return false;
    }

    // Do not allow running more than once.
    if (this->dataPtr->running)
    {
      ignwarn << "The server is already runnnng.\n";
      return false;
    }
  }

  if (_blocking)
    return this->dataPtr->Run(_iterations);

  // Make sure two threads are not created
  std::unique_lock<std::mutex> lock(this->dataPtr->runMutex);
  if (this->dataPtr->runThread.get_id() == std::thread::id())
  {
    std::condition_variable cond;
    this->dataPtr->runThread =
      std::thread(&ServerPrivate::Run, this->dataPtr.get(), _iterations, &cond);

    // Wait for the thread to start. We do this to guarantee that the
    // running variable gets updated before this function returns. With
    // a small number of iterations it is possible that the run thread
    // successfuly completes before this function returns.
    cond.wait(lock);
    return true;
  }

  return false;
}

/////////////////////////////////////////////////
void Server::SetUpdatePeriod(
    const std::chrono::steady_clock::duration &_updatePeriod,
    const unsigned int _worldIndex)
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    this->dataPtr->simRunners[_worldIndex]->SetUpdatePeriod(_updatePeriod);
}

//////////////////////////////////////////////////
bool Server::Running() const
{
  return this->dataPtr->running;
}

//////////////////////////////////////////////////
std::optional<bool> Server::Running(const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->Running();
  return std::nullopt;
}

//////////////////////////////////////////////////
bool Server::SetPaused(const bool _paused,
    const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
  {
    this->dataPtr->simRunners[_worldIndex]->SetPaused(_paused);
    return true;
  }

  return false;
}

//////////////////////////////////////////////////
std::optional<bool> Server::Paused(const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->Paused();
  return std::nullopt;
}

//////////////////////////////////////////////////
std::optional<uint64_t> Server::IterationCount(
    const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->IterationCount();
  return std::nullopt;
}

//////////////////////////////////////////////////
std::optional<size_t> Server::EntityCount(const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->EntityCount();
  return std::nullopt;
}

//////////////////////////////////////////////////
std::optional<size_t> Server::SystemCount(const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->SystemCount();
  return std::nullopt;
}

//////////////////////////////////////////////////
std::optional<bool> Server::AddSystem(const SystemPluginPtr &_system,
                                      const unsigned int _worldIndex)
{
  // Check the current state, and return early if preconditions are not met.
  std::lock_guard<std::mutex> lock(this->dataPtr->runMutex);
  // Do not allow running more than once.
  if (this->dataPtr->running)
  {
    ignerr << "Cannot add system while the server is runnnng.\n";
    return false;
  }

  if (_worldIndex < this->dataPtr->simRunners.size())
  {
    this->dataPtr->simRunners[_worldIndex]->AddSystem(_system);
    return true;
  }

  return std::nullopt;
}

//////////////////////////////////////////////////
bool Server::HasEntity(const std::string &_name,
                       const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->HasEntity(_name);

  return false;
}

//////////////////////////////////////////////////
std::optional<EntityId> Server::EntityByName(const std::string &_name,
    const unsigned int _worldIndex) const
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->EntityByName(_name);

  return std::nullopt;
}

//////////////////////////////////////////////////
bool Server::RequestEraseEntity(const std::string &_name,
                       const unsigned int _worldIndex)
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->RequestEraseEntity(_name);

  return false;
}

//////////////////////////////////////////////////
bool Server::RequestEraseEntity(const EntityId _id,
                                const unsigned int _worldIndex)
{
  if (_worldIndex < this->dataPtr->simRunners.size())
    return this->dataPtr->simRunners[_worldIndex]->RequestEraseEntity(_id);

  return false;
}

