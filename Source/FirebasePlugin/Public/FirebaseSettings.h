// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FirebaseSettings.generated.h"

/**
 * Firebase Configuration Method
 */
UENUM(BlueprintType)
enum class EFirebaseConfigMethod : uint8
{
	ManualInput UMETA(DisplayName = "Manual Input"),
	ImportFromFile UMETA(DisplayName = "Import from google-services.json")
};

/**
 * Firebase Authentication Provider Types
 */
UENUM(BlueprintType)
enum class EFirebaseAuthProvider : uint8
{
	Email UMETA(DisplayName = "Email/Password"),
	Google UMETA(DisplayName = "Google Sign-In"),
	Facebook UMETA(DisplayName = "Facebook Login"),
	Anonymous UMETA(DisplayName = "Anonymous"),
	Phone UMETA(DisplayName = "Phone Number")
};

/**
 * Firebase Database Region
 */
UENUM(BlueprintType)
enum class EFirebaseDatabaseRegion : uint8
{
	US_Central UMETA(DisplayName = "US Central (us-central1)"),
	Europe_West UMETA(DisplayName = "Europe West (europe-west1)"),
	Asia_Southeast UMETA(DisplayName = "Asia Southeast (asia-southeast1)")
};

/**
 * Firebase Plugin Settings
 */
UCLASS(config = Engine, defaultconfig)
class FIREBASEPLUGIN_API UFirebaseSettings : public UObject
{
	GENERATED_BODY()

public:
	UFirebaseSettings(const FObjectInitializer& ObjectInitializer);

	// === CONFIGURATION METHOD ===
	
	/** Choose how to configure Firebase settings */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Setup", 
		meta = (DisplayName = "Configuration Method"))
	EFirebaseConfigMethod ConfigurationMethod = EFirebaseConfigMethod::ManualInput;

	/** Path to google-services.json file (for ImportFromFile method) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Setup", 
		meta = (DisplayName = "google-services.json Path", 
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ImportFromFile",
		FilePathFilter = "json"))
	FFilePath GoogleServicesJsonPath;

	/** Automatically copy google-services.json to Build folder on import */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Setup", 
		meta = (DisplayName = "Auto-copy to Build Folder", 
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ImportFromFile"))
	bool bAutoCopyToBuildFolder = true;

	// === FIREBASE PROJECT SETTINGS ===
	
	/** Firebase Project ID from Firebase Console */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Project", 
		meta = (DisplayName = "Project ID",
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ManualInput"))
	FString ProjectId;

	/** Firebase Application ID for Android */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Project", 
		meta = (DisplayName = "Android App ID",
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ManualInput"))
	FString AndroidAppId;

	/** Firebase API Key for Android */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Project", 
		meta = (DisplayName = "Android API Key",
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ManualInput"))
	FString AndroidApiKey;

	/** Firebase Database URL */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Project", 
		meta = (DisplayName = "Database URL",
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ManualInput"))
	FString DatabaseUrl;

	/** Firebase Storage Bucket */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Project", 
		meta = (DisplayName = "Storage Bucket",
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ManualInput"))
	FString StorageBucket;

	// === AUTHENTICATION SETTINGS ===

	/** Enable Firebase Authentication */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Enable Authentication"))
	bool bEnableAuthentication = true;

	/** Enabled Authentication Providers */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Enabled Auth Providers", EditCondition = "bEnableAuthentication"))
	TArray<EFirebaseAuthProvider> EnabledAuthProviders;

	/** Google Client ID for Google Sign-In */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Authentication|Google", 
		meta = (DisplayName = "Google Client ID", EditCondition = "bEnableAuthentication"))
	FString GoogleClientId;

	/** Facebook App ID for Facebook Login */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Authentication|Facebook", 
		meta = (DisplayName = "Facebook App ID", EditCondition = "bEnableAuthentication"))
	FString FacebookAppId;

	/** Enable email verification requirement */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Require Email Verification", EditCondition = "bEnableAuthentication"))
	bool bRequireEmailVerification = false;

	/** Auto sign-in on app start (if credentials saved) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Auto Sign-In", EditCondition = "bEnableAuthentication"))
	bool bAutoSignIn = true;

	// === REALTIME DATABASE SETTINGS ===

	/** Enable Realtime Database */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Database", 
		meta = (DisplayName = "Enable Realtime Database"))
	bool bEnableRealtimeDatabase = true;

	/** Database Region */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Database", 
		meta = (DisplayName = "Database Region", EditCondition = "bEnableRealtimeDatabase"))
	EFirebaseDatabaseRegion DatabaseRegion = EFirebaseDatabaseRegion::US_Central;

	/** Enable offline persistence */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Database", 
		meta = (DisplayName = "Enable Offline Persistence", EditCondition = "bEnableRealtimeDatabase"))
	bool bEnableOfflinePersistence = true;

	/** Cache size in MB for offline persistence */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Database", 
		meta = (DisplayName = "Cache Size (MB)", EditCondition = "bEnableOfflinePersistence", ClampMin = "1", ClampMax = "100"))
	int32 CacheSizeMB = 10;

	/** Enable automatic reconnection */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Database", 
		meta = (DisplayName = "Auto Reconnect", EditCondition = "bEnableRealtimeDatabase"))
	bool bAutoReconnect = true;

	// === SECURITY & PRIVACY ===

	/** Enable SSL/TLS Certificate Pinning */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Security", 
		meta = (DisplayName = "Enable SSL Pinning"))
	bool bEnableSSLPinning = true;

	/** Timeout for network operations (seconds) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Security", 
		meta = (DisplayName = "Network Timeout (Seconds)", ClampMin = "5", ClampMax = "60"))
	int32 NetworkTimeoutSeconds = 15;

	// === DEBUG SETTINGS ===

	/** Enable verbose logging */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Debug", 
		meta = (DisplayName = "Enable Verbose Logging"))
	bool bEnableVerboseLogging = false;

	/** Enable Firebase Analytics */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Analytics", 
		meta = (DisplayName = "Enable Analytics"))
	bool bEnableAnalytics = false;

	// === PLATFORM SETTINGS ===

	/** Use REST API on non-Android platforms (iOS, Windows, Mac, Linux) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Platform", 
		meta = (DisplayName = "Use REST API for Cross-Platform Support"))
	bool bUseRestApiForNonAndroid = true;

	/** Polling interval for REST API real-time listeners (seconds) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Platform", 
		meta = (DisplayName = "REST API Polling Interval (Seconds)", 
		EditCondition = "bUseRestApiForNonAndroid", ClampMin = "1.0", ClampMax = "60.0"))
	float RestApiPollingInterval = 5.0f;

	/** Messaging Sender ID (for Cloud Messaging) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Firebase|Project", 
		meta = (DisplayName = "Messaging Sender ID",
		EditCondition = "ConfigurationMethod == EFirebaseConfigMethod::ManualInput"))
	FString MessagingSenderId;

	// === HELPER FUNCTIONS ===

	/** Get full database URL with project ID */
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	FString GetFullDatabaseUrl() const;

	/** Check if authentication is properly configured */
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	bool IsAuthenticationConfigured() const;

	/** Check if database is properly configured */
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	bool IsDatabaseConfigured() const;

	/** Validate all Firebase settings */
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	bool ValidateSettings(FString& OutErrorMessage) const;

	/** Import settings from google-services.json file */
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	bool ImportFromGoogleServicesJson(const FString& FilePath, FString& OutErrorMessage);

	/** Copy google-services.json to Build folder for Android packaging */
	UFUNCTION(BlueprintCallable, Category = "Firebase")
	bool CopyGoogleServicesJsonToBuildFolder(FString& OutErrorMessage);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
