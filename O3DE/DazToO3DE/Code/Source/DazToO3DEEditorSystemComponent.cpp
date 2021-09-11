
#include <AzCore/Serialization/SerializeContext.h>
#include <DazToO3DEEditorSystemComponent.h>

namespace DazToO3DE
{
    void DazToO3DEEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<DazToO3DEEditorSystemComponent, DazToO3DESystemComponent>()
                ->Version(0);
        }
    }

    DazToO3DEEditorSystemComponent::DazToO3DEEditorSystemComponent() = default;

    DazToO3DEEditorSystemComponent::~DazToO3DEEditorSystemComponent() = default;

    void DazToO3DEEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("DazToO3DEEditorService"));
    }

    void DazToO3DEEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("DazToO3DEEditorService"));
    }

    void DazToO3DEEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void DazToO3DEEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void DazToO3DEEditorSystemComponent::Activate()
    {
        DazToO3DESystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void DazToO3DEEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        DazToO3DESystemComponent::Deactivate();
    }

} // namespace DazToO3DE
