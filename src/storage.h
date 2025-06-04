//
// Created by Standa Křikava on 29.04.2025.
//

#ifndef STORAGE_H
#define STORAGE_H

#include <gtk/gtk.h>

// Storage functions
void add_new_entry(const char *service_text, const char *email_text, const char *password_text);
void load_entries_from_file();
void create_password_file_if_not_exists();
void delete_entry_from_file(const char *service_encrypted, const char *email_encrypted, const char *password_encrypted);

#endif //STORAGE_H
