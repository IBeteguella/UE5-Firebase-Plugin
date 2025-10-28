// Copyright. All Rights Reserved.

#include "FirebaseAuth.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

// Initialize static members
TMap<FString, FOnFirebaseAuthComplete> UFirebaseAuth::PendingCallbacks;
int32 UFirebaseAuth::CurrentOperationId = 0;

FString UFirebaseAuth::GenerateOperationId()
{
	return FString::Printf(TEXT("AUTH_%d"), ++CurrentOperationId);
}

void UFirebaseAuth::RegisterCallback(const FString& OperationId, const FOnFirebaseAuthComplete& Callback)
{
	if (Callback.IsBound())
	{
		PendingCallbacks.Add(OperationId, Callback);
	}
}

// === EMAIL/PASSWORD AUTHENTICATION ===

void UFirebaseAuth::SignUpWithEmail(const FString& Email, const FString& Password, 
	const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jEmail = Env->NewStringUTF(TCHAR_TO_UTF8(*Email));
		jstring jPassword = Env->NewStringUTF(TCHAR_TO_UTF8(*Password));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SignUpMethod = Env->GetStaticMethodID(FirebaseHelperClass, "signUpWithEmail", 
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			if (SignUpMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SignUpMethod, jEmail, jPassword, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jEmail);
		Env->DeleteLocalRef(jPassword);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("Firebase Auth: SignUpWithEmail only available on Android"));
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

void UFirebaseAuth::SignInWithEmail(const FString& Email, const FString& Password, 
	const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jEmail = Env->NewStringUTF(TCHAR_TO_UTF8(*Email));
		jstring jPassword = Env->NewStringUTF(TCHAR_TO_UTF8(*Password));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SignInMethod = Env->GetStaticMethodID(FirebaseHelperClass, "signInWithEmail", 
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
			if (SignInMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SignInMethod, jEmail, jPassword, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jEmail);
		Env->DeleteLocalRef(jPassword);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("Firebase Auth: SignInWithEmail only available on Android"));
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === ANONYMOUS AUTHENTICATION ===

void UFirebaseAuth::SignInAnonymously(const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SignInMethod = Env->GetStaticMethodID(FirebaseHelperClass, "signInAnonymously", 
				"(Ljava/lang/String;)V");
			if (SignInMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SignInMethod, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jOperationId);
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("Firebase Auth: SignInAnonymously only available on Android"));
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === GOOGLE SIGN-IN ===

void UFirebaseAuth::SignInWithGoogle(const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SignInMethod = Env->GetStaticMethodID(FirebaseHelperClass, "signInWithGoogle", 
				"(Ljava/lang/String;)V");
			if (SignInMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SignInMethod, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jOperationId);
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("Firebase Auth: SignInWithGoogle only available on Android"));
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === USER MANAGEMENT ===

void UFirebaseAuth::SignOut()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SignOutMethod = Env->GetStaticMethodID(FirebaseHelperClass, "signOut", "()V");
			if (SignOutMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SignOutMethod);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	UE_LOG(LogTemp, Log, TEXT("Firebase Auth: User signed out"));
}

bool UFirebaseAuth::IsUserSignedIn()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID IsSignedInMethod = Env->GetStaticMethodID(FirebaseHelperClass, "isUserSignedIn", "()Z");
			if (IsSignedInMethod)
			{
				jboolean result = Env->CallStaticBooleanMethod(FirebaseHelperClass, IsSignedInMethod);
				Env->DeleteLocalRef(FirebaseHelperClass);
				return (bool)result;
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	return false;
}

FString UFirebaseAuth::GetCurrentUserId()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID GetUserIdMethod = Env->GetStaticMethodID(FirebaseHelperClass, "getCurrentUserId", 
				"()Ljava/lang/String;");
			if (GetUserIdMethod)
			{
				jstring jUserId = (jstring)Env->CallStaticObjectMethod(FirebaseHelperClass, GetUserIdMethod);
				if (jUserId)
				{
					const char* userId = Env->GetStringUTFChars(jUserId, nullptr);
					FString result = FString(UTF8_TO_TCHAR(userId));
					Env->ReleaseStringUTFChars(jUserId, userId);
					Env->DeleteLocalRef(jUserId);
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

FString UFirebaseAuth::GetCurrentUserEmail()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID GetEmailMethod = Env->GetStaticMethodID(FirebaseHelperClass, "getCurrentUserEmail", 
				"()Ljava/lang/String;");
			if (GetEmailMethod)
			{
				jstring jEmail = (jstring)Env->CallStaticObjectMethod(FirebaseHelperClass, GetEmailMethod);
				if (jEmail)
				{
					const char* email = Env->GetStringUTFChars(jEmail, nullptr);
					FString result = FString(UTF8_TO_TCHAR(email));
					Env->ReleaseStringUTFChars(jEmail, email);
					Env->DeleteLocalRef(jEmail);
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

FString UFirebaseAuth::GetCurrentUserDisplayName()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID GetDisplayNameMethod = Env->GetStaticMethodID(FirebaseHelperClass, 
				"getCurrentUserDisplayName", "()Ljava/lang/String;");
			if (GetDisplayNameMethod)
			{
				jstring jDisplayName = (jstring)Env->CallStaticObjectMethod(FirebaseHelperClass, 
					GetDisplayNameMethod);
				if (jDisplayName)
				{
					const char* displayName = Env->GetStringUTFChars(jDisplayName, nullptr);
					FString result = FString(UTF8_TO_TCHAR(displayName));
					Env->ReleaseStringUTFChars(jDisplayName, displayName);
					Env->DeleteLocalRef(jDisplayName);
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

FString UFirebaseAuth::GetAuthToken()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID GetTokenMethod = Env->GetStaticMethodID(FirebaseHelperClass, "getAuthToken", 
				"()Ljava/lang/String;");
			if (GetTokenMethod)
			{
				jstring jToken = (jstring)Env->CallStaticObjectMethod(FirebaseHelperClass, GetTokenMethod);
				if (jToken)
				{
					const char* token = Env->GetStringUTFChars(jToken, nullptr);
					FString result = FString(UTF8_TO_TCHAR(token));
					Env->ReleaseStringUTFChars(jToken, token);
					Env->DeleteLocalRef(jToken);
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

// === EMAIL VERIFICATION ===

void UFirebaseAuth::SendEmailVerification(const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SendVerificationMethod = Env->GetStaticMethodID(FirebaseHelperClass, 
				"sendEmailVerification", "(Ljava/lang/String;)V");
			if (SendVerificationMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SendVerificationMethod, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

bool UFirebaseAuth::IsEmailVerified()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID IsVerifiedMethod = Env->GetStaticMethodID(FirebaseHelperClass, "isEmailVerified", "()Z");
			if (IsVerifiedMethod)
			{
				jboolean result = Env->CallStaticBooleanMethod(FirebaseHelperClass, IsVerifiedMethod);
				Env->DeleteLocalRef(FirebaseHelperClass);
				return (bool)result;
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}
	}
#endif
	return false;
}

// === PASSWORD RESET ===

void UFirebaseAuth::SendPasswordResetEmail(const FString& Email, const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jEmail = Env->NewStringUTF(TCHAR_TO_UTF8(*Email));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID SendResetMethod = Env->GetStaticMethodID(FirebaseHelperClass, "sendPasswordResetEmail", 
				"(Ljava/lang/String;Ljava/lang/String;)V");
			if (SendResetMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, SendResetMethod, jEmail, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jEmail);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

void UFirebaseAuth::UpdatePassword(const FString& NewPassword, const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jPassword = Env->NewStringUTF(TCHAR_TO_UTF8(*NewPassword));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID UpdateMethod = Env->GetStaticMethodID(FirebaseHelperClass, "updatePassword", 
				"(Ljava/lang/String;Ljava/lang/String;)V");
			if (UpdateMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, UpdateMethod, jPassword, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jPassword);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === PROFILE MANAGEMENT ===

void UFirebaseAuth::UpdateDisplayName(const FString& DisplayName, const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jDisplayName = Env->NewStringUTF(TCHAR_TO_UTF8(*DisplayName));
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID UpdateMethod = Env->GetStaticMethodID(FirebaseHelperClass, "updateDisplayName", 
				"(Ljava/lang/String;Ljava/lang/String;)V");
			if (UpdateMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, UpdateMethod, jDisplayName, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jDisplayName);
		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

void UFirebaseAuth::DeleteUserAccount(const FOnFirebaseAuthComplete& OnComplete)
{
#if PLATFORM_ANDROID
	FString OperationId = GenerateOperationId();
	RegisterCallback(OperationId, OnComplete);

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring jOperationId = Env->NewStringUTF(TCHAR_TO_UTF8(*OperationId));

		jclass FirebaseHelperClass = FAndroidApplication::FindJavaClass("com/epicgames/unreal/FirebaseHelper");
		if (FirebaseHelperClass)
		{
			jmethodID DeleteMethod = Env->GetStaticMethodID(FirebaseHelperClass, "deleteUserAccount", 
				"(Ljava/lang/String;)V");
			if (DeleteMethod)
			{
				Env->CallStaticVoidMethod(FirebaseHelperClass, DeleteMethod, jOperationId);
			}
			Env->DeleteLocalRef(FirebaseHelperClass);
		}

		Env->DeleteLocalRef(jOperationId);
	}
#else
	FFirebaseAuthResult Result;
	Result.bSuccess = false;
	Result.ErrorMessage = TEXT("Platform not supported");
	OnComplete.ExecuteIfBound(Result);
#endif
}

// === CALLBACK HANDLER ===

void UFirebaseAuth::OnAuthResultReceived(bool bSuccess, const FString& UserId, 
	const FString& Email, const FString& DisplayName, 
	const FString& ErrorMessage, const FString& AuthToken)
{
	// Extract operation ID from the error message or use a different mechanism
	// For now, we'll call all pending callbacks
	FFirebaseAuthResult Result;
	Result.bSuccess = bSuccess;
	Result.UserId = UserId;
	Result.Email = Email;
	Result.DisplayName = DisplayName;
	Result.ErrorMessage = ErrorMessage;
	Result.AuthToken = AuthToken;

	// Execute all pending callbacks (we'll need to improve this with proper operation ID tracking)
	for (auto& Pair : PendingCallbacks)
	{
		Pair.Value.ExecuteIfBound(Result);
	}
	PendingCallbacks.Empty();

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Firebase Auth: Operation successful - User: %s"), *UserId);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Firebase Auth: Operation failed - %s"), *ErrorMessage);
	}
}

// === JNI CALLBACK FROM JAVA ===

#if PLATFORM_ANDROID
extern "C" JNIEXPORT void JNICALL Java_com_epicgames_unreal_FirebaseHelper_nativeOnAuthResult(
	JNIEnv* env, jclass clazz, jboolean success, jstring userId, jstring email, 
	jstring displayName, jstring errorMessage, jstring authToken)
{
	bool bSuccess = (bool)success;
	
	const char* userIdChars = env->GetStringUTFChars(userId, nullptr);
	FString UserIdStr = FString(UTF8_TO_TCHAR(userIdChars));
	env->ReleaseStringUTFChars(userId, userIdChars);

	const char* emailChars = env->GetStringUTFChars(email, nullptr);
	FString EmailStr = FString(UTF8_TO_TCHAR(emailChars));
	env->ReleaseStringUTFChars(email, emailChars);

	const char* displayNameChars = env->GetStringUTFChars(displayName, nullptr);
	FString DisplayNameStr = FString(UTF8_TO_TCHAR(displayNameChars));
	env->ReleaseStringUTFChars(displayName, displayNameChars);

	const char* errorChars = env->GetStringUTFChars(errorMessage, nullptr);
	FString ErrorStr = FString(UTF8_TO_TCHAR(errorChars));
	env->ReleaseStringUTFChars(errorMessage, errorChars);

	const char* tokenChars = env->GetStringUTFChars(authToken, nullptr);
	FString TokenStr = FString(UTF8_TO_TCHAR(tokenChars));
	env->ReleaseStringUTFChars(authToken, tokenChars);

	// Call on game thread
	AsyncTask(ENamedThreads::GameThread, [bSuccess, UserIdStr, EmailStr, DisplayNameStr, ErrorStr, TokenStr]()
	{
		UFirebaseAuth::OnAuthResultReceived(bSuccess, UserIdStr, EmailStr, DisplayNameStr, ErrorStr, TokenStr);
	});
}
#endif
