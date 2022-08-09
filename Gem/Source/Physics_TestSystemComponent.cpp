
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

#include "Physics_TestSystemComponent.h"

namespace Physics_Test
{
    void Physics_TestSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<Physics_TestSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<Physics_TestSystemComponent>("Physics_Test", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void Physics_TestSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("Physics_TestService"));
    }

    void Physics_TestSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("Physics_TestService"));
    }

    void Physics_TestSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void Physics_TestSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    Physics_TestSystemComponent::Physics_TestSystemComponent()
    {
        if (Physics_TestInterface::Get() == nullptr)
        {
            Physics_TestInterface::Register(this);
        }
    }

    Physics_TestSystemComponent::~Physics_TestSystemComponent()
    {
        if (Physics_TestInterface::Get() == this)
        {
            Physics_TestInterface::Unregister(this);
        }
    }

    void Physics_TestSystemComponent::Init()
    {
    }

    void Physics_TestSystemComponent::Activate()
    {
        Physics_TestRequestBus::Handler::BusConnect();
    }

    void Physics_TestSystemComponent::Deactivate()
    {
        Physics_TestRequestBus::Handler::BusDisconnect();
    }
}
