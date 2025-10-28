// Copyright. All Rights Reserved.

#include "FirebaseSettings.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UFirebaseSettings::UFirebaseSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set default values
	ConfigurationMethod = EFirebaseConfigMethod::ManualInput;
	ProjectId = TEXT("your-project-id");
	AndroidAppId = TEXT("1:1234567890:android:abcdef1234567890");
	AndroidApiKey = TEXT("AIza...");
	DatabaseUrl = TEXT("https://your-project-id-default-rtdb.firebaseio.com");
	StorageBucket = TEXT("your-project-id.appspot.com");
	MessagingSenderId = TEXT("1234567890");

	// Default auth providers
	EnabledAuthProviders.Add(EFirebaseAuthProvider::Email);
	EnabledAuthProviders.Add(EFirebaseAuthProvider::Anonymous);
	
	// Platform settings
	bUseRestApiForNonAndroid = true;
	RestApiPollingInterval = 5.0f;
}

FString UFirebaseSettings::GetFullDatabaseUrl() const
{
	if (!DatabaseUrl.IsEmpty())
	{
		return DatabaseUrl;
	}
	
	if (!ProjectId.IsEmpty())
	{
		// Auto-generate database URL from project ID
		FString Region;
		switch (DatabaseRegion)
		{
			case EFirebaseDatabaseRegion::US_Central:
				Region = TEXT("");
				break;
			case EFirebaseDatabaseRegion::Europe_West:
				Region = TEXT("-europe-west1");
				break;
			case EFirebaseDatabaseRegion::Asia_Southeast:
				Region = TEXT("-asia-southeast1");
				break;
		}
		return FString::Printf(TEXT("https://%s%s.firebaseio.com"), *ProjectId, *Region);
	}
	
	return TEXT("");
}

bool UFirebaseSettings::IsAuthenticationConfigured() const
{
	return bEnableAuthentication && 
		   !ProjectId.IsEmpty() && 
		   !AndroidApiKey.IsEmpty() &&
		   EnabledAuthProviders.Num() > 0;
}

bool UFirebaseSettings::IsDatabaseConfigured() const
{
	return bEnableRealtimeDatabase && 
		   !ProjectId.IsEmpty() && 
		   !GetFullDatabaseUrl().IsEmpty();
}

bool UFirebaseSettings::ValidateSettings(FString& OutErrorMessage) const
{
	if (ProjectId.IsEmpty())
	{
		OutErrorMessage = TEXT("Project ID is required");
		return false;
	}

	if (AndroidAppId.IsEmpty())
	{
		OutErrorMessage = TEXT("Android App ID is required");
		return false;
	}

	if (AndroidApiKey.IsEmpty())
	{
		OutErrorMessage = TEXT("Android API Key is required");
		return false;
	}

	if (bEnableAuthentication && EnabledAuthProviders.Num() == 0)
	{
		OutErrorMessage = TEXT("At least one authentication provider must be enabled");
		return false;
	}

	if (bEnableRealtimeDatabase && GetFullDatabaseUrl().IsEmpty())
	{
		OutErrorMessage = TEXT("Database URL is required when Realtime Database is enabled");
		return false;
	}

	OutErrorMessage = TEXT("Settings are valid");
	return true;
}

bool UFirebaseSettings::ImportFromGoogleServicesJson(const FString& FilePath, FString& OutErrorMessage)
{
	// Read the JSON file
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to read file: %s"), *FilePath);
		return false;
	}

	// Parse JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);
	
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		OutErrorMessage = TEXT("Failed to parse JSON file");
		return false;
	}

	// Extract project info
	TSharedPtr<FJsonObject> ProjectInfo = JsonObject->GetObjectField(TEXT("project_info"));
	if (ProjectInfo.IsValid())
	{
		ProjectId = ProjectInfo->GetStringField(TEXT("project_id"));
		StorageBucket = ProjectInfo->GetStringField(TEXT("storage_bucket"));
		
		// Get database URL from firebase_url if available
		if (ProjectInfo->HasField(TEXT("firebase_url")))
		{
			DatabaseUrl = ProjectInfo->GetStringField(TEXT("firebase_url"));
		}
		else
		{
			// Generate from project ID
			DatabaseUrl = FString::Printf(TEXT("https://%s-default-rtdb.firebaseio.com"), *ProjectId);
		}
	}

	// Extract client info (Android app)
	TArray<TSharedPtr<FJsonValue>> Clients = JsonObject->GetArrayField(TEXT("client"));
	if (Clients.Num() > 0)
	{
		TSharedPtr<FJsonObject> ClientInfo = Clients[0]->AsObject()->GetObjectField(TEXT("client_info"));
		if (ClientInfo.IsValid())
		{
			AndroidAppId = ClientInfo->GetObjectField(TEXT("android_client_info"))->GetStringField(TEXT("mobilesdk_app_id"));
		}

		// Get API key
		TArray<TSharedPtr<FJsonValue>> ApiKeys = Clients[0]->AsObject()->GetArrayField(TEXT("api_key"));
		if (ApiKeys.Num() > 0)
		{
			AndroidApiKey = ApiKeys[0]->AsObject()->GetStringField(TEXT("current_key"));
		}
	}

	// Get project number (messaging sender ID)
	if (ProjectInfo.IsValid() && ProjectInfo->HasField(TEXT("project_number")))
	{
		MessagingSenderId = ProjectInfo->GetStringField(TEXT("project_number"));
	}

	// Update configuration method
	ConfigurationMethod = EFirebaseConfigMethod::ImportFromFile;
	GoogleServicesJsonPath.FilePath = FilePath;

	// Save settings
	SaveConfig();

	OutErrorMessage = TEXT("Successfully imported Firebase configuration");
	return true;
}

bool UFirebaseSettings::CopyGoogleServicesJsonToBuildFolder(FString& OutErrorMessage)
{
	if (GoogleServicesJsonPath.FilePath.IsEmpty())
	{
		OutErrorMessage = TEXT("google-services.json path is not set");
		return false;
	}

	// Get source file path
	FString SourcePath = GoogleServicesJsonPath.FilePath;
	if (!FPaths::FileExists(SourcePath))
	{
		OutErrorMessage = FString::Printf(TEXT("Source file does not exist: %s"), *SourcePath);
		return false;
	}

	// Get project Build folder path
	FString ProjectDir = FPaths::ProjectDir();
	FString BuildDir = FPaths::Combine(ProjectDir, TEXT("Build"), TEXT("Android"));
	FString DestPath = FPaths::Combine(BuildDir, TEXT("google-services.json"));

	// Create Build/Android directory if it doesn't exist
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*BuildDir))
	{
		if (!PlatformFile.CreateDirectoryTree(*BuildDir))
		{
			OutErrorMessage = FString::Printf(TEXT("Failed to create directory: %s"), *BuildDir);
			return false;
		}
	}

	// Copy file
	if (!PlatformFile.CopyFile(*DestPath, *SourcePath))
	{
		OutErrorMessage = FString::Printf(TEXT("Failed to copy file to: %s"), *DestPath);
		return false;
	}

	OutErrorMessage = FString::Printf(TEXT("Successfully copied to: %s"), *DestPath);
	return true;
}

#if WITH_EDITOR
void UFirebaseSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		FName PropertyName = PropertyChangedEvent.Property->GetFName();

		// Auto-import when file path is changed
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UFirebaseSettings, GoogleServicesJsonPath))
		{
			if (!GoogleServicesJsonPath.FilePath.IsEmpty())
			{
				FString ErrorMessage;
				if (ImportFromGoogleServicesJson(GoogleServicesJsonPath.FilePath, ErrorMessage))
				{
					UE_LOG(LogTemp, Log, TEXT("Firebase: %s"), *ErrorMessage);
					
					// Auto-copy if enabled
					if (bAutoCopyToBuildFolder)
					{
						FString CopyMessage;
						CopyGoogleServicesJsonToBuildFolder(CopyMessage);
						UE_LOG(LogTemp, Log, TEXT("Firebase: %s"), *CopyMessage);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Firebase: %s"), *ErrorMessage);
				}
			}
		}
		
		// Auto-copy when toggle is changed
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(UFirebaseSettings, bAutoCopyToBuildFolder))
		{
			if (bAutoCopyToBuildFolder && !GoogleServicesJsonPath.FilePath.IsEmpty())
			{
				FString CopyMessage;
				CopyGoogleServicesJsonToBuildFolder(CopyMessage);
				UE_LOG(LogTemp, Log, TEXT("Firebase: %s"), *CopyMessage);
			}
		}
	}
}
#endif
