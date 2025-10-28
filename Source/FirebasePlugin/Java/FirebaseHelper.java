package com.epicgames.unreal;

import android.app.Activity;
import android.util.Log;
import androidx.annotation.NonNull;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.FirebaseApp;
import com.google.firebase.auth.AuthResult;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.auth.UserProfileChangeRequest;
import com.google.firebase.database.ChildEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.MutableData;
import com.google.firebase.database.Query;
import com.google.firebase.database.Transaction;
import com.google.firebase.database.ValueEventListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class FirebaseHelper {
    private static final String TAG = "FirebaseHelper";
    private static Activity currentActivity;
    private static FirebaseAuth mAuth;
    private static FirebaseDatabase mDatabase;
    private static boolean isInitialized = false;
    private static Map<String, ValueEventListener> valueListeners = new HashMap<>();

    /**
     * Initialize Firebase SDK
     */
    public static void initialize(final Activity activity, final String databaseUrl) {
        if (isInitialized) {
            Log.w(TAG, "Firebase already initialized");
            return;
        }

        currentActivity = activity;

        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    // Initialize Firebase App
                    FirebaseApp.initializeApp(activity);
                    
                    // Initialize Firebase Auth
                    mAuth = FirebaseAuth.getInstance();
                    
                    // Initialize Firebase Database
                    if (databaseUrl != null && !databaseUrl.isEmpty()) {
                        mDatabase = FirebaseDatabase.getInstance(databaseUrl);
                    } else {
                        mDatabase = FirebaseDatabase.getInstance();
                    }
                    
                    // Enable offline persistence by default
                    try {
                        mDatabase.setPersistenceEnabled(true);
                    } catch (Exception e) {
                        Log.w(TAG, "Persistence already enabled or failed: " + e.getMessage());
                    }
                    
                    isInitialized = true;
                    Log.i(TAG, "Firebase initialized successfully");
                } catch (Exception e) {
                    Log.e(TAG, "Firebase initialization failed: " + e.getMessage());
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Check if Firebase is initialized
     */
    public static boolean isInitialized() {
        return isInitialized;
    }

    // ====== AUTHENTICATION METHODS ======

    /**
     * Sign up with email and password
     */
    public static void signUpWithEmail(final String email, final String password, final String operationId) {
        if (!checkAuth()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAuth.createUserWithEmailAndPassword(email, password)
                        .addOnCompleteListener(currentActivity, new OnCompleteListener<AuthResult>() {
                            @Override
                            public void onComplete(@NonNull Task<AuthResult> task) {
                                if (task.isSuccessful()) {
                                    FirebaseUser user = mAuth.getCurrentUser();
                                    if (user != null) {
                                        String userId = user.getUid();
                                        String userEmail = user.getEmail() != null ? user.getEmail() : "";
                                        String displayName = user.getDisplayName() != null ? user.getDisplayName() : "";
                                        
                                        user.getIdToken(false).addOnCompleteListener(new OnCompleteListener<com.google.firebase.auth.GetTokenResult>() {
                                            @Override
                                            public void onComplete(@NonNull Task<com.google.firebase.auth.GetTokenResult> tokenTask) {
                                                String token = "";
                                                if (tokenTask.isSuccessful() && tokenTask.getResult() != null) {
                                                    token = tokenTask.getResult().getToken();
                                                }
                                                nativeOnAuthResult(true, userId, userEmail, displayName, "", token);
                                            }
                                        });
                                        
                                        Log.i(TAG, "Sign up successful: " + userId);
                                    }
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    Log.e(TAG, "Sign up failed: " + error);
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    /**
     * Sign in with email and password
     */
    public static void signInWithEmail(final String email, final String password, final String operationId) {
        if (!checkAuth()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAuth.signInWithEmailAndPassword(email, password)
                        .addOnCompleteListener(currentActivity, new OnCompleteListener<AuthResult>() {
                            @Override
                            public void onComplete(@NonNull Task<AuthResult> task) {
                                if (task.isSuccessful()) {
                                    FirebaseUser user = mAuth.getCurrentUser();
                                    if (user != null) {
                                        String userId = user.getUid();
                                        String userEmail = user.getEmail() != null ? user.getEmail() : "";
                                        String displayName = user.getDisplayName() != null ? user.getDisplayName() : "";
                                        
                                        user.getIdToken(false).addOnCompleteListener(new OnCompleteListener<com.google.firebase.auth.GetTokenResult>() {
                                            @Override
                                            public void onComplete(@NonNull Task<com.google.firebase.auth.GetTokenResult> tokenTask) {
                                                String token = "";
                                                if (tokenTask.isSuccessful() && tokenTask.getResult() != null) {
                                                    token = tokenTask.getResult().getToken();
                                                }
                                                nativeOnAuthResult(true, userId, userEmail, displayName, "", token);
                                            }
                                        });
                                        
                                        Log.i(TAG, "Sign in successful: " + userId);
                                    }
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    Log.e(TAG, "Sign in failed: " + error);
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    /**
     * Sign in anonymously
     */
    public static void signInAnonymously(final String operationId) {
        if (!checkAuth()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAuth.signInAnonymously()
                        .addOnCompleteListener(currentActivity, new OnCompleteListener<AuthResult>() {
                            @Override
                            public void onComplete(@NonNull Task<AuthResult> task) {
                                if (task.isSuccessful()) {
                                    FirebaseUser user = mAuth.getCurrentUser();
                                    if (user != null) {
                                        String userId = user.getUid();
                                        
                                        user.getIdToken(false).addOnCompleteListener(new OnCompleteListener<com.google.firebase.auth.GetTokenResult>() {
                                            @Override
                                            public void onComplete(@NonNull Task<com.google.firebase.auth.GetTokenResult> tokenTask) {
                                                String token = "";
                                                if (tokenTask.isSuccessful() && tokenTask.getResult() != null) {
                                                    token = tokenTask.getResult().getToken();
                                                }
                                                nativeOnAuthResult(true, userId, "", "Anonymous", "", token);
                                            }
                                        });
                                        
                                        Log.i(TAG, "Anonymous sign in successful: " + userId);
                                    }
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    Log.e(TAG, "Anonymous sign in failed: " + error);
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    /**
     * Sign in with Google (requires Google Sign-In setup)
     */
    public static void signInWithGoogle(final String operationId) {
        // TODO: Implement Google Sign-In
        Log.w(TAG, "Google Sign-In not yet implemented");
        nativeOnAuthResult(false, "", "", "", "Google Sign-In not yet implemented", "");
    }

    /**
     * Sign out current user
     */
    public static void signOut() {
        if (mAuth != null) {
            mAuth.signOut();
            Log.i(TAG, "User signed out");
        }
    }

    /**
     * Check if user is signed in
     */
    public static boolean isUserSignedIn() {
        return mAuth != null && mAuth.getCurrentUser() != null;
    }

    /**
     * Get current user ID
     */
    public static String getCurrentUserId() {
        if (mAuth != null && mAuth.getCurrentUser() != null) {
            return mAuth.getCurrentUser().getUid();
        }
        return "";
    }

    /**
     * Get current user email
     */
    public static String getCurrentUserEmail() {
        if (mAuth != null && mAuth.getCurrentUser() != null) {
            String email = mAuth.getCurrentUser().getEmail();
            return email != null ? email : "";
        }
        return "";
    }

    /**
     * Get current user display name
     */
    public static String getCurrentUserDisplayName() {
        if (mAuth != null && mAuth.getCurrentUser() != null) {
            String displayName = mAuth.getCurrentUser().getDisplayName();
            return displayName != null ? displayName : "";
        }
        return "";
    }

    /**
     * Get authentication token
     */
    public static String getAuthToken() {
        // This is a synchronous method, but token retrieval is async
        // In production, you'd want to cache the token
        return "";
    }

    /**
     * Send email verification
     */
    public static void sendEmailVerification(final String operationId) {
        if (!checkAuth() || mAuth.getCurrentUser() == null) {
            nativeOnAuthResult(false, "", "", "", "No user signed in", "");
            return;
        }

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAuth.getCurrentUser().sendEmailVerification()
                        .addOnCompleteListener(new OnCompleteListener<Void>() {
                            @Override
                            public void onComplete(@NonNull Task<Void> task) {
                                if (task.isSuccessful()) {
                                    Log.i(TAG, "Email verification sent");
                                    nativeOnAuthResult(true, getCurrentUserId(), getCurrentUserEmail(), "", "Verification email sent", "");
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    /**
     * Check if email is verified
     */
    public static boolean isEmailVerified() {
        if (mAuth != null && mAuth.getCurrentUser() != null) {
            return mAuth.getCurrentUser().isEmailVerified();
        }
        return false;
    }

    /**
     * Send password reset email
     */
    public static void sendPasswordResetEmail(final String email, final String operationId) {
        if (!checkAuth()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAuth.sendPasswordResetEmail(email)
                        .addOnCompleteListener(new OnCompleteListener<Void>() {
                            @Override
                            public void onComplete(@NonNull Task<Void> task) {
                                if (task.isSuccessful()) {
                                    Log.i(TAG, "Password reset email sent");
                                    nativeOnAuthResult(true, "", email, "", "Password reset email sent", "");
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    /**
     * Update user password
     */
    public static void updatePassword(final String newPassword, final String operationId) {
        if (!checkAuth() || mAuth.getCurrentUser() == null) {
            nativeOnAuthResult(false, "", "", "", "No user signed in", "");
            return;
        }

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAuth.getCurrentUser().updatePassword(newPassword)
                        .addOnCompleteListener(new OnCompleteListener<Void>() {
                            @Override
                            public void onComplete(@NonNull Task<Void> task) {
                                if (task.isSuccessful()) {
                                    Log.i(TAG, "Password updated");
                                    nativeOnAuthResult(true, getCurrentUserId(), getCurrentUserEmail(), "", "Password updated", "");
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    /**
     * Update user display name
     */
    public static void updateDisplayName(final String displayName, final String operationId) {
        if (!checkAuth() || mAuth.getCurrentUser() == null) {
            nativeOnAuthResult(false, "", "", "", "No user signed in", "");
            return;
        }

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                UserProfileChangeRequest profileUpdates = new UserProfileChangeRequest.Builder()
                        .setDisplayName(displayName)
                        .build();

                mAuth.getCurrentUser().updateProfile(profileUpdates)
                        .addOnCompleteListener(new OnCompleteListener<Void>() {
                            @Override
                            public void onComplete(@NonNull Task<Void> task) {
                                if (task.isSuccessful()) {
                                    Log.i(TAG, "Display name updated");
                                    nativeOnAuthResult(true, getCurrentUserId(), getCurrentUserEmail(), displayName, "", "");
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    /**
     * Delete user account
     */
    public static void deleteUserAccount(final String operationId) {
        if (!checkAuth() || mAuth.getCurrentUser() == null) {
            nativeOnAuthResult(false, "", "", "", "No user signed in", "");
            return;
        }

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAuth.getCurrentUser().delete()
                        .addOnCompleteListener(new OnCompleteListener<Void>() {
                            @Override
                            public void onComplete(@NonNull Task<Void> task) {
                                if (task.isSuccessful()) {
                                    Log.i(TAG, "User account deleted");
                                    nativeOnAuthResult(true, "", "", "", "Account deleted", "");
                                } else {
                                    String error = task.getException() != null ? task.getException().getMessage() : "Unknown error";
                                    nativeOnAuthResult(false, "", "", "", error, "");
                                }
                            }
                        });
            }
        });
    }

    // ====== DATABASE METHODS ======

    /**
     * Set value in database
     */
    public static void setDatabaseValue(final String path, final String jsonData, final String operationId) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    DatabaseReference ref = mDatabase.getReference(path);
                    Object value = jsonToObject(jsonData);
                    
                    ref.setValue(value)
                            .addOnSuccessListener(new OnSuccessListener<Void>() {
                                @Override
                                public void onSuccess(Void aVoid) {
                                    Log.i(TAG, "Database value set: " + path);
                                    nativeOnDatabaseResult(true, path, jsonData, "");
                                }
                            })
                            .addOnFailureListener(new OnFailureListener() {
                                @Override
                                public void onFailure(@NonNull Exception e) {
                                    Log.e(TAG, "Failed to set database value: " + e.getMessage());
                                    nativeOnDatabaseResult(false, path, "", e.getMessage());
                                }
                            });
                } catch (Exception e) {
                    Log.e(TAG, "Error setting database value: " + e.getMessage());
                    nativeOnDatabaseResult(false, path, "", e.getMessage());
                }
            }
        });
    }

    /**
     * Update value in database
     */
    public static void updateDatabaseValue(final String path, final String jsonData, final String operationId) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    DatabaseReference ref = mDatabase.getReference(path);
                    Map<String, Object> updates = jsonToMap(jsonData);
                    
                    ref.updateChildren(updates)
                            .addOnSuccessListener(new OnSuccessListener<Void>() {
                                @Override
                                public void onSuccess(Void aVoid) {
                                    Log.i(TAG, "Database value updated: " + path);
                                    nativeOnDatabaseResult(true, path, jsonData, "");
                                }
                            })
                            .addOnFailureListener(new OnFailureListener() {
                                @Override
                                public void onFailure(@NonNull Exception e) {
                                    Log.e(TAG, "Failed to update database value: " + e.getMessage());
                                    nativeOnDatabaseResult(false, path, "", e.getMessage());
                                }
                            });
                } catch (Exception e) {
                    Log.e(TAG, "Error updating database value: " + e.getMessage());
                    nativeOnDatabaseResult(false, path, "", e.getMessage());
                }
            }
        });
    }

    /**
     * Push value to database (generates unique key)
     */
    public static void pushDatabaseValue(final String path, final String jsonData, final String operationId) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    DatabaseReference ref = mDatabase.getReference(path);
                    DatabaseReference pushRef = ref.push();
                    Object value = jsonToObject(jsonData);
                    
                    pushRef.setValue(value)
                            .addOnSuccessListener(new OnSuccessListener<Void>() {
                                @Override
                                public void onSuccess(Void aVoid) {
                                    String newPath = pushRef.getKey();
                                    Log.i(TAG, "Database value pushed: " + newPath);
                                    nativeOnDatabaseResult(true, path + "/" + newPath, jsonData, "");
                                }
                            })
                            .addOnFailureListener(new OnFailureListener() {
                                @Override
                                public void onFailure(@NonNull Exception e) {
                                    Log.e(TAG, "Failed to push database value: " + e.getMessage());
                                    nativeOnDatabaseResult(false, path, "", e.getMessage());
                                }
                            });
                } catch (Exception e) {
                    Log.e(TAG, "Error pushing database value: " + e.getMessage());
                    nativeOnDatabaseResult(false, path, "", e.getMessage());
                }
            }
        });
    }

    /**
     * Delete value from database
     */
    public static void deleteDatabaseValue(final String path, final String operationId) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                DatabaseReference ref = mDatabase.getReference(path);
                
                ref.removeValue()
                        .addOnSuccessListener(new OnSuccessListener<Void>() {
                            @Override
                            public void onSuccess(Void aVoid) {
                                Log.i(TAG, "Database value deleted: " + path);
                                nativeOnDatabaseResult(true, path, "", "");
                            }
                        })
                        .addOnFailureListener(new OnFailureListener() {
                            @Override
                            public void onFailure(@NonNull Exception e) {
                                Log.e(TAG, "Failed to delete database value: " + e.getMessage());
                                nativeOnDatabaseResult(false, path, "", e.getMessage());
                            }
                        });
            }
        });
    }

    /**
     * Get value from database
     */
    public static void getDatabaseValue(final String path, final String operationId) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                DatabaseReference ref = mDatabase.getReference(path);
                
                ref.get().addOnSuccessListener(new OnSuccessListener<DataSnapshot>() {
                    @Override
                    public void onSuccess(DataSnapshot snapshot) {
                        String jsonData = dataSnapshotToJson(snapshot);
                        Log.i(TAG, "Database value retrieved: " + path);
                        nativeOnDatabaseResult(true, path, jsonData, "");
                    }
                }).addOnFailureListener(new OnFailureListener() {
                    @Override
                    public void onFailure(@NonNull Exception e) {
                        Log.e(TAG, "Failed to get database value: " + e.getMessage());
                        nativeOnDatabaseResult(false, path, "", e.getMessage());
                    }
                });
            }
        });
    }

    /**
     * Listen for value changes
     */
    public static void listenForValueChanges(final String path) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                DatabaseReference ref = mDatabase.getReference(path);
                
                ValueEventListener listener = new ValueEventListener() {
                    @Override
                    public void onDataChange(@NonNull DataSnapshot snapshot) {
                        String jsonData = dataSnapshotToJson(snapshot);
                        Log.i(TAG, "Database value changed: " + path);
                        nativeOnDatabaseValueChanged(path, jsonData);
                    }

                    @Override
                    public void onCancelled(@NonNull DatabaseError error) {
                        Log.e(TAG, "Database listener cancelled: " + error.getMessage());
                    }
                };
                
                ref.addValueEventListener(listener);
                valueListeners.put(path, listener);
            }
        });
    }

    /**
     * Stop listening for value changes
     */
    public static void stopListening(final String path) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ValueEventListener listener = valueListeners.get(path);
                if (listener != null) {
                    DatabaseReference ref = mDatabase.getReference(path);
                    ref.removeEventListener(listener);
                    valueListeners.remove(path);
                    Log.i(TAG, "Stopped listening: " + path);
                }
            }
        });
    }

    /**
     * Query database values
     */
    public static void queryDatabaseValues(final String path, final String orderByKey, final int limitToFirst, 
                                          final String startAt, final String endAt, final String operationId) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                DatabaseReference ref = mDatabase.getReference(path);
                Query query = ref;
                
                if (orderByKey != null && !orderByKey.isEmpty()) {
                    query = query.orderByChild(orderByKey);
                }
                
                if (limitToFirst > 0) {
                    query = query.limitToFirst(limitToFirst);
                }
                
                // Additional query parameters would go here
                
                query.get().addOnSuccessListener(new OnSuccessListener<DataSnapshot>() {
                    @Override
                    public void onSuccess(DataSnapshot snapshot) {
                        String jsonData = dataSnapshotToJson(snapshot);
                        Log.i(TAG, "Database query successful: " + path);
                        nativeOnDatabaseResult(true, path, jsonData, "");
                    }
                }).addOnFailureListener(new OnFailureListener() {
                    @Override
                    public void onFailure(@NonNull Exception e) {
                        Log.e(TAG, "Database query failed: " + e.getMessage());
                        nativeOnDatabaseResult(false, path, "", e.getMessage());
                    }
                });
            }
        });
    }

    /**
     * Run transaction
     */
    public static void runTransaction(final String path, final String jsonData, final String operationId) {
        if (!checkDatabase()) return;

        currentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                DatabaseReference ref = mDatabase.getReference(path);
                
                ref.runTransaction(new Transaction.Handler() {
                    @NonNull
                    @Override
                    public Transaction.Result doTransaction(@NonNull MutableData mutableData) {
                        // Simple transaction: just set the value
                        try {
                            Object value = jsonToObject(jsonData);
                            mutableData.setValue(value);
                            return Transaction.success(mutableData);
                        } catch (Exception e) {
                            return Transaction.abort();
                        }
                    }

                    @Override
                    public void onComplete(DatabaseError error, boolean committed, DataSnapshot snapshot) {
                        if (committed && error == null) {
                            Log.i(TAG, "Transaction successful: " + path);
                            nativeOnDatabaseResult(true, path, jsonData, "");
                        } else {
                            String errorMsg = error != null ? error.getMessage() : "Transaction failed";
                            Log.e(TAG, "Transaction failed: " + errorMsg);
                            nativeOnDatabaseResult(false, path, "", errorMsg);
                        }
                    }
                });
            }
        });
    }

    /**
     * Enable offline persistence
     */
    public static void enableOfflinePersistence() {
        if (mDatabase != null) {
            try {
                mDatabase.setPersistenceEnabled(true);
                Log.i(TAG, "Offline persistence enabled");
            } catch (Exception e) {
                Log.w(TAG, "Failed to enable offline persistence: " + e.getMessage());
            }
        }
    }

    /**
     * Disable offline persistence
     */
    public static void disableOfflinePersistence() {
        if (mDatabase != null) {
            try {
                mDatabase.setPersistenceEnabled(false);
                Log.i(TAG, "Offline persistence disabled");
            } catch (Exception e) {
                Log.w(TAG, "Failed to disable offline persistence: " + e.getMessage());
            }
        }
    }

    /**
     * Keep data synced
     */
    public static void keepSynced(final String path, final boolean keepSynced) {
        if (!checkDatabase()) return;

        DatabaseReference ref = mDatabase.getReference(path);
        ref.keepSynced(keepSynced);
        Log.i(TAG, "Keep synced " + (keepSynced ? "enabled" : "disabled") + " for: " + path);
    }

    /**
     * Go online
     */
    public static void goOnline() {
        if (mDatabase != null) {
            mDatabase.goOnline();
            Log.i(TAG, "Database going online");
        }
    }

    /**
     * Go offline
     */
    public static void goOffline() {
        if (mDatabase != null) {
            mDatabase.goOffline();
            Log.i(TAG, "Database going offline");
        }
    }

    /**
     * Check if connected
     */
    public static boolean isConnected() {
        // This would require implementing a connection state listener
        // For now, return true if database is initialized
        return mDatabase != null;
    }

    /**
     * Generate push ID
     */
    public static String generatePushId() {
        if (mDatabase != null) {
            return mDatabase.getReference().push().getKey();
        }
        return "";
    }

    // ====== HELPER METHODS ======

    private static boolean checkAuth() {
        if (!isInitialized || mAuth == null) {
            Log.e(TAG, "Firebase Auth not initialized");
            nativeOnAuthResult(false, "", "", "", "Firebase not initialized", "");
            return false;
        }
        return true;
    }

    private static boolean checkDatabase() {
        if (!isInitialized || mDatabase == null) {
            Log.e(TAG, "Firebase Database not initialized");
            return false;
        }
        return true;
    }

    private static Object jsonToObject(String jsonData) throws JSONException {
        if (jsonData == null || jsonData.isEmpty() || jsonData.equals("null")) {
            return null;
        }
        
        JSONObject json = new JSONObject(jsonData);
        return jsonToMap(json);
    }

    private static Map<String, Object> jsonToMap(String jsonData) throws JSONException {
        JSONObject json = new JSONObject(jsonData);
        return jsonToMap(json);
    }

    private static Map<String, Object> jsonToMap(JSONObject json) throws JSONException {
        Map<String, Object> map = new HashMap<>();
        java.util.Iterator<String> keys = json.keys();
        
        while (keys.hasNext()) {
            String key = keys.next();
            Object value = json.get(key);
            
            if (value instanceof JSONObject) {
                map.put(key, jsonToMap((JSONObject) value));
            } else {
                map.put(key, value);
            }
        }
        
        return map;
    }

    private static String dataSnapshotToJson(DataSnapshot snapshot) {
        if (snapshot == null || !snapshot.exists()) {
            return "{}";
        }
        
        try {
            Object value = snapshot.getValue();
            if (value == null) {
                return "{}";
            }
            
            // Convert to JSON string
            if (value instanceof Map) {
                JSONObject json = new JSONObject((Map) value);
                return json.toString();
            } else if (value instanceof String) {
                return "\"" + value + "\"";
            } else {
                return value.toString();
            }
        } catch (Exception e) {
            Log.e(TAG, "Failed to convert snapshot to JSON: " + e.getMessage());
            return "{}";
        }
    }

    // Native callbacks to Unreal Engine
    public static native void nativeOnAuthResult(boolean success, String userId, String email, 
                                                 String displayName, String errorMessage, String authToken);
    public static native void nativeOnDatabaseResult(boolean success, String path, 
                                                     String data, String errorMessage);
    public static native void nativeOnDatabaseValueChanged(String path, String data);
}
