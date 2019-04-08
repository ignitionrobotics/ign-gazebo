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

#include <gtest/gtest.h>

#include <ignition/msgs/double.pb.h>
#include <ignition/msgs/Utility.hh>

#include <ignition/common/Console.hh>
#include <ignition/math/Pose3.hh>

#include "ignition/gazebo/components/AngularVelocity.hh"
#include "ignition/gazebo/components/Joint.hh"
#include "ignition/gazebo/components/LinearAcceleration.hh"
#include "ignition/gazebo/components/LinearVelocity.hh"
#include "ignition/gazebo/components/Link.hh"
#include "ignition/gazebo/components/Name.hh"
#include "ignition/gazebo/components/PendingExternalWorldWrench.hh"
#include "ignition/gazebo/components/PendingJointForce.hh"
#include "ignition/gazebo/components/Pose.hh"

#include "ignition/gazebo/Server.hh"
#include "ignition/gazebo/SystemLoader.hh"
#include "ignition/gazebo/test_config.hh"

#include "plugins/MockSystem.hh"

#define TOL 1e-4

using namespace ignition;
using namespace gazebo;

/// \brief Test fixture for LiftDrag system
class LiftDragTestFixture : public ::testing::Test
{
  // Documentation inherited
  protected: void SetUp() override
  {
    ignition::common::Console::SetVerbosity(4);
    setenv("IGN_GAZEBO_SYSTEM_PLUGIN_PATH",
           (std::string(PROJECT_BINARY_PATH) + "/lib").c_str(), 1);
  }
};

class Relay
{
  public: Relay()
  {
    auto plugin = loader.LoadPlugin("libMockSystem.so",
                                    "ignition::gazebo::MockSystem", nullptr);
    EXPECT_TRUE(plugin.has_value());

    this->systemPtr = plugin.value();

    this->mockSystem =
        dynamic_cast<MockSystem *>(systemPtr->QueryInterface<System>());
    EXPECT_NE(nullptr, this->mockSystem);
  }

  public: Relay &OnPreUpdate(MockSystem::CallbackType _cb)
  {
    this->mockSystem->preUpdateCallback = std::move(_cb);
    return *this;
  }

  public: Relay &OnUpdate(MockSystem::CallbackType _cb)
  {
    this->mockSystem->updateCallback = std::move(_cb);
    return *this;
  }

  public: Relay &OnPostUpdate(MockSystem::CallbackTypeConst _cb)
  {
    this->mockSystem->postUpdateCallback = std::move(_cb);
    return *this;
  }

  public: SystemPluginPtr systemPtr;

  private: SystemLoader loader;
  private: MockSystem *mockSystem;
};

/////////////////////////////////////////////////
/// Measure / verify force torques against analytical answers.
TEST_F(LiftDragTestFixture, VerifyVerticalForce)
{
  using namespace std::chrono_literals;

  // Start server
  ServerConfig serverConfig;
  const auto sdfFile =
      std::string(PROJECT_SOURCE_PATH) + "/test/worlds/lift_drag.sdf";
  serverConfig.SetSdfFile(sdfFile);

  Server server(serverConfig);
  EXPECT_FALSE(server.Running());
  EXPECT_FALSE(*server.Running(0));

  server.SetUpdatePeriod(0ns);

  const std::string bodyName = "body";
  const std::string bladeName = "wing_1";
  const std::string jointName = "body_joint";
  const double desiredVel = -0.2;

  Relay testSystem;
  std::vector<math::Vector3d> linearVelocities;
  std::vector<math::Vector3d> forces;
  testSystem.OnPreUpdate(
      [&](const gazebo::UpdateInfo &, gazebo::EntityComponentManager &_ecm)
      {
        // Create velocity and acceleration components if they dont't exist.
        // This signals physics system to populate the component
        auto bladeLink = _ecm.EntityByComponents(components::Link(),
                                                 components::Name(bladeName));

        if (nullptr == _ecm.Component<components::AngularVelocity>(bladeLink))
        {
          _ecm.CreateComponent(bladeLink, components::AngularVelocity());
        }

        auto bodyLink = _ecm.EntityByComponents(components::Link(),
                                                components::Name(bodyName));

        if (nullptr ==
            _ecm.Component<components::WorldLinearVelocity>(bodyLink))
        {
          _ecm.CreateComponent(bladeLink, components::WorldLinearVelocity());
        }
        if (nullptr ==
            _ecm.Component<components::WorldLinearAcceleration>(bodyLink))
        {
          _ecm.CreateComponent(bodyLink, components::WorldLinearAcceleration());
        }
      });

  server.AddSystem(testSystem.systemPtr);
  server.Run(true, 1, false);

  testSystem.OnPostUpdate([&](const gazebo::UpdateInfo &,
                              const gazebo::EntityComponentManager &_ecm)
      {
        auto bladeLink = _ecm.EntityByComponents(components::Link(),
                                                 components::Name(bladeName));
        auto bodyLink = _ecm.EntityByComponents(components::Link(),
                                                components::Name(bodyName));
        auto linVelComp =
            _ecm.Component<components::WorldLinearVelocity>(bodyLink);
        auto wrenchComp =
            _ecm.Component<components::PendingExternalWorldWrench>(bladeLink);

        if (linVelComp)
        {
          linearVelocities.push_back(linVelComp->Data());
        }
        else
        {
          linearVelocities.push_back(math::Vector3d::Zero);
        }

        if (wrenchComp)
        {
          math::Vector3d force = msgs::Convert(wrenchComp->Data().force());
          forces.push_back(force);
        }
        else
        {
          forces.push_back(math::Vector3d::Zero);
        }
      });

  const double kp = 100.0;
  // Set a constant velocity to the prismatic joint
  testSystem.OnPreUpdate(
      [&](const gazebo::UpdateInfo &, gazebo::EntityComponentManager &_ecm)
      {
        auto joint = _ecm.EntityByComponents(components::Joint(),
                                             components::Name(jointName));

        auto bodyLink = _ecm.EntityByComponents(components::Link(),
                                                components::Name(bodyName));
        auto linVelComp =
            _ecm.Component<components::WorldLinearVelocity>(bodyLink);

        if (!linVelComp)
          return;

        auto jointCmd = kp * (desiredVel - linVelComp->Data().X());

        if (nullptr == _ecm.Component<components::PendingJointForce>(joint))
        {
          _ecm.CreateComponent(joint,
                               components::PendingJointForce({jointCmd}));
        }
        else
        {
          _ecm.Component<components::PendingJointForce>(joint)->Data()[0] =
              jointCmd;
        }
      });

  // parameters from SDF
  const double a0 = 0.1;
  const double cla = 4.0;
  // This the angle the blade makes with the horizontal axis. It's set in the
  // <pose> of the link as the roll
  const double dihedral = 0.1;
  const double rho = 1.2041;
  const double area = 10;

  const std::size_t testIters = 1000;
  server.Run(true, testIters , false);

  // It takes a few iterations before the
  EXPECT_EQ(testIters, forces.size());

  const double v = linearVelocities.back().X();
  const double q = 0.5 * rho * v * v;
  const double cl = cla * a0 * q * area;
  const double vertForce = forces.back().Z();

  const double expVertForce = cl * cos(dihedral);
  EXPECT_NEAR(expVertForce, vertForce, TOL);
}


