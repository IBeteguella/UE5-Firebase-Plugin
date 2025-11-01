// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "FirebaseRestAPI.generated.h"

DECLARE_DELEGATE_TwoParams(FFirebaseRestCallback, bool /*bSuccess*/, const FString& /*Response*/);

/**
 * Firebase REST API wrapper for cross-platform support
 * Uses Firebase REST API endpoints for Authentication and Realtime Database
 * Works on all platforms (Android, iOS, Windows, Mac, Linux, Web)
 */
UCLASS()
class FIREBASEPLUGIN_API UFirebaseRestAPI : public UObject
{
	GENERATED_BODY()

public:
	UFirebaseRestAPI();

	// Initialize with Firebase configuration
	void Initialize(const FString& InApiKey, const FString& InProjectId, const FString& InDatabaseUrl);

	// === AUTHENTICATION REST API ===

	/** Sign up with email and password */
	void SignUpWithEmail(const FString& Email, const FString& Password, FFirebaseRestCallback Callback);

	/** Sign in with email and password */
	void SignInWithEmail(const FString& Email, const FString& Password, FFirebaseRestCallback Callback);

	/** Sign in anonymously */
	void SignInAnonymously(FFirebaseRestCallback Callback);

	/** Refresh ID token */
	void RefreshIdToken(const FString& RefreshToken, FFirebaseRestCallback Callback);

	/** Send password reset email */
	void SendPasswordResetEmail(const FString& Email, FFirebaseRestCallback Callback);

	/** Update email */
	void UpdateEmail(const FString& IdToken, const FString& NewEmail, FFirebaseRestCallback Callback);

	/** Update password */
	void UpdatePassword(const FString& IdToken, const FString& NewPassword, FFirebaseRestCallback Callback);

	/** Send email verification */
	void SendEmailVerification(const FString& IdToken, FFirebaseRestCallback Callback);

	/** Delete user account */
	void DeleteAccount(const FString& IdToken, FFirebaseRestCallback Callback);

	/** Get user data */
	void GetUserData(const FString& IdToken, FFirebaseRestCallback Callback);

	/** Update user profile (display name, photo URL) */
	void UpdateProfile(const FString& IdToken, const FString& DisplayName, const FString& PhotoUrl, FFirebaseRestCallback Callback);

	// === DATABASE REST API ===

	/** Set value at path */
	void SetValue(const FString& Path, const FString& JsonValue, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Get value at path */
	void GetValue(const FString& Path, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Update value at path (partial update) */
	void UpdateValue(const FString& Path, const FString& JsonValue, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Delete value at path */
	void DeleteValue(const FString& Path, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Push new child to path */
	void PushValue(const FString& Path, const FString& JsonValue, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Query with order by child */
	void QueryOrderByChild(const FString& Path, const FString& ChildKey, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Query with limit to first */
	void QueryLimitToFirst(const FString& Path, int32 Limit, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Query with limit to last */
	void QueryLimitToLast(const FString& Path, int32 Limit, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Query with start at */
	void QueryStartAt(const FString& Path, const FString& Value, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Query with end at */
	void QueryEndAt(const FString& Path, const FString& Value, const FString& AuthToken, FFirebaseRestCallback Callback);

	/** Query with equal to */
	void QueryEqualTo(const FString& Path, const FString& Value, const FString& AuthToken, FFirebaseRestCallback Callback);

	// === HELPER FUNCTIONS ===

	/** Get current ID token (cached) */
	FString GetIdToken() const { return CachedIdToken; }

	/** Get current refresh token (cached) */
	FString GetRefreshToken() const { return CachedRefreshToken; }

	/** Get current user ID */
	FString GetUserId() const { return CachedUserId; }

	/** Check if user is signed in */
	bool IsSignedIn() const { return !CachedIdToken.IsEmpty(); }

	/** Clear cached tokens */
	void ClearTokens();

	/** 
	 * Get trusted server time from external time API
	 * Uses WorldTimeAPI.org to get accurate time (cannot be spoofed by client)
	 * @param Callback Returns timestamp in milliseconds on success
	 */
	void GetTrustedServerTime(FFirebaseRestCallback Callback);

private:
	// Configuration
	FString ApiKey;
	FString ProjectId;
	FString DatabaseUrl;

	// Cached authentication data
	FString CachedIdToken;
	FString CachedRefreshToken;
	FString CachedUserId;
	FString CachedEmail;

	// REST API endpoints
	static const FString AUTH_SIGNUP_ENDPOINT;
	static const FString AUTH_SIGNIN_ENDPOINT;
	static const FString AUTH_REFRESH_ENDPOINT;
	static const FString AUTH_RESET_PASSWORD_ENDPOINT;
	static const FString AUTH_UPDATE_ENDPOINT;
	static const FString AUTH_DELETE_ENDPOINT;
	static const FString AUTH_GET_USER_ENDPOINT;
	static const FString AUTH_SEND_VERIFICATION_ENDPOINT;

	// Helper functions
	void SendAuthRequest(const FString& Endpoint, const TSharedPtr<FJsonObject>& JsonPayload, FFirebaseRestCallback Callback, bool bCacheTokens = false);
	void SendDatabaseRequest(const FString& Path, const FString& Method, const FString& JsonBody, const FString& AuthToken, const FString& QueryParams, FFirebaseRestCallback Callback);
	FString BuildDatabaseUrl(const FString& Path, const FString& QueryParams = TEXT("")) const;
	void CacheAuthResponse(const FString& Response);
	TSharedPtr<FJsonObject> ParseJsonResponse(const FString& Response) const;
};
