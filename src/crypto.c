//
// Created by Standa Křikava on 29.04.2025.
//

#include "crypto.h"
#include <gtk/gtk.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <string.h>

// Encryption key and initialization vector
static unsigned char key[32] = "01234567890123456789012345678901";
static unsigned char iv[16] = "0123456789012345"; 

void encrypt_data(const char *plaintext, unsigned char *encrypted, char *encoded, int *ciphertext_len) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, encrypted, &len, (unsigned char *)plaintext, strlen(plaintext));
    *ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, encrypted + len, &len);
    *ciphertext_len += len;
    EVP_EncodeBlock((unsigned char *)encoded, encrypted, *ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);
}

void decrypt_and_show_entry(const char *encoded_text, GtkWidget *label) {
    unsigned char decoded[512];
    unsigned char decrypted[512] = {0};
    int len, plaintext_len;
    
    // Base64 decode
    EVP_ENCODE_CTX *decode_ctx = EVP_ENCODE_CTX_new();
    EVP_DecodeInit(decode_ctx);
    int decoded_len = 0;
    EVP_DecodeUpdate(decode_ctx, decoded, &decoded_len, (unsigned char *)encoded_text, strlen(encoded_text));
    int final_len;
    EVP_DecodeFinal(decode_ctx, decoded + decoded_len, &final_len);
    decoded_len += final_len;
    EVP_ENCODE_CTX_free(decode_ctx);
    
    // Decryption method
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, decrypted, &len, decoded, decoded_len);
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, decrypted + len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    
    decrypted[plaintext_len] = '\0';
    gtk_label_set_text(GTK_LABEL(label), (char *)decrypted);
}










