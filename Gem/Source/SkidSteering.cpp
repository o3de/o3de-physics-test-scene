/*
* Copyright (c) Contributors to the Open 3D Engine Project.
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
 */

#include "SkidSteering.h"


#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Components/TransformComponent.h>

#include <imgui/imgui.h>
#include <PhysX/Joint/PhysXJointBus.h>
#include <AzFramework/Physics/RigidBodyBus.h>
namespace TestScene
{
void SkidSteeringDemo::Activate()
{
  ImGui::ImGuiUpdateListenerBus::Handler::BusConnect();
}

void SkidSteeringDemo::Deactivate()
{
  ImGui::ImGuiUpdateListenerBus::Handler::BusDisconnect();
}


void SkidSteeringDemo::Reflect(AZ::ReflectContext* context)
{
  if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
  {
    serialize->Class<SkidSteeringDemo, AZ::Component>()
        ->Field("Left Joints", &SkidSteeringDemo::m_leftJoints)
        ->Field("Right Joints", &SkidSteeringDemo::m_rightJoints)
        ->Field("Base link", &SkidSteeringDemo::m_baseLink)
        ->Field("Brick mode", &SkidSteeringDemo::m_brickSkidSteering)

        ->Version(1);

    if (AZ::EditContext* ec = serialize->GetEditContext())
    {
      ec->Class<SkidSteeringDemo>("SkidSteeringDemo", "SkidSteeringDemo")
          ->ClassElement(AZ::Edit::ClassElements::EditorData, "SkidSteeringDemo")
          ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
          ->Attribute(AZ::Edit::Attributes::Category, "TestScene")
          ->DataElement(AZ::Edit::UIHandlers::EntityId, &SkidSteeringDemo::m_leftJoints, "Left joints", "")
          ->DataElement(AZ::Edit::UIHandlers::EntityId, &SkidSteeringDemo::m_rightJoints, "Right joints", "")
          ->DataElement(AZ::Edit::UIHandlers::EntityId, &SkidSteeringDemo::m_baseLink, "Base link", "")
          ->DataElement(AZ::Edit::UIHandlers::Default, &SkidSteeringDemo::m_brickSkidSteering, "Brick mode", "");

    }
  }
}

void setSpeedAndForce(AZ::EntityId id, float force, float speed){
  PhysX::JointInterfaceRequestBus::Event(id, [&](PhysX::JointInterface * joint){
        joint->SetMaximumForce(force);
        joint->SetVelocity(speed);
      });

}
void SkidSteeringDemo::OnImGuiUpdate()
{

  ImGui::Begin("SkidSteeringDemo");

  const float max_speed = 3;
  ImGui::Checkbox("BrickSkiedSteering", &m_brickSkidSteering);
  ImGui::SliderFloat("LinVel", &m_linearVel, -max_speed,max_speed);
  ImGui::SliderFloat("RotVel", &m_rotVel, -max_speed,max_speed);

  if(!m_brickSkidSteering){
      ImGui::InputFloat("MaxForce", &m_maxForce);

      for (auto id : m_leftJoints){
        float command = m_linearVel + m_rotVel;
        setSpeedAndForce(id, m_maxForce ,command);
      }
      for (auto id : m_rightJoints) {
        float command = m_linearVel - m_rotVel;
        setSpeedAndForce(id, m_maxForce, command);
      }
  }else{
    for (auto id : m_leftJoints){
      setSpeedAndForce(id, 0 ,0);
    }
    for (auto id : m_rightJoints) {
      setSpeedAndForce(id, 0, 0);
    }
    AZ::Vector3 linear{m_linearVel,0,0};
    AZ::Vector3 angular{0,0,m_rotVel};

    AZ::Vector3 currentLinearVelocity;
//    AZ::Vector3 currentAngularVelocity;

    Physics::RigidBodyRequestBus::EventResult(currentLinearVelocity, m_baseLink, &Physics::RigidBodyRequests::GetLinearVelocity);
//    Physics::RigidBodyRequestBus::EventResult(currentAngularVelocity, m_baseLink, &Physics::RigidBodyRequests::GetAngularVelocity);


    // Convert local steering to world frame
    AZ::Transform robotTransform;
    AZ::TransformBus::EventResult(robotTransform, m_baseLink, &AZ::TransformBus::Events::GetWorldTM);
    auto transformedLinearVelocity = robotTransform.TransformVector(linear);
    auto transformedAngularVelocity = robotTransform.TransformVector(angular);


    //    // Overwrite control velocities on two axis
    currentLinearVelocity.SetX(transformedLinearVelocity.GetX());
    currentLinearVelocity.SetY(transformedLinearVelocity.GetY());


    // Reapply desired velocities
    Physics::RigidBodyRequestBus::Event(m_baseLink, &Physics::RigidBodyRequests::SetLinearVelocity, currentLinearVelocity);
    Physics::RigidBodyRequestBus::Event(m_baseLink, &Physics::RigidBodyRequests::SetAngularVelocity, transformedAngularVelocity);
  }

  ImGui::End();

}
}