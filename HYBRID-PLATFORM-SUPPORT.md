# Firebase Plugin - Hybrid Platform Support

## Overview

The Firebase Plugin now supports **all platforms** through a hybrid approach:

- **Android**: Native Firebase SDK via Java/JNI (optimal performance)
- **Other Platforms** (iOS, Windows, Mac, Linux, Web): Firebase REST API (universal compatibility)

## Architecture

### Platform Detection

The plugin automatically detects the platform and chooses the appropriate implementation:

```
┌─────────────────┐
│  Blueprint API  │ (Same interface for all platforms)
└────────┬────────┘
         │
    ┌────▼─────┐
    │ Platform │
    │ Detection│
    └────┬─────┘
         │
    ┌────┴─────┐
    │          │
┌───▼───┐  ┌──▼─────┐
│Android│  │  Other │
│  JNI  │  │Platforms│
│+ Java │  │  REST  │
│  SDK  │  │  API   │
└───────┘  └────────┘
```

### Configuration Methods

#### Method 1: Manual Input
Enter Firebase configuration directly in Project Settings:
- Project ID
- API Key
- Database URL
- App ID
- Storage Bucket
- Messaging Sender ID

#### Method 2: Import from google-services.json
1. Select google-services.json file in Project Settings
2. Plugin automatically extracts all configuration
3. Optionally auto-copies to Build/Android folder

## Features

### REST API Implementation (`FirebaseRestAPI.h/cpp`)

**Authentication:**
- ✅ Sign up with email/password
- ✅ Sign in with email/password
- ✅ Sign in anonymously
- ✅ Password reset
- ✅ Update email/password
- ✅ Send email verification
- ✅ Delete account
- ✅ Update profile
- ✅ Token refresh

**Database:**
- ✅ Set value
- ✅ Get value
- ✅ Update value (partial)
- ✅ Delete value
- ✅ Push value (auto-generated key)
- ✅ Query with order by child
- ✅ Query with limit to first/last
- ✅ Query with start at/end at/equal to

### Platform-Specific Optimizations

#### Android (Native SDK)
- Direct Java Firebase SDK calls
- Real-time listeners with native callbacks
- Offline persistence
- Full transaction support
- Optimal battery usage

#### Other Platforms (REST API)
- HTTP-based Firebase REST API
- Polling for real-time updates (configurable interval)
- JSON-based communication
- No additional dependencies
- Works on **all** platforms

## Usage

### In Project Settings

1. Navigate to **Project Settings → Plugins → Firebase**

2. Choose **Configuration Method**:
   - **Manual Input**: Enter values directly
   - **Import from google-services.json**: Browse and select file

3. Configure **Platform Settings**:
   - **Use REST API for Non-Android**: Enable REST API fallback (default: true)
   - **REST API Polling Interval**: Seconds between updates (default: 5.0)

### In Blueprints

**Same API works on all platforms!**

```blueprint
// Sign in with email - works everywhere!
Sign In With Email
  Email: "user@example.com"
  Password: "password123"
  On Complete: [Handle Result]
```

```blueprint
// Set database value - works everywhere!
Set Value
  Path: "users/123/name"
  JSON Value: "\"John Doe\""
  On Complete: [Handle Result]
```

## Comparison

| Feature | Android (Native) | Other Platforms (REST) |
|---------|------------------|------------------------|
| Authentication | ✅ Full support | ✅ Full support |
| Database Read/Write | ✅ Real-time | ✅ Polling-based |
| Offline Mode | ✅ Native | ⚠️ Limited |
| Transactions | ✅ Optimistic | ⚠️ REST-based |
| Performance | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| Latency | Very Low | Low |
| Battery Usage | Optimized | Standard HTTP |
| Setup Complexity | Medium | Low |

## REST API Limitations

### Real-Time Listeners
- REST API uses **polling** instead of WebSocket connections
- Default interval: 5 seconds (configurable)
- Trade-off: Slight delay vs. universal compatibility

**Workaround**: Reduce polling interval for more responsive updates

### Transactions
- REST API transactions are **not** optimistic
- May fail under high concurrency
- Suitable for most game scenarios

**Workaround**: Use conditional writes or implement retry logic

### Offline Persistence
- No automatic offline caching (REST API)
- Must implement custom caching if needed

**Workaround**: Use Unreal's SaveGame system for offline data

## Configuration File Parsing

The plugin automatically parses `google-services.json` and extracts:

```json
{
  "project_info": {
    "project_id": "your-project",
    "firebase_url": "https://your-project.firebaseio.com",
    "project_number": "1234567890",
    "storage_bucket": "your-project.appspot.com"
  },
  "client": [{
    "client_info": {
      "android_client_info": {
        "mobilesdk_app_id": "1:1234:android:abcd"
      }
    },
    "api_key": [{
      "current_key": "AIza..."
    }]
  }]
}
```

Values are automatically populated in Firebase settings!

## Best Practices

### For Multi-Platform Games

1. **Test on target platforms early**
   - REST API behavior may differ from native
   - Adjust polling interval based on requirements

2. **Design for eventual consistency**
   - REST API has slight delays
   - Use optimistic UI updates

3. **Implement proper error handling**
   - Network errors more common with REST
   - Show retry options to users

### For Android-Only Games

1. **Keep REST API disabled**
   - Uncheck "Use REST API for Non-Android"
   - Get full native performance

2. **Use advanced features**
   - Offline persistence
   - Optimistic transactions
   - Connection state monitoring

## File Import Auto-Copy

When **Auto-copy to Build Folder** is enabled:

1. Select google-services.json in settings
2. Plugin parses configuration
3. File is automatically copied to `Build/Android/google-services.json`
4. Android packaging includes the file
5. No manual copying required!

## REST API Endpoints Used

### Authentication
- `https://identitytoolkit.googleapis.com/v1/accounts:signUp`
- `https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword`
- `https://identitytoolkit.googleapis.com/v1/accounts:update`
- `https://identitytoolkit.googleapis.com/v1/accounts:delete`
- `https://securetoken.googleapis.com/v1/token`

### Database
- `https://your-project.firebaseio.com/{path}.json`
- Query parameters: `auth`, `orderBy`, `limitToFirst`, `limitToLast`, `startAt`, `endAt`, `equalTo`

## Migration Guide

### From Android-Only Version

1. **No code changes required!**
2. Enable "Use REST API for Non-Android" in settings
3. Test on new platforms
4. Adjust polling interval if needed

### From Other Firebase Plugins

1. Import google-services.json using configuration UI
2. Replace old authentication calls with new API
3. Replace old database calls with new API
4. Test thoroughly

## Troubleshooting

### "Platform not supported" error
- **Solution**: Enable "Use REST API for Non-Android" in Project Settings

### Slow real-time updates on non-Android
- **Solution**: Reduce "REST API Polling Interval" (e.g., to 2-3 seconds)
- **Caution**: Lower intervals = more network usage

### google-services.json not found during packaging
- **Solution**: Enable "Auto-copy to Build Folder" and re-select the file
- **Alternative**: Manually copy to `ProjectRoot/Build/Android/`

### REST API authentication fails
- **Solution**: Verify API Key in Project Settings matches Firebase Console
- **Check**: Ensure API Key has no restrictions or allows your domains

## Performance Tips

1. **Batch database operations** - Reduce REST API calls
2. **Cache user authentication tokens** - Avoid repeated sign-ins
3. **Use query limits** - Fetch only needed data
4. **Implement exponential backoff** - For retry logic
5. **Monitor network usage** - Adjust polling interval accordingly

## Future Enhancements

Planned features for REST API implementation:

- [ ] WebSocket support for true real-time updates
- [ ] Client-side offline caching
- [ ] Optimistic transaction emulation
- [ ] Automatic retry with exponential backoff
- [ ] Connection state monitoring
- [ ] Bandwidth usage optimization

## Summary

The hybrid approach provides:

✅ **Universal platform support** - Works on iOS, Windows, Mac, Linux, Web
✅ **Optimal Android performance** - Native SDK when available
✅ **Single Blueprint API** - Same code for all platforms
✅ **Easy configuration** - Manual or file-based setup
✅ **Production-ready** - Comprehensive error handling
✅ **Well-documented** - Clear examples and guides

Choose the best configuration for your game's needs!
