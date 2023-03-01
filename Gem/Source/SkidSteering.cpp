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

#include <AzFramework/Physics/RigidBodyBus.h>
#include <PhysX/Joint/PhysXJointRequestsBus.h>
#include <HingeJointComponent.h>
#include <imgui/imgui.h>

namespace TestScene
{
void SkidSteeringDemo::Activate()
{
  m_registered = false;
  AZ::TickBus::Handler::BusConnect();
  ImGui::ImGuiUpdateListenerBus::Handler::BusConnect();
  m_rightJointsPairs.clear();
  m_leftJointsPairs.clear();

  m_onSceneActiveSimulatedBodiesEvent = AzPhysics::SceneEvents::OnSceneActiveSimulatedBodiesEvent::Handler(
      [](AzPhysics::SceneHandle sceneHandle, const AzPhysics::SimulatedBodyHandleList& activeBodyList, float deltaTime)
      {
        AZ_Printf("SkidSteeringDemo::OnSceneActiveSimulatedBodiesEvent", "activeBodyList count %d delta time %f", activeBodyList.size(),deltaTime);
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        for (auto bodyHandle : activeBodyList){
          auto * body = sceneInterface->GetSimulatedBodyFromHandle(sceneHandle,bodyHandle);
          AZ_Printf("SkidSteeringDemo::OnSceneActiveSimulatedBodiesEvent", "\t %f",body->GetPosition().GetZ())
        }
      });

  m_onPostsimulateEvent = AzPhysics::SystemEvents::OnPostsimulateEvent::Handler (
      [ ](float deltaTime)
      {
        AZ_Printf("SkidSteeringDemo::OnPostsimulateEvent", "delta time %f %f",deltaTime);
      });




}

void SkidSteeringDemo::Deactivate()
{
  AZ::TickBus::Handler::BusDisconnect();
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
void SkidSteeringDemo::OnTick(float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
{
  AZ_Printf("SkidSteeringDemo::OnTick", "ontick");
  if (!m_registered)
  {
    m_registered = true;
    auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
    auto* systemInterface = AZ::Interface<AzPhysics::SystemInterface>::Get();
    AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
    sceneInterface->RegisterSceneActiveSimulatedBodiesHandler(sceneHandle, m_onSceneActiveSimulatedBodiesEvent);
    systemInterface->RegisterPreSimulateEvent(m_onPostsimulateEvent);

  }

}

void setSpeedAndForce(const AZ::EntityComponentIdPair& id, float force, float speed){

  PhysX::JointRequestBus::Event(id, [&](PhysX::JointRequests *joint) {
    joint->SetMaximumForce(force);
    joint->SetVelocity(speed);
  });
}
void SkidSteeringDemo::OnImGuiUpdate()
{
  AZ_Printf("SkidSteeringDemo", "OnImGuiUpdate");

  if (m_rightJointsPairs.empty() || m_leftJointsPairs.empty()){
    m_rightJointsPairs.clear();
    m_leftJointsPairs.clear();
    auto getComponentIdPair = [&](AZ::EntityId entityId){
      AZ::Entity * entity{nullptr};
      PhysX::HingeJointComponent* component {nullptr};
      AZ::ComponentApplicationBus::BroadcastResult(entity, &AZ::ComponentApplicationRequests::FindEntity, entityId);
      if (entity){
        AZ_Printf("SkidSteeringDemo", "Found entity %s", entity->GetName().c_str());
        component = entity->FindComponent<PhysX::HingeJointComponent>();
      }
      if (component){
        AZ_Printf("SkidSteeringDemo", "Found component %llu", component->GetId());
        return AZ::EntityComponentIdPair(entityId, component->GetId());
      }
      AZ_Printf("SkidSteeringDemo::Activate",  "Nothing found : %s", entityId.ToString().c_str());
      return AZ::EntityComponentIdPair();
    };
    for (const auto & entityId : m_rightJoints )
    {
      m_rightJointsPairs.emplace_back(getComponentIdPair(entityId));
    }
    for (const auto & entityId : m_leftJoints )
    {
      m_leftJointsPairs.emplace_back(getComponentIdPair(entityId));
    }
  }

  ImGui::Begin("SkidSteeringDemo");
  for (auto id : m_leftJointsPairs){
    ImGui::Text("%s %llu", id.GetEntityId().ToString().c_str(), (long long unsigned) id.GetComponentId());
  }
  for (auto id : m_rightJointsPairs){
    ImGui::Text("%s %llu", id.GetEntityId().ToString().c_str(), (long long unsigned) id.GetComponentId());
  }

  const float max_speed = 8;
  ImGui::Checkbox("BrickSkiedSteering", &m_brickSkidSteering);
  ImGui::SliderFloat("LinVel", &m_linearVel, -max_speed,max_speed);
  ImGui::SliderFloat("SideVel", &m_sideVel, -max_speed,max_speed);
  ImGui::SliderFloat("RotVel", &m_rotVel, -max_speed,max_speed);

  if(!m_brickSkidSteering){
      ImGui::InputFloat("MaxForce", &m_maxForce);

      float command;
      

      command = m_linearVel + m_rotVel;
      setSpeedAndForce(m_leftJointsPairs.front(), m_maxForce ,command + m_sideVel);
      setSpeedAndForce(m_leftJointsPairs.back(), m_maxForce ,command - m_sideVel);

      command = m_linearVel - m_rotVel;
      setSpeedAndForce(m_rightJointsPairs.front(), m_maxForce ,command - m_sideVel);
      setSpeedAndForce(m_rightJointsPairs.back(), m_maxForce ,command + m_sideVel);

      /*
      for (auto id : m_leftJointsPairs){

        float command = m_linearVel + m_rotVel;
        setSpeedAndForce(id, m_maxForce ,command);
      }
      for (auto id : m_rightJointsPairs) {
        float command = m_linearVel - m_rotVel;
        setSpeedAndForce(id, m_maxForce, command);
      }
      */
  }else{
    for (auto id : m_leftJointsPairs){
      setSpeedAndForce(id, 0 ,0);
    }
    for (auto id : m_rightJointsPairs) {
      setSpeedAndForce(id, 0, 0);
    }
    AZ::Vector3 linear{m_linearVel,m_sideVel,0};
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