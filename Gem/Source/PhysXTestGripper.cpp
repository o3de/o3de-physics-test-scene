/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root
 * of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "Utils.h"
#include "PhysXTestGripper.h"

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Components/TransformComponent.h>

#include <HingeJointComponent.h>
#include <PhysX/Joint/PhysXJointRequestsBus.h>

#include <PrismaticJointComponent.h>
#include <imgui/imgui.h>

namespace TestScene {
void ImGuiGripperTest::Activate() {
  ImGui::ImGuiUpdateListenerBus::Handler::BusConnect();
}

void ImGuiGripperTest::Deactivate() {
  ImGui::ImGuiUpdateListenerBus::Handler::BusDisconnect();
}

void ImGuiGripperTest::Reflect(AZ::ReflectContext *context) {
  if (AZ::SerializeContext *serialize =
          azrtti_cast<AZ::SerializeContext *>(context)) {
    serialize->Class<ImGuiGripperTest, AZ::Component>()
        ->Field("finger1", &ImGuiGripperTest::m_finger1)
        ->Field("finger2", &ImGuiGripperTest::m_finger2)
        ->Version(1);

    if (AZ::EditContext *ec = serialize->GetEditContext()) {
      ec->Class<ImGuiGripperTest>("ImGuiGripperTest", "ImGuiGripperTest")
          ->ClassElement(AZ::Edit::ClassElements::EditorData,
                         "ImGuiGripperTest")
          ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu,
                      AZ_CRC("Game"))
          ->Attribute(AZ::Edit::Attributes::Category, "TestScene")
          ->DataElement(AZ::Edit::UIHandlers::EntityId,
                        &ImGuiGripperTest::m_finger1, "finger1", "")
          ->DataElement(AZ::Edit::UIHandlers::EntityId,
                        &ImGuiGripperTest::m_finger2, "finger2", "");
    }
  }
}

void ImGuiGripperTest::OnImGuiUpdate() {

  ImGui::Begin("ImGuiGripperTest");
  auto joint1 = Utils::GetJoint(m_finger1);
  auto joint2 = Utils::GetJoint(m_finger2);

  ImGui::InputFloat("m_velocity", &m_velocity);
  ImGui::InputFloat("m_force", &m_force);

  if (ImGui::Button("Open")) {
    m_desiredVel = m_velocity;
  }
  if (ImGui::Button("Close")) {
    m_desiredVel = -m_velocity;
  }

  PhysX::JointRequestBus::Event(joint1, &PhysX::JointRequests::SetVelocity,
                                m_desiredVel);
  PhysX::JointRequestBus::Event(joint1, &PhysX::JointRequests::SetMaximumForce,
                                m_force);

  PhysX::JointRequestBus::Event(joint2, &PhysX::JointRequests::SetVelocity,
                                m_desiredVel);
  PhysX::JointRequestBus::Event(joint2, &PhysX::JointRequests::SetMaximumForce,
                                m_force);
  ImGui::End();
}

} // namespace TestScene