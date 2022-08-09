
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace Physics_Test
{
    class Physics_TestRequests
    {
    public:
        AZ_RTTI(Physics_TestRequests, "{266573ed-e25d-46f3-b5c2-671482cfd824}");
        virtual ~Physics_TestRequests() = default;
        // Put your public methods here
    };

    class Physics_TestBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using Physics_TestRequestBus = AZ::EBus<Physics_TestRequests, Physics_TestBusTraits>;
    using Physics_TestInterface = AZ::Interface<Physics_TestRequests>;

} // namespace Physics_Test
