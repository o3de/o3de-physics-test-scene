/*
* Copyright (c) Contributors to the Open 3D Engine Project.
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/

#include "PhysXJointTestComponent.h"


#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Components/TransformComponent.h>

#include <HingeJointComponent.h>
#include <PrismaticJointComponent.h>
#include <PhysX/Joint/PhysXJointRequestsBus.h>
#include <PhysX/ArticulationJointBus.h>
#include <PhysX/ArticulationSensorBus.h>
#include <ArticulationLinkComponent.h>
#include <PhysX/ArticulationTypes.h>
#include <imgui/imgui.h>

namespace TestScene
{
  void ImGuiJointDemo::Activate()
  {
    ImGui::ImGuiUpdateListenerBus::Handler::BusConnect();
  }

  void ImGuiJointDemo::Deactivate()
  {
    ImGui::ImGuiUpdateListenerBus::Handler::BusDisconnect();
  }


  void ImGuiJointDemo::Reflect(AZ::ReflectContext* context)
  {
    if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
    {
      serialize->Class<ImGuiJointDemo, AZ::Component>()
          ->Version(3);

      if (AZ::EditContext* ec = serialize->GetEditContext())
      {
        ec->Class<ImGuiJointDemo>("ImGuiJointDemo", "ImGuiJointDemo")
            ->ClassElement(AZ::Edit::ClassElements::EditorData, "ImGuiJointDemo")
            ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
            ->Attribute(AZ::Edit::Attributes::Category, "TestScene");
      }
    }
  }

  void ImGuiJointDemo::OnImGuiUpdate()
  {
    float velocity{0.0f};
    float position{0.0f};
    AZStd::pair<float, float> limits{0.0f,0.0f};
    auto* componentArticulation = GetEntity()->FindComponent<PhysX::ArticulationLinkComponent>();
    if (componentArticulation)
    {
      ImGui::Begin("SimplePhysXJointDemo-Articulation");

      AZStd::string sectionName = AZStd::string::format("Test ArticulationJointRequests for %s ", GetEntity()->GetName().c_str());
      if (ImGui::CollapsingHeader(sectionName.c_str()))
      {

        const char* items[] = { "Twist", "SwingX", "SwingY", "X", "Y", "Z"};
        static int axis = 0;
        ImGui::Combo("Axis", &axis, items, IM_ARRAYSIZE(items));

        AZStd::pair<float, float> pair{-1, -1};
        PhysX::ArticulationJointRequestBus::EventResult(
            pair, GetEntityId(), &PhysX::ArticulationJointRequests::GetLimit,
            (PhysX::ArticulationJointAxis)axis);
        ImGui::Text("Limits : %f %f", pair.first, pair.second);

        float damping{-1};
        PhysX::ArticulationJointRequestBus::EventResult(
            damping, GetEntityId(),
            &PhysX::ArticulationJointRequests::GetDriveDamping,
            (PhysX::ArticulationJointAxis)axis);
        ImGui::Text("GetDriveDamping : %f", damping);

        float maxforce{-1};
        PhysX::ArticulationJointRequestBus::EventResult(
            maxforce, GetEntityId(),
            &PhysX::ArticulationJointRequests::GetMaxForce,
            (PhysX::ArticulationJointAxis)axis);
        ImGui::Text("GetMaxForce : %f", maxforce);

        bool isAccelerationDrive{false};
        PhysX::ArticulationJointRequestBus::EventResult(
            isAccelerationDrive, GetEntityId(),
            &PhysX::ArticulationJointRequests::IsAccelerationDrive,
            (PhysX::ArticulationJointAxis)axis);
        ImGui::Text("isAccelerationDrive : %d", isAccelerationDrive);

        float getDriveTarget{-1.f};
        PhysX::ArticulationJointRequestBus::EventResult(
            getDriveTarget, GetEntityId(),
            &PhysX::ArticulationJointRequests::GetDriveTarget,
            (PhysX::ArticulationJointAxis)axis);
        ImGui::Text("GetDriveTarget : %f", getDriveTarget);

        float getDriveTargetVelocity{-1.f};
        PhysX::ArticulationJointRequestBus::EventResult(
            getDriveTargetVelocity, GetEntityId(),
            &PhysX::ArticulationJointRequests::GetDriveTargetVelocity,
            (PhysX::ArticulationJointAxis)axis);
        ImGui::Text("GetDriveTargetVelocity : %f", getDriveTargetVelocity);

        float getFrictionCoefficient{-1.f};
        PhysX::ArticulationJointRequestBus::EventResult(
            getFrictionCoefficient, GetEntityId(),
            &PhysX::ArticulationJointRequests::GetFrictionCoefficient);
        ImGui::Text("GetFrictionCoefficient : %f", getDriveTargetVelocity);

        float getMaxJointVelocity{-1.f};
        PhysX::ArticulationJointRequestBus::EventResult(
            getMaxJointVelocity, GetEntityId(),
            &PhysX::ArticulationJointRequests::GetMaxJointVelocity);
        ImGui::Text("GetMaxJointVelocity : %f", getMaxJointVelocity);

        const char* motionTypes[] = { "Locked", "Limited", "Free"};
        static int motiontype = 0;
        ImGui::Combo("motionTypes", &motiontype, motionTypes, IM_ARRAYSIZE(motionTypes));
        ImGui::SameLine();
        if (ImGui::Button("SetMotionType"))
        {
          PhysX::ArticulationJointRequestBus::Event(
              GetEntityId(),
              &PhysX::ArticulationJointRequests::SetMotion,
              (PhysX::ArticulationJointAxis)axis,
              (PhysX::ArticulationJointMotionType)motiontype);
        }

        static float inStiffnes {0};
        ImGui::InputFloat("stiffness", &inStiffnes);
        ImGui::SameLine();
        if (ImGui::Button("SetDriveStiffness"))
        {
          PhysX::ArticulationJointRequestBus::Event(
              GetEntityId(),
              &PhysX::ArticulationJointRequests::SetDriveStiffness,
              (PhysX::ArticulationJointAxis)axis,
              inStiffnes);
        }


        static float inDamping {0};
        ImGui::InputFloat("damping", &inDamping);
        ImGui::SameLine();
        if (ImGui::Button("SetDriveDamping"))
        {
          PhysX::ArticulationJointRequestBus::Event(
              GetEntityId(),
              &PhysX::ArticulationJointRequests::SetDriveDamping,
              (PhysX::ArticulationJointAxis)axis,
              inDamping);
        }

        static float inMaxForce {0};
        ImGui::InputFloat("maxforce", &inMaxForce);
        ImGui::SameLine();
        if (ImGui::Button("SetMaxForce"))
        {
          PhysX::ArticulationJointRequestBus::Event(
              GetEntityId(),
              &PhysX::ArticulationJointRequests::SetMaxForce,
              (PhysX::ArticulationJointAxis)axis,
              inMaxForce);
        }

        static float inDriveTarget {0};
        ImGui::InputFloat("inDriveTarget", &inDriveTarget);
        ImGui::SameLine();
        if (ImGui::Button("SetDriveTarget"))
        {
          PhysX::ArticulationJointRequestBus::Event(
              GetEntityId(),
              &PhysX::ArticulationJointRequests::SetDriveTarget,
              (PhysX::ArticulationJointAxis)axis,
              inDriveTarget);
        }

        static float inDriveTargetVelocity {0};
        ImGui::InputFloat("inDriveTargetVelocity", &inDriveTargetVelocity);
        ImGui::SameLine();
        if (ImGui::Button("SetDriveTargetVelocity"))
        {
          PhysX::ArticulationJointRequestBus::Event(
              GetEntityId(),
              &PhysX::ArticulationJointRequests::SetDriveTargetVelocity,
              (PhysX::ArticulationJointAxis)axis,
              inDriveTargetVelocity);
        }

        static float inMaxVelocity {0};
        ImGui::InputFloat("inMaxVelocity", &inMaxVelocity);
        ImGui::SameLine();
        if (ImGui::Button("SetMaxJointVelocity"))
        {
          PhysX::ArticulationJointRequestBus::Event(
              GetEntityId(),
              &PhysX::ArticulationJointRequests::SetMaxJointVelocity,
              inMaxVelocity);
        }

      }


      ImGui::End();
    }


    auto* component1 = GetEntity()->FindComponent<PhysX::HingeJointComponent>();
    auto* component2 = GetEntity()->FindComponent<PhysX::PrismaticJointComponent>();
    auto componentId (AZ::InvalidComponentId);
    if (component1){
      componentId = component1->GetId();
    }
    if (component2){
      componentId = component2->GetId();
    }
    if (componentId != AZ::InvalidComponentId) {
      const AZ::EntityComponentIdPair id(GetEntityId(), componentId);
      PhysX::JointRequestBus::EventResult(velocity, id,
                                          &PhysX::JointRequests::GetVelocity);
      PhysX::JointRequestBus::EventResult(position, id,
                                          &PhysX::JointRequests::GetPosition);
      PhysX::JointRequestBus::EventResult(limits, id,
                                          &PhysX::JointRequests::GetLimits);

      ImGui::Begin("SimplePhysXJointDemo");

      AZStd::string group_name = "Joint " + GetEntity()->GetName();

      ImGui::Text("%s", GetEntity()->GetName().c_str());

      ImGui::SliderFloat(
          AZStd::string("Position##" + GetEntity()->GetName()).c_str(),
          &position, limits.first, limits.second);
      ImGui::SliderFloat(
          AZStd::string("Velocity##" + GetEntity()->GetName()).c_str(),
          &velocity, -0.5f, 0.5f);

      ImGui::InputFloat(
          AZStd::string("Force##" + GetEntity()->GetName()).c_str(),
          &m_forceSet);
      ImGui::SliderFloat(
          AZStd::string("Speed##" + GetEntity()->GetName()).c_str(),
          &m_velocitySet, -1.5f, 1.5f);

      float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
      ImGui::PushButtonRepeat(true);
      float commandVel = 0;
      if (ImGui::ArrowButton(
              AZStd::string("##left" + GetEntity()->GetName()).c_str(),
              ImGuiDir_Left)) {
        commandVel = -m_velocitySet;
      }
      ImGui::SameLine(0.0f, spacing);
      if (ImGui::ArrowButton(
              AZStd::string("##right" + GetEntity()->GetName()).c_str(),
              ImGuiDir_Right)) {
        commandVel = m_velocitySet;
      }
      ImGui::Text("----------------------------------------------------------");
      PhysX::JointRequestBus::Event(id, &PhysX::JointRequests::SetVelocity,
                                    commandVel);
      PhysX::JointRequestBus::Event(id, &PhysX::JointRequests::SetMaximumForce,
                                    m_forceSet);
    }

    ImGui::End();

  }


}