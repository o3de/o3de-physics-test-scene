
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "Physics_TestSystemComponent.h"
#include "PhysXJointTestComponent.h"
#include "PhysXTestGripper.h"
#include "SkidSteering.h"
#include "PhysXTestVaccumGripper.h"
namespace Physics_Test
{
    class Physics_TestModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(Physics_TestModule, "{8e9ea463-7ca5-4e68-a3b5-a9f5446d4701}", AZ::Module);
        AZ_CLASS_ALLOCATOR(Physics_TestModule, AZ::SystemAllocator, 0);

        Physics_TestModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                Physics_TestSystemComponent::CreateDescriptor(),
                TestScene::ImGuiJointDemo::CreateDescriptor(),
                TestScene::SkidSteeringDemo::CreateDescriptor(),
                TestScene::ImGuiGripperTest::CreateDescriptor(),
                TestScene::ImGuiVaccumGripperTest::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<Physics_TestSystemComponent>(),
            };
        }
    };
}// namespace Physics_Test

AZ_DECLARE_MODULE_CLASS(Gem_Physics_Test, Physics_Test::Physics_TestModule)
