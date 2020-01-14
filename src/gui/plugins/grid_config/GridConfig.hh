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

#ifndef IGNITION_GAZEBO_GUI_GRIDCONFIG_HH_
#define IGNITION_GAZEBO_GUI_GRIDCONFIG_HH_

#include <memory>

#include <ignition/rendering.hh>
#include <ignition/gui/Plugin.hh>
#include <ignition/math/Color.hh>

namespace ignition
{
namespace gazebo
{
  class GridConfigPrivate;

  class GridConfig : public ignition::gui::Plugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: GridConfig();

    /// \brief Destructor
    public: ~GridConfig() override;

    // Documentation inherited
    public: void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

    /// \brief Funtion to search for scene created
    public slots: void SearchScene();

    /// \brief Callback to initiaize scene with default grid.
    public slots: void InitGrid(rendering::ScenePtr scene, bool reload=false);

    public: rendering::VisualPtr GetVisual();

    public: rendering::MaterialPtr GetMaterial();


    /// \brief Callback to update grid with new params.
    public slots: void UpdateVerCellCount(int c);

    /// \brief Callback to update grid with new params.
    public slots: void UpdateHonCellCount(int c);

    public slots: void UpdateCellLength(double l);

    public slots: void SetPose(double x, double y, double z,
                               double roll, double pitch, double yaw);

    public slots: void SetColor(double r, double g, double b, double a);

    public slots: void SetCustomColor(math::Color c);

    /// \brief Callback in Qt thread when checkbox is clicked.
    /// \param[in] checked checkbox state
    public slots: void OnShow(bool checked);

    public: void DestroyGrid();

    /// \internal
    /// \brief Pointer to private data.
    private: std::unique_ptr<GridConfigPrivate> dataPtr;
  };
}
}

#endif
