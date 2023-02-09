/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root
 * of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */
#pragma once

#include <AzCore/Component/Component.h>

#include <ImGuiBus.h>

namespace TestScene {
//! Component responsible for storing counters of apples gathered by Kraken.
class ImGuiGripperTest : public AZ::Component,
                         public ImGui::ImGuiUpdateListenerBus::Handler {
public:
  AZ_COMPONENT(ImGuiGripperTest, "{fa8124da-a7d9-11ed-afa1-0242ac120002}",
               AZ::Component);
  ImGuiGripperTest() = default;
  ~ImGuiGripperTest() = default;
  void Activate() override;
  void Deactivate() override;
  static void Reflect(AZ::ReflectContext *context);

private:
  void OnImGuiUpdate() override;

  float m_desiredVel{0};
  float m_velocity{1};
  float m_force{1};
  AZ::EntityId m_finger1;
  AZ::EntityId m_finger2;
};
} // namespace TestScene