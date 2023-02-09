#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/ComponentBus.h>

namespace TestScene::Utils {

AZ::EntityComponentIdPair GetJoint(const AZ::EntityId entityId);

}