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

#include <ignition/common/Console.hh>
#include <ignition/gui/Application.hh>
#include <ignition/plugin/Register.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/rendering.hh>

#include "ignition/gazebo/components/Name.hh"
#include "ignition/gazebo/components/ParentEntity.hh"
#include "ignition/gazebo/EntityComponentManager.hh"

#include "GridConfig.hh"

namespace ignition::gazebo
{
  class GridConfigPrivate
  {
    /// \brief Parent window
    public: QQuickWindow *quickWindow;

    /// \brief Ptr to singleton engine
    public: rendering::RenderEngine *engine;

    /// \brief default engine name
    public: std::string engineName{"ogre"};
    
    /// \brief ptr to grid node
    public: rendering::GridPtr grid;
  };
}

using namespace ignition;
using namespace gazebo;

/////////////////////////////////////////////////
GridConfig::GridConfig()
  : gui::Plugin(), dataPtr(std::make_unique<GridConfigPrivate>())
{
}

/////////////////////////////////////////////////
GridConfig::~GridConfig() = default;
/////////////////////////////////////////////////
void GridConfig::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Grid Config";
  
  if (_pluginElem)
  {
    if (auto elem = _pluginElem->FirstChildElement("engine"))
      this->dataPtr->engineName = elem->GetText();
  }
}

/////////////////////////////////////////////////
void GridConfig::Initialize()
{

  // if gridPtr found, load the gridPtr to class

  // else, init a default grid and load that ptr to class
}

/////////////////////////////////////////////////
void GridConfig::Update()
{
  // update grid based on user input 

  // print to terminal for now
}

/////////////////////////////////////////////////
void GridConfig::OnShow(bool checked)
{
  // if checked, show grid

  // if not checked, remove grid
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gazebo::GridConfig,
                    ignition::gui::Plugin)
