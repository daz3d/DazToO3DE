
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace DazToO3DE
{
    class DazToO3DERequests
    {
    public:
        AZ_RTTI(DazToO3DERequests, "{87a6c9ee-45b9-420b-a5cb-7f660b338caf}");
        virtual ~DazToO3DERequests() = default;
        // Put your public methods here
    };
    
    class DazToO3DEBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using DazToO3DERequestBus = AZ::EBus<DazToO3DERequests, DazToO3DEBusTraits>;
    using DazToO3DEInterface = AZ::Interface<DazToO3DERequests>;

} // namespace DazToO3DE
