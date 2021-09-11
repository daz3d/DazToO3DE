
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <DazToO3DE/DazToO3DEBus.h>

namespace DazToO3DE
{
    class DazToO3DESystemComponent
        : public AZ::Component
        , protected DazToO3DERequestBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(DazToO3DESystemComponent, "{3c41e385-3360-4022-a69a-c679c105e9a5}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        DazToO3DESystemComponent();
        ~DazToO3DESystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // DazToO3DERequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZTickBus interface implementation
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        ////////////////////////////////////////////////////////////////////////
    };

} // namespace DazToO3DE
