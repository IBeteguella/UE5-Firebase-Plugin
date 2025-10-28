// Copyright. All Rights Reserved.

#include "FirebaseSettings.h"

UFirebaseSettings::UFirebaseSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set default values
	ProjectId = TEXT("your-project-id");
	AndroidAppId = TEXT("1:1234567890:android:abcdef1234567890");
	AndroidApiKey = TEXT("AIza...");
	DatabaseUrl = TEXT("https://your-project-id-default-rtdb.firebaseio.com");
	StorageBucket = TEXT("your-project-id.appspot.com");

	// Default auth providers
	EnabledAuthProviders.Add(EFirebaseAuthProvider::Email);
	EnabledAuthProviders.Add(EFirebaseAuthProvider::Anonymous);
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
