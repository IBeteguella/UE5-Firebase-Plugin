# Changelog

All notable changes to the Firebase Plugin for Unreal Engine will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-XX

### 🎉 Initial Release

First public release of the Firebase Realtime Database & Authentication plugin for Unreal Engine.

### ✨ Added - Authentication

- Email/Password authentication
  - Sign up with email and password
  - Sign in with email and password
  - Email verification
  - Password reset via email
  - Update user password
- Anonymous authentication
  - Sign in without credentials
  - Automatic user ID assignment
- User management
  - Get current user information (ID, email, display name)
  - Check authentication status
  - Update user display name
  - Delete user account
  - Sign out functionality
  - Get authentication token
- Google Sign-In framework (implementation ready)

### ✨ Added - Realtime Database

- Write operations
  - Set value (create/overwrite data)
  - Update value (merge with existing data)
  - Push value (auto-generate unique keys)
  - Delete value (remove data)
- Read operations
  - Get value (one-time read)
  - Listen for value changes (real-time updates)
  - Stop listening (cleanup listeners)
  - Query values with filters and ordering
- Advanced features
  - Run transaction (atomic updates)
  - Offline persistence support
  - Keep synced for priority data
  - Manual online/offline control
  - Connection status checking
  - Generate push IDs
  - Server timestamp support

### ✨ Added - Blueprint Integration

- 50+ Blueprint-accessible nodes
- Full async operation support with delegates
- Result structures with success flags
- Comprehensive error messages
- Pure functions for getters
- JSON helper utilities
  - Make JSON from String, Int, Float, Bool
  - Combine multiple JSON objects
  - Extract values from JSON

### ✨ Added - Platform Support

- Android platform (primary)
  - Full Firebase SDK integration
  - JNI bridge implementation
  - Gradle build system integration
  - ProGuard rules
  - UPL (Unreal Plugin Language) configuration
- iOS framework (ready for implementation)

### ✨ Added - Documentation

- README.md (comprehensive guide)
- SETUP_GUIDE.md (step-by-step instructions)
- BLUEPRINT_EXAMPLES.md (10+ working examples)
- ARCHITECTURE.md (technical details)
- QUICK_REFERENCE.md (cheat sheet)
- SUMMARY.md (complete overview)
- google-services.json.template

### ✨ Added - Developer Experience

- Plugin settings in Project Settings UI
- Automatic Firebase SDK initialization
- Thread-safe callback handling
- Memory leak prevention
- Comprehensive error handling
- Verbose logging option
- Settings validation

### 🔒 Security

- Database rules documentation
- Security best practices guide
- Auth token management
- Proper data validation examples

### 📦 Dependencies

- Firebase Android SDK BoM 33.7.0
- Firebase Authentication
- Firebase Realtime Database
- Google Play Services 18.5.0+
- AndroidX Core 1.13.1
- AndroidX AppCompat 1.7.0

### 🎯 Requirements

- Unreal Engine 5.0 or higher
- Android SDK 23 (minimum)
- Android SDK 34 (target)
- Firebase project with Realtime Database
- google-services.json file

### 📝 Known Limitations

- Android only (iOS planned for future release)
- Google Sign-In UI flow not implemented (framework ready)
- Facebook authentication not included
- Phone authentication not included
- Cloud Storage not included
- Cloud Functions not directly supported
- Firebase Cloud Messaging (FCM) not included

---

## [Unreleased]

### 🔮 Planned Features

#### Version 1.1.0 (Next Minor Release)
- iOS platform support
- Complete Google Sign-In implementation
- Enhanced query capabilities
- Batch write operations
- Advanced transaction support

#### Version 1.2.0
- Facebook authentication
- Phone number authentication
- Cloud Storage integration
- Enhanced offline capabilities

#### Version 2.0.0 (Major Release)
- Cloud Firestore support
- Firebase Cloud Messaging (FCM)
- Remote Config integration
- Analytics enhancements
- Crashlytics integration
- Performance monitoring

---

## Version History

### Version Naming Convention

- **Major.Minor.Patch** (Semantic Versioning)
- **Major**: Breaking changes, major new features
- **Minor**: New features, backward compatible
- **Patch**: Bug fixes, minor improvements

### Release Schedule

- **Minor releases**: Every 2-3 months
- **Patch releases**: As needed for critical fixes
- **Major releases**: Annually or for breaking changes

---

## Migration Guides

### Future Upgrades

When upgrading to a new version:

1. **Read the changelog** for breaking changes
2. **Backup your project** before updating
3. **Test thoroughly** in development first
4. **Update documentation links** if changed
5. **Review deprecated features** and migrate

### Breaking Changes (None yet in v1.0.0)

No breaking changes in initial release.

---

## Bug Fixes

### None yet in v1.0.0

This is the initial release.

### How to Report Bugs

1. Check existing issues
2. Provide detailed description
3. Include error logs
4. Specify Unreal Engine version
5. Specify Android SDK version
6. Provide reproduction steps

---

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### Areas for Contribution

- Platform support (iOS, Windows)
- Additional authentication providers
- Enhanced error handling
- Performance optimizations
- Documentation improvements
- Example projects
- Bug fixes

---

## Support

- **Documentation**: See README.md and other guides
- **Issues**: Report on GitHub Issues
- **Discussions**: Community discussions welcome
- **Email**: [Your contact email]

---

## License

MIT License - See LICENSE file for details

---

## Acknowledgments

- Firebase team at Google for the excellent SDKs
- Epic Games for Unreal Engine
- Community contributors
- Beta testers and early adopters

---

## Statistics (v1.0.0)

- **Lines of Code**: ~3,500
- **Blueprint Nodes**: 50+
- **Documentation Pages**: 7
- **Example Use Cases**: 10+
- **Test Coverage**: In progress
- **Development Time**: Optimized for production use

---

**Thank you for using the Firebase Plugin for Unreal Engine!**

*For detailed usage instructions, see the documentation files.*
