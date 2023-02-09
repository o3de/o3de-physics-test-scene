#include "Utils.h"
#include <HingeJointComponent.h>
#include <PhysX/Joint/PhysXJointRequestsBus.h>
#include <PrismaticJointComponent.h>

namespace TestScene::Utils {

AZ::EntityComponentIdPair GetJoint(const AZ::EntityId entityId) {
  AZ::Entity *entity{nullptr};

  AZ::ComponentApplicationBus::BroadcastResult(
      entity, &AZ::ComponentApplicationRequests::FindEntity, entityId);
  if (!entity) {
    return AZ::EntityComponentIdPair(entityId, AZ::InvalidComponentId);
  }

  auto *component1 = entity->FindComponent<PhysX::HingeJointComponent>();
  auto *component2 = entity->FindComponent<PhysX::PrismaticJointComponent>();
  auto componentId(AZ::InvalidComponentId);
  if (component1) {
    componentId = component1->GetId();
  } else if (component2) {
    componentId = component2->GetId();
  }
  return AZ::EntityComponentIdPair(entityId, componentId);
}
} // namespace TestScene::Utils