# Firebase Realtime Database & Authentication Plugin for Unreal Engine

A comprehensive Firebase plugin for Unreal Engine with full Blueprint support for Android. This plugin provides complete integration with Firebase Authentication and Realtime Database, allowing you to build online multiplayer games, user management systems, and cloud-synced data storage.

## Features

### 🔐 Firebase Authentication
- **Email/Password Authentication** - Sign up and sign in with email/password
- **Anonymous Authentication** - Allow users to play without registration
- **Google Sign-In** - One-tap sign-in with Google accounts (coming soon)
- **User Management** - Update profiles, change passwords, delete accounts
- **Email Verification** - Send and verify email addresses
- **Password Reset** - Send password reset emails

### 💾 Firebase Realtime Database
- **Real-time Sync** - Automatic data synchronization across all clients
- **CRUD Operations** - Create, Read, Update, Delete data with simple Blueprint nodes
- **Push Operations** - Generate unique IDs for list items
- **Query Support** - Filter and order data with advanced queries
- **Transactions** - Atomic data updates to prevent race conditions
- **Offline Support** - Data persistence and automatic sync when reconnected
- **Value Listeners** - Real-time callbacks when data changes

### 🎮 Full Blueprint Support
- All functionality accessible through Blueprint nodes
- Async operations with callbacks and delegates
- Easy-to-use JSON helper functions
- No C++ knowledge required

## Installation

### 1. Download the Plugin
- Copy the `FirebasePlugin` folder to your project's `Plugins` directory
- If the `Plugins` folder doesn't exist, create it in your project root

### 2. Enable the Plugin
1. Open your Unreal Engine project
2. Go to **Edit → Plugins**
3. Search for "Firebase"
4. Check the box to enable **Firebase Realtime Database & Auth**
5. Restart the editor when prompted

### 3. Configure Firebase Project

#### Create Firebase Project
1. Go to [Firebase Console](https://console.firebase.google.com/)
2. Click **Add Project**
3. Enter your project name and follow the setup wizard
4. Once created, click the **Android** icon to add an Android app

#### Get Firebase Configuration
1. In Firebase Console, go to **Project Settings** (gear icon)
2. Under **Your apps**, select your Android app
3. Copy the following values:
   - **Project ID**: Found under "Project settings → General"
   - **Android App ID**: `1:123456789:android:abcdef...`
   - **API Key**: `AIza...`
   - **Database URL**: `https://your-project-default-rtdb.firebaseio.com`
   - **Storage Bucket**: `your-project.appspot.com`

#### Download google-services.json
1. In the Android app section, click **Download google-services.json**
2. Place this file in your project root directory (same level as your `.uproject` file)

### 4. Configure Plugin Settings
1. In Unreal Editor, go to **Edit → Project Settings**
2. Scroll down to **Plugins → Firebase**
3. Enter your Firebase configuration:
   - **Project ID**: Your Firebase project ID
   - **Android App ID**: Your Android app ID from Firebase
   - **Android API Key**: Your API key
   - **Database URL**: Your Realtime Database URL
   - **Storage Bucket**: Your storage bucket name

#### Enable Authentication Providers
1. In the Firebase settings, check **Enable Authentication**
2. Add the authentication providers you want to use:
   - Email/Password
   - Anonymous
   - Google Sign-In (requires additional setup)

#### Configure Database Settings
1. Check **Enable Realtime Database**
2. Choose your **Database Region** (US Central, Europe West, or Asia Southeast)
3. Enable **Offline Persistence** for offline support
4. Set **Cache Size** (default: 10 MB)

### 5. Set Database Rules
1. Go to Firebase Console → **Realtime Database** → **Rules**
2. For development, you can use open rules (⚠️ **NOT for production**):
```json
{
  "rules": {
    ".read": "auth != null",
    ".write": "auth != null"
  }
}
```

3. For production, implement proper security rules based on your data structure

### 6. Build Settings
1. Go to **Project Settings → Platforms → Android**
2. Set **Minimum SDK Version**: 23 (Android 6.0)
3. Set **Target SDK Version**: 34 (Android 14)
4. Enable **Package game data inside .apk**
5. Under **Advanced APK Packaging**, enable:
   - ✅ Use ExternalFilesDir
   - ✅ Configure rules for Firebase

## Usage Examples

### Authentication

#### Sign Up with Email
```
// Create Event Graph nodes:
1. Create "Sign Up with Email" node
2. Enter Email and Password inputs
3. Connect OnComplete delegate to handle result
4. Check "bSuccess" in result to verify signup
```

#### Sign In with Email
```
1. Create "Sign In with Email" node
2. Enter Email and Password
3. On success, user is authenticated
4. Use "Get Current User ID" to get the user's UID
```

#### Check if User is Signed In
```
1. Use "Is User Signed In" node (Pure function)
2. Returns true if user is authenticated
3. Use in Branch nodes to control flow
```

#### Sign Out
```
1. Use "Sign Out" node
2. Clears current user session
```

### Realtime Database

#### Save Data
```
1. Create "Make JSON String" nodes for your data:
   - "username": MakeJsonString("username", "Player123")
   - "score": MakeJsonInt("score", 1000)
   - "level": MakeJsonInt("level", 5)

2. Combine JSON strings using "Combine JSON"

3. Use "Set Value" node:
   - Path: "users/[USER_ID]/profile"
   - JSON Data: Combined JSON string
   
4. Handle OnComplete callback
```

#### Read Data
```
1. Use "Get Value" node:
   - Path: "users/[USER_ID]/profile"
   
2. In OnComplete callback:
   - Check bSuccess
   - Parse Data (JSON string) using "Get JSON Value"
```

#### Listen for Real-time Updates
```
1. Use "Listen for Value Changes" node:
   - Path: "game/status"
   
2. Connect OnValueChanged delegate:
   - Fires whenever data at path changes
   - Receives Path and Data parameters
   
3. Remember to "Stop Listening" when done!
```

#### Push to List
```
1. Use "Push Value" node to add items with auto-generated IDs:
   - Path: "chat/messages"
   - JSON Data: {"text": "Hello World", "sender": "User123"}
   
2. Firebase creates unique ID for each message
```

#### Delete Data
```
1. Use "Delete Value" node:
   - Path: "users/[USER_ID]/temp_data"
   
2. Removes data at specified path
```

### JSON Helper Functions

The plugin includes helpful Blueprint nodes for working with JSON:

- **Make JSON String** - Create JSON from string value
- **Make JSON Int** - Create JSON from integer value
- **Make JSON Float** - Create JSON from float value
- **Make JSON Bool** - Create JSON from boolean value
- **Combine JSON** - Merge multiple JSON strings into one object
- **Get JSON Value** - Extract value from JSON string by key

## Example: User Profile System

Here's a complete example of a user profile system:

### 1. Sign Up Flow
```
Event BeginPlay
  → Is User Signed In?
    → True: Load User Profile
    → False: Show Login UI

Button "Sign Up" Clicked
  → Sign Up with Email (Email, Password)
    → OnComplete:
      → If Success:
        → Create Initial Profile
        → Go to Main Menu
      → If Failed:
        → Show Error Message
```

### 2. Create Profile
```
Function: CreateUserProfile

1. Get Current User ID → UserID

2. Create Profile JSON:
   - MakeJsonString("displayName", DisplayName)
   - MakeJsonInt("level", 1)
   - MakeJsonInt("score", 0)
   - MakeJsonInt("createdAt", CurrentTimestamp)
   - Combine JSON → ProfileJSON

3. Set Value:
   - Path: "users/" + UserID + "/profile"
   - JSON Data: ProfileJSON
```

### 3. Update Score
```
Function: UpdatePlayerScore

1. Get Current User ID → UserID

2. MakeJsonInt("score", NewScore) → ScoreJSON

3. Update Value:
   - Path: "users/" + UserID + "/profile"
   - JSON Data: ScoreJSON
```

### 4. Real-time Leaderboard
```
Event BeginPlay
  → Listen for Value Changes
    - Path: "leaderboard"
    - OnValueChanged:
      → Parse JSON
      → Update UI with top players
      
Event EndPlay
  → Stop Listening
    - Path: "leaderboard"
```

## Advanced Features

### Offline Support

The plugin automatically handles offline scenarios:
- **Data Persistence**: Enable in plugin settings
- **Auto-sync**: Changes sync when connection restored
- **Keep Synced**: Mark specific paths for offline access

```
// Keep high-priority data synced
Keep Synced
  - Path: "users/[USER_ID]/inventory"
  - Keep Synced: True
```

### Transactions

Use transactions for atomic updates (e.g., preventing double-spending):

```
Run Transaction
  - Path: "users/[USER_ID]/coins"
  - JSON Data: {"coins": NewAmount}
  - OnComplete: Handle result
```

### Queries

Filter and order data:

```
Query Values
  - Path: "scores"
  - Order By Key: "score"
  - Limit To First: 10
  - Start At: "0"
  - End At: "1000"
```

## Troubleshooting

### Build Errors

**Error: "Firebase not initialized"**
- Solution: Make sure google-services.json is in your project root
- Verify Firebase settings in Project Settings

**Error: "Duplicate class found"**
- Solution: Clean and rebuild project
- Delete Intermediate and Binaries folders

**Error: "API key not valid"**
- Solution: Check that you've entered the correct API key
- Make sure the API key is enabled in Firebase Console

### Runtime Issues

**Authentication fails silently**
- Check Logcat for detailed error messages
- Verify email/password authentication is enabled in Firebase Console
- Ensure proper internet connectivity

**Database operations don't work**
- Verify Database Rules allow read/write for authenticated users
- Check that Database URL is correct in settings
- Look for Firebase errors in Android logcat

**Data doesn't sync in real-time**
- Make sure you're using "Listen for Value Changes", not just "Get Value"
- Check network connectivity
- Verify the path is correct

## Best Practices

### Security
1. **Never use open database rules in production**
2. **Validate all data on the server side** (use Cloud Functions)
3. **Don't store sensitive data** in Realtime Database
4. **Enable email verification** for email/password auth
5. **Implement rate limiting** to prevent abuse

### Performance
1. **Use listeners sparingly** - Too many listeners can impact performance
2. **Always stop listening** when done (in EndPlay event)
3. **Denormalize data** for faster queries
4. **Use indexes** for complex queries
5. **Keep data structures flat** when possible

### Data Structure
```
✅ Good Structure:
users/
  user123/
    profile/
      name: "Player"
      level: 5
    inventory/
      item1: {name: "Sword", quantity: 1}
      item2: {name: "Shield", quantity: 2}

❌ Bad Structure:
users/
  user123: {
    name: "Player",
    level: 5,
    inventory: [
      {name: "Sword", quantity: 1},
      {name: "Shield", quantity: 2}
    ]
  }
```

## Support & Resources

- **Firebase Documentation**: https://firebase.google.com/docs
- **Realtime Database Guide**: https://firebase.google.com/docs/database
- **Authentication Guide**: https://firebase.google.com/docs/auth
- **Unreal Engine Forums**: Post issues in the Mobile Development section

## License

This plugin is provided as-is for use in your Unreal Engine projects.

## Version History

- **v1.0.0** - Initial release
  - Firebase Authentication (Email/Password, Anonymous)
  - Firebase Realtime Database (Read, Write, Listen, Query)
  - Full Blueprint support
  - Offline persistence
  - Android support

## Roadmap

Future features planned:
- iOS support
- Google Sign-In implementation
- Facebook authentication
- Cloud Firestore support
- Cloud Functions triggers
- Firebase Storage integration
- Push notifications (FCM)

---

**Made for Unreal Engine 5.x** | **Android Support** | **Blueprint Native**
