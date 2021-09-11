

#include <DazToO3DEModuleInterface.h>
#include <DazToO3DESystemComponent.h>

namespace DazToO3DE
{
    class DazToO3DEModule
        : public DazToO3DEModuleInterface
    {
    public:
        AZ_RTTI(DazToO3DEModule, "{5ab09223-ec2a-4b66-aba4-6527ac5af971}", DazToO3DEModuleInterface);
        AZ_CLASS_ALLOCATOR(DazToO3DEModule, AZ::SystemAllocator, 0);
    };
}// namespace DazToO3DE

AZ_DECLARE_MODULE_CLASS(Gem_DazToO3DE, DazToO3DE::DazToO3DEModule)
