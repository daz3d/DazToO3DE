
#pragma once

#include <DazToO3DESystemComponent.h>

#include <AzToolsFramework/Entity/EditorEntityContextBus.h>
#include <Component/EditorComponentAPIBus.h>
#include <Viewport/ActionBus.h>
#include <AzCore/Component/TickBus.h>

namespace DazToO3DE
{
    /// System component for DazToO3DE editor
    class DazToO3DEEditorSystemComponent
        : public DazToO3DESystemComponent
        , private AzToolsFramework::EditorEvents::Bus::Handler
		, public AzToolsFramework::EditorMenuNotificationBus::Handler
    {
        using BaseSystemComponent = DazToO3DESystemComponent;
    public:
        AZ_COMPONENT(DazToO3DEEditorSystemComponent, "{316c9e4a-3795-4b58-a2a7-9d59715dc7ab}", BaseSystemComponent);
        static void Reflect(AZ::ReflectContext* context);

        DazToO3DEEditorSystemComponent();
        ~DazToO3DEEditorSystemComponent();

    	enum AssetStatus
    	{
    		Completed,
    		Pending,
    		Failed,
    		None
    	};

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

    	// EditorMenuNotificationBus::Handler overrides ...
    	void OnPopulateToolMenuItems() override;
    	void OnResetToolMenuItems() override;

    	// TickBus::Handler overrides ...
    	void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        bool CopyDazAssetToProject() const;
        void SetupAssetEntity();
        bool CreateMaterialAssets() const;
        void UpdateMaterialComponent() const;

        QAction* m_runDazImporterEditorAction = nullptr;

        AZ::EntityId m_entityId;

    };
} // namespace DazToO3DE
