# Firebase Plugin - Quick Reference Card

## 🔐 Authentication Nodes

| Node Name | Description | Parameters |
|-----------|-------------|------------|
| **Sign Up with Email** | Create new user account | Email, Password → Result |
| **Sign In with Email** | Sign in existing user | Email, Password → Result |
| **Sign In Anonymously** | Sign in without credentials | → Result |
| **Sign Out** | Sign out current user | None |
| **Is User Signed In** | Check if authenticated | → Bool |
| **Get Current User ID** | Get user's UID | → String |
| **Get Current User Email** | Get user's email | → String |
| **Send Email Verification** | Send verification email | → Result |
| **Send Password Reset Email** | Send password reset | Email → Result |
| **Update Password** | Change user password | NewPassword → Result |
| **Update Display Name** | Change user name | DisplayName → Result |
| **Delete User Account** | Delete current user | → Result |

## 💾 Database Nodes

### Write Operations
| Node Name | Description | Parameters |
|-----------|-------------|------------|
| **Set Value** | Create/overwrite data | Path, JSON → Result |
| **Update Value** | Merge with existing | Path, JSON → Result |
| **Push Value** | Add with auto ID | Path, JSON → Result |
| **Delete Value** | Remove data | Path → Result |

### Read Operations
| Node Name | Description | Parameters |
|-----------|-------------|------------|
| **Get Value** | One-time read | Path → Result |
| **Listen for Value Changes** | Real-time updates | Path → OnChanged |
| **Stop Listening** | Stop real-time | Path |
| **Query Values** | Filter and order | Path, Options → Result |

### Advanced
| Node Name | Description | Parameters |
|-----------|-------------|------------|
| **Run Transaction** | Atomic update | Path, JSON → Result |
| **Enable Offline Persistence** | Cache data locally | None |
| **Keep Synced** | Priority sync | Path, Bool |
| **Go Online/Offline** | Manual connection | None |
| **Generate Push ID** | Unique ID | → String |

## 🔧 JSON Helper Nodes

| Node Name | Description | Example |
|-----------|-------------|---------|
| **Make JSON String** | String to JSON | "name", "John" → `{"name":"John"}` |
| **Make JSON Int** | Integer to JSON | "score", 100 → `{"score":100}` |
| **Make JSON Float** | Float to JSON | "health", 75.5 → `{"health":75.5}` |
| **Make JSON Bool** | Boolean to JSON | "active", true → `{"active":true}` |
| **Combine JSON** | Merge objects | [JSON1, JSON2] → Combined |
| **Get JSON Value** | Extract value | JSON, "key" → Value |

## 📊 Result Structures

### FirebaseAuthResult
```
- bSuccess (Bool)        // True if operation succeeded
- UserId (String)        // User's unique ID (UID)
- Email (String)         // User's email address
- DisplayName (String)   // User's display name
- ErrorMessage (String)  // Error description if failed
- AuthToken (String)     // Authentication token
```

### FirebaseDatabaseResult
```
- bSuccess (Bool)        // True if operation succeeded
- Path (String)          // Database path accessed
- Data (String)          // Retrieved data as JSON
- ErrorMessage (String)  // Error description if failed
```

## 🎯 Common Patterns

### Pattern 1: Sign Up Flow
```
Sign Up with Email (Email, Password)
  → OnComplete (Result)
    → If Success:
      → Create User Profile
      → Go to Main Menu
    → If Failed:
      → Show Error Message
```

### Pattern 2: Save Player Data
```
1. Make JSON String ("username", Username)
2. Make JSON Int ("level", Level)
3. Combine JSON → PlayerData
4. Set Value ("users/{UID}/profile", PlayerData)
```

### Pattern 3: Load Player Data
```
Get Value ("users/{UID}/profile")
  → OnComplete (Result)
    → Get JSON Value (Result.Data, "username")
    → Get JSON Value (Result.Data, "level")
    → Update UI
```

### Pattern 4: Real-Time Listener
```
// Start
Listen for Value Changes ("game/status")
  → OnValueChanged (Path, Data)
    → Parse Data
    → Update Game State

// Stop (in EndPlay)
Stop Listening ("game/status")
```

### Pattern 5: Check Auth Before Action
```
Is User Signed In?
  → Branch
    → True: Perform Action
    → False: Show Login Screen
```

## ⚙️ Project Settings Location

**Edit → Project Settings → Plugins → Firebase**

Required Settings:
- Project ID
- Android App ID
- Android API Key
- Database URL
- Storage Bucket

## 📁 File Locations

| File | Location |
|------|----------|
| **google-services.json** | Project Root (next to .uproject) |
| **Plugin** | Plugins/FirebasePlugin/ |
| **Settings** | Config/DefaultEngine.ini |

## 🐛 Quick Troubleshooting

| Problem | Solution |
|---------|----------|
| "Firebase not initialized" | Check google-services.json location |
| "Auth failed" | Enable provider in Firebase Console |
| "Permission denied" | Update database rules |
| Build fails | Clean: Delete Intermediate + Binaries |
| No real-time updates | Use "Listen" not "Get Value" |

## 🔒 Security Checklist

- [ ] Database rules not in "test mode"
- [ ] User data readable only by owner
- [ ] Sensitive data not in database
- [ ] Email verification enabled
- [ ] Rate limiting implemented

## 📱 Build Requirements

- **Min SDK**: 23 (Android 6.0)
- **Target SDK**: 34 (Android 14)
- **Package game data inside APK**: ✓
- **Internet permission**: Auto-added

## 🚀 Performance Tips

1. **Stop listeners** when not needed
2. **Cache data** locally
3. **Use queries** to limit data
4. **Enable offline persistence**
5. **Denormalize data structure**

## 📚 Documentation Quick Links

- Full Guide: `README.md`
- Setup: `SETUP_GUIDE.md`
- Examples: `BLUEPRINT_EXAMPLES.md`
- Architecture: `ARCHITECTURE.md`

## 💻 Example Data Structures

### User Profile
```json
users/{uid}/profile/
  {
    "displayName": "Player123",
    "level": 5,
    "score": 1000,
    "createdAt": 1234567890
  }
```

### Leaderboard
```json
leaderboard/global/
  {
    "user1": {"name": "Player1", "score": 1000},
    "user2": {"name": "Player2", "score": 950}
  }
```

### Game State
```json
games/{gameId}/
  {
    "status": "active",
    "players": {
      "player1": {"health": 100, "position": "x,y,z"},
      "player2": {"health": 85, "position": "x,y,z"}
    }
  }
```

## 🎓 Learning Path

**Day 1**: Setup + Basic Auth
**Day 2**: Save/Load Data
**Day 3**: Real-time Listeners
**Week 1**: Complete Single-Player Cloud Save
**Week 2**: Add Multiplayer Features

---

**Keep this card handy while developing! 📌**

*Firebase Plugin v1.0.0 for Unreal Engine*
