# SafeKeep Password Manager

A secure, cross-platform password manager built with GTK4 and OpenSSL encryption.

## Features

- **Secure Encryption**: Uses AES-256-CBC encryption with OpenSSL
- **Master Password Protection**: Single master password protects all stored credentials
- **User-Friendly Interface**: Clean GTK4-based GUI
- **Password Management**: Add, view, copy, and delete password entries
- **Hidden Storage**: Passwords stored in encrypted format in hidden directory
- **Cross-Platform**: Works on Linux, macOS, and Windows

## Screenshots

![Login Screen](screenshots/login.png)
![Main Interface](screenshots/main.png)

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
git clone https://github.com/yourusername/SafeKeep.git
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
3. This master password will be used to encrypt and protect all your stored passwords

### Adding Password Entries

1. In the main window, fill in the three fields:
   - **Service**: The name of the service (e.g., "Gmail", "Facebook")
   - **Email**: Your email/username for that service
   - **Password**: Your password for that service

2. Click "New Entry" to save the entry

### Managing Passwords

- **Show Password**: Click the "Show" button to temporarily reveal the password
- **Copy Password**: Click the "Copy" button to copy the password to clipboard without showing it
- **Delete Entry**: Click the "Delete" button to permanently remove an entry

## Security Features

- **AES-256-CBC Encryption**: Industry-standard encryption for all stored data
- **Master Password**: Single point of authentication
- **Hidden Storage**: Data stored in `~/.safekeep/` directory with restricted permissions
- **Memory Protection**: Passwords cleared from memory after use
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
├── data/               # Application data directory
│   └── passwords.dat   # Encrypted password storage
├── ui/                 # User interface resources
│   └── styles.css      # GTK4 styling
├── Makefile           # Build configuration
└── README.md          # This file
```

## Build Options

- **Debug Build**: `make debug` - Includes debug symbols and verbose output
- **Release Build**: `make release` - Optimized for production use
- **Clean Build**: `make clean` - Remove build artifacts
- **Run Application**: `make run` - Build and run in one command

## Advanced Usage

### Memory Leak Checking
```bash
make check-memory
```

### Code Formatting
```bash
make format
```

### Static Analysis
```bash
make analyze
```

## Configuration

SafeKeep stores its configuration and data in:
- **Linux/macOS**: `~/.safekeep/`
- **Windows**: `%APPDATA%\SafeKeep\`

### Master Password File
The master password hash is stored in:
- `~/.safekeep/main-pass.dat`

### Password Database
Encrypted passwords are stored in:
- `data/passwords.dat`

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
- Verify that the master password file isn't corrupted

### Debug Mode
Run with debug information:
```bash
make run-debug
```

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
- Email: your.email@example.com

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