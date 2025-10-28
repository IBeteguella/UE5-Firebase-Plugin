// Copyright. All Rights Reserved.

#include "FirebaseDatabase.h"
#include "FirebaseSettings.h"
#include "FirebaseAuth.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

// Initialize static members
TMap<FString, FOnFirebaseDatabaseComplete> UFirebaseDatabase::PendingCallbacks;
TMap<FString, FOnFirebaseDatabaseValueChanged> UFirebaseDatabase::ValueListeners;
int32 UFirebaseDatabase::CurrentOperationId = 0;
UFirebaseRestAPI* UFirebaseDatabase::RestAPIInstance = nullptr;

FString UFirebaseDatabase::GenerateOperationId()
{
	return FString::Printf(TEXT("DB_%d"), ++CurrentOperationId);
}

void UFirebaseDatabase::RegisterCallback(const FString& OperationId, const FOnFirebaseDatabaseComplete& Callback)
{
	if (Callback.IsBound())
	{
		PendingCallbacks.Add(OperationId, Callback);
	}
}

void UFirebaseDatabase::RegisterListener(const FString& Path, const FOnFirebaseDatabaseValueChanged& Listener)
{
	if (Listener.IsBound())
	{
		ValueListeners.Add(Path, Listener);
	}
}

bool UFirebaseDatabase::ShouldUseRestAPI()
{
#if PLATFORM_ANDROID
	const UFirebaseSettings* Settings = GetDefault<UFirebaseSettings>();
	// Use REST API if enabled in settings
	return Settings && Settings->bUseRestApiForNonAndroid;
#else
	// Always use REST API on non-Android platforms
	return true;
#endif
}

UFirebaseRestAPI* UFirebaseDatabase::GetRestAPI()
{
	if (!RestAPIInstance)
	{
		RestAPIInstance = NewObject<UFirebaseRestAPI>();
		
		// Initialize with settings
		const UFirebaseSettings* Settings = GetDefault<UFirebaseSettings>();
		if (Settings)
		{
			RestAPIInstance->Initialize(
				Settings->AndroidApiKey,
				Settings->ProjectId,
				Settings->GetFullDatabaseUrl()
			);
		}
	}
	
	return RestAPIInstance;
}

// === WRITE OPERATIONS ===

void UFirebaseDatabase::SetValue(const FString& Path, const FString& JsonData, 
	const FOnFirebaseDatabaseComplete& OnComplete)
{
	// Use REST API on non-Android or if enabled
	if (ShouldUseRestAPI())
	{
		UFirebaseRestAPI* RestAPI = GetRestAPI();
		if (RestAPI)
		{
			// Get auth token from FirebaseAuth
			FString AuthToken = UFirebaseAuth::GetRestAPI() ? UFirebaseAuth::GetRestAPI()->GetIdToken() : TEXT("");
			
			RestAPI->SetValue(Path, JsonData, AuthToken,
				FFirebaseRestCallback::CreateLambda([OnComplete, Path](bool bSuccess, const FString& Response)
			{
				FFirebaseDatabaseResult Result;
				Result.bSuccess = bSuccess;
				Result.Path = Path;
				Result.Data = Response;
				
				if (!bSuccess)
				{
					Result.ErrorMessage = Response;
				}
				
				// Execute callback on game thread
				AsyncTask(ENamedThreads::GameThread, [OnComplete, Result]()
				{
					OnComplete.ExecuteIfBound(Result);
				});
			}));
		}
		return;
	}

#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));
		jstring jData = Env->NewStringUTF(TCHAR_TO_UTF8(*JsonData));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SetValueMethod = Env->GetStaticMethodID(FirebaseHelperClass, "setDatabaseValue", 
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			if (SetValueMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SetValueMethod, jPath, jData, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
		Env->DeleteLocalRef(jData);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("Firebase Database: SetValue not available"));
	FFirebaseDatabaseResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

void UFirebaseDatabase::UpdateValue(const FString& Path, const FString& JsonData, 
	const FOnFirebaseDatabaseComplete& OnComplete)
{
	// Use REST API on non-Android or if enabled
	if (ShouldUseRestAPI())
	{
		UFirebaseRestAPI* RestAPI = GetRestAPI();
		if (RestAPI)
		{
			// Get auth token from FirebaseAuth
			FString AuthToken = UFirebaseAuth::GetRestAPI() ? UFirebaseAuth::GetRestAPI()->GetIdToken() : TEXT("");
			
			RestAPI->UpdateValue(Path, JsonData, AuthToken,
				FFirebaseRestCallback::CreateLambda([OnComplete, Path](bool bSuccess, const FString& Response)
			{
				FFirebaseDatabaseResult Result;
				Result.bSuccess = bSuccess;
				Result.Path = Path;
				Result.Data = Response;
				
				if (!bSuccess)
				{
					Result.ErrorMessage = Response;
				}
				
				// Execute callback on game thread
				AsyncTask(ENamedThreads::GameThread, [OnComplete, Result]()
				{
					OnComplete.ExecuteIfBound(Result);
				});
			}));
		}
		return;
	}

#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));
		jstring jData = Env->NewStringUTF(TCHAR_TO_UTF8(*JsonData));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID UpdateMethod = Env->GetStaticMethodID(FirebaseHelperClass, "updateDatabaseValue", 
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			if (UpdateMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, UpdateMethod, jPath, jData, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
		Env->DeleteLocalRef(jData);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseDatabaseResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

void UFirebaseDatabase::PushValue(const FString& Path, const FString& JsonData, 
	const FOnFirebaseDatabaseComplete& OnComplete)
{
	// Use REST API on non-Android or if enabled
	if (ShouldUseRestAPI())
	{
		UFirebaseRestAPI* RestAPI = GetRestAPI();
		if (RestAPI)
		{
			// Get auth token from FirebaseAuth
			FString AuthToken = UFirebaseAuth::GetRestAPI() ? UFirebaseAuth::GetRestAPI()->GetIdToken() : TEXT("");
			
			RestAPI->PushValue(Path, JsonData, AuthToken,
				FFirebaseRestCallback::CreateLambda([OnComplete, Path](bool bSuccess, const FString& Response)
			{
				FFirebaseDatabaseResult Result;
				Result.bSuccess = bSuccess;
				Result.Path = Path;
				Result.Data = Response;
				
				if (!bSuccess)
				{
					Result.ErrorMessage = Response;
				}
				
				// Execute callback on game thread
				AsyncTask(ENamedThreads::GameThread, [OnComplete, Result]()
				{
					OnComplete.ExecuteIfBound(Result);
				});
			}));
		}
		return;
	}

#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));
		jstring jData = Env->NewStringUTF(TCHAR_TO_UTF8(*JsonData));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID PushMethod = Env->GetStaticMethodID(FirebaseHelperClass, "pushDatabaseValue", 
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			if (PushMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, PushMethod, jPath, jData, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
		Env->DeleteLocalRef(jData);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseDatabaseResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

void UFirebaseDatabase::DeleteValue(const FString& Path, const FOnFirebaseDatabaseComplete& OnComplete)
{
	// Use REST API on non-Android or if enabled
	if (ShouldUseRestAPI())
	{
		UFirebaseRestAPI* RestAPI = GetRestAPI();
		if (RestAPI)
		{
			// Get auth token from FirebaseAuth
			FString AuthToken = UFirebaseAuth::GetRestAPI() ? UFirebaseAuth::GetRestAPI()->GetIdToken() : TEXT("");
			
			RestAPI->DeleteValue(Path, AuthToken,
				FFirebaseRestCallback::CreateLambda([OnComplete, Path](bool bSuccess, const FString& Response)
			{
				FFirebaseDatabaseResult Result;
				Result.bSuccess = bSuccess;
				Result.Path = Path;
				Result.Data = Response;
				
				if (!bSuccess)
				{
					Result.ErrorMessage = Response;
				}
				
				// Execute callback on game thread
				AsyncTask(ENamedThreads::GameThread, [OnComplete, Result]()
				{
					OnComplete.ExecuteIfBound(Result);
				});
			}));
		}
		return;
	}

#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID DeleteMethod = Env->GetStaticMethodID(FirebaseHelperClass, "deleteDatabaseValue", 
				"(Ljava/lang/String;Ljava/lang/String;)V");
			if (DeleteMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, DeleteMethod, jPath, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseDatabaseResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === READ OPERATIONS ===

void UFirebaseDatabase::GetValue(const FString& Path, const FOnFirebaseDatabaseComplete& OnComplete)
{
	// Use REST API on non-Android or if enabled
	if (ShouldUseRestAPI())
	{
		UFirebaseRestAPI* RestAPI = GetRestAPI();
		if (RestAPI)
		{
			// Get auth token from FirebaseAuth
			FString AuthToken = UFirebaseAuth::GetRestAPI() ? UFirebaseAuth::GetRestAPI()->GetIdToken() : TEXT("");
			
			RestAPI->GetValue(Path, AuthToken,
				FFirebaseRestCallback::CreateLambda([OnComplete, Path](bool bSuccess, const FString& Response)
			{
				FFirebaseDatabaseResult Result;
				Result.bSuccess = bSuccess;
				Result.Path = Path;
				Result.Data = Response;
				
				if (!bSuccess)
				{
					Result.ErrorMessage = Response;
				}
				
				// Execute callback on game thread
				AsyncTask(ENamedThreads::GameThread, [OnComplete, Result]()
				{
					OnComplete.ExecuteIfBound(Result);
				});
			}));
		}
		return;
	}

#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID GetMethod = Env->GetStaticMethodID(FirebaseHelperClass, "getDatabaseValue", 
				"(Ljava/lang/String;Ljava/lang/String;)V");
			if (GetMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, GetMethod, jPath, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseDatabaseResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

void UFirebaseDatabase::ListenForValueChanges(const FString& Path, 
	const FOnFirebaseDatabaseValueChanged& OnValueChanged)
{
#if PLATFORM_ANDROID
	RegisterListener(Path, OnValueChanged);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID ListenMethod = Env->GetStaticMethodID(FirebaseHelperClass, "listenForValueChanges", 
				"(Ljava/lang/String;)V");
			if (ListenMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, ListenMethod, jPath);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("Firebase Database: ListenForValueChanges only available on Android"));
#endif
}

void UFirebaseDatabase::StopListening(const FString& Path)
{
#if PLATFORM_ANDROID
	ValueListeners.Remove(Path);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID StopMethod = Env->GetStaticMethodID(FirebaseHelperClass, "stopListening", 
				"(Ljava/lang/String;)V");
			if (StopMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, StopMethod, jPath);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
	}
#endif
}

// === QUERY OPERATIONS ===

void UFirebaseDatabase::QueryValues(const FString& Path, const FString& OrderByKey, 
	int32 LimitToFirst, const FString& StartAt, const FString& EndAt,
	const FOnFirebaseDatabaseComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));
		jstring jOrderBy = Env->NewStringUTF(TCHAR_TO_UTF8(*OrderByKey));
		jstring jStartAt = Env->NewStringUTF(TCHAR_TO_UTF8(*StartAt));
		jstring jEndAt = Env->NewStringUTF(TCHAR_TO_UTF8(*EndAt));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID QueryMethod = Env->GetStaticMethodID(FirebaseHelperClass, "queryDatabaseValues", 
				"(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			if (QueryMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, QueryMethod, jPath, jOrderBy, 
					(jint)LimitToFirst, jStartAt, jEndAt, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
		Env->DeleteLocalRef(jOrderBy);
		Env->DeleteLocalRef(jStartAt);
		Env->DeleteLocalRef(jEndAt);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseDatabaseResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === TRANSACTION OPERATIONS ===

void UFirebaseDatabase::RunTransaction(const FString& Path, const FString& JsonData, 
	const FOnFirebaseDatabaseComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));
		jstring jData = Env->NewStringUTF(TCHAR_TO_UTF8(*JsonData));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID TransactionMethod = Env->GetStaticMethodID(FirebaseHelperClass, "runTransaction", 
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			if (TransactionMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, TransactionMethod, jPath, jData, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
		Env->DeleteLocalRef(jData);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseDatabaseResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === OFFLINE SUPPORT ===

void UFirebaseDatabase::EnableOfflinePersistence()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID EnableMethod = Env->GetStaticMethodID(FirebaseHelperClass, "enableOfflinePersistence", "()V");
			if (EnableMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, EnableMethod);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	UE_LOG(LogTemp, Log, TEXT("Firebase Database: Offline persistence enabled"));
}

void UFirebaseDatabase::DisableOfflinePersistence()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID DisableMethod = Env->GetStaticMethodID(FirebaseHelperClass, "disableOfflinePersistence", "()V");
			if (DisableMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, DisableMethod);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	UE_LOG(LogTemp, Log, TEXT("Firebase Database: Offline persistence disabled"));
}

void UFirebaseDatabase::KeepSynced(const FString& Path, bool bKeepSynced)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPath = Env->NewStringUTF(TCHAR_TO_UTF8(*Path));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID KeepSyncedMethod = Env->GetStaticMethodID(FirebaseHelperClass, "keepSynced", 
				"(Ljava/lang/String;Z)V");
			if (KeepSyncedMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, KeepSyncedMethod, jPath, (jboolean)bKeepSynced);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPath);
	}
#endif
}

void UFirebaseDatabase::GoOnline()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID OnlineMethod = Env->GetStaticMethodID(FirebaseHelperClass, "goOnline", "()V");
			if (OnlineMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, OnlineMethod);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	UE_LOG(LogTemp, Log, TEXT("Firebase Database: Going online"));
}

void UFirebaseDatabase::GoOffline()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID OfflineMethod = Env->GetStaticMethodID(FirebaseHelperClass, "goOffline", "()V");
			if (OfflineMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, OfflineMethod);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	UE_LOG(LogTemp, Log, TEXT("Firebase Database: Going offline"));
}

bool UFirebaseDatabase::IsConnected()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID IsConnectedMethod = Env->GetStaticMethodID(FirebaseHelperClass, "isConnected", "()Z");
			if (IsConnectedMethod)
			{
				jboolean result = Env->CallStaticBooleanMethod(FirebaseHelperClass, IsConnectedMethod);
				Env->DeleteLocalRef(FirebaseHelperClass);
				return (bool)result;
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	return false;
}

// === UTILITY FUNCTIONS ===

FString UFirebaseDatabase::GeneratePushId()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID GenerateMethod = Env->GetStaticMethodID(FirebaseHelperClass, "generatePushId", 
				"()Ljava/lang/String;");
			if (GenerateMethod)
			{
				jstring jPushId = (jstring)Env->CallStaticObjectMethod(FirebaseHelperClass, GenerateMethod);
				if (jPushId)
				{
					const char* pushId = Env->GetStringUTFChars(jPushId, nullptr);
					FString result = FString(UTF8_TO_TCHAR(pushId));
					Env->ReleaseStringUTFChars(jPushId, pushId);
					Env->DeleteLocalRef(jPushId);
					Env->DeleteLocalRef(FirebaseHelperClass);
					return result;
				}
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	return FString();
}

FString UFirebaseDatabase::GetServerTimestamp()
{
	return TEXT("{\"$timestamp\": true}");
}

// === JSON HELPER FUNCTIONS ===

FString UFirebaseDatabase::MakeJsonString(const FString& Key, const FString& Value)
{
	return FString::Printf(TEXT("{\"%s\": \"%s\"}"), *Key, *Value);
}

FString UFirebaseDatabase::MakeJsonInt(const FString& Key, int32 Value)
{
	return FString::Printf(TEXT("{\"%s\": %d}"), *Key, Value);
}

FString UFirebaseDatabase::MakeJsonFloat(const FString& Key, float Value)
{
	return FString::Printf(TEXT("{\"%s\": %.6f}"), *Key, Value);
}

FString UFirebaseDatabase::MakeJsonBool(const FString& Key, bool Value)
{
	return FString::Printf(TEXT("{\"%s\": %s}"), *Key, Value ? TEXT("true") : TEXT("false"));
}

FString UFirebaseDatabase::CombineJson(const TArray<FString>& JsonStrings)
{
	if (JsonStrings.Num() == 0)
	{
		return TEXT("{}");
	}

	FString Combined = TEXT("{");
	for (int32 i = 0; i < JsonStrings.Num(); i++)
	{
		FString JsonStr = JsonStrings[i];
		// Remove outer braces
		JsonStr.RemoveFromStart(TEXT("{"));
		JsonStr.RemoveFromEnd(TEXT("}"));
		Combined += JsonStr;
		
		if (i < JsonStrings.Num() - 1)
		{
			Combined += TEXT(", ");
		}
	}
	Combined += TEXT("}");
	return Combined;
}

bool UFirebaseDatabase::GetJsonValue(const FString& JsonString, const FString& Key, FString& OutValue)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->HasField(Key))
		{
			TSharedPtr<FJsonValue> Value = JsonObject->TryGetField(Key);
			if (Value.IsValid())
			{
				if (Value->Type == EJson::String)
				{
					OutValue = Value->AsString();
				}
				else if (Value->Type == EJson::Number)
				{
					OutValue = FString::SanitizeFloat(Value->AsNumber());
				}
				else if (Value->Type == EJson::Boolean)
				{
					OutValue = Value->AsBool() ? TEXT("true") : TEXT("false");
				}
				else
				{
					// For objects or arrays, serialize back to string
					FString OutputString;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
					FJsonSerializer::Serialize(Value.ToSharedRef(), TEXT(""), Writer);
					OutValue = OutputString;
				}
				return true;
			}
		}
	}
	return false;
}

// === CALLBACK HANDLERS ===

void UFirebaseDatabase::OnDatabaseResultReceived(bool bSuccess, const FString& Path, 
	const FString& Data, const FString& ErrorMessage)
{
	FFirebaseDatabaseResult Result;
	Result.bSuccess = bSuccess;
	Result.Path = Path;
	Result.Data = Data;
	Result.ErrorMessage = ErrorMessage;

	// Execute all pending callbacks (we'll need to improve this with proper operation ID tracking)
	for (auto& Pair : PendingCallbacks)
	{
		Pair.Value.ExecuteIfBound(Result);
	}
	PendingCallbacks.Empty();

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Firebase Database: Operation successful - Path: %s"), *Path);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Firebase Database: Operation failed - %s"), *ErrorMessage);
	}
}

void UFirebaseDatabase::OnDatabaseValueChanged(const FString& Path, const FString& Data)
{
	if (ValueListeners.Contains(Path))
	{
		ValueListeners[Path].ExecuteIfBound(Path, Data);
		UE_LOG(LogTemp, Log, TEXT("Firebase Database: Value changed - Path: %s"), *Path);
	}
}

// === JNI CALLBACKS FROM JAVA ===

#if PLATFORM_ANDROID
extern "C" JNIEXPORT void JNICALL Java_com_epicgames_unreal_FirebaseHelper_nativeOnDatabaseResult(
	JNIEnv* env, jclass clazz, jboolean success, jstring path, jstring data, jstring errorMessage)
{
	bool bSuccess = (bool)success;
	
	const char* pathChars = env->GetStringUTFChars(path, nullptr);
	FString PathStr = FString(UTF8_TO_TCHAR(pathChars));
	env->ReleaseStringUTFChars(path, pathChars);

	const char* dataChars = env->GetStringUTFChars(data, nullptr);
	FString DataStr = FString(UTF8_TO_TCHAR(dataChars));
	env->ReleaseStringUTFChars(data, dataChars);

	const char* errorChars = env->GetStringUTFChars(errorMessage, nullptr);
	FString ErrorStr = FString(UTF8_TO_TCHAR(errorChars));
	env->ReleaseStringUTFChars(errorMessage, errorChars);

	// Call on game thread
	AsyncTask(ENamedThreads::GameThread, [bSuccess, PathStr, DataStr, ErrorStr]()
	{
		UFirebaseDatabase::OnDatabaseResultReceived(bSuccess, PathStr, DataStr, ErrorStr);
	});
}

extern "C" JNIEXPORT void JNICALL Java_com_epicgames_unreal_FirebaseHelper_nativeOnDatabaseValueChanged(
	JNIEnv* env, jclass clazz, jstring path, jstring data)
{
	const char* pathChars = env->GetStringUTFChars(path, nullptr);
	FString PathStr = FString(UTF8_TO_TCHAR(pathChars));
	env->ReleaseStringUTFChars(path, pathChars);

	const char* dataChars = env->GetStringUTFChars(data, nullptr);
	FString DataStr = FString(UTF8_TO_TCHAR(dataChars));
	env->ReleaseStringUTFChars(data, dataChars);

	// Call on game thread
	AsyncTask(ENamedThreads::GameThread, [PathStr, DataStr]()
	{
		UFirebaseDatabase::OnDatabaseValueChanged(PathStr, DataStr);
	});
}
#endif
