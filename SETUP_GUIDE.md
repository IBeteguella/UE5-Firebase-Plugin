# Firebase Plugin - Quick Setup Guide

## Step-by-Step Setup (15 minutes)

### Part 1: Firebase Console Setup (5 minutes)

1. **Create Firebase Project**
   - Go to https://console.firebase.google.com/
   - Click "Add project"
   - Enter project name (e.g., "MyUnrealGame")
   - Disable Google Analytics (optional for games)
   - Click "Create project"

2. **Add Android App**
   - Click the Android icon
   - **Package Name**: Get from Unreal Editor
     - Edit → Project Settings → Android → APK Packaging
     - Copy the "Package Name" (e.g., com.yourcompany.yourgame)
   - **App Nickname**: "My Unreal Game - Android"
   - **Debug SHA-1**: Leave blank for now (optional)
   - Click "Register app"

3. **Download Configuration**
   - Click "Download google-services.json"
   - Save this file - you'll need it in step 7!

4. **Enable Authentication**
   - In Firebase Console, click "Authentication" in left menu
   - Click "Get Started"
   - Go to "Sign-in method" tab
   - Enable "Email/Password" - Toggle ON and Save
   - Enable "Anonymous" - Toggle ON and Save

5. **Create Realtime Database**
   - Click "Realtime Database" in left menu
   - Click "Create Database"
   - Choose location (US, Europe, or Asia)
   - Start in "Test mode" (we'll secure it later)
   - Click "Enable"

6. **Get Your Configuration Values**
   - Click the gear icon → "Project settings"
   - Under "Your apps" → Android app → Copy these values:
     ```
     Project ID: your-project-id
     App ID: 1:123456789:android:abc123...
     API Key: AIza...
     ```
   - Under "Realtime Database" section:
     ```
     Database URL: https://your-project-default-rtdb.firebaseio.com
     ```
   - Under "Storage" section (if you plan to use it):
     ```
     Storage Bucket: your-project.appspot.com
     ```

### Part 2: Unreal Engine Setup (10 minutes)

7. **Place google-services.json**
   - Copy the downloaded `google-services.json` file
   - Paste it in your Unreal project root folder
   - Location: Same folder as your `.uproject` file
   ```
   MyProject/
   ├── MyProject.uproject  ← Your project file
   ├── google-services.json  ← Place here!
   ├── Content/
   ├── Plugins/
   │   └── FirebasePlugin/
   ```

8. **Enable Firebase Plugin**
   - Open your Unreal project
   - Edit → Plugins
   - Search for "Firebase"
   - Check the box to enable it
   - Click "Restart Now"

9. **Configure Plugin Settings**
   - Edit → Project Settings
   - Scroll down to: Plugins → Firebase
   - Fill in your Firebase configuration:

   **Firebase Project Settings:**
   ```
   Project ID: [Your project ID from step 6]
   Android App ID: [Your app ID from step 6]
   Android API Key: [Your API key from step 6]
   Database URL: [Your database URL from step 6]
   Storage Bucket: [Your storage bucket from step 6]
   ```

   **Authentication Settings:**
   - ✅ Enable Authentication
   - Add providers: Email, Anonymous

   **Database Settings:**
   - ✅ Enable Realtime Database
   - Database Region: [Match what you chose in step 5]
   - ✅ Enable Offline Persistence
   - Cache Size: 10 MB

   **Click "Save" at the bottom!**

10. **Configure Android Build Settings**
    - Project Settings → Platforms → Android
    - **Minimum SDK Version**: 23
    - **Target SDK Version**: 34
    - Advanced APK Packaging:
      - ✅ Package game data inside .apk
      - ✅ Use ExternalFilesDir

11. **Test the Setup**
    - Create a new Blueprint (e.g., BP_FirebaseTest)
    - In Event BeginPlay:
      - Add node: "Sign In Anonymously"
      - Connect OnComplete delegate
      - Print "Success: " + Result.UserId (if success)
    - Place the Blueprint in your level
    - Package for Android (or use Mobile Preview)
    - Check logs for "Firebase initialized successfully"

### Part 3: Secure Your Database (Production)

When ready for production, secure your database rules:

12. **Update Database Rules**
    - Firebase Console → Realtime Database → Rules
    - Replace test rules with secure rules:
    ```json
    {
      "rules": {
        "users": {
          "$uid": {
            ".read": "$uid === auth.uid",
            ".write": "$uid === auth.uid"
          }
        },
        "public": {
          ".read": true,
          ".write": "auth != null"
        }
      }
    }
    ```
    - Click "Publish"

## Verification Checklist

Before building your game, verify:

- [ ] google-services.json is in project root
- [ ] Plugin is enabled and editor restarted
- [ ] All Firebase settings are filled in Project Settings
- [ ] Authentication providers enabled in Firebase Console
- [ ] Database created in Firebase Console
- [ ] Database rules allow authenticated access
- [ ] Android Minimum SDK is 23 or higher
- [ ] Test Blueprint successfully authenticates

## Common Setup Issues

### Issue: "Firebase not initialized"
**Solution:** 
- Verify google-services.json exists in project root
- Check that file name is exactly `google-services.json` (not .txt)
- Rebuild project (delete Intermediate and Binaries folders)

### Issue: "Failed to authenticate"
**Solution:**
- Verify authentication provider is enabled in Firebase Console
- Check API key is correct in Project Settings
- Ensure device has internet connection

### Issue: "Package name mismatch"
**Solution:**
- Package name in Unreal must match Firebase Android app
- Check: Project Settings → Android → APK Packaging → Package Name
- Must match the package name you entered in Firebase Console

### Issue: Build fails with "Duplicate class"
**Solution:**
- Clean build: Delete Intermediate and Binaries folders
- File → Refresh Visual Studio Project
- Rebuild project

## Quick Test Blueprint

Create this simple test to verify everything works:

```
Event BeginPlay
  → Sign In Anonymously
    → OnComplete (Result)
      → Branch (Result.bSuccess)
        → True:
          → Print String: "Firebase Auth Success: " + Result.UserId
          → Set Value:
            - Path: "test/hello"
            - JSON: MakeJsonString("message", "Hello Firebase!")
            → OnComplete:
              → Print String: "Database Write Success!"
              → Get Value:
                - Path: "test/hello"
                → OnComplete:
                  → Print String: "Database Read Success: " + Result.Data
        → False:
          → Print String: "Firebase Error: " + Result.ErrorMessage
```

If this works, your Firebase plugin is correctly configured! 🎉

## Next Steps

Once setup is complete:
1. Read the main README.md for detailed usage examples
2. Check out the Blueprint examples in the plugin
3. Plan your data structure before building
4. Implement proper error handling
5. Test thoroughly before release

---

**Need Help?** Check the Troubleshooting section in README.md
