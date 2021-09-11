
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>
#include <DazToO3DESystemComponent.h>

namespace DazToO3DE
{
    class DazToO3DEModuleInterface
        : public AZ::Module
    {
    public:
        AZ_RTTI(DazToO3DEModuleInterface, "{3f961813-5824-4c15-ae46-a0148f883a68}", AZ::Module);
        AZ_CLASS_ALLOCATOR(DazToO3DEModuleInterface, AZ::SystemAllocator, 0);

        DazToO3DEModuleInterface()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                DazToO3DESystemComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<DazToO3DESystemComponent>(),
            };
        }
    };
}// namespace DazToO3DE
