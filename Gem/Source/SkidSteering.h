/*
* Copyright (c) Contributors to the Open 3D Engine Project.
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
 */
#pragma once

#include <AzCore/Component/Component.h>

#include <ImGuiBus.h>
namespace TestScene
{
//! Component responsible for storing counters of apples gathered by Kraken.
class SkidSteeringDemo
    : public AZ::Component
    , public ImGui::ImGuiUpdateListenerBus::Handler
{
public:
  AZ_COMPONENT(SkidSteeringDemo, "{a0b93eea-8111-11ed-a1eb-0242ac120002}", AZ::Component);
  SkidSteeringDemo() = default;
  ~SkidSteeringDemo() = default;
  void Activate() override;
  void Deactivate() override;
  static void Reflect(AZ::ReflectContext* context);

private:
  void OnImGuiUpdate() override;
  AZStd::vector<AZ::EntityId> m_leftJoints;
  AZStd::vector<AZ::EntityId> m_rightJoints;

  AZStd::vector<AZ::EntityComponentIdPair> m_leftJointsPairs;
  AZStd::vector<AZ::EntityComponentIdPair> m_rightJointsPairs;

  AZ::EntityId m_leftFront;
  AZ::EntityId m_leftRear;
  AZ::EntityId m_rightFront;
  AZ::EntityId m_rightRear;

  AZ::EntityId m_baseLink;
  float m_linearVel{0};
  float m_sideVel{0};
  float m_rotVel{0};
  float m_maxForce{1};
  bool m_brickSkidSteering{false};


};
} // namespace AppleKraken