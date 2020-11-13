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
#ifndef IGNITION_GAZEBO_UTIL_HH_
#define IGNITION_GAZEBO_UTIL_HH_

#include <string>
#include <vector>

#include <ignition/math/Pose3.hh>
#include "ignition/gazebo/config.hh"
#include "ignition/gazebo/Entity.hh"
#include "ignition/gazebo/EntityComponentManager.hh"
#include "ignition/gazebo/Export.hh"
#include "ignition/gazebo/Types.hh"

namespace ignition
{
  namespace gazebo
  {
    // Inline bracket to help doxygen filtering.
    inline namespace IGNITION_GAZEBO_VERSION_NAMESPACE {
    //
    /// \brief Helper function to compute world pose of an entity
    /// \param[in] _entity Entity to get the world pose for
    /// \param[in] _ecm Immutable reference to ECM.
    /// \return World pose of entity
    math::Pose3d IGNITION_GAZEBO_VISIBLE worldPose(const Entity &_entity,
        const EntityComponentManager &_ecm);

    /// \brief Helper function to generate scoped name for an entity.
    /// \param[in] _entity Entity to get the name for.
    /// \param[in] _ecm Immutable reference to ECM.
    /// \param[in] _delim Delimiter to put between names, defaults to "/".
    /// \param[in] _includePrefix True to include the type prefix before the
    /// entity name
    std::string IGNITION_GAZEBO_VISIBLE scopedName(const Entity &_entity,
      const EntityComponentManager &_ecm, const std::string &_delim = "/",
      bool _includePrefix = true);

    /// \brief Generally, each entity will be of some specific high-level type,
    /// such as World, Sensor, Collision, etc, and one type only.
    /// The entity type is usually marked by having some component that
    /// represents that type, such as `ignition::gazebo::components::Visual`.
    ///
    /// This function returns the type ID of the given entity's type, which
    /// can be checked against different types. For example, if the
    /// entity is a model, this will be true:
    ///
    /// `gazebo::components::Model::typeId == entityTypeId(entity, ecm)`
    ///
    /// In case the entity isn't of any known type, this will return
    /// `ignition::gazebo::kComponentTypeIdInvalid`.
    ///
    /// In case the entity has more than one type, only one of them will be
    /// returned. This is not standard usage.
    ///
    /// \param[in] _entity Entity to get the type for.
    /// \param[in] _ecm Immutable reference to ECM.
    /// \return ID of entity's type-defining components.
    ComponentTypeId IGNITION_GAZEBO_VISIBLE entityTypeId(const Entity &_entity,
        const EntityComponentManager &_ecm);

    /// \brief Generally, each entity will be of some specific high-level type,
    /// such as "world", "sensor", "collision", etc, and one type only.
    ///
    /// This function returns a lowercase string for each type. For example,
    /// "light", "actor", etc.
    ///
    /// In case the entity isn't of any known type, this will return an empty
    /// string.
    ///
    /// In case the entity has more than one type, only one of them will be
    /// returned. This is not standard usage.
    ///
    /// Note that this is different from  component type names.
    ///
    /// \param[in] _entity Entity to get the type for.
    /// \param[in] _ecm Immutable reference to ECM.
    /// \return ID of entity's type-defining components.
    std::string IGNITION_GAZEBO_VISIBLE entityTypeStr(const Entity &_entity,
        const EntityComponentManager &_ecm);

    /// \brief Get the world to which the given entity belongs.
    /// \param[in] _entity Entity to get the world for.
    /// \param[in] _ecm Immutable reference to ECM.
    /// \return World entity ID.
    Entity IGNITION_GAZEBO_VISIBLE worldEntity(const Entity &_entity,
        const EntityComponentManager &_ecm);

    /// \brief Helper function to remove a parent scope from a given name.
    /// This removes the first name found before the delimiter.
    /// \param[in] _name Input name possibly generated by scopedName.
    /// \param[in] _delim Delimiter between names.
    /// \return A new string with the parent scope removed.
    std::string IGNITION_GAZEBO_VISIBLE removeParentScope(
        const std::string &_name, const std::string &_delim);

    /// \brief Combine a URI and a file path into a full path.
    /// If the URI is already a full path or contains a scheme, it won't be
    /// modified.
    /// If the URI is a relative path, the file path will be prepended.
    /// \param[in] _uri URI, which can have a scheme, or be full or relative
    /// paths.
    /// \param[in] _filePath The path to a file in disk.
    /// \return The full path URI.
    std::string IGNITION_GAZEBO_VISIBLE asFullPath(const std::string &_uri,
        const std::string &_filePath);

    /// \brief Get resource paths based on latest environment variables.
    /// \return All paths in the IGN_GAZEBO_RESOURCE_PATH variable.
    std::vector<std::string> IGNITION_GAZEBO_VISIBLE resourcePaths();

    /// \brief Add resource paths based on latest environment variables.
    /// This will update the SDF and Ignition environment variables, and
    /// optionally add more paths to the list.
    /// \param[in] _paths Optional paths to add.
    void IGNITION_GAZEBO_VISIBLE addResourcePaths(
        const std::vector<std::string> &_paths = {});

    /// \brief Get the top level model of an entity
    /// \param[in] _entity Input entity
    /// \param[in] _ecm Constant reference to ECM.
    /// \return Entity of top level model
    ignition::gazebo::Entity IGNITION_GAZEBO_VISIBLE topLevelModel(
        const Entity &_entity,
        const EntityComponentManager &_ecm);

    /// \brief Environment variable holding resource paths.
    const std::string kResourcePathEnv{"IGN_GAZEBO_RESOURCE_PATH"};

    /// \brief Environment variable used by SDFormat to find URIs inside
    /// `<include>`
    const std::string kSdfPathEnv{"SDF_PATH"};

    /// \brief Environment variable holding paths to custom rendering engine
    /// plugins.
    const std::string kRenderPluginPathEnv{"IGN_GAZEBO_RENDER_ENGINE_PATH"};
    }
  }
}
#endif
