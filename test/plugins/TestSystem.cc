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

#include "TestSystem.hh"

#include <ignition/plugin/Register.hh>

using namespace ignition;
using namespace gazebo;

/////////////////////////////////////////////////
TestSystem::TestSystem()
  : System()
{
}

/////////////////////////////////////////////////
TestSystem::~TestSystem()
{
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gazebo::TestSystem, ignition::gazebo::System)
