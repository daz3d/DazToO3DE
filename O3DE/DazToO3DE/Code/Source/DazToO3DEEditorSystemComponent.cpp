
#include <AzCore/Serialization/SerializeContext.h>
#include <DazToO3DEEditorSystemComponent.h>
#include <QAction>
#include <filesystem>
#include <ShlObj.h>
#include <string>
#include <AzCore/Utils/Utils.h>

using namespace std;

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
        AzToolsFramework::EditorMenuNotificationBus::Handler::BusConnect();
    }

    void DazToO3DEEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorMenuNotificationBus::Handler::BusDisconnect();
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        DazToO3DESystemComponent::Deactivate();
    }
    
	void DazToO3DEEditorSystemComponent::OnPopulateToolMenuItems()
	{
		if(!m_runDazImporterEditorAction)
		{
            m_runDazImporterEditorAction = new QAction("Daz Importer");
            m_runDazImporterEditorAction->setCheckable(false);
            m_runDazImporterEditorAction->setChecked(false);
            QObject::connect(
                m_runDazImporterEditorAction,
                &QAction::triggered,
                m_runDazImporterEditorAction,
                [this]()
                {
                    AZ_Printf("DazToO3DE", "Launch Daz Importer")
                    CopyDazAssetToProject();
                }
            );

            AzToolsFramework::EditorMenuRequestBus::Broadcast(
                &AzToolsFramework::EditorMenuRequestBus::Handler::AddMenuAction,
                "ToolMenu",
                m_runDazImporterEditorAction,
                true
            );
		}
	}
	
    void DazToO3DEEditorSystemComponent::OnResetToolMenuItems()
    {
	    if(m_runDazImporterEditorAction)
	    {
            delete m_runDazImporterEditorAction;
            m_runDazImporterEditorAction = nullptr;
	    }
    }

	void DazToO3DEEditorSystemComponent::CopyDazAssetToProject() const
	{
    	// Project folder path
		const string projPath = AZ::Utils::GetProjectPath().c_str();

    	// User Documents folder path
        wchar_t* docsPath = nullptr;
        SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &docsPath);
        wstring ws(docsPath);
		const string myDocsStr(ws.begin(), ws.end());

		const string fromPath = myDocsStr + "/DAZ 3D/Bridges/Daz To O3DE/Exports";
		const string toPath = projPath + "/Assets/DazImports";
		const filesystem::copy_options copyOptions = filesystem::copy_options::recursive;
        error_code errorCode;

    	// Check if the exported assets from Daz Studio exist
    	if(!filesystem::exists(fromPath))
    	{
            AZ_Printf("DazToO3DE", "Unable to find exported assets from Daz Studio.")
            return;
    	}

    	// Delete the contents of the destination folder
    	for(auto const& dirEntry: filesystem::directory_iterator(toPath))
    	{
    		filesystem::remove_all(dirEntry.path());
    	}

    	// Copy assets from intermediate directory to the project directory
        filesystem::copy(fromPath, toPath, copyOptions, errorCode);
    	
        AZ_Printf("DazToO3DE", errorCode.message().c_str())
	}

} // namespace DazToO3DE
