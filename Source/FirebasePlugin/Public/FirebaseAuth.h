// Copyright. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebaseRestAPI.h"
#include "FirebaseAuth.generated.h"

/**
 * Firebase Authentication Result
 */
USTRUCT(BlueprintType)
struct FFirebaseAuthResult
{
	GENERATED_BODY()

	/** Whether the operation was successful */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Auth")
	bool bSuccess = false;

	/** User ID (UID) if successful */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Auth")
	FString UserId;

	/** User email if available */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Auth")
	FString Email;

	/** Display name if available */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Auth")
	FString DisplayName;

	/** Error message if failed */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Auth")
	FString ErrorMessage;

	/** Authentication token (for API calls) */
	UPROPERTY(BlueprintReadOnly, Category = "Firebase|Auth")
	FString AuthToken;
};

/**
 * Delegate for authentication callbacks
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFirebaseAuthComplete, const FFirebaseAuthResult&, Result);

/**
 * Firebase Authentication Blueprint Function Library
 */
UCLASS()
class FIREBASEPLUGIN_API UFirebaseAuth : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// === EMAIL/PASSWORD AUTHENTICATION ===

	/** 
	 * Sign up a new user with email and password
	 * @param Email User's email address
	 * @param Password User's password (minimum 6 characters)
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Sign Up with Email"))
	static void SignUpWithEmail(const FString& Email, const FString& Password, 
		const FOnFirebaseAuthComplete& OnComplete);

	/** 
	 * Sign in an existing user with email and password
	 * @param Email User's email address
	 * @param Password User's password
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Sign In with Email"))
	static void SignInWithEmail(const FString& Email, const FString& Password, 
		const FOnFirebaseAuthComplete& OnComplete);

	// === ANONYMOUS AUTHENTICATION ===

	/** 
	 * Sign in anonymously (no credentials required)
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Sign In Anonymously"))
	static void SignInAnonymously(const FOnFirebaseAuthComplete& OnComplete);

	// === GOOGLE SIGN-IN ===

	/** 
	 * Sign in with Google account
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Sign In with Google"))
	static void SignInWithGoogle(const FOnFirebaseAuthComplete& OnComplete);

	// === USER MANAGEMENT ===

	/** 
	 * Sign out the current user
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Sign Out"))
	static void SignOut();

	/** 
	 * Check if a user is currently signed in
	 * @return True if user is signed in
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Is User Signed In"))
	static bool IsUserSignedIn();

	/** 
	 * Get the current user's ID (UID)
	 * @return User ID or empty string if not signed in
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Get Current User ID"))
	static FString GetCurrentUserId();

	/** 
	 * Get the current user's email
	 * @return User email or empty string if not available
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Get Current User Email"))
	static FString GetCurrentUserEmail();

	/** 
	 * Get the current user's display name
	 * @return Display name or empty string if not set
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Get Current User Display Name"))
	static FString GetCurrentUserDisplayName();

	/** 
	 * Get the current user's authentication token
	 * @return Auth token or empty string if not signed in
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Get Auth Token"))
	static FString GetAuthToken();

	// === EMAIL VERIFICATION ===

	/** 
	 * Send email verification to current user
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Send Email Verification"))
	static void SendEmailVerification(const FOnFirebaseAuthComplete& OnComplete);

	/** 
	 * Check if current user's email is verified
	 * @return True if email is verified
	 */
	UFUNCTION(BlueprintPure, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Is Email Verified"))
	static bool IsEmailVerified();

	// === PASSWORD RESET ===

	/** 
	 * Send password reset email
	 * @param Email Email address to send reset link to
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Send Password Reset Email"))
	static void SendPasswordResetEmail(const FString& Email, 
		const FOnFirebaseAuthComplete& OnComplete);

	/** 
	 * Update current user's password
	 * @param NewPassword New password (minimum 6 characters)
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Update Password"))
	static void UpdatePassword(const FString& NewPassword, 
		const FOnFirebaseAuthComplete& OnComplete);

	// === PROFILE MANAGEMENT ===

	/** 
	 * Update current user's display name
	 * @param DisplayName New display name
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Update Display Name"))
	static void UpdateDisplayName(const FString& DisplayName, 
		const FOnFirebaseAuthComplete& OnComplete);

	/** 
	 * Delete the current user account
	 * @param OnComplete Callback when operation completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Authentication", 
		meta = (DisplayName = "Delete User Account"))
	static void DeleteUserAccount(const FOnFirebaseAuthComplete& OnComplete);

	// === INTERNAL METHODS ===

	/** Called from Java when auth operation completes */
	static void OnAuthResultReceived(bool bSuccess, const FString& UserId, 
		const FString& Email, const FString& DisplayName, 
		const FString& ErrorMessage, const FString& AuthToken);

	/** Get REST API instance (for non-Android platforms) */
	static UFirebaseRestAPI* GetRestAPI();

	/** Check if should use REST API (non-Android or forced) */
	static bool ShouldUseRestAPI();

private:
	/** Store callbacks for async operations */
	static TMap<FString, FOnFirebaseAuthComplete> PendingCallbacks;
	
	/** Current operation ID counter */
	static int32 CurrentOperationId;

	/** REST API instance for cross-platform support */
	static UFirebaseRestAPI* RestAPIInstance;

	/** Generate unique operation ID */
	static FString GenerateOperationId();

	/** Register callback for operation */
	static void RegisterCallback(const FString& OperationId, const FOnFirebaseAuthComplete& Callback);
};
