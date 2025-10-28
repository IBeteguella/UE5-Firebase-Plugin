# Blueprint Examples for Firebase Plugin

This document provides ready-to-use Blueprint examples for common Firebase operations.

## Table of Contents
1. [Authentication Examples](#authentication-examples)
2. [Database Examples](#database-examples)
3. [Complete Use Cases](#complete-use-cases)

---

## Authentication Examples

### Example 1: Sign Up Form

**Scenario:** User enters email and password, clicks Sign Up button

**Blueprint Setup:**
```
Widget Blueprint: WBP_SignUp

Variables:
- EmailText (Text)
- PasswordText (Text)
- ErrorMessage (Text)

Button "SignUp" OnClicked Event:
  → Get Text from Email Input → EmailText
  → Get Text from Password Input → PasswordText
  
  → Sign Up with Email
    - Email: EmailText (converted to String)
    - Password: PasswordText (converted to String)
    - OnComplete: Custom Event "OnSignUpComplete"

Custom Event: OnSignUpComplete (Result: FirebaseAuthResult)
  → Branch (Result.bSuccess)
    → True:
      → Print String: "Welcome! User ID: " + Result.UserId
      → Open Level: "MainMenu"
    → False:
      → Set ErrorMessage: Result.ErrorMessage
      → Update UI Error Text
```

### Example 2: Auto Sign-In on Game Start

**Scenario:** Check if user is already signed in when game starts

```
Level Blueprint / Game Instance

Event BeginPlay:
  → Is User Signed In?
    → Branch
      → True:
        → Get Current User ID → UserID
        → Print String: "Already signed in: " + UserID
        → Call: LoadUserProfile(UserID)
        → Open Level: "MainMenu"
      → False:
        → Print String: "No user signed in"
        → Open Level: "Login"
```

### Example 3: Sign Out Button

**Scenario:** User clicks sign out button in settings menu

```
Widget Blueprint: WBP_Settings

Button "SignOut" OnClicked Event:
  → Sign Out
  → Open Level: "Login"
  → Print String: "User signed out successfully"
```

---

## Database Examples

### Example 4: Save Player Profile

**Scenario:** Create and save a player profile after sign up

```
Function: CreatePlayerProfile
  Inputs:
    - PlayerName (String)
    - StartingLevel (Integer) = 1
  
  → Get Current User ID → UserID
  
  → MakeJsonString("displayName", PlayerName) → JSON_Name
  → MakeJsonInt("level", StartingLevel) → JSON_Level
  → MakeJsonInt("score", 0) → JSON_Score
  → MakeJsonInt("coins", 100) → JSON_Coins
  → Get Server Timestamp → JSON_Timestamp
  
  → Combine JSON
    - Input Array: [JSON_Name, JSON_Level, JSON_Score, JSON_Coins, JSON_Timestamp]
    → ProfileJSON
  
  → Set Value
    - Path: "users/" + UserID + "/profile"
    - JSON Data: ProfileJSON
    - OnComplete: Custom Event "OnProfileSaved"

Custom Event: OnProfileSaved (Result: FirebaseDatabaseResult)
  → Branch (Result.bSuccess)
    → True:
      → Print String: "Profile created successfully!"
    → False:
      → Print String: "Error: " + Result.ErrorMessage
```

### Example 5: Load Player Profile

**Scenario:** Load player data when entering main menu

```
Event: LoadPlayerData

→ Get Current User ID → UserID

→ Get Value
  - Path: "users/" + UserID + "/profile"
  - OnComplete: Custom Event "OnProfileLoaded"

Custom Event: OnProfileLoaded (Result: FirebaseDatabaseResult)
  → Branch (Result.bSuccess)
    → True:
      → Get JSON Value (Result.Data, "displayName") → PlayerName
      → Get JSON Value (Result.Data, "level") → PlayerLevel (String)
      → String to Int (PlayerLevel) → LevelInt
      → Get JSON Value (Result.Data, "score") → PlayerScore (String)
      → String to Int (PlayerScore) → ScoreInt
      
      → Update UI with values
      → Print String: "Loaded profile for " + PlayerName
    → False:
      → Print String: "Failed to load profile"
```

### Example 6: Update Player Score

**Scenario:** Add points to player score after completing a level

```
Function: UpdatePlayerScore
  Inputs:
    - PointsEarned (Integer)
  
  → Get Current User ID → UserID
  
  → MakeJsonInt("score", PointsEarned) → ScoreJSON
  → MakeJsonInt("lastUpdated", Get Current Unix Timestamp) → TimeJSON
  → Combine JSON: [ScoreJSON, TimeJSON] → UpdateJSON
  
  → Update Value
    - Path: "users/" + UserID + "/stats"
    - JSON Data: UpdateJSON
    - OnComplete: Custom Event "OnScoreUpdated"

Custom Event: OnScoreUpdated (Result: FirebaseDatabaseResult)
  → Branch (Result.bSuccess)
    → True:
      → Play Sound: "Score Update Sound"
      → Show Notification: "+" + PointsEarned + " points!"
    → False:
      → Print String: "Failed to update score"
```

### Example 7: Real-Time Multiplayer Chat

**Scenario:** Send and receive chat messages in real-time

```
Actor Component: ChatComponent

Event BeginPlay:
  → Get Current User ID → UserID
  
  → Listen for Value Changes
    - Path: "chat/global"
    - OnValueChanged: Custom Event "OnChatMessageReceived"

Event EndPlay:
  → Stop Listening
    - Path: "chat/global"

Function: SendMessage
  Input: MessageText (String)
  
  → Get Current User ID → UserID
  → Get Current User Display Name → Username
  
  → MakeJsonString("text", MessageText) → JSON_Text
  → MakeJsonString("sender", Username) → JSON_Sender
  → MakeJsonString("senderId", UserID) → JSON_SenderId
  → Get Server Timestamp → JSON_Time
  → Combine JSON: [JSON_Text, JSON_Sender, JSON_SenderId, JSON_Time] → MessageJSON
  
  → Push Value
    - Path: "chat/global"
    - JSON Data: MessageJSON
    - OnComplete: "OnMessageSent"

Custom Event: OnChatMessageReceived (Path: String, Data: String)
  → Get JSON Value (Data, "text") → MessageText
  → Get JSON Value (Data, "sender") → SenderName
  
  → Add Chat Message to UI: SenderName + ": " + MessageText
```

---

## Complete Use Cases

### Use Case 1: Player Inventory System

**Complete implementation of a cloud-synced inventory**

```
Data Structure:
users/
  {userId}/
    inventory/
      {itemId}: { name: "Sword", quantity: 1, rarity: "Epic" }
      {itemId}: { name: "Potion", quantity: 5, rarity: "Common" }

Functions:

1. LoadInventory:
  → Get Current User ID → UserID
  → Get Value: "users/" + UserID + "/inventory"
  → OnComplete:
    → Parse JSON and populate inventory UI
    → Store items in local array

2. AddItem (ItemName, Quantity, Rarity):
  → Get Current User ID → UserID
  → Generate Push ID → ItemID
  
  → MakeJsonString("name", ItemName) → JSON_Name
  → MakeJsonInt("quantity", Quantity) → JSON_Qty
  → MakeJsonString("rarity", Rarity) → JSON_Rarity
  → Combine JSON → ItemJSON
  
  → Set Value:
    - Path: "users/" + UserID + "/inventory/" + ItemID
    - JSON Data: ItemJSON

3. RemoveItem (ItemID):
  → Get Current User ID → UserID
  → Delete Value: "users/" + UserID + "/inventory/" + ItemID

4. UpdateItemQuantity (ItemID, NewQuantity):
  → Get Current User ID → UserID
  → MakeJsonInt("quantity", NewQuantity) → QuantityJSON
  → Update Value:
    - Path: "users/" + UserID + "/inventory/" + ItemID
    - JSON Data: QuantityJSON
```

### Use Case 2: Leaderboard System

**Global leaderboard with real-time updates**

```
Data Structure:
leaderboard/
  global/
    {userId}: { name: "Player1", score: 1000 }
    {userId}: { name: "Player2", score: 950 }

Widget Blueprint: WBP_Leaderboard

Event BeginPlay:
  → Listen for Value Changes
    - Path: "leaderboard/global"
    - OnValueChanged: "OnLeaderboardUpdated"

Event EndPlay:
  → Stop Listening: "leaderboard/global"

Custom Event: OnLeaderboardUpdated (Path, Data)
  → Clear Leaderboard UI
  → Parse JSON Data
  → For Each Entry:
    → Get JSON Value: "name" → PlayerName
    → Get JSON Value: "score" → Score
    → Add Entry to Leaderboard UI (sorted by score)

Function: SubmitScore (NewScore):
  → Get Current User ID → UserID
  → Get Current User Display Name → Username
  
  → MakeJsonString("name", Username) → JSON_Name
  → MakeJsonInt("score", NewScore) → JSON_Score
  → Combine JSON → ScoreJSON
  
  → Set Value:
    - Path: "leaderboard/global/" + UserID
    - JSON Data: ScoreJSON
    - OnComplete:
      → Print String: "Score submitted!"

Function: GetTopPlayers (Limit):
  → Query Values:
    - Path: "leaderboard/global"
    - Order By Key: "score"
    - Limit To First: Limit
    - OnComplete:
      → Parse and display top players
```

### Use Case 3: Matchmaking System

**Simple matchmaking queue for 1v1 games**

```
Data Structure:
matchmaking/
  queue/
    {playerId}: { name: "Player", rating: 1200, timestamp: 123456789 }
  matches/
    {matchId}: { player1: "id1", player2: "id2", status: "ready" }

Actor Component: MatchmakingComponent

Function: JoinQueue:
  → Get Current User ID → UserID
  → Get Current User Display Name → Username
  
  → MakeJsonString("name", Username) → JSON_Name
  → MakeJsonInt("rating", PlayerRating) → JSON_Rating
  → Get Server Timestamp → JSON_Time
  → Combine JSON → QueueJSON
  
  → Set Value:
    - Path: "matchmaking/queue/" + UserID
    - JSON Data: QueueJSON
    - OnComplete: "OnJoinedQueue"
  
  → Listen for Value Changes:
    - Path: "matchmaking/matches"
    - OnValueChanged: "OnMatchFound"

Function: LeaveQueue:
  → Get Current User ID → UserID
  → Delete Value: "matchmaking/queue/" + UserID
  → Stop Listening: "matchmaking/matches"

Custom Event: OnMatchFound (Path, Data):
  → Parse JSON to find if current player is in a match
  → If found:
    → Stop Listening
    → Get match details (opponent ID, match ID)
    → Open Level: "GameLevel"
```

### Use Case 4: Cloud Save System

**Auto-save game progress to cloud**

```
Game Instance Blueprint

Variables:
- SaveData (Map: String → String)
- IsSaving (Boolean)
- LastSaveTime (Float)

Function: SaveGameProgress:
  → Set IsSaving: True
  → Get Current User ID → UserID
  
  // Collect all game data
  → Get Player Level → Level
  → Get Player Position → Position
  → Get Completed Quests → Quests
  → Get Equipment → Equipment
  
  // Create JSON
  → MakeJsonInt("level", Level) → JSON_Level
  → MakeJsonString("position", Position.ToString()) → JSON_Pos
  → MakeJsonString("quests", Quests.ToString()) → JSON_Quests
  → MakeJsonString("equipment", Equipment.ToString()) → JSON_Equip
  → Get Server Timestamp → JSON_Time
  → Combine JSON → SaveJSON
  
  → Set Value:
    - Path: "saves/" + UserID + "/current"
    - JSON Data: SaveJSON
    - OnComplete: "OnSaveComplete"

Custom Event: OnSaveComplete (Result):
  → Set IsSaving: False
  → Set LastSaveTime: Current Time
  → Branch (Result.bSuccess)
    → True: Show "Game Saved" notification
    → False: Show "Save Failed - Will retry" warning

Function: LoadGameProgress:
  → Get Current User ID → UserID
  
  → Get Value:
    - Path: "saves/" + UserID + "/current"
    - OnComplete: "OnLoadComplete"

Custom Event: OnLoadComplete (Result):
  → Branch (Result.bSuccess)
    → True:
      → Get JSON Value: "level" → Level
      → Get JSON Value: "position" → Position
      → Get JSON Value: "quests" → Quests
      → Apply saved data to game
      → Print: "Game loaded successfully"
    → False:
      → Start new game (no save found)

Event Tick:
  // Auto-save every 5 minutes
  → Current Time - LastSaveTime > 300?
    → SaveGameProgress
```

---

## Best Practices from Examples

1. **Always check Result.bSuccess** in callbacks
2. **Use Custom Events** for Firebase callbacks
3. **Store UserID** in variables instead of calling Get Current User ID repeatedly
4. **Stop Listening** in EndPlay events to prevent memory leaks
5. **Handle errors gracefully** - show user-friendly messages
6. **Use Server Timestamp** for consistent timing across clients
7. **Denormalize data** - duplicate data to avoid complex queries
8. **Test offline** - ensure offline persistence works as expected

## Common Patterns

### Pattern: Retry Failed Operations
```
Custom Event: OnOperationFailed (ErrorMessage)
  → Increment RetryCount
  → Branch (RetryCount < MaxRetries)
    → True:
      → Delay 2 seconds
      → Retry Operation
    → False:
      → Show Error to User
      → Reset RetryCount
```

### Pattern: Loading State Management
```
Before Firebase Operation:
  → Set IsLoading: True
  → Show Loading UI

On Complete:
  → Set IsLoading: False
  → Hide Loading UI
  → Process Result
```

### Pattern: Caching Data
```
Variables:
- CachedData (Map)
- CacheExpiry (Map)

Function: GetData (Path):
  → Is Data Cached and Fresh?
    → True: Return Cached Data
    → False:
      → Get Value from Firebase
      → Cache Data with Timestamp
      → Return Data
```

---

**More examples available in the plugin's Content/Examples folder!**
