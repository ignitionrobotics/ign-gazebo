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
#include "ServerPrivate.hh"

#include <tinyxml2.h>

#include <sdf/Root.hh>
#include <sdf/World.hh>

#include <ignition/common/Console.hh>

#include <ignition/gui/Application.hh>

#include "SimulationRunner.hh"

using namespace ignition;
using namespace gazebo;

//////////////////////////////////////////////////
ServerPrivate::ServerPrivate()
: systemLoader(std::make_shared<SystemLoader>())
{
  // Add the signal handler
  this->sigHandler.AddCallback(
      std::bind(&ServerPrivate::OnSignal, this, std::placeholders::_1));
}

//////////////////////////////////////////////////
ServerPrivate::~ServerPrivate()
{
  this->Stop();
  if (this->runThread.joinable())
  {
    this->runThread.join();
  }
}

//////////////////////////////////////////////////
void ServerPrivate::OnSignal(int _sig)
{
  igndbg << "Server received signal[" << _sig  << "]\n";
  this->Stop();
}

/////////////////////////////////////////////////
void ServerPrivate::Stop()
{
  this->running = false;
  for (std::unique_ptr<SimulationRunner> &runner : this->simRunners)
  {
    runner->Stop();
  }
}

/////////////////////////////////////////////////
bool ServerPrivate::Run(const uint64_t _iterations,
    std::optional<std::condition_variable *> _cond)
{
  this->runMutex.lock();
  this->running = true;
  if (_cond)
    _cond.value()->notify_all();
  this->runMutex.unlock();

  bool result = true;

  // Minor performance tweak. In many situations there will only be one
  // simulation runner, and we can avoid using the thread pool.
  if (this->simRunners.size() == 1)
  {
    result = this->simRunners[0]->Run(_iterations);
  }
  else
  {
    for (std::unique_ptr<SimulationRunner> &runner : this->simRunners)
    {
      this->workerPool.AddWork([&runner, &_iterations] ()
        {
          runner->Run(_iterations);
        });
    }

    // Wait for the runner to complete.
    result = this->workerPool.WaitForResults();
  }

  this->running = false;
  return result;
}

//////////////////////////////////////////////////
void ServerPrivate::CreateEntities()
{
  // Create a simulation runner for each world.
  for (uint64_t worldIndex = 0; worldIndex <
       this->sdfRoot.WorldCount(); ++worldIndex)
  {
    auto world = this->sdfRoot.WorldByIndex(worldIndex);

    this->simRunners.push_back(std::make_unique<SimulationRunner>(
        world, this->systemLoader, this->useLevels));
  }
}

//////////////////////////////////////////////////
void ServerPrivate::LoadGui()
{
  if (!gui::App())
    return;

  for (uint64_t worldIndex = 0; worldIndex < this->sdfRoot.WorldCount();
       ++worldIndex)
  {
    auto world = this->sdfRoot.WorldByIndex(worldIndex);
    auto element = world->Element();

    // GUI plugins
    if (!element->HasElement("gui") ||
        !element->GetElement("gui")->HasElement("plugin"))
    {
      continue;
    }

    auto pluginElem = element->GetElement("gui")->GetElement("plugin");
    while (pluginElem)
    {
      auto fileName = pluginElem->Get<std::string>("filename");

      auto pluginStr = pluginElem->ToString("");

      tinyxml2::XMLDocument pluginDoc;
      pluginDoc.Parse(pluginStr.c_str());

      gui::App()->LoadPlugin(fileName,
          pluginDoc.FirstChildElement("plugin"));

      pluginElem = pluginElem->GetNextElement("plugin");
    }
  }
}
