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

#include <imgui/imgui.h>
#include <PhysX/Joint/PhysXJointBus.h>

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
    float velocity{0};
    float position{0};
    AZStd::pair<float, float> limits{0,0};
    PhysX::JointInterfaceRequestBus::EventResult(velocity,GetEntityId(), &PhysX::JointInterface::GetVelocity);
    PhysX::JointInterfaceRequestBus::EventResult(position,GetEntityId(), &PhysX::JointInterface::GetPosition);
    PhysX::JointInterfaceRequestBus::EventResult(limits,GetEntityId(), &PhysX::JointInterface::GetLimits);

    ImGui::Begin("SimplePhysXJointDemo");

    AZStd::string group_name = "Joint " +GetEntity()->GetName();

    ImGui::Text("%s", GetEntity()->GetName().c_str());


    ImGui::SliderFloat(AZStd::string("Position"+GetEntity()->GetName()).c_str()
                           , &position, limits.first,limits.second);
    ImGui::SliderFloat(AZStd::string("Velocity"+GetEntity()->GetName()).c_str()
                           , &velocity, -5.f , 5.f);

    ImGui::InputFloat(AZStd::string("Force"+GetEntity()->GetName()).c_str(), &m_forceSet);
    ImGui::SliderFloat(AZStd::string("Command"+GetEntity()->GetName()).c_str(), &m_velocitySet, -5.f , 5.f);
    ImGui::SameLine();
    if (ImGui::Button(AZStd::string("Zero"+GetEntity()->GetName()).c_str())){
      m_velocitySet = 0;
    }

    PhysX::JointInterfaceRequestBus::Event(GetEntityId(), &PhysX::JointInterface::SetVelocity, m_velocitySet);
    PhysX::JointInterfaceRequestBus::Event(GetEntityId(), &PhysX::JointInterface::SetMaximumForce, m_forceSet);

    ImGui::End();

  }


}