//
// Created by Standa Křikava on 29.04.2025.
//

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdbool.h>

#define CRYPTO_SALT_LEN 16

// Fills salt with random bytes.
bool crypto_new_salt(unsigned char salt[CRYPTO_SALT_LEN]);

// Derives the encryption key from the master password and keeps it in memory until crypto_clear_key().
bool crypto_set_key(const char *master_password, const unsigned char salt[CRYPTO_SALT_LEN]);
void crypto_clear_key(void);

// Both return a new string (base64 ciphertext / plaintext) or NULL on failure.
// Decryption fails on a wrong key or tampered data. Release results with crypto_free().
char *crypto_encrypt(const char *plaintext);
char *crypto_decrypt(const char *encoded);

// Wipes and frees a string returned by crypto_encrypt() or crypto_decrypt(). Accepts NULL.
void crypto_free(char *text);

#endif //CRYPTO_H
