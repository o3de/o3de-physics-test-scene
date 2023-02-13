/*
* Copyright (c) Contributors to the Open 3D Engine Project.
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/
#pragma once

#include <AzCore/Component/Component.h>

#include <ImGuiBus.h>
namespace TestScene
{
//! Component responsible for storing counters of apples gathered by Kraken.
class ImGuiJointDemo
   : public AZ::Component
    , public ImGui::ImGuiUpdateListenerBus::Handler
{
public:
   AZ_COMPONENT(ImGuiJointDemo, "{12640e16-461e-450d-a1fa-88b6435fb8ab}", AZ::Component);
   ImGuiJointDemo() = default;
   ~ImGuiJointDemo() = default;
   void Activate() override;
   void Deactivate() override;
   static void Reflect(AZ::ReflectContext* context);

private:
  void OnImGuiUpdate() override;

  float m_velocitySet{0.2};
  float m_forceSet{5};
};
} // namespace AppleKraken