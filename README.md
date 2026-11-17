# SafeKeep Password Manager

A secure, cross-platform password manager built with GTK4 and OpenSSL encryption.

## Features

- **Secure Encryption**: AES-256-GCM via OpenSSL, with the key derived from your master password
- **Master Password Protection**: The master password is never stored; it is the only way to decrypt your data
- **User-Friendly Interface**: Clean GTK4-based GUI
- **Password Management**: Add, view, copy, and delete password entries
- **Hidden Storage**: Passwords stored in encrypted format in hidden directory
- **Cross-Platform**: Works on Linux, macOS, and Windows

## Installation

### Prerequisites

- GTK4 development libraries
- OpenSSL development libraries
- GCC compiler
- pkg-config

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install libgtk-4-dev libssl-dev gcc pkg-config
```

#### macOS (with Homebrew)
```bash
brew install gtk4 openssl pkg-config
```

#### Fedora/RHEL
```bash
sudo dnf install gtk4-devel openssl-devel gcc pkg-config
```

### Building from Source

1. Clone the repository:
```bash
git clone https://github.com/Shadowinger/SafeKeep.git
cd SafeKeep
```

2. Set up the build environment:
```bash
make setup
```

3. Build the application:
```bash
make
```

4. Run the application:
```bash
make run
```

## Usage

### First Time Setup

1. Launch SafeKeep
2. Enter a master password when prompted
3. This master password will be used to encrypt and protect all your stored passwords. It cannot be recovered; if you forget it, your stored passwords are lost

### Adding Password Entries

1. In the main window, fill in the three fields:
   - **Service**: The name of the service (e.g., "Gmail", "Facebook")
   - **Email**: Your email/username for that service
   - **Password**: Your password for that service

2. Click "New Entry" to save the entry

### Managing Passwords

- **Show Password**: Click the "Show" button to temporarily reveal the password
- **Copy Password**: Click the "Copy" button to copy the password to clipboard without showing it. The clipboard is cleared after 30 seconds
- **Delete Entry**: Click the "Delete" button to permanently remove an entry

## Security Features

- **Key Derivation**: The encryption key is derived from the master password with PBKDF2-HMAC-SHA256 (600,000 iterations) and a random 16-byte salt. The key exists only in memory while the app is running
- **Authenticated Encryption**: Every value is encrypted with AES-256-GCM using a fresh random 12-byte nonce, so identical passwords produce different ciphertexts and tampering with the file is detected
- **Master Password Check**: The master password is never stored. It is verified by decrypting a check record created on first launch
- **Hidden Storage**: Data stored in `~/.safekeep/` (directory `0700`, file `0600`)
- **Memory Protection**: The key and decrypted passwords are wiped with `OPENSSL_cleanse` after use, and password fields use GTK's locked (non-swappable) password buffer. Text shown in labels or placed on the clipboard is managed by GTK and cannot be wiped by the app
- **No Plain Text Storage**: All sensitive data encrypted at rest

## File Structure

```
SafeKeep/
├── src/
│   ├── main.c          # Application entry point
│   ├── gui.c           # GTK4 user interface
│   ├── gui.h           # GUI function declarations
│   ├── storage.c       # File I/O and data management
│   ├── storage.h       # Storage function declarations
│   ├── crypto.c        # Encryption/decryption functions
│   └── crypto.h        # Crypto function declarations
├── ui/                 # User interface resources
│   └── styles.css      # GTK4 styling
├── Makefile           # Build configuration
└── README.md          # This file
```

## Build Options

- **Default Build**: `make` - Includes debug symbols
- **Release Build**: `make release` - Optimized for production use
- **Clean Build**: `make clean` - Remove build artifacts
- **Run Application**: `make run` - Build and run in one command

## Advanced Usage

### Memory Leak Checking
Requires [valgrind](https://valgrind.org/):
```bash
make check-memory
```

### Static Analysis
Requires [cppcheck](https://cppcheck.sourceforge.io/):
```bash
make analyze
```

## Configuration

SafeKeep stores everything in a single file:
- **Linux/macOS**: `~/.safekeep/passwords.dat`
- **Windows**: `%USERPROFILE%\.safekeep\passwords.dat`

The file holds the salt, the master password check record, and the encrypted entries.

### Upgrading from v1.0.0
Version 1.0.0 encrypted data with a fixed key and stored the master password in plain text. That data is not migrated:
- Delete `~/.safekeep/main-pass.dat`, it contains your old master password in plain text
- Entries in the old `data/passwords.dat` are not read anymore; add them again after setting a new master password

## Security Considerations

1. **Master Password**: Choose a strong, unique master password
2. **Backup**: Regularly backup your `~/.safekeep/` directory
3. **Permissions**: Ensure only you have access to the SafeKeep data directory
4. **Memory**: The application clears sensitive data from memory when possible

## Troubleshooting

### Common Issues

**Build Errors**
- Ensure all dependencies are installed
- Check that GTK4 and OpenSSL development packages are available

**Permission Errors**
- Verify you have write permissions in your home directory
- Check that the data directory permissions are set correctly

**Encryption Errors**
- Ensure OpenSSL is properly installed and linked
- "Wrong master password or damaged vault" means the password is wrong or `~/.safekeep/passwords.dat` was modified

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes and test them
4. Commit your changes: `git commit -am 'Add some feature'`
5. Push to the branch: `git push origin feature-name`
6. Submit a pull request

## Development

### Code Style
- Use consistent indentation (4 spaces)
- Follow C naming conventions
- Document functions with comments
- Handle all error cases

### Testing
Before submitting changes:
```bash
make clean
make
make check-memory  # If valgrind is available
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- GTK4 development team for the excellent UI toolkit
- OpenSSL project for robust cryptographic functions
- Contributors and testers who helped improve SafeKeep

## Changelog

### Unreleased
- Key derived from the master password (PBKDF2-HMAC-SHA256) instead of a hard-coded key
- AES-256-GCM with a random nonce per value instead of AES-256-CBC with a fixed IV
- Master password is no longer stored
- Data moved to `~/.safekeep/passwords.dat`
- Clipboard is cleared 30 seconds after copying a password

### v1.0.0 (2025-06-04)
- Initial release
- AES-256-CBC encryption
- GTK4 user interface
- Master password protection
- Copy to clipboard functionality
- Delete entries feature

## Support

For support, bug reports, or feature requests:
- Open an issue on GitHub

## Roadmap

- [ ] Password generation tool
- [ ] Import/export functionality
- [ ] Multiple database support
- [ ] Browser integration
- [ ] Mobile application
- [ ] Cloud synchronization (encrypted)
- [ ] Two-factor authentication support

---

**⚠️ Security Notice**: This software handles sensitive information. While we strive for security, use at your own risk and always maintain backups of your data.