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

class ImGuiVaccumGripperTest : public AZ::Component,
                        public ImGui::ImGuiUpdateListenerBus::Handler {
public:
 AZ_COMPONENT(ImGuiVaccumGripperTest, "{ce95c756-0c2c-11ee-be56-0242ac120002}",
              AZ::Component);
 ImGuiVaccumGripperTest() = default;
 ~ImGuiVaccumGripperTest() = default;
 void Activate() override;
 void Deactivate() override;
 static void Reflect(AZ::ReflectContext *context);

private:
 void OnImGuiUpdate() override;
  AZ::EntityId m_grippedObject;
  AzPhysics::JointHandle m_vaccumJoint;
};
} // namespace TestScene