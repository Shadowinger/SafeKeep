//
// Created by Standa Křikava on 29.04.2025.
//

#include "crypto.h"
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <limits.h>
#include <string.h>

#define KEY_LEN 32
#define NONCE_LEN 12
#define TAG_LEN 16
#define PBKDF2_ITERATIONS 600000

static unsigned char key[KEY_LEN];
static bool key_ready = false;

bool crypto_new_salt(unsigned char salt[CRYPTO_SALT_LEN]) {
    return RAND_bytes(salt, CRYPTO_SALT_LEN) == 1;
}

bool crypto_set_key(const char *master_password, const unsigned char salt[CRYPTO_SALT_LEN]) {
    crypto_clear_key();
    size_t password_len = strlen(master_password);
    if (password_len > INT_MAX) {
        return false;
    }
    key_ready = PKCS5_PBKDF2_HMAC(master_password, (int)password_len, salt, CRYPTO_SALT_LEN,
                                  PBKDF2_ITERATIONS, EVP_sha256(), KEY_LEN, key) == 1;
    if (!key_ready) {
        OPENSSL_cleanse(key, sizeof(key));
    }
    return key_ready;
}

void crypto_clear_key(void) {
    OPENSSL_cleanse(key, sizeof(key));
    key_ready = false;
}

// Output format: base64(nonce || ciphertext || tag), a fresh random nonce for every call.
char *crypto_encrypt(const char *plaintext) {
    size_t plaintext_len = strlen(plaintext);
    if (!key_ready || plaintext_len > INT_MAX / 2) {
        return NULL;
    }

    int blob_len = NONCE_LEN + (int)plaintext_len + TAG_LEN;
    unsigned char *blob = OPENSSL_malloc(blob_len);
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    char *encoded = NULL;
    int len;

    bool ok = blob && ctx
        && RAND_bytes(blob, NONCE_LEN) == 1
        && EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, blob) == 1
        && EVP_EncryptUpdate(ctx, blob + NONCE_LEN, &len, (const unsigned char *)plaintext, (int)plaintext_len) == 1
        && EVP_EncryptFinal_ex(ctx, blob + NONCE_LEN + len, &len) == 1
        && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, blob + NONCE_LEN + plaintext_len) == 1;

    if (ok) {
        encoded = OPENSSL_malloc(4 * ((blob_len + 2) / 3) + 1);
        if (encoded) {
            EVP_EncodeBlock((unsigned char *)encoded, blob, blob_len);
        }
    }

    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_free(blob);
    return encoded;
}

char *crypto_decrypt(const char *encoded) {
    size_t encoded_len = strlen(encoded);
    if (!key_ready || encoded_len == 0 || encoded_len % 4 != 0 || encoded_len > INT_MAX) {
        return NULL;
    }

    unsigned char *blob = OPENSSL_malloc(encoded_len / 4 * 3);
    if (!blob) {
        return NULL;
    }
    int blob_len = EVP_DecodeBlock(blob, (const unsigned char *)encoded, (int)encoded_len);
    // EVP_DecodeBlock counts the '=' padding as decoded bytes
    if (blob_len > 0 && encoded[encoded_len - 1] == '=') blob_len--;
    if (blob_len > 0 && encoded[encoded_len - 2] == '=') blob_len--;
    if (blob_len < NONCE_LEN + TAG_LEN) {
        OPENSSL_free(blob);
        return NULL;
    }

    int ciphertext_len = blob_len - NONCE_LEN - TAG_LEN;
    char *plaintext = OPENSSL_malloc(ciphertext_len + 1);
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len = 0;
    int final_len = 0;

    bool ok = plaintext && ctx
        && EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, blob) == 1
        && EVP_DecryptUpdate(ctx, (unsigned char *)plaintext, &len, blob + NONCE_LEN, ciphertext_len) == 1
        && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, blob + NONCE_LEN + ciphertext_len) == 1
        && EVP_DecryptFinal_ex(ctx, (unsigned char *)plaintext + len, &final_len) == 1;

    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_free(blob);

    if (!ok) {
        // Wipe whatever was decrypted before authentication failed
        if (plaintext) {
            OPENSSL_clear_free(plaintext, ciphertext_len + 1);
        }
        return NULL;
    }
    plaintext[len + final_len] = '\0';
    return plaintext;
}

void crypto_free(char *text) {
    if (text) {
        OPENSSL_clear_free(text, strlen(text));
    }
}
