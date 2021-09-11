
#include <DazToO3DEModuleInterface.h>
#include <DazToO3DEEditorSystemComponent.h>

namespace DazToO3DE
{
    class DazToO3DEEditorModule
        : public DazToO3DEModuleInterface
    {
    public:
        AZ_RTTI(DazToO3DEEditorModule, "{5ab09223-ec2a-4b66-aba4-6527ac5af971}", DazToO3DEModuleInterface);
        AZ_CLASS_ALLOCATOR(DazToO3DEEditorModule, AZ::SystemAllocator, 0);

        DazToO3DEEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                DazToO3DEEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<DazToO3DEEditorSystemComponent>(),
            };
        }
    };
}// namespace DazToO3DE

AZ_DECLARE_MODULE_CLASS(Gem_DazToO3DE, DazToO3DE::DazToO3DEEditorModule)
