//
// Created by Standa Křikava on 29.04.2025.
//

#ifndef CRYPTO_H
#define CRYPTO_H

#include <gtk/gtk.h>

// Encryption/decryption functions
void encrypt_data(const char *plaintext, unsigned char *encrypted, char *encoded, int *ciphertext_len);
void decrypt_and_show_entry(const char *encoded_text, GtkWidget *label);

#endif //CRYPTO_H
