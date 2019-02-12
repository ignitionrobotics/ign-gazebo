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

#include <ignition/plugin/Register.hh>

#include <sdf/Sensor.hh>

#include <ignition/common/Time.hh>
#include <ignition/math/Helpers.hh>

#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>
#include <ignition/sensors/CameraSensor.hh>
#include <ignition/sensors/GpuLidarSensor.hh>
#include <ignition/sensors/Manager.hh>

#include "ignition/gazebo/components/Camera.hh"
#include "ignition/gazebo/components/GpuLidar.hh"
#include "ignition/gazebo/components/Geometry.hh"
#include "ignition/gazebo/components/Light.hh"
#include "ignition/gazebo/components/Link.hh"
#include "ignition/gazebo/components/Material.hh"
#include "ignition/gazebo/components/Model.hh"
#include "ignition/gazebo/components/Name.hh"
#include "ignition/gazebo/components/ParentEntity.hh"
#include "ignition/gazebo/components/Pose.hh"
#include "ignition/gazebo/components/Sensor.hh"
#include "ignition/gazebo/components/Visual.hh"
#include "ignition/gazebo/EntityComponentManager.hh"

#include "SceneManager.hh"
#include "Sensors.hh"

using namespace ignition;
using namespace gazebo;
using namespace systems;

// Private data class.
class ignition::gazebo::systems::SensorsPrivate
{
  /// \brief Sensor manager object. This manages the lifecycle of the
  /// instantiated sensors.
  public: sensors::Manager sensorManager;

  /// \brief used to store whether rendering objects have been created.
  public: bool initialized = false;

  /// \brief Create rendering entities
  /// \param[in] _ecm The entity-component manager
  public: void CreateRenderingEntities(const EntityComponentManager &_ecm);

  /// \brief Remove rendering entities
  /// \param[in] _ecm The entity-component manager
  public: void RemoveRenderingEntities(const EntityComponentManager &_ecm);

  /// \brief Update rendering entities
  /// \param[in] _ecm The entity-component manager
  public: void UpdateRenderingEntities(const EntityComponentManager &_ecm);

  /// \brief Name of rendering engine
  public: std::string engineName;

  /// \brief Scene manager
  public: SceneManager sceneManager;

  public: ignition::rendering::RenderEngine *engine;
};

//////////////////////////////////////////////////
Sensors::Sensors() : System(), dataPtr(std::make_unique<SensorsPrivate>())
{
}

//////////////////////////////////////////////////
Sensors::~Sensors() = default;

//////////////////////////////////////////////////
void Sensors::Configure(const Entity &/*_id*/,
    const std::shared_ptr<const sdf::Element> &_sdf,
    EntityComponentManager &/*_ecm*/,
    EventManager &/*_eventMgr*/)
{
  // Setup rendering
  this->dataPtr->engineName =
      _sdf->Get<std::string>("render_engine", "ogre").first;
}

//////////////////////////////////////////////////
void Sensors::PostUpdate(const UpdateInfo &_info,
                         const EntityComponentManager &_ecm)
{
  // Only initialize if there are rendering sensors
  if (!this->dataPtr->initialized &&
      (_ecm.HasComponentType(_ecm.ComponentType<components::Camera>()) ||
        _ecm.HasComponentType(_ecm.ComponentType<components::GpuLidar>())))
  {
    this->dataPtr->engine =
        ignition::rendering::engine(this->dataPtr->engineName);
    if (!this->dataPtr->engine)
    {
      ignerr << "Failed to load engine ["
             << this->dataPtr->engineName << "]" << std::endl;
      return;
    }
    auto scene = this->dataPtr->engine->CreateScene("scene");

    // Create simulation runner sensor manager
    this->dataPtr->sensorManager.SetRenderingScene(scene);
    this->dataPtr->sceneManager.SetScene(scene);

    this->dataPtr->initialized = true;
  }

  if (this->dataPtr->engine != nullptr)
  {
    this->dataPtr->CreateRenderingEntities(_ecm);
    this->dataPtr->UpdateRenderingEntities(_ecm);
    this->dataPtr->RemoveRenderingEntities(_ecm);
  }

  auto time = math::durationToSecNsec(_info.simTime);
  this->dataPtr->sensorManager.RunOnce(common::Time(time.first, time.second));
}

//////////////////////////////////////////////////
void SensorsPrivate::CreateRenderingEntities(const EntityComponentManager &_ecm)
{
  _ecm.EachNew<components::Model, components::Name, components::Pose,
            components::ParentEntity>(
      [&](const Entity &_entity,
          const components::Model *,
          const components::Name *_name,
          const components::Pose *_pose,
          const components::ParentEntity *_parent)->bool
      {
        sdf::Model model;
        model.SetName(_name->Data());
        model.SetPose(_pose->Data());
        this->sceneManager.CreateModel(_entity, model, _parent->Data());
        return true;
      });

  _ecm.EachNew<components::Link, components::Name, components::Pose,
            components::ParentEntity>(
      [&](const Entity &_entity,
          const components::Link *,
          const components::Name *_name,
          const components::Pose *_pose,
          const components::ParentEntity *_parent)->bool
      {
        sdf::Link link;
        link.SetName(_name->Data());
        link.SetPose(_pose->Data());
        this->sceneManager.CreateLink(_entity, link, _parent->Data());
        return true;
      });

  // visuals
  _ecm.EachNew<components::Visual, components::Name, components::Pose,
            components::Geometry, components::ParentEntity>(
      [&](const Entity &_entity,
          const components::Visual *,
          const components::Name *_name,
          const components::Pose *_pose,
          const components::Geometry *_geom,
          const components::ParentEntity *_parent)->bool
      {
        sdf::Visual visual;
        visual.SetName(_name->Data());
        visual.SetPose(_pose->Data());
        visual.SetGeom(_geom->Data());

        // Optional components
        auto material = _ecm.Component<components::Material>(_entity);
        if (material != nullptr)
        {
          visual.SetMaterial(material->Data());
        }

        this->sceneManager.CreateVisual(_entity, visual, _parent->Data());
        return true;
      });

  // lights
  _ecm.EachNew<components::Light, components::ParentEntity>(
      [&](const Entity &_entity,
          const components::Light *_light,
          const components::ParentEntity *_parent) -> bool
      {
        this->sceneManager.CreateLight(_entity, _light->Data(),
                                       _parent->Data());
        return true;
      });

  // Create cameras
  _ecm.EachNew<components::Camera, components::ParentEntity>(
    [&](const Entity &_entity,
        const components::Camera *_camera,
        const components::ParentEntity *_parent)->bool
      {
        // two camera models with the same camera sensor name
        // causes name conflicts. We'll need to use scoped names
        // TODO(anyone) do this in ign-sensors?
        auto parent = sceneManager.EntityById(_parent->Data());
        if (!parent)
          return false;
        auto data = _camera->Data()->Clone();
        std::string scopedName = parent->Name() + "::"
            + data->Get<std::string>("name");
        data->GetAttribute("name")->Set(scopedName);
        auto sensor =
            this->sensorManager.CreateSensor<sensors::CameraSensor>(data);
        return this->sceneManager.AddSensor(
            _entity, sensor->Name(), _parent->Data());
      });

  // Create gpu lidar
  _ecm.EachNew<components::GpuLidar, components::ParentEntity>(
    [&](const Entity &_entity,
        const components::GpuLidar *_gpuLidar,
        const components::ParentEntity *_parent)->bool
      {
        auto parent = sceneManager.EntityById(_parent->Data());
        if (!parent)
          return false;
        auto data = _gpuLidar->Data()->Clone();
        std::string scopedName = parent->Name() + "::"
            + data->Get<std::string>("name");
        data->GetAttribute("name")->Set(scopedName);
        data->GetAttribute("type")->Set("gpu_lidar");
        auto sensor =
            this->sensorManager.CreateSensor<sensors::GpuLidarSensor>(data);
        sensor->SetParent(parent->Name());
        return this->sceneManager.AddSensor(
            _entity, sensor->Name(), _parent->Data());
      });
}

//////////////////////////////////////////////////
void SensorsPrivate::UpdateRenderingEntities(const EntityComponentManager &_ecm)
{
  _ecm.Each<components::Model, components::Pose>(
      [&](const Entity &_entity,
        const components::Model *,
        const components::Pose *_pose)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          entity->SetLocalPose(_pose->Data());
        }
        return true;
      });

  _ecm.Each<components::Link, components::Pose>(
      [&](const Entity &_entity,
        const components::Link *,
        const components::Pose *_pose)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          entity->SetLocalPose(_pose->Data());
        }
        return true;
      });

  // visuals
  _ecm.Each<components::Visual, components::Pose >(
      [&](const Entity &_entity,
        const components::Visual *,
        const components::Pose *_pose)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          entity->SetLocalPose(_pose->Data());
        }
        return true;
      });

  // lights
  _ecm.Each<components::Light, components::Pose>(
      [&](const Entity &_entity,
        const components::Light *,
        const components::Pose *_pose)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          entity->SetLocalPose(_pose->Data());
        }
        return true;
      });

  // Create cameras
  _ecm.Each<components::Camera, components::Pose>(
    [&](const Entity &_entity,
        const components::Camera *,
        const components::Pose *_pose)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          entity->SetLocalPose(_pose->Data());
        }
        return true;
      });

  // Update gpu_lidar
  _ecm.Each<components::GpuLidar, components::Pose>(
    [&](const Entity &_entity,
        const components::GpuLidar *,
        const components::Pose *_pose)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          entity->SetLocalPose(_pose->Data());
        }
        return true;
      });
}

//////////////////////////////////////////////////
void SensorsPrivate::RemoveRenderingEntities(const EntityComponentManager &_ecm)
{
  _ecm.EachRemoved<components::Model>(
      [&](const Entity &_entity, const components::Model *)->bool
      {
        this->sceneManager.RemoveEntity(_entity);
        return true;
      });

  _ecm.EachRemoved<components::Link>(
      [&](const Entity &_entity, const components::Link *)->bool
      {
        this->sceneManager.RemoveEntity(_entity);
        return true;
      });

  // visuals
  _ecm.EachRemoved<components::Visual>(
      [&](const Entity &_entity, const components::Visual *)->bool
      {
        this->sceneManager.RemoveEntity(_entity);
        return true;
      });

  // lights
  _ecm.EachRemoved<components::Light>(
      [&](const Entity &_entity, const components::Light *)->bool
      {
        this->sceneManager.RemoveEntity(_entity);
        return true;
      });

  // cameras
  _ecm.EachRemoved<components::Camera>(
    [&](const Entity &_entity, const components::Camera *)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          this->sensorManager.Remove(_entity);
          // FISME(louise) SensorId not implemented in ign-sensors
          // auto sensorID = this->sensorManager.SensorId(entity->Name());
          // this->sensorManager.Remove(sensorID);
        }
        this->sceneManager.RemoveEntity(_entity);
        return true;
      });

  // gpu_lidars
  _ecm.EachRemoved<components::GpuLidar>(
    [&](const Entity &_entity, const components::GpuLidar *)->bool
      {
        auto entity = this->sceneManager.EntityById(_entity);
        if (entity)
        {
          this->sensorManager.Remove(_entity);
          // FISME(louise) SensorId not implemented in ign-sensors
          // auto sensorID = this->sensorManager.SensorId(entity->Name());
          // this->sensorManager.Remove(sensorID);
        }
        this->sceneManager.RemoveEntity(_entity);
        return true;
      });
}

IGNITION_ADD_PLUGIN(Sensors, System,
  Sensors::ISystemConfigure,
  Sensors::ISystemPostUpdate
)
