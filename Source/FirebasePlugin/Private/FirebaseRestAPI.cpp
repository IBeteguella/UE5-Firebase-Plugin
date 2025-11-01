// Copyright. All Rights Reserved.

#include "FirebaseRestAPI.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

// Firebase REST API endpoints
const FString UFirebaseRestAPI::AUTH_SIGNUP_ENDPOINT = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signUp");
const FString UFirebaseRestAPI::AUTH_SIGNIN_ENDPOINT = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword");
const FString UFirebaseRestAPI::AUTH_REFRESH_ENDPOINT = TEXT("https://securetoken.googleapis.com/v1/token");
const FString UFirebaseRestAPI::AUTH_RESET_PASSWORD_ENDPOINT = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:sendOobCode");
const FString UFirebaseRestAPI::AUTH_UPDATE_ENDPOINT = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:update");
const FString UFirebaseRestAPI::AUTH_DELETE_ENDPOINT = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:delete");
const FString UFirebaseRestAPI::AUTH_GET_USER_ENDPOINT = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:lookup");
const FString UFirebaseRestAPI::AUTH_SEND_VERIFICATION_ENDPOINT = TEXT("https://identitytoolkit.googleapis.com/v1/accounts:sendOobCode");

UFirebaseRestAPI::UFirebaseRestAPI()
{
}

void UFirebaseRestAPI::Initialize(const FString& InApiKey, const FString& InProjectId, const FString& InDatabaseUrl)
{
	ApiKey = InApiKey;
	ProjectId = InProjectId;
	DatabaseUrl = InDatabaseUrl;
}

void UFirebaseRestAPI::ClearTokens()
{
	CachedIdToken.Empty();
	CachedRefreshToken.Empty();
	CachedUserId.Empty();
	CachedEmail.Empty();
}

void UFirebaseRestAPI::GetTrustedServerTime(FFirebaseRestCallback Callback)
{
	// Multiple time API options with fast timeout
	// We'll try worldtimeapi.org which is simpler and faster
	
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	// Use worldtimeapi.org - simple, fast, reliable
	HttpRequest->SetURL(TEXT("http://worldtimeapi.org/api/timezone/Etc/UTC"));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetTimeout(5.0f); // 5 second timeout instead of 30
	
	UE_LOG(LogTemp, Log, TEXT("Fetching server time from worldtimeapi.org (5s timeout)..."));
	
	// Keep a strong reference to this object to prevent garbage collection
	UFirebaseRestAPI* StrongThis = this;
	
	HttpRequest->OnProcessRequestComplete().BindLambda([Callback, StrongThis](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("==== Time API Lambda Callback Fired! Success: %s ===="), bWasSuccessful ? TEXT("true") : TEXT("false"));
		
		if (bWasSuccessful && Response.IsValid())
		{
			FString ResponseString = Response->GetContentAsString();
			int32 ResponseCode = Response->GetResponseCode();
			
			UE_LOG(LogTemp, Log, TEXT("Time API Response Code: %d"), ResponseCode);
			UE_LOG(LogTemp, Log, TEXT("Time API Response: %s"), *ResponseString);
			
			if (ResponseCode >= 200 && ResponseCode < 300)
			{
				// Parse JSON response
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
				
				if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
				{
					// timeapi.io returns different fields based on endpoint
					// Try "dateTime" field first (ISO format)
					if (JsonObject->HasField(TEXT("dateTime")))
					{
						FString DateTimeStr = JsonObject->GetStringField(TEXT("dateTime"));
						
						// Parse ISO 8601 format: 2025-10-30T12:00:00.000Z
						FDateTime ParsedDateTime;
						if (FDateTime::ParseIso8601(*DateTimeStr, ParsedDateTime))
						{
							int64 TimestampMs = ParsedDateTime.ToUnixTimestamp() * 1000LL;
							FString TimestampString = FString::Printf(TEXT("%lld"), TimestampMs);
							UE_LOG(LogTemp, Log, TEXT("Successfully parsed time: %s"), *TimestampString);
							Callback.ExecuteIfBound(true, TimestampString);
							return;
						}
					}
					
					// Try "unixtime" field (WorldTimeAPI format)
					if (JsonObject->HasField(TEXT("unixtime")))
					{
						int64 UnixTimeSeconds = (int64)JsonObject->GetNumberField(TEXT("unixtime"));
						int64 TimestampMs = UnixTimeSeconds * 1000LL;
						FString TimestampString = FString::Printf(TEXT("%lld"), TimestampMs);
						UE_LOG(LogTemp, Log, TEXT("Successfully parsed time: %s"), *TimestampString);
						Callback.ExecuteIfBound(true, TimestampString);
						return;
					}
					
					// Try "timestamp" field
					if (JsonObject->HasField(TEXT("timestamp")))
					{
						int64 TimestampSec = (int64)JsonObject->GetNumberField(TEXT("timestamp"));
						int64 TimestampMs = TimestampSec * 1000LL;
						FString TimestampString = FString::Printf(TEXT("%lld"), TimestampMs);
						UE_LOG(LogTemp, Log, TEXT("Successfully parsed time: %s"), *TimestampString);
						Callback.ExecuteIfBound(true, TimestampString);
						return;
					}
				}
			}
			
			UE_LOG(LogTemp, Error, TEXT("Failed to parse time API response. Response Code: %d, Body: %s"), ResponseCode, *ResponseString);
			
			// FALLBACK: Use device time with warning
			UE_LOG(LogTemp, Warning, TEXT("Using device time as fallback (can be spoofed!)"));
			FDateTime Now = FDateTime::UtcNow();
			int64 TimestampMs = Now.ToUnixTimestamp() * 1000LL + Now.GetMillisecond();
			FString TimestampString = FString::Printf(TEXT("%lld"), TimestampMs);
			Callback.ExecuteIfBound(true, TimestampString);
		}
		else
		{
			FString ErrorMsg = TEXT("Network error");
			if (Request.IsValid())
			{
				ErrorMsg = FString::Printf(TEXT("Request failed. Status: %s"), 
					Response.IsValid() ? *FString::FromInt(Response->GetResponseCode()) : TEXT("No Response"));
			}
			
			UE_LOG(LogTemp, Error, TEXT("Time API request failed: %s"), *ErrorMsg);
			
			// FALLBACK: Use device time with warning
			UE_LOG(LogTemp, Warning, TEXT("Using device time as fallback (can be spoofed!)"));
			FDateTime Now = FDateTime::UtcNow();
			int64 TimestampMs = Now.ToUnixTimestamp() * 1000LL + Now.GetMillisecond();
			FString TimestampString = FString::Printf(TEXT("%lld"), TimestampMs);
			Callback.ExecuteIfBound(true, TimestampString);
		}
	});
	
	bool bRequestStarted = HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("Time API HTTP Request Started: %s"), bRequestStarted ? TEXT("true") : TEXT("false"));
}

TSharedPtr<FJsonObject> UFirebaseRestAPI::ParseJsonResponse(const FString& Response) const
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response);
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	return JsonObject;
}

void UFirebaseRestAPI::CacheAuthResponse(const FString& Response)
{
	TSharedPtr<FJsonObject> JsonResponse = ParseJsonResponse(Response);
	if (JsonResponse.IsValid())
	{
		if (JsonResponse->HasField(TEXT("idToken")))
		{
			CachedIdToken = JsonResponse->GetStringField(TEXT("idToken"));
		}
		if (JsonResponse->HasField(TEXT("refreshToken")))
		{
			CachedRefreshToken = JsonResponse->GetStringField(TEXT("refreshToken"));
		}
		if (JsonResponse->HasField(TEXT("localId")))
		{
			CachedUserId = JsonResponse->GetStringField(TEXT("localId"));
		}
		if (JsonResponse->HasField(TEXT("email")))
		{
			CachedEmail = JsonResponse->GetStringField(TEXT("email"));
		}
	}
}

FString UFirebaseRestAPI::BuildDatabaseUrl(const FString& Path, const FString& QueryParams) const
{
	FString CleanPath = Path;
	if (!CleanPath.StartsWith(TEXT("/")))
	{
		CleanPath = TEXT("/") + CleanPath;
	}
	
	FString Url = DatabaseUrl + CleanPath + TEXT(".json");
	
	if (!QueryParams.IsEmpty())
	{
		Url += TEXT("?") + QueryParams;
	}
	
	return Url;
}

// === AUTHENTICATION ===

void UFirebaseRestAPI::SignUpWithEmail(const FString& Email, const FString& Password, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("email"), Email);
	JsonPayload->SetStringField(TEXT("password"), Password);
	JsonPayload->SetBoolField(TEXT("returnSecureToken"), true);

	SendAuthRequest(AUTH_SIGNUP_ENDPOINT, JsonPayload, Callback, true);
}

void UFirebaseRestAPI::SignInWithEmail(const FString& Email, const FString& Password, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("email"), Email);
	JsonPayload->SetStringField(TEXT("password"), Password);
	JsonPayload->SetBoolField(TEXT("returnSecureToken"), true);

	SendAuthRequest(AUTH_SIGNIN_ENDPOINT, JsonPayload, Callback, true);
}

void UFirebaseRestAPI::SignInAnonymously(FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetBoolField(TEXT("returnSecureToken"), true);

	SendAuthRequest(AUTH_SIGNUP_ENDPOINT, JsonPayload, Callback, true);
}

void UFirebaseRestAPI::RefreshIdToken(const FString& RefreshToken, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("grant_type"), TEXT("refresh_token"));
	JsonPayload->SetStringField(TEXT("refresh_token"), RefreshToken);

	SendAuthRequest(AUTH_REFRESH_ENDPOINT, JsonPayload, Callback, true);
}

void UFirebaseRestAPI::SendPasswordResetEmail(const FString& Email, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("requestType"), TEXT("PASSWORD_RESET"));
	JsonPayload->SetStringField(TEXT("email"), Email);

	SendAuthRequest(AUTH_RESET_PASSWORD_ENDPOINT, JsonPayload, Callback);
}

void UFirebaseRestAPI::UpdateEmail(const FString& IdToken, const FString& NewEmail, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("idToken"), IdToken);
	JsonPayload->SetStringField(TEXT("email"), NewEmail);
	JsonPayload->SetBoolField(TEXT("returnSecureToken"), true);

	SendAuthRequest(AUTH_UPDATE_ENDPOINT, JsonPayload, Callback, true);
}

void UFirebaseRestAPI::UpdatePassword(const FString& IdToken, const FString& NewPassword, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("idToken"), IdToken);
	JsonPayload->SetStringField(TEXT("password"), NewPassword);
	JsonPayload->SetBoolField(TEXT("returnSecureToken"), true);

	SendAuthRequest(AUTH_UPDATE_ENDPOINT, JsonPayload, Callback, true);
}

void UFirebaseRestAPI::SendEmailVerification(const FString& IdToken, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("requestType"), TEXT("VERIFY_EMAIL"));
	JsonPayload->SetStringField(TEXT("idToken"), IdToken);

	SendAuthRequest(AUTH_SEND_VERIFICATION_ENDPOINT, JsonPayload, Callback);
}

void UFirebaseRestAPI::DeleteAccount(const FString& IdToken, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("idToken"), IdToken);

	SendAuthRequest(AUTH_DELETE_ENDPOINT, JsonPayload, Callback);
}

void UFirebaseRestAPI::GetUserData(const FString& IdToken, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("idToken"), IdToken);

	SendAuthRequest(AUTH_GET_USER_ENDPOINT, JsonPayload, Callback);
}

void UFirebaseRestAPI::UpdateProfile(const FString& IdToken, const FString& DisplayName, const FString& PhotoUrl, FFirebaseRestCallback Callback)
{
	TSharedPtr<FJsonObject> JsonPayload = MakeShareable(new FJsonObject());
	JsonPayload->SetStringField(TEXT("idToken"), IdToken);
	
	if (!DisplayName.IsEmpty())
	{
		JsonPayload->SetStringField(TEXT("displayName"), DisplayName);
	}
	if (!PhotoUrl.IsEmpty())
	{
		JsonPayload->SetStringField(TEXT("photoUrl"), PhotoUrl);
	}
	
	JsonPayload->SetBoolField(TEXT("returnSecureToken"), true);

	SendAuthRequest(AUTH_UPDATE_ENDPOINT, JsonPayload, Callback, true);
}

void UFirebaseRestAPI::SendAuthRequest(const FString& Endpoint, const TSharedPtr<FJsonObject>& JsonPayload, FFirebaseRestCallback Callback, bool bCacheTokens)
{
	// Create HTTP request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	// Add API key to URL
	FString Url = Endpoint + TEXT("?key=") + ApiKey;
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Serialize JSON payload
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonPayload.ToSharedRef(), JsonWriter);
	HttpRequest->SetContentAsString(JsonString);

	// Set callback
	HttpRequest->OnProcessRequestComplete().BindLambda([this, Callback, bCacheTokens](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid())
		{
			FString ResponseString = Response->GetContentAsString();
			int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode >= 200 && ResponseCode < 300)
			{
				// Cache tokens if requested
				if (bCacheTokens)
				{
					CacheAuthResponse(ResponseString);
				}

				Callback.ExecuteIfBound(true, ResponseString);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Firebase Auth Error: %d - %s"), ResponseCode, *ResponseString);
				Callback.ExecuteIfBound(false, ResponseString);
			}
		}
		else
		{
			FString ErrorMessage = TEXT("Network error");
			UE_LOG(LogTemp, Error, TEXT("Firebase Auth Network Error"));
			Callback.ExecuteIfBound(false, ErrorMessage);
		}
	});

	// Send request
	HttpRequest->ProcessRequest();
}

// === DATABASE ===

void UFirebaseRestAPI::SetValue(const FString& Path, const FString& JsonValue, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = AuthToken.IsEmpty() ? TEXT("") : FString::Printf(TEXT("auth=%s"), *AuthToken);
	SendDatabaseRequest(Path, TEXT("PUT"), JsonValue, AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::GetValue(const FString& Path, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = AuthToken.IsEmpty() ? TEXT("") : FString::Printf(TEXT("auth=%s"), *AuthToken);
	SendDatabaseRequest(Path, TEXT("GET"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::UpdateValue(const FString& Path, const FString& JsonValue, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = AuthToken.IsEmpty() ? TEXT("") : FString::Printf(TEXT("auth=%s"), *AuthToken);
	SendDatabaseRequest(Path, TEXT("PATCH"), JsonValue, AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::DeleteValue(const FString& Path, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = AuthToken.IsEmpty() ? TEXT("") : FString::Printf(TEXT("auth=%s"), *AuthToken);
	SendDatabaseRequest(Path, TEXT("DELETE"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::PushValue(const FString& Path, const FString& JsonValue, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = AuthToken.IsEmpty() ? TEXT("") : FString::Printf(TEXT("auth=%s"), *AuthToken);
	SendDatabaseRequest(Path, TEXT("POST"), JsonValue, AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::QueryOrderByChild(const FString& Path, const FString& ChildKey, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = FString::Printf(TEXT("orderBy=\"%s\""), *ChildKey);
	if (!AuthToken.IsEmpty())
	{
		QueryParams += FString::Printf(TEXT("&auth=%s"), *AuthToken);
	}
	SendDatabaseRequest(Path, TEXT("GET"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::QueryLimitToFirst(const FString& Path, int32 Limit, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = FString::Printf(TEXT("limitToFirst=%d"), Limit);
	if (!AuthToken.IsEmpty())
	{
		QueryParams += FString::Printf(TEXT("&auth=%s"), *AuthToken);
	}
	SendDatabaseRequest(Path, TEXT("GET"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::QueryLimitToLast(const FString& Path, int32 Limit, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = FString::Printf(TEXT("limitToLast=%d"), Limit);
	if (!AuthToken.IsEmpty())
	{
		QueryParams += FString::Printf(TEXT("&auth=%s"), *AuthToken);
	}
	SendDatabaseRequest(Path, TEXT("GET"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::QueryStartAt(const FString& Path, const FString& Value, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = FString::Printf(TEXT("startAt=\"%s\""), *Value);
	if (!AuthToken.IsEmpty())
	{
		QueryParams += FString::Printf(TEXT("&auth=%s"), *AuthToken);
	}
	SendDatabaseRequest(Path, TEXT("GET"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::QueryEndAt(const FString& Path, const FString& Value, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = FString::Printf(TEXT("endAt=\"%s\""), *Value);
	if (!AuthToken.IsEmpty())
	{
		QueryParams += FString::Printf(TEXT("&auth=%s"), *AuthToken);
	}
	SendDatabaseRequest(Path, TEXT("GET"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::QueryEqualTo(const FString& Path, const FString& Value, const FString& AuthToken, FFirebaseRestCallback Callback)
{
	FString QueryParams = FString::Printf(TEXT("equalTo=\"%s\""), *Value);
	if (!AuthToken.IsEmpty())
	{
		QueryParams += FString::Printf(TEXT("&auth=%s"), *AuthToken);
	}
	SendDatabaseRequest(Path, TEXT("GET"), TEXT(""), AuthToken, QueryParams, Callback);
}

void UFirebaseRestAPI::SendDatabaseRequest(const FString& Path, const FString& Method, const FString& JsonBody, const FString& AuthToken, const FString& QueryParams, FFirebaseRestCallback Callback)
{
	// Create HTTP request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	
	// Build URL
	FString Url = BuildDatabaseUrl(Path, QueryParams);
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(Method);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Set body if provided
	if (!JsonBody.IsEmpty())
	{
		HttpRequest->SetContentAsString(JsonBody);
	}

	// Set callback
	HttpRequest->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid())
		{
			FString ResponseString = Response->GetContentAsString();
			int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode >= 200 && ResponseCode < 300)
			{
				Callback.ExecuteIfBound(true, ResponseString);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Firebase Database Error: %d - %s"), ResponseCode, *ResponseString);
				Callback.ExecuteIfBound(false, ResponseString);
			}
		}
		else
		{
			FString ErrorMessage = TEXT("Network error");
			UE_LOG(LogTemp, Error, TEXT("Firebase Database Network Error"));
			Callback.ExecuteIfBound(false, ErrorMessage);
		}
	});

	// Send request
	HttpRequest->ProcessRequest();
}
