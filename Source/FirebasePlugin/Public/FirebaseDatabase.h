// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebaseRestAPI.h"
#include "FirebaseDatabase.generated.h"

/**
 * Firebase Database Operation Result
 */
USTRUCT(BlueprintType)
struct FFirebaseDatabaseResult
{
	GENERATED_BODY()

	/** Whether the operation was successful */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Database")
	bool bSuccess = false;

	/** Retrieved data as JSON string */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Database")
	FString Data;

	/** Error message if failed */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Database")
	FString ErrorMessage;

	/** Database path that was accessed */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Database")
	FString Path;
};

/**
 * Delegate for database operation callbacks
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFirebaseDatabaseComplete, const FFirebaseDatabaseResult&, Result);

/**
 * Delegate for database value change callbacks (real-time updates)
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFirebaseDatabaseValueChanged, const FString&, Path, const FString&, Data);

/**
 * Firebase Realtime Database Blueprint Function Library
 */
UCLASS()
class FIREBASEPLUGIN_API UFirebaseDatabase : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// === WRITE OPERATIONS ===

	/** 
	 * Set data at a specific path (overwrites existing data)
	 * @param Path Database path (e.g., "users/user123/profile")
	 * @param JsonData Data to set as JSON string
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Write", 
		meta = (DisplayName = "Set Value"))
	static void SetValue(const FString& Path, const FString& JsonData, 
		const FOnFirebaseDatabaseComplete& OnComplete);

	/** 
	 * Update data at a specific path (merges with existing data)
	 * @param Path Database path
	 * @param JsonData Data to update as JSON string
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Write", 
		meta = (DisplayName = "Update Value"))
	static void UpdateValue(const FString& Path, const FString& JsonData, 
		const FOnFirebaseDatabaseComplete& OnComplete);

	/** 
	 * Push new data to a list (generates unique key)
	 * @param Path Database path
	 * @param JsonData Data to push as JSON string
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Write", 
		meta = (DisplayName = "Push Value"))
	static void PushValue(const FString& Path, const FString& JsonData, 
		const FOnFirebaseDatabaseComplete& OnComplete);

	/** 
	 * Delete data at a specific path
	 * @param Path Database path
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Write", 
		meta = (DisplayName = "Delete Value"))
	static void DeleteValue(const FString& Path, 
		const FOnFirebaseDatabaseComplete& OnComplete);

	// === READ OPERATIONS ===

	/** 
	 * Get data at a specific path (one-time read)
	 * @param Path Database path
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Read", 
		meta = (DisplayName = "Get Value"))
	static void GetValue(const FString& Path, 
		const FOnFirebaseDatabaseComplete& OnComplete);

	/** 
	 * Listen for data changes at a specific path (real-time updates)
	 * @param Path Database path
	 * @param OnValueChanged Callback when data changes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Read", 
		meta = (DisplayName = "Listen for Value Changes"))
	static void ListenForValueChanges(const FString& Path, 
		const FOnFirebaseDatabaseValueChanged& OnValueChanged);

	/** 
	 * Stop listening for data changes at a specific path
	 * @param Path Database path
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Read", 
		meta = (DisplayName = "Stop Listening"))
	static void StopListening(const FString& Path);

	// === QUERY OPERATIONS ===

	/** 
	 * Query data with ordering and filtering
	 * @param Path Database path
	 * @param OrderByKey Order by key name
	 * @param LimitToFirst Limit to first N results
	 * @param StartAt Start at this value
	 * @param EndAt End at this value
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query", 
		meta = (DisplayName = "Query Values"))
	static void QueryValues(const FString& Path, const FString& OrderByKey, 
		int32 LimitToFirst, const FString& StartAt, const FString& EndAt,
		const FOnFirebaseDatabaseComplete& OnComplete);

	// === TRANSACTION OPERATIONS ===

	/** 
	 * Run a transaction at a specific path (atomic update)
	 * @param Path Database path
	 * @param JsonData Data to set in transaction as JSON string
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Transaction", 
		meta = (DisplayName = "Run Transaction"))
	static void RunTransaction(const FString& Path, const FString& JsonData, 
		const FOnFirebaseDatabaseComplete& OnComplete);

	// === OFFLINE SUPPORT ===

	/** 
	 * Enable offline data persistence
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Offline", 
		meta = (DisplayName = "Enable Offline Persistence"))
	static void EnableOfflinePersistence();

	/** 
	 * Disable offline data persistence
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Offline", 
		meta = (DisplayName = "Disable Offline Persistence"))
	static void DisableOfflinePersistence();

	/** 
	 * Keep data synced at a specific path (for offline access)
	 * @param Path Database path
	 * @param KeepSynced Whether to keep synced
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Offline", 
		meta = (DisplayName = "Keep Synced"))
	static void KeepSynced(const FString& Path, bool KeepSynced);

	/** 
	 * Go online (reconnect to Firebase)
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Connection", 
		meta = (DisplayName = "Go Online"))
	static void GoOnline();

	/** 
	 * Go offline (disconnect from Firebase)
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Connection", 
		meta = (DisplayName = "Go Offline"))
	static void GoOffline();

	/** 
	 * Check if connected to Firebase
	 * @return True if connected
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|Connection", 
		meta = (DisplayName = "Is Connected"))
	static bool IsConnected();

	// === UTILITY FUNCTIONS ===

	/** 
	 * Generate a unique push ID
	 * @return Unique push ID string
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|Utility", 
		meta = (DisplayName = "Generate Push ID"))
	static FString GeneratePushId();

	/** 
	 * Get server timestamp as string
	 * @return Server timestamp marker
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|Utility", 
		meta = (DisplayName = "Get Server Timestamp"))
	static FString GetServerTimestamp();

	// === HELPER FUNCTIONS FOR JSON ===

	/** 
	 * Create JSON string from string value
	 * @param Key JSON key
	 * @param Value String value
	 * @return JSON string
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|JSON", 
		meta = (DisplayName = "Make JSON String"))
	static FString MakeJsonString(const FString& Key, const FString& Value);

	/** 
	 * Create JSON string from integer value
	 * @param Key JSON key
	 * @param Value Integer value
	 * @return JSON string
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|JSON", 
		meta = (DisplayName = "Make JSON Int"))
	static FString MakeJsonInt(const FString& Key, int32 Value);

	/** 
	 * Create JSON string from float value
	 * @param Key JSON key
	 * @param Value Float value
	 * @return JSON string
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|JSON", 
		meta = (DisplayName = "Make JSON Float"))
	static FString MakeJsonFloat(const FString& Key, float Value);

	/** 
	 * Create JSON string from boolean value
	 * @param Key JSON key
	 * @param Value Boolean value
	 * @return JSON string
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|JSON", 
		meta = (DisplayName = "Make JSON Bool"))
	static FString MakeJsonBool(const FString& Key, bool Value);

	/** 
	 * Combine multiple JSON strings into one object
	 * @param JsonStrings Array of JSON strings to combine
	 * @return Combined JSON string
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|JSON", 
		meta = (DisplayName = "Combine JSON"))
	static FString CombineJson(const TArray<FString>& JsonStrings);

	/** 
	 * Parse JSON string and get value by key
	 * @param JsonString JSON string to parse
	 * @param Key Key to retrieve
	 * @param OutValue Retrieved value as string
	 * @return True if key was found
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Database|JSON", 
		meta = (DisplayName = "Get JSON Value"))
	static bool GetJsonValue(const FString& JsonString, const FString& Key, FString& OutValue);

	// === INTERNAL METHODS ===

	/** Called from Java when database operation completes */
	static void OnDatabaseResultReceived(bool bSuccess, const FString& Path, 
		const FString& Data, const FString& ErrorMessage);

	/** Called from Java when database value changes */
	static void OnDatabaseValueChanged(const FString& Path, const FString& Data);

private:
	/** Store callbacks for async operations */
	static TMap<FString, FOnFirebaseDatabaseComplete> PendingCallbacks;
	
	/** Store listeners for value changes */
	static TMap<FString, FOnFirebaseDatabaseValueChanged> ValueListeners;

	/** Current operation ID counter */
	static int32 CurrentOperationId;

	/** REST API instance for cross-platform support */
	static UFirebaseRestAPI* RestAPIInstance;

	/** Generate unique operation ID */
	static FString GenerateOperationId();

	/** Register callback for operation */
	static void RegisterCallback(const FString& OperationId, const FOnFirebaseDatabaseComplete& Callback);

	/** Register listener for path */
	static void RegisterListener(const FString& Path, const FOnFirebaseDatabaseValueChanged& Listener);

	/** Get REST API instance (for non-Android platforms) */
	static UFirebaseRestAPI* GetRestAPI();

	/** Check if should use REST API (non-Android or forced) */
	static bool ShouldUseRestAPI();
};
