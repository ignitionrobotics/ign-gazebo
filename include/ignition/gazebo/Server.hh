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
#ifndef IGNITION_GAZEBO_SERVER_HH_
#define IGNITION_GAZEBO_SERVER_HH_

#include <cstdint>
#include <memory>
#include <ignition/gazebo/Entity.hh>
#include <ignition/gazebo/Export.hh>
#include <ignition/gazebo/ServerConfig.hh>

namespace ignition
{
  namespace gazebo
  {
    // Forware declarations
    class ServerPrivate;

    /// \brief The server instantiates and controls simulation.
    ///
    /// ## Services
    ///
    /// The following are services provided by the Server.
    /// List syntax: *topic_name(request_message) : response_message*
    ///
    /// 1. /ign/gazebo/scene(none) : ignition::msgs::Scene
    ///   + Returns the current scene information.
    ///
    class IGNITION_GAZEBO_VISIBLE Server
    {
      /// \brief Constructor
      public: Server();

      /// \brief Construct the server using the parameters specified in a
      /// ServerConfig.
      /// \param[in] _config Server configuration parameters.
      public: explicit Server(const ServerConfig &_config);

      /// \brief Destructor
      public: ~Server();

      /// \brief Run the server. By default this is a non-blocking call,
      /// which means the server runs simulation in a separate thread. Pass
      /// in true to the _blocking argument to run the server in the current
      /// thread.
      /// \param[in] _blocking False to run the server in a new thread. True
      /// to run the server in the current thread.
      /// \param[in] _iterations Number of steps to perform. A value of
      /// zero will run indefinitely.
      public: bool Run(const bool _blocking = false,
                       const uint64_t _iterations = 0);

      /// \brief Get whether this server is running. When running is true,
      /// then simulation is stepping forward.
      /// \return True if the server is running.
      public: bool Running() const;

      /// \brief Get the number of iterations the server has executed.
      /// \return The current iteration count.
      public: uint64_t IterationCount() const;

      /// \brief Get the number of entities on the server.
      /// \return Entity count.
      public: size_t EntityCount() const;

      /// \brief Get the number of systems on the server.
      /// \return System count.
      public: size_t SystemCount() const;

      /// \brief Private data
      private: std::unique_ptr<ServerPrivate> dataPtr;
    };
  }
}

#endif
