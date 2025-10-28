# Firebase Plugin - Project Structure

## Complete Plugin File Structure

```
Plugins/FirebasePlugin/
│
├── FirebasePlugin.uplugin          # Plugin descriptor
├── README.md                        # Comprehensive documentation
├── SETUP_GUIDE.md                   # Quick setup guide
├── BLUEPRINT_EXAMPLES.md            # Blueprint usage examples
├── google-services.json.template    # Template for Firebase config
│
├── Resources/                       # Plugin resources
│   └── Icon128.png                  # Plugin icon (128x128)
│
└── Source/FirebasePlugin/
    ├── FirebasePlugin.Build.cs      # Build configuration
    ├── FirebasePlugin_UPL.xml       # Android UPL configuration
    │
    ├── Public/                      # Public header files
    │   ├── FirebasePluginModule.h   # Module definition
    │   ├── FirebaseSettings.h       # Plugin settings (Editor)
    │   ├── FirebaseAuth.h           # Authentication API
    │   └── FirebaseDatabase.h       # Database API
    │
    ├── Private/                     # Implementation files
    │   ├── FirebasePluginModule.cpp # Module implementation
    │   ├── FirebaseSettings.cpp     # Settings implementation
    │   ├── FirebaseAuth.cpp         # Auth implementation
    │   └── FirebaseDatabase.cpp     # Database implementation
    │
    └── Java/                        # Android Java code
        └── FirebaseHelper.java      # JNI bridge for Android
```

## Plugin Architecture

### C++ Layer (Unreal Engine)
- **FirebaseAuth**: Blueprint function library for authentication
- **FirebaseDatabase**: Blueprint function library for database operations
- **FirebaseSettings**: Configuration in Project Settings
- **FirebasePluginModule**: Plugin lifecycle management

### JNI Bridge
- **JNI calls**: C++ calls Java methods via JNI
- **Native callbacks**: Java calls C++ via native methods
- **Thread safety**: All callbacks marshaled to game thread

### Java Layer (Android)
- **FirebaseHelper**: Wrapper around Firebase Android SDK
- **Authentication**: Email/password, anonymous, Google sign-in
- **Database**: CRUD operations, listeners, queries, transactions
- **Error handling**: Proper error propagation to C++ layer

## Data Flow

### Authentication Flow
```
Blueprint Node (Sign In)
    ↓
UFirebaseAuth::SignInWithEmail (C++)
    ↓
JNI Call → FirebaseHelper.signInWithEmail (Java)
    ↓
Firebase Auth SDK (Android)
    ↓
OnCompleteListener → nativeOnAuthResult (Java)
    ↓
JNI Callback → UFirebaseAuth::OnAuthResultReceived (C++)
    ↓
Execute Blueprint Delegate (OnComplete)
```

### Database Flow
```
Blueprint Node (Set Value)
    ↓
UFirebaseDatabase::SetValue (C++)
    ↓
JNI Call → FirebaseHelper.setDatabaseValue (Java)
    ↓
Firebase Database SDK (Android)
    ↓
OnSuccessListener → nativeOnDatabaseResult (Java)
    ↓
JNI Callback → UFirebaseDatabase::OnDatabaseResultReceived (C++)
    ↓
Execute Blueprint Delegate (OnComplete)
```

### Real-time Listener Flow
```
Blueprint Node (Listen for Value Changes)
    ↓
UFirebaseDatabase::ListenForValueChanges (C++)
    ↓
JNI Call → FirebaseHelper.listenForValueChanges (Java)
    ↓
Firebase ValueEventListener (Android)
    ↓
onDataChange → nativeOnDatabaseValueChanged (Java)
    ↓
JNI Callback → UFirebaseDatabase::OnDatabaseValueChanged (C++)
    ↓
Execute Blueprint Delegate (OnValueChanged)
    ↓ (continuously fires on changes)
```

## Key Components

### 1. UPL (Unreal Plugin Language)
**File**: `FirebasePlugin_UPL.xml`

Handles Android-specific integration:
- Modifies AndroidManifest.xml
- Adds Gradle dependencies
- Copies Java files
- Configures ProGuard rules
- Injects code into GameActivity

### 2. Settings System
**Files**: `FirebaseSettings.h/cpp`

Provides configuration interface:
- Accessible via Project Settings → Plugins → Firebase
- Stores configuration in DefaultEngine.ini
- Validates settings before use
- Supports different configurations per platform

### 3. Blueprint Function Libraries
**Files**: `FirebaseAuth.h/cpp`, `FirebaseDatabase.h/cpp`

Pure static functions callable from Blueprint:
- Marked with `UFUNCTION(BlueprintCallable)`
- Use delegates for async callbacks
- Platform-aware (only works on Android)
- Thread-safe callback handling

### 4. Java Helper
**File**: `FirebaseHelper.java`

Android Firebase SDK wrapper:
- Singleton pattern for Firebase instances
- Handles UI thread requirements
- Implements all Firebase operations
- Provides native callback methods

## Build Process

### 1. When Packaging for Android:

```
1. Unreal Build Tool reads .Build.cs file
   → Includes FirebasePlugin module
   → Adds platform-specific dependencies

2. UPL system processes FirebasePlugin_UPL.xml
   → Copies FirebaseHelper.java to build directory
   → Modifies build.gradle with Firebase dependencies
   → Updates AndroidManifest.xml with permissions
   → Injects initialization code into GameActivity

3. Gradle build includes Firebase SDK
   → Downloads Firebase libraries from Maven
   → Processes google-services.json
   → Bundles everything into APK

4. C++ code compiled with JNI support
   → Links to Android JNI headers
   → Includes native callback exports
```

## Configuration Files

### DefaultEngine.ini (auto-generated)
```ini
[/Script/FirebasePlugin.FirebaseSettings]
ProjectId=your-project-id
AndroidAppId=1:123456789:android:abc
AndroidApiKey=AIza...
DatabaseUrl=https://your-project.firebaseio.com
bEnableAuthentication=True
bEnableRealtimeDatabase=True
```

### google-services.json (required)
- Download from Firebase Console
- Place in project root
- Contains app credentials and configuration
- Processed by Gradle during build

## Memory Management

### C++ Side
- Static maps store pending callbacks
- Callbacks removed after execution
- Listeners stored until explicitly removed
- No memory leaks from JNI references (properly deleted)

### Java Side
- Firebase instances managed by SDK
- Listeners stored in HashMap
- Proper cleanup on activity destroy
- Weak references where appropriate

## Thread Safety

### Game Thread Marshaling
All callbacks from Java are marshaled to game thread:
```cpp
AsyncTask(ENamedThreads::GameThread, [...]() {
    // Execute callback on game thread
});
```

### UI Thread Operations
All Firebase operations run on Android UI thread:
```java
activity.runOnUiThread(new Runnable() {
    @Override
    public void run() {
        // Firebase operation
    }
});
```

## Error Handling

### Layered Error Handling
1. **Java Layer**: Catches Firebase exceptions
2. **JNI Layer**: Validates parameters
3. **C++ Layer**: Checks initialization state
4. **Blueprint Layer**: Result struct with bSuccess flag

### Error Propagation
```
Firebase SDK Error
    ↓
Java Exception Caught
    ↓
Error message extracted
    ↓
Passed to native callback
    ↓
C++ logs error
    ↓
Blueprint receives error in Result
```

## Performance Considerations

### Optimization Techniques
1. **Callback pooling**: Reuse callback objects
2. **JSON caching**: Cache parsed JSON objects
3. **Listener management**: Stop unused listeners
4. **Offline persistence**: Reduces network calls
5. **Batch operations**: Combine multiple updates

### Network Efficiency
- Firebase SDK handles connection pooling
- Automatic retry with exponential backoff
- Delta sync for real-time listeners
- Local cache reduces bandwidth

## Security Notes

### API Key Security
- API key in google-services.json is public
- Server-side security via Database Rules
- Never trust client-side validation alone

### Database Rules
Always implement proper security rules:
```json
{
  "rules": {
    "users": {
      "$uid": {
        ".read": "$uid === auth.uid",
        ".write": "$uid === auth.uid"
      }
    }
  }
}
```

### Best Practices
1. Validate all data server-side
2. Use auth.uid in rules
3. Limit read/write access
4. Implement rate limiting
5. Monitor usage in Firebase Console

## Debugging

### Enable Verbose Logging
In FirebaseSettings:
- Enable "Verbose Logging"
- Check Android logcat for detailed logs

### Common Debug Commands
```
# Filter Firebase logs
adb logcat | grep Firebase

# Filter plugin logs  
adb logcat | grep FirebasePlugin

# Clear app data
adb shell pm clear com.yourcompany.yourgame
```

### Logcat Tags
- `FirebaseHelper`: Java layer logs
- `FirebasePlugin`: C++ layer logs
- `Firebase*`: Firebase SDK logs

## Extension Points

### Adding New Features
1. Add C++ function in header file
2. Implement JNI call in .cpp file
3. Add Java method in FirebaseHelper.java
4. Update UPL XML if needed
5. Test and document

### Adding New Platforms
1. Create platform-specific implementation
2. Add preprocessor directives (#if PLATFORM_IOS)
3. Create platform UPL file
4. Update Build.cs with platform dependencies

## Version Compatibility

### Minimum Requirements
- Unreal Engine: 5.0+
- Android SDK: 23 (Android 6.0)
- Target SDK: 34 (Android 14)
- Firebase SDK: 33.7.0+
- Google Play Services: 18.5.0+

### Tested Configurations
- UE 5.4 + Android SDK 34
- UE 5.3 + Android SDK 33
- Firebase BoM 33.7.0

---

**This plugin provides a complete Firebase integration for Unreal Engine with production-ready code!**
