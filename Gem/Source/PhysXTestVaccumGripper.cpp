/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root
 * of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "PhysXTestVaccumGripper.h"
#include "Utils.h"

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Components/TransformComponent.h>

#include "Source/ArticulationLinkComponent.h"
#include <AzFramework/Physics/PhysicsSystem.h>
#include <AzFramework/Physics/RigidBodyBus.h>
#include <AzFramework/Physics/SimulatedBodies/RigidBody.h>
#include <imgui/imgui.h>

namespace TestScene {
void ImGuiVaccumGripperTest::Activate() {
  ImGui::ImGuiUpdateListenerBus::Handler::BusConnect();
  m_vaccumJoint = AzPhysics::InvalidJointHandle;
}

void ImGuiVaccumGripperTest::Deactivate() {
  ImGui::ImGuiUpdateListenerBus::Handler::BusDisconnect();
}

void ImGuiVaccumGripperTest::Reflect(AZ::ReflectContext *context) {
  if (AZ::SerializeContext *serialize =
          azrtti_cast<AZ::SerializeContext *>(context)) {
    serialize->Class<ImGuiVaccumGripperTest, AZ::Component>()
        ->Field("GrippedObject", &ImGuiVaccumGripperTest::m_grippedObject)

        ->Version(1);

    if (AZ::EditContext *ec = serialize->GetEditContext()) {
      ec->Class<ImGuiVaccumGripperTest>("ImGuiVaccumGripperTest",
                                        "ImGuiVaccumGripperTest")
          ->ClassElement(AZ::Edit::ClassElements::EditorData,
                         "ImGuiVaccumGripperTest")
          ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu,
                      AZ_CRC("Game"))
          ->Attribute(AZ::Edit::Attributes::Category, "TestScene")
          ->DataElement(AZ::Edit::UIHandlers::EntityId,
                        &ImGuiVaccumGripperTest::m_grippedObject,
                        "GrippedObject", "");
    }
  }
}

void ImGuiVaccumGripperTest::OnImGuiUpdate() {

  ImGui::Begin("ImGuiVaccumGripperTest");

  if (ImGui::Button("Grip")) {
    PhysX::ArticulationLinkComponent *component =
        m_entity->FindComponent<PhysX::ArticulationLinkComponent>();
    AZ_Assert(component,
              "No PhysX::ArticulationLinkComponent found on entity ");

    //    physx::PxArticulationLink* link =
    //    component->GetArticulationLink(m_entity->GetId()); AZ_Assert(link, "No
    //    PhysX::ArticulationLink found on entity ");
    //

    auto *sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
    AzPhysics::SceneHandle defaultSceneHandle =
        sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
    AZ_Assert(defaultSceneHandle != AzPhysics::InvalidSceneHandle,
              "Invalid default physics scene handle");

    AzPhysics::RigidBody *grippedRigidBody = nullptr;
    Physics::RigidBodyRequestBus::EventResult(
        grippedRigidBody, m_grippedObject,
        &Physics::RigidBodyRequests::GetRigidBody);
    AZ_Assert(grippedRigidBody,
              "No RigidBody found on entity grippedRigidBody");

    physx::PxArticulationLink *articulationLink =
        component->GetArticulationLink(m_entity->GetId());
    AZ_Assert(articulationLink, "No articulation links in component");

    AZStd::vector<AzPhysics::SimulatedBodyHandle> handles =
        component->GetSimualatedBodyHandles();
    AZ_Assert(handles.size() > 1,
              "Expected more than one body handles in articulations");

    for (auto &h : handles) {
      AzPhysics::SimulatedBody *simulatedBody =
          AZ::Interface<AzPhysics::SceneInterface>::Get()
              ->GetSimulatedBodyFromHandle(defaultSceneHandle, h);
      AZ_Printf("PhysXTestVaccumGripper", "Handle : %s",
                simulatedBody->GetEntityId().ToString().c_str());
    }

    //    AzPhysics::SimulatedBodyHandle grippedBodyHandle =
    //    sceneInterface->GetSimulatedBodyHandleFromEntityId(defaultSceneHandle,
    //    m_grippedObject); AZ_Assert(grippedBodyHandle !=
    //    AzPhysics::InvalidSimulatedBodyHandle, "Invalid gripped body handle");
    //
    //    AzPhysics::SimulatedBodyHandle gripperHandle =
    //    sceneInterface->GetSimulatedBodyHandleFromEntityId(defaultSceneHandle,
    //    m_entity->GetId() AZ_Assert(gripperHandle !=
    //    AzPhysics::InvalidSimulatedBodyHandle, "Invalid gripper handle");

    PhysX::FixedJointConfiguration jointConfig;
    jointConfig.m_debugName = "test";
    jointConfig.m_parentLocalRotation = AZ::Quaternion::CreateIdentity();
    jointConfig.m_childLocalRotation = AZ::Quaternion::CreateIdentity();
    jointConfig.m_childLocalPosition = AZ::Vector3(0.0f, 0.0f, -0.15f);
    jointConfig.m_startSimulationEnabled = true;

    if (m_vaccumJoint != AzPhysics::InvalidJointHandle) {
      AZ_Printf("PhysXTestVaccumGripper", "Already created m_vaccumJoint");
      return;
    }
    m_vaccumJoint = sceneInterface->AddJoint(defaultSceneHandle, &jointConfig,
                                               handles.back(),
                                               grippedRigidBody->m_bodyHandle);
    AZ_Printf("PhysXTestVaccumGripper", "Created new m_vaccumJoint ");
    AzPhysics::Joint* joint = sceneInterface->GetJointFromHandle(defaultSceneHandle, m_vaccumJoint);
    AZ_Assert(joint, "No joint found");

  }

  if (ImGui::Button("Release")) {
    auto *sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
    AzPhysics::SceneHandle defaultSceneHandle =
        sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
    AZ_Assert(defaultSceneHandle != AzPhysics::InvalidSceneHandle,
              "Invalid default physics scene handle");

    AzPhysics::Joint* joint = sceneInterface->GetJointFromHandle(defaultSceneHandle, m_vaccumJoint);
    AZ_Assert(joint, "No joint found");

    sceneInterface->RemoveJoint(defaultSceneHandle, m_vaccumJoint);
    m_vaccumJoint = AzPhysics::InvalidJointHandle;
  }
  ImGui::End();
}

} // namespace TestScene