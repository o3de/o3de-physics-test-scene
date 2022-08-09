
#pragma once

#include <AzCore/Component/Component.h>

#include <Physics_Test/Physics_TestBus.h>

namespace Physics_Test
{
    class Physics_TestSystemComponent
        : public AZ::Component
        , protected Physics_TestRequestBus::Handler
    {
    public:
        AZ_COMPONENT(Physics_TestSystemComponent, "{3287f44b-6ba5-43e0-ab79-8d1835dce5f7}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        Physics_TestSystemComponent();
        ~Physics_TestSystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // Physics_TestRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
