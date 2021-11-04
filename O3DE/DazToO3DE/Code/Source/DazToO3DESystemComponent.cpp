
#include <DazToO3DESystemComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

namespace DazToO3DE
{
    void DazToO3DESystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<DazToO3DESystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<DazToO3DESystemComponent>("DazToO3DE", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void DazToO3DESystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DazToO3DEService"));
    }

    void DazToO3DESystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DazToO3DEService"));
    }

    void DazToO3DESystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void DazToO3DESystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    DazToO3DESystemComponent::DazToO3DESystemComponent()
    {
        if (DazToO3DEInterface::Get() == nullptr)
        {
            DazToO3DEInterface::Register(this);
        }
    }

    DazToO3DESystemComponent::~DazToO3DESystemComponent()
    {
        if (DazToO3DEInterface::Get() == this)
        {
            DazToO3DEInterface::Unregister(this);
        }
    }

    void DazToO3DESystemComponent::Init()
    {
    }

    void DazToO3DESystemComponent::Activate()
    {
        DazToO3DERequestBus::Handler::BusConnect();
        //AZ::TickBus::Handler::BusConnect();
    }

    void DazToO3DESystemComponent::Deactivate()
    {
        //AZ::TickBus::Handler::BusDisconnect();
        DazToO3DERequestBus::Handler::BusDisconnect();
    }

    void DazToO3DESystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace DazToO3DE
