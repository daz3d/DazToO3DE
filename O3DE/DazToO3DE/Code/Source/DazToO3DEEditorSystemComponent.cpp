
#include <AzCore/Serialization/SerializeContext.h>
#include <DazToO3DEEditorSystemComponent.h>
#include <QAction>
#include <filesystem>
#include <ShlObj.h>
#include <string>
#include <AzCore/Utils/Utils.h>
#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <AzToolsFramework/Component/EditorComponentAPIBus.h>
#include <AzCore/Asset/AssetManagerBus.h>
#include <AzToolsFramework/API/EditorAssetSystemAPI.h>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace std;

namespace DazToO3DE
{
    std::vector<std::string> s_exportsList = std::vector<std::string>();
    std::vector<std::string> s_materialNames = std::vector<std::string>();
    std::vector<std::string> s_pendingMaterials = std::vector<std::string>();
	
    string s_importedFbxName = "";

    bool s_waitingForFbx = false;
    bool s_waitingForMaterials = false;
	
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
        //AZ::TickBus::Handler::BusConnect();
    }

    void DazToO3DEEditorSystemComponent::Deactivate()
    {
        //AZ::TickBus::Handler::BusDisconnect();
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
                    s_waitingForFbx = false;
                    s_waitingForMaterials = false;
                    if (CopyDazAssetToProject())
                    {
                        s_waitingForFbx = true;
	                    AZ::TickBus::Handler::BusConnect();
                    }
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

	DazToO3DEEditorSystemComponent::AssetStatus CheckAssetStatus(AZStd::string assetPath)
    {
	    typedef AzToolsFramework::AssetSystem::JobInfo AssetJobInfo;
        AZ::Outcome<AZStd::vector<AssetJobInfo>> assetJobsOutcome;
        AzToolsFramework::AssetSystemJobRequestBus::BroadcastResult(
            assetJobsOutcome,
            &AzToolsFramework::AssetSystemJobRequestBus::Handler::GetAssetJobsInfo,
            assetPath,
            false
        );
    	if (assetJobsOutcome.IsSuccess())
    	{
            AZ_Printf("DazToO3DE:FbxStatus:", "Asset Processor job success")
            const auto assetJobInfoList = assetJobsOutcome.GetValue();
            if (assetJobInfoList.size() > 0) {
	            const AssetJobInfo assetJobInfo = assetJobsOutcome.GetValue().front();
                if (assetJobInfo.m_status == AzToolsFramework::AssetSystem::JobStatus::Completed)
                {
                    AZ_Printf("DazToO3DE:FbxStatus:", "Completed")
                    return DazToO3DEEditorSystemComponent::AssetStatus::Completed;
                }
                else if (assetJobInfo.m_status == AzToolsFramework::AssetSystem::JobStatus::Queued
                      || assetJobInfo.m_status == AzToolsFramework::AssetSystem::JobStatus::InProgress)
                {
                    AZ_Printf("DazToO3DE:FbxStatus:", "Pending")
                    return DazToO3DEEditorSystemComponent::AssetStatus::Pending;
                }
                else
                {
		            AZ_Printf("DazToO3DE:FbxStatus:", "Asset Processor job failed")
		            return DazToO3DEEditorSystemComponent::AssetStatus::Failed;
                }
            }
    	}
        else
        {
            AZ_Printf("DazToO3DE:FbxStatus:", "Asset Processor job failed")
            return DazToO3DEEditorSystemComponent::AssetStatus::Failed;
        }

    	AZ_Printf("DazToO3DE:FbxStatus:", "Asset Processor job NONE")
        return DazToO3DEEditorSystemComponent::AssetStatus::None;
    }

	string GetImportedFbxName()
    {
        if (!s_importedFbxName.empty())
        {
            return s_importedFbxName;
        }
		
	    // Project folder path and actor asset directory path
		const string projPath = AZ::Utils::GetProjectPath().c_str();
        const string assetDirRelPath = "/Assets/DazImports/" + s_exportsList.front();

    	// Get asset actor name and path used for setting actor component
    	for (auto const& fileEntry: filesystem::directory_iterator(projPath + assetDirRelPath))
		{
			if (auto assetName = fileEntry.path().filename().string();
                assetName.find(".fbx") != std::string::npos)
    		{
                s_importedFbxName = assetName;
                break;
    		}
    	}
        return s_importedFbxName;
    }

	void DazToO3DEEditorSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
	{
        AZ_Printf("DazToO3DE:OnTick:", "Running")

		if(s_waitingForFbx == true)
		{
			string assetDirPath = "Assets/DazImports/" + s_exportsList.front();
            auto assetStatus = CheckAssetStatus((assetDirPath + "/" + GetImportedFbxName()).c_str());
            if (assetStatus == Completed)
            {
                s_waitingForFbx = false;
	            AZ::TickBus::Handler::BusDisconnect();
                SetupAssetEntity();
                if (CreateMaterialAssets())
                {
                    s_waitingForMaterials = true;
					AZ::TickBus::Handler::BusConnect();
                }
            }
            else if (assetStatus == Pending)
            {
	            // Wait until the processing is done.
            }
            else if (assetStatus == Failed)
            {
                s_waitingForFbx = false;
	            AZ::TickBus::Handler::BusDisconnect();
            }
		}
		if (s_waitingForMaterials == true)
		{
			if (s_pendingMaterials.empty())
			{
                s_waitingForMaterials = false;
	            AZ::TickBus::Handler::BusDisconnect();
                UpdateMaterialComponent();
			}
            else
            {
                std::vector<int> indexesToRemove = std::vector<int>();
				string assetDirPath = "Assets/DazImports/" + s_exportsList.front();
		        for (int i = 0; i < s_pendingMaterials.size(); ++i)
		        {
	                auto matPath = (assetDirPath + "/Materials/" + s_pendingMaterials[i] + ".material").c_str();
                    auto matStatus = CheckAssetStatus(matPath);
		            if (matStatus == Completed)
		            {
                        indexesToRemove.push_back(i);
		            }
                    else if (matStatus == Pending)
                    {
                    	// Wait until processing is finished
                        continue;
                    }
                    else if (matStatus == Failed)
                    {
                        indexesToRemove.clear();
		                s_waitingForMaterials = false;
			            AZ::TickBus::Handler::BusDisconnect();
                        break;
                    }
		        }
            	for (int i=0; i < indexesToRemove.size(); ++i)
            	{
                    s_pendingMaterials.erase(s_pendingMaterials.begin() + indexesToRemove[i] - i);
            	}
                indexesToRemove.clear();
            }
		}
		if (s_waitingForFbx == false && s_waitingForMaterials == false)
		{
			AZ::TickBus::Handler::BusDisconnect();
		}
	}

	bool DazToO3DEEditorSystemComponent::CopyDazAssetToProject() const
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
    	if (!filesystem::exists(fromPath))
    	{
            AZ_Printf("DazToO3DE", "Unable to find exported assets from Daz Studio.")
            return false;
    	}

    	// Get list of files from the source path
        for (auto const& fromDirEntry : filesystem::directory_iterator(fromPath))
        {
            auto fileName = fromDirEntry.path().filename().string();
            s_exportsList.push_back(fileName);
    		AZ_Printf("DazToO3DE:FromPathFile", fileName.c_str())
        }

    	// Create DazImports folder if it does not exists
    	if (!filesystem::exists(toPath))
    	{
            filesystem::create_directory(toPath);
    	}

    	// Delete the contents of the destination folder
    	for (auto const& toDirEntry: filesystem::directory_iterator(toPath))
    	{
            auto fileName = toDirEntry.path().filename().string();
    		AZ_Printf("DazToO3DE:ToPathFile", fileName.c_str())

    		if(std::find( s_exportsList.begin(), s_exportsList.end(), fileName)
               != s_exportsList.end())
    		{
    			filesystem::remove_all(toDirEntry.path());
    		}
    	}

		// Clear imported fbx name
        s_importedFbxName = "";

    	// Copy assets from intermediate directory to the project directory
        filesystem::copy(fromPath, toPath, copyOptions, errorCode);
    	
        AZ_Printf("DazToO3DE", errorCode.message().c_str())

        return true;
	}

	void DazToO3DEEditorSystemComponent::SetupAssetEntity()
	{	
    	// Temporary way to enable component property changes in slice mode
    	// https://github.com/o3de/o3de/issues/4490
    	AzToolsFramework::ScopedUndoBatch undoBatch("TEST");
    	undoBatch.MarkEntityDirty(m_entityId);
    	
    	// Create Entity for the imported asset
        AzToolsFramework::ToolsApplicationRequestBus::BroadcastResult(
            m_entityId,
            &AzToolsFramework::ToolsApplicationRequestBus::Handler::CreateNewEntity,
            AZ::EntityId()
        );

        // TODO:: Hard coded EditorActorComponent ID
        const char actorIdStr[40] = "{A863EE1B-8CFD-4EDD-BA0D-1CEC2879AD44}";
        auto actorId = AZ::Uuid(actorIdStr);
        
        // Add Actor Component
        AzToolsFramework::EditorComponentAPIRequests::AddComponentsOutcome actorOutcome;
        AzToolsFramework::EditorComponentAPIBus::BroadcastResult(
            actorOutcome,
            &AzToolsFramework::EditorComponentAPIBus::Handler::AddComponentOfType,
            m_entityId,
            actorId
        );

    	// Check if there are imported assets
        if (s_exportsList.empty())
        {
        	AZ_Printf( "DazToO3DE", "No imported assets found.")
            return;
        }
    	
    	// Project folder path and actor asset directory path
		const string projPath = AZ::Utils::GetProjectPath().c_str();
        const string assetDirRelPath = "/Assets/DazImports/" + s_exportsList.front();

    	// Get asset actor name and path used for setting actor component
        string actorAssetName;
    	for (auto const& fileEntry: filesystem::directory_iterator(projPath + assetDirRelPath))
		{
			if (auto assetName = fileEntry.path().filename().string();
                assetName.find(".fbx") != std::string::npos)
    		{
                actorAssetName = assetName;
                break;
    		}
    	}
        actorAssetName = actorAssetName.substr(0, actorAssetName.size() - 4) + ".actor";
        // Convert actor name to lowercase
    	for (char& i : actorAssetName)
        {
	        i = tolower(i);
    	}
    	
    	// Get asset ID using actor asset path
        const char* assetPath = (assetDirRelPath + "/" + actorAssetName).c_str();
        AZ::Data::AssetType assetType;
        AZ::Data::AssetId actorAssetId;
        AZ::Data::AssetCatalogRequestBus::BroadcastResult(
            actorAssetId,
            &AZ::Data::AssetCatalogRequestBus::Handler::GetAssetIdByPath,
            assetPath,
            assetType,
            false
        );    	
    	
    	// Set Actor asset Property on the Actor
        AzToolsFramework::EditorComponentAPIRequests::PropertyOutcome propertyOutcome;
        AZ::EntityComponentIdPair actorComponent = actorOutcome.GetValue().front();
        auto propertyPath = AZStd::string_view("Actor asset");
        AzToolsFramework::EditorComponentAPIBus::BroadcastResult(
            propertyOutcome,
            &AzToolsFramework::EditorComponentAPIBus::Handler::SetComponentProperty,
            actorComponent,
            propertyPath,
            AZStd::any{actorAssetId}
        );

    	// Set Draw Skeleton Property on the Actor
        propertyPath = AZStd::string_view("Render options|Draw skeleton");
        AzToolsFramework::EditorComponentAPIBus::BroadcastResult(
            propertyOutcome,
            &AzToolsFramework::EditorComponentAPIBus::Handler::SetComponentProperty,
            actorComponent,
            propertyPath,
            AZStd::any{true}
        );
    	
    	// TODO:: Hard coded EditorMaterialComponent ID 
        const char materialIdStr[40] = "{02B60E9D-470B-447D-A6EE-7D635B154183}";
        auto materialId = AZ::Uuid(materialIdStr);
        
        // Add Material Component
        AzToolsFramework::EditorComponentAPIRequests::AddComponentsOutcome materialOutcome;
        AzToolsFramework::EditorComponentAPIBus::BroadcastResult(
            materialOutcome,
            &AzToolsFramework::EditorComponentAPIBus::Handler::AddComponentOfType,
            m_entityId,
            materialId
        );
	}
    
	void ConvertColor(const char* hexColor, double* rgbColor)
    {
	    // Takes a hex rgb string (e.g. #ffffff) and returns an RGB float array.
        int r;
        int g;
    	int b;
        sscanf_s(hexColor, "#%02x%02x%02x", &r, &g, &b);
    	
        rgbColor[0] = static_cast<double>(r) / 255;
    	rgbColor[1] = static_cast<double>(g) / 255;
    	rgbColor[2] = static_cast<double>(b) / 255;
        rgbColor[3] = 1.0;
    }

	bool DazToO3DEEditorSystemComponent::CreateMaterialAssets() const
	{
		// Material property mapping from daz to o3de
        std::map<string, string> matPropMap;
        matPropMap["Diffuse Color"] = "baseColor";
        matPropMap["Glossy Roughness"] = "roughness";
        matPropMap["Specular Lobe 1 Roughness"] = "roughness";
        matPropMap["Normal Map"] = "normal";
        matPropMap["Cutout Opacity"] = "opacity";
        matPropMap["Refraction Weight"] = "opacity";
        matPropMap["Metallic Weight"] = "metallic";
		
		// Project folder path and asset directory path
		const string projPath = AZ::Utils::GetProjectPath().c_str();
        const string assetDirRelPath = "/Assets/DazImports/" + s_exportsList.front();
        
		// Material folder path
        QString matDirPath = (projPath + assetDirRelPath + "/Materials").c_str();
    	if (!QDir(matDirPath).exists()) QDir().mkdir(matDirPath);

		// Get dtu json file path
        QString dtuPath;
		for (auto const& fileEntry: filesystem::directory_iterator(projPath + assetDirRelPath))
		{
			if (auto dtuFile = fileEntry.path().filename().string();
                dtuFile.find(".dtu") != std::string::npos)
    		{
                dtuPath = (projPath + assetDirRelPath + "/" + dtuFile).c_str();
                break;
    		}
    	}
        QFile dtuFile(dtuPath);
        // Verify if dtu exists
    	if (!dtuFile.open( QIODevice::ReadOnly ))
    	{
            AZ_Printf("DazToO3DE:DtuFile", "File Open failed")
            return false;
    	}
		// Read dtu json
		// TODO: Use rapidjson for dtu reading to keep things consistent
    	QByteArray dtuData = dtuFile.readAll();
        QJsonDocument dtuJsonDoc(QJsonDocument::fromJson(dtuData));

		// Get material info from dtu and create o3de material files
        s_materialNames.clear();
        s_pendingMaterials.clear();
        QJsonArray materialArray = dtuJsonDoc["Materials"].toArray();
    	for (int matIndex = 0; matIndex < materialArray.size(); ++matIndex)
    	{
    		// Get material name from dtu
            QJsonObject matObject = materialArray[matIndex].toObject();
            auto matName = matObject["Material Name"].toString();

    		// Material file for o3de
    		QString matPath = matDirPath + "/" + matName + ".material";
            QFile matFile(matPath);
    		if (!matFile.open(QIODevice::WriteOnly))
    		{
	            AZ_Printf("DazToO3DE:DtuFile", "Material file creation failed")
	            return false;
    		}

    		// Jsonbuffer to store o3de material json data
            rapidjson::StringBuffer matJsonBuffer;
            rapidjson::PrettyWriter matJsonWriter(matJsonBuffer);

            matJsonWriter.StartObject();
            matJsonWriter.Key("description");
            matJsonWriter.String("");
    		matJsonWriter.Key("materialType");
            matJsonWriter.String("Materials/Types/StandardPBR.materialtype");
    		matJsonWriter.Key("parentMaterial");
            matJsonWriter.String("");
    		matJsonWriter.Key("propertyLayoutVersion");
            matJsonWriter.Int(3);
    		
            matJsonWriter.Key("properties");
            matJsonWriter.StartObject();

    		matJsonWriter.Key("emissive");
    		matJsonWriter.StartObject();
            double emissiveColor[4];
            ConvertColor("#000000", emissiveColor);
            matJsonWriter.Key("color");
            matJsonWriter.StartArray();
    		for (int i = 0; i < 4; ++i)
				matJsonWriter.Double(emissiveColor[i]);
            matJsonWriter.EndArray();
            matJsonWriter.EndObject();

            matJsonWriter.Key("opacity");
            matJsonWriter.StartObject();
            matJsonWriter.Key("factor");
            matJsonWriter.Double(1.0);
            matJsonWriter.EndObject();

            bool hasMetallicWeight = false;
            QJsonArray matPropArray = matObject["Properties"].toArray();
    		for (int propIndex = 0; propIndex < matPropArray.size(); ++propIndex)
    		{
                QJsonObject matPropObj = matPropArray[propIndex].toObject();
                auto matPropName = matPropObj["Name"].toString().toStdString();
    			if (matPropMap.find(matPropName) == matPropMap.end())
    			{
    				// Could not find the property in the property map
    				continue;
    			}

                auto targetPropName = matPropMap[matPropName];
                if(string jsonContent = matJsonBuffer.GetString(); 
                   jsonContent.find( targetPropName ) != string::npos)
    			{
    				// Property already exists
                    continue;
    			}
    			
                matJsonWriter.Key(targetPropName.c_str());
                matJsonWriter.StartObject();

                if (auto matPropDataType = matPropObj["Data Type"].toString().toStdString();
                    matPropDataType == "Color")
    			{
                    auto colorValue = matPropObj["Value"].toString().toStdString();
                    double rgbColor[4];
                    ConvertColor(colorValue.c_str(), rgbColor);
                    matJsonWriter.Key("color");
                    matJsonWriter.StartArray();
                    for (int i = 0; i < 4; ++i)
                        matJsonWriter.Double(rgbColor[i]);
                    matJsonWriter.EndArray();
    			}

    			auto matPropTexture = matPropObj["Texture"].toString().toStdString();
    			if (!matPropTexture.empty())
    			{
                    matPropTexture = "Assets/DazImports/" + matPropTexture;
                    matJsonWriter.Key("textureMap");
                    matJsonWriter.String(matPropTexture.c_str());

    				if (matPropName == "Normal Map")
    				{
	                    matJsonWriter.Key("flipY");
	                    matJsonWriter.Bool(true);
    				}
    				
    				if (matPropName == "Cutout Opacity")
    				{
	                    matJsonWriter.Key("alphaSource");
	                    matJsonWriter.String("Split");
	                    matJsonWriter.Key("doubleSided");
	                    matJsonWriter.Bool(true);
	                    matJsonWriter.Key("mode");
	                    matJsonWriter.String("Cutout");
    				}
    			}

    			if(matPropName == "Refraction Weight")
    			{
	                if (auto propValue = matPropObj["Value"].toDouble(); propValue > 0)
    				{
	                    matJsonWriter.Key("alphaSource");
	                    matJsonWriter.String("None");
	                    matJsonWriter.Key("doubleSided");
	                    matJsonWriter.Bool(true);
	                    matJsonWriter.Key("mode");
	                    matJsonWriter.String("Blended");
	                    matJsonWriter.Key("factor");
    					matJsonWriter.Double(1 - propValue);
    				}
    			}

    			if(matPropName == "Metallic Weight")
    			{
	                if (auto propValue = matPropObj["Value"].toDouble(); propValue > 0)
    				{
	                    matJsonWriter.Key("factor");
    					matJsonWriter.Double(propValue);
                        hasMetallicWeight = true;
    				}
    			}

                matJsonWriter.EndObject();
    		}

            matJsonWriter.Key("specularF0");
            matJsonWriter.StartObject();
    		matJsonWriter.Key("factor");
            matJsonWriter.Double(hasMetallicWeight? 1.0 : 0.02);
            matJsonWriter.EndObject();
    		
            matJsonWriter.EndObject(); // "properties"
    		matJsonWriter.EndObject(); // root

    		// write json data to the material file
            QByteArray matJsonData;
            matJsonData.append(matJsonBuffer.GetString());
            matFile.write(matJsonData);

    		// Check if the material name already exists in the vector before pushing
            auto matNameStdStr = matName.toStdString();
            if(std::find(s_materialNames.begin(), s_materialNames.end(), matNameStdStr)
               == s_materialNames.end())
            {
                s_materialNames.push_back(matNameStdStr);
                s_pendingMaterials.push_back(matNameStdStr);
			}            
    	}

		// Sort material names
        std::sort(s_materialNames.begin(), s_materialNames.end());
        return true;
	}

	void DazToO3DEEditorSystemComponent::UpdateMaterialComponent() const
	{
    	// TODO:: Hard coded EditorMaterialComponent ID 
        const char materialIdStr[40] = "{02B60E9D-470B-447D-A6EE-7D635B154183}";
        auto materialId = AZ::Uuid(materialIdStr);

		// Get material component from the entity
        AzToolsFramework::EditorComponentAPIRequests::GetComponentOutcome getComponentOutcome;
        AzToolsFramework::EditorComponentAPIBus::BroadcastResult(
			getComponentOutcome,
            &AzToolsFramework::EditorComponentAPIBus::Handler::GetComponentOfType,
            m_entityId,
            materialId
        );
        AZ::EntityComponentIdPair materialComponent = getComponentOutcome.GetValue();

		// Get materials directory path
        string matDirPath = "Assets/DazImports/" + s_exportsList.front() + "/Materials/";
		
		// Loop through loaded materialNames and assign each material slot with corresponding material asset
		// TODO: Correct way is to get EditorMaterialComponentSlot vector from the material component and assign materials.
		for (int i=0; i < s_materialNames.size(); ++i)
		{
    		// Get asset ID using path
	        const string assetPath = matDirPath + s_materialNames[i] + ".azmaterial";
	        AZ::Data::AssetType assetType;
	        AZ::Data::AssetId materialAssetId;
	        AZ::Data::AssetCatalogRequestBus::BroadcastResult(
	            materialAssetId,
	            &AZ::Data::AssetCatalogRequestBus::Handler::GetAssetIdByPath,
	            assetPath.c_str(),
	            assetType,
	            false
	        );

			// Set material asset for current indexed material slot
    		AZ::Outcome<AZStd::any, AZStd::string> propertyOutcome;
            std::string matAsset = "Model Materials|[" + std::to_string(i) + "]|Material Asset";
            auto propertyPath = AZStd::string_view(matAsset.c_str());
    		AzToolsFramework::EditorComponentAPIBus::BroadcastResult(
				propertyOutcome,
	            &AzToolsFramework::EditorComponentAPIBus::Handler::SetComponentProperty,
	            materialComponent,
	            propertyPath,
	            AZStd::any{materialAssetId}
	        );            
		}
	}

} // namespace DazToO3DE
