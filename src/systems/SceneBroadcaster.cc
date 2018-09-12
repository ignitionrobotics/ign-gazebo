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

#include <ignition/msgs/scene.pb.h>
#include <ignition/math/graph/Graph.hh>
#include <ignition/plugin/RegisterMore.hh>
#include <ignition/transport/Node.hh>

#include "ignition/gazebo/components/Geometry.hh"
#include "ignition/gazebo/components/Link.hh"
#include "ignition/gazebo/components/Material.hh"
#include "ignition/gazebo/components/Model.hh"
#include "ignition/gazebo/components/Name.hh"
#include "ignition/gazebo/components/ParentEntity.hh"
#include "ignition/gazebo/components/Pose.hh"
#include "ignition/gazebo/components/Visual.hh"
#include "ignition/gazebo/components/World.hh"
#include "ignition/gazebo/Conversions.hh"
#include "ignition/gazebo/EntityComponentManager.hh"
#include "ignition/gazebo/systems/SceneBroadcaster.hh"

using namespace ignition;
using namespace gazebo;
using namespace systems;

//////////////////////////////////////////////////
void AddVisuals(msgs::Link *_msg,
    const EntityId _id,
    const math::graph::DirectedGraph<google::protobuf::Message *, bool>
        &_graph)
{
  if (!_msg)
    return;

  for (const auto &vertex : _graph.AdjacentsFrom(_id))
  {
    auto visualMsg = dynamic_cast<msgs::Visual *>(vertex.second.get().Data());
    if (!visualMsg)
      continue;

    _msg->add_visual()->CopyFrom(*visualMsg);
  }
}

//////////////////////////////////////////////////
void AddLinks(msgs::Model *_msg,
    const EntityId _id,
    const math::graph::DirectedGraph<google::protobuf::Message *, bool>
        &_graph)
{
  if (!_msg)
    return;

  for (const auto &vertex : _graph.AdjacentsFrom(_id))
  {
    auto linkMsg = dynamic_cast<msgs::Link *>(vertex.second.get().Data());
    if (!linkMsg)
      continue;

    // Visuals
    AddVisuals(linkMsg, vertex.second.get().Id(), _graph);

    _msg->add_link()->CopyFrom(*linkMsg);
  }
}

//////////////////////////////////////////////////
template<typename T>
void AddModels(T _msg,
    const EntityId _id,
    const math::graph::DirectedGraph<google::protobuf::Message *, bool>
        &_graph)
{
  for (const auto &vertex : _graph.AdjacentsFrom(_id))
  {
    auto modelMsg = dynamic_cast<msgs::Model *>(vertex.second.get().Data());
    if (!modelMsg)
      continue;

    // Nested models
    AddModels(modelMsg, vertex.second.get().Id(), _graph);

    // Links
    AddLinks(modelMsg, vertex.second.get().Id(), _graph);

    _msg->add_model()->CopyFrom(*modelMsg);
  }
}

// Private data class.
class ignition::gazebo::systems::SceneBroadcasterPrivate
{
  /// \brief Query callback for entity that has physics components.
  /// \param[in] _info Update information.
  /// \param[in] _manager Entity component manager.
  public: void OnUpdate(const UpdateInfo _info,
      EntityComponentManager &_manager);

  /// \brief Setup Ignition transport services and publishers
  /// \param[in] _worldName Name of world.
  public: void SetupTransport(const std::string &_worldName);

  /// \brief Callback for scene info service.
  /// \param[out] _res Response containing the latest scene message.
  /// \return True if successful.
  public: bool SceneInfoService(ignition::msgs::Scene &_res);

  /// \brief Callback for scene graph service.
  /// \param[out] _res Response containing the the scene graph in DOT format.
  /// \return True if successful.
  public: bool SceneGraphService(ignition::msgs::StringMsg &_res);

  /// \brief Transport node.
  public: transport::Node node;

  /// \brief Pose publisher.
  public: transport::Node::Publisher posePub;

  /// \brief Graph containing latest information from entities.
  public: math::graph::DirectedGraph<google::protobuf::Message *, bool>
      sceneGraph;

  /// \brief Keep the id of the world entity so we know how to traverse the
  /// graph.
  public: EntityId worldId;
};

//////////////////////////////////////////////////
SceneBroadcaster::SceneBroadcaster()
  : System(), dataPtr(std::make_unique<SceneBroadcasterPrivate>())
{
}

//////////////////////////////////////////////////
SceneBroadcaster::~SceneBroadcaster()
{
}

//////////////////////////////////////////////////
void SceneBroadcaster::Init(std::vector<EntityQueryCallback> &_cbs)
{
  _cbs.push_back(
      std::bind(&SceneBroadcasterPrivate::OnUpdate, this->dataPtr.get(),
        std::placeholders::_1, std::placeholders::_2));
}

//////////////////////////////////////////////////
void SceneBroadcasterPrivate::SetupTransport(const std::string &_worldName)
{
  // Scene info service
  std::string infoService{"/world/" + _worldName + "/scene/info"};

  this->node.Advertise(infoService, &SceneBroadcasterPrivate::SceneInfoService,
      this);

  ignmsg << "Serving scene information on [" << infoService << "]" << std::endl;

  // Scene graph service
  std::string graphService{"/world/" + _worldName + "/scene/graph"};

  this->node.Advertise(graphService,
      &SceneBroadcasterPrivate::SceneGraphService, this);

  ignmsg << "Serving scene graph on [" << graphService << "]" << std::endl;

  // Pose info publisher
  std::string topic{"/world/" + _worldName + "/pose/info"};

  transport::AdvertiseMessageOptions advertOpts;
  advertOpts.SetMsgsPerSec(60);
  this->posePub = this->node.Advertise<msgs::Scene>(topic, advertOpts);

  ignmsg << "Publishing pose messages on [" << topic << "]" << std::endl;
}

//////////////////////////////////////////////////
bool SceneBroadcasterPrivate::SceneInfoService(ignition::msgs::Scene &_res)
{
  _res.Clear();

  // Populate scene message
  AddModels(&_res, this->worldId, this->sceneGraph);

  return true;
}

//////////////////////////////////////////////////
bool SceneBroadcasterPrivate::SceneGraphService(ignition::msgs::StringMsg &_res)
{
  _res.Clear();

  std::stringstream graphStr;
  graphStr << this->sceneGraph;

  _res.set_data(graphStr.str());

  return true;
}

//////////////////////////////////////////////////
void SceneBroadcasterPrivate::OnUpdate(const UpdateInfo /*_info*/,
    EntityComponentManager &_manager)
{
  // TODO(louise) Get <scene> from SDF
  // TODO(louise) Fill message header

  // Populate a graph with latest information from all entities
  // TODO(louise) once we know what entities are added/deleted process only
  // those. For now, recreating graph at every iteration.
  this->sceneGraph =
      math::graph::DirectedGraph<google::protobuf::Message *, bool>();

  // World
  this->worldId = kNullEntity;
  _manager.Each<components::World,
                components::Name>(
    [&](const EntityId &_entity,
        const components::World */*_worldComp*/,
        const components::Name *_nameComp)
    {
      if (kNullEntity != this->worldId)
      {
        ignerr << "Internal error, more than one world found." << std::endl;
        return;
      }
      this->worldId = _entity;

      if (!this->posePub)
      {
        this->SetupTransport(_nameComp->Data());
      }

      this->sceneGraph.AddVertex(_nameComp->Data(), nullptr, _entity);
    });

  if (kNullEntity == this->worldId)
  {
    ignerr << "Failed to find world entity" << std::endl;
    return;
  }

  // Models
  _manager.Each<components::Model,
                components::Name,
                components::ParentEntity,
                components::Pose>(
    [&](const EntityId &_entity,
        const components::Model */*_modelComp*/,
        const components::Name *_nameComp,
        const components::ParentEntity *_parentComp,
        const components::Pose *_poseComp)
    {
      auto modelMsg = new msgs::Model();
      modelMsg->set_id(_entity);
      modelMsg->set_name(_nameComp->Data());
      modelMsg->mutable_pose()->CopyFrom(msgs::Convert(
          _poseComp->Data()));

      this->sceneGraph.AddVertex(_nameComp->Data(), modelMsg, _entity);
      this->sceneGraph.AddEdge({_parentComp->Id(), _entity}, true);
    });

  // Links
  _manager.Each<components::Link,
                components::Name,
                components::ParentEntity,
                components::Pose>(
    [&](const EntityId &_entity,
        const components::Link */*_linkComp*/,
        const components::Name *_nameComp,
        const components::ParentEntity *_parentComp,
        const components::Pose *_poseComp)
    {
      auto linkMsg = new msgs::Link();
      linkMsg->set_id(_entity);
      linkMsg->set_name(_nameComp->Data());
      linkMsg->mutable_pose()->CopyFrom(msgs::Convert(
          _poseComp->Data()));

      this->sceneGraph.AddVertex(_nameComp->Data(), linkMsg, _entity);
      this->sceneGraph.AddEdge({_parentComp->Id(), _entity}, true);
    });

  // Visuals
  _manager.Each<components::Visual,
                components::Name,
                components::ParentEntity,
                components::Pose>(
    [&](const EntityId &_entity,
        const components::Visual */*_visualComp*/,
        const components::Name *_nameComp,
        const components::ParentEntity *_parentComp,
        const components::Pose *_poseComp)
    {
      auto visualMsg = new msgs::Visual();
      visualMsg->set_id(_entity);
      visualMsg->set_parent_id(_parentComp->Id());
      visualMsg->set_name(_nameComp->Data());
      visualMsg->mutable_pose()->CopyFrom(msgs::Convert(
          _poseComp->Data()));

      // Geometry is optional
      auto geometryComp = _manager.Component<components::Geometry>(_entity);
      if (geometryComp)
      {
        visualMsg->mutable_geometry()->CopyFrom(
            Convert<msgs::Geometry>(geometryComp->Data()));
      }

      // Material is optional
      auto materialComp = _manager.Component<components::Material>(_entity);
      if (materialComp)
      {
        visualMsg->mutable_material()->CopyFrom(
            Convert<msgs::Material>(materialComp->Data()));
      }

      this->sceneGraph.AddVertex(_nameComp->Data(), visualMsg, _entity);
      this->sceneGraph.AddEdge({_parentComp->Id(), _entity}, true);
    });
}

IGNITION_ADD_PLUGIN(ignition::gazebo::systems::SceneBroadcaster,
                    ignition::gazebo::System)

