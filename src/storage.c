//
// Created by Standa Křikava on 29.04.2025.
//

#include "storage.h"
#include "gui.h"
#include "crypto.h"
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>

// Vault file layout, one base64 value per line:
//   salt, verifier (VERIFIER_TEXT encrypted with the master key), then service/email/password per entry.
#define VERIFIER_TEXT "SAFEKEEP-OK"
#define HEADER_LINES 2
#define RECORD_LINES 3

extern GtkWidget *grid;
extern int row_counter;

static const char *vault_path(void) {
    static char *path = NULL;
    if (!path) {
        path = g_build_filename(g_get_home_dir(), ".safekeep", "passwords.dat", NULL);
    }
    return path;
}

// Reads the vault split into lines. *lines stays NULL if the vault does not exist yet or is empty.
static gboolean read_vault(char ***lines) {
    char *contents = NULL;
    GError *error = NULL;

    *lines = NULL;
    if (!g_file_get_contents(vault_path(), &contents, NULL, &error)) {
        gboolean missing = g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT);
        if (!missing) {
            g_warning("Cannot read %s: %s", vault_path(), error->message);
        }
        g_error_free(error);
        return missing;
    }
    if (contents[0] != '\0') {
        *lines = g_strsplit(contents, "\n", -1);
    }
    g_free(contents);
    return TRUE;
}

// Replaces the vault atomically, so a failed write never leaves a half-written file.
static gboolean write_vault(const char *contents) {
    char *dir = g_path_get_dirname(vault_path());
    GError *error = NULL;
    gboolean ok = FALSE;

    if (g_mkdir_with_parents(dir, 0700) != 0) {
        g_warning("Cannot create %s: %s", dir, g_strerror(errno));
    } else if (!g_file_set_contents_full(vault_path(), contents, -1, G_FILE_SET_CONTENTS_CONSISTENT, 0600, &error)) {
        g_warning("Cannot write %s: %s", vault_path(), error->message);
        g_error_free(error);
    } else {
        ok = TRUE;
    }
    g_free(dir);
    return ok;
}

static gboolean create_vault(const char *master_password) {
    unsigned char salt[CRYPTO_SALT_LEN];
    if (!crypto_new_salt(salt) || !crypto_set_key(master_password, salt)) {
        g_warning("Cannot derive encryption key.");
        return FALSE;
    }

    char *verifier = crypto_encrypt(VERIFIER_TEXT);
    char *salt_encoded = g_base64_encode(salt, sizeof(salt));
    gboolean ok = FALSE;
    if (verifier) {
        char *contents = g_strdup_printf("%s\n%s\n", salt_encoded, verifier);
        ok = write_vault(contents);
        g_free(contents);
    }
    g_free(salt_encoded);
    crypto_free(verifier);

    if (ok) {
        g_message("Master password set.");
    } else {
        crypto_clear_key();
    }
    return ok;
}

// Creates the vault on first run, otherwise derives the key and checks it against the verifier.
gboolean storage_unlock(const char *master_password) {
    char **lines;
    if (!read_vault(&lines)) {
        return FALSE;
    }
    if (!lines) {
        return create_vault(master_password);
    }

    gboolean ok = FALSE;
    if (g_strv_length(lines) >= HEADER_LINES) {
        gsize salt_len = 0;
        guchar *salt = g_base64_decode(lines[0], &salt_len);
        if (salt_len == CRYPTO_SALT_LEN && crypto_set_key(master_password, salt)) {
            char *check = crypto_decrypt(lines[1]);
            ok = check && strcmp(check, VERIFIER_TEXT) == 0;
            crypto_free(check);
        }
        g_free(salt);
    }
    g_strfreev(lines);

    if (!ok) {
        crypto_clear_key();
        g_warning("Wrong master password or damaged vault.");
    }
    return ok;
}

static GtkWidget *new_cell_label(const char *text, const char *name) {
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_name(label, name);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label, 10);
    gtk_widget_set_margin_end(label, 10);
    gtk_widget_set_margin_top(label, 5);
    gtk_widget_set_margin_bottom(label, 5);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    return label;
}

static GtkWidget *new_row_button(const char *text, const char *name, GCallback handler) {
    GtkWidget *button = gtk_button_new_with_label(text);
    gtk_widget_set_name(button, name);
    gtk_widget_set_margin_start(button, 5);
    gtk_widget_set_margin_end(button, 10);
    gtk_widget_set_margin_top(button, 2);
    gtk_widget_set_margin_bottom(button, 2);
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
    g_object_set_data(G_OBJECT(button), "row", GINT_TO_POINTER(row_counter));
    g_signal_connect(button, "clicked", handler, NULL);
    return button;
}

static void add_entry_row(const char *service_text, const char *email_text,
                          const char *service_encrypted, const char *email_encrypted, const char *password_encrypted) {
    gtk_grid_attach(GTK_GRID(grid), new_cell_label(service_text, "label-service"), 0, row_counter, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), new_cell_label(email_text, "label-email"), 1, row_counter, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), new_cell_label("********", "label-password"), 2, row_counter, 1, 1);

    GtkWidget *show_button = new_row_button("Show", "show-button", G_CALLBACK(on_show_button_clicked));
    g_object_set_data_full(G_OBJECT(show_button), "encrypted_password", g_strdup(password_encrypted), g_free);
    gtk_grid_attach(GTK_GRID(grid), show_button, 3, row_counter, 1, 1);

    GtkWidget *copy_button = new_row_button("Copy", "copy-button", G_CALLBACK(on_copy_button_clicked));
    g_object_set_data_full(G_OBJECT(copy_button), "encrypted_password", g_strdup(password_encrypted), g_free);
    gtk_grid_attach(GTK_GRID(grid), copy_button, 4, row_counter, 1, 1);

    GtkWidget *delete_button = new_row_button("Delete", "delete-button", G_CALLBACK(on_delete_button_clicked));
    g_object_set_data_full(G_OBJECT(delete_button), "service_encrypted", g_strdup(service_encrypted), g_free);
    g_object_set_data_full(G_OBJECT(delete_button), "email_encrypted", g_strdup(email_encrypted), g_free);
    g_object_set_data_full(G_OBJECT(delete_button), "password_encrypted", g_strdup(password_encrypted), g_free);
    gtk_grid_attach(GTK_GRID(grid), delete_button, 5, row_counter, 1, 1);

    row_counter++;
}

void add_new_entry(const char *service_text, const char *email_text, const char *password_text) {
    char *service_encrypted = crypto_encrypt(service_text);
    char *email_encrypted = crypto_encrypt(email_text);
    char *password_encrypted = crypto_encrypt(password_text);
    char *contents = NULL;
    GError *error = NULL;

    if (!service_encrypted || !email_encrypted || !password_encrypted) {
        g_warning("Encryption failed, entry was not saved.");
    } else if (!g_file_get_contents(vault_path(), &contents, NULL, &error)) {
        g_warning("Cannot read %s: %s", vault_path(), error->message);
        g_error_free(error);
    } else {
        char *updated = g_strdup_printf("%s%s\n%s\n%s\n", contents, service_encrypted, email_encrypted, password_encrypted);
        if (write_vault(updated)) {
            add_entry_row(service_text, email_text, service_encrypted, email_encrypted, password_encrypted);
        }
        g_free(updated);
    }

    g_free(contents);
    crypto_free(service_encrypted);
    crypto_free(email_encrypted);
    crypto_free(password_encrypted);
}

void load_entries_from_file(void) {
    char **lines;
    if (!read_vault(&lines) || !lines) {
        return;
    }

    guint count = g_strv_length(lines);
    for (guint i = HEADER_LINES; i + RECORD_LINES <= count; i += RECORD_LINES) {
        char *service = crypto_decrypt(lines[i]);
        char *email = crypto_decrypt(lines[i + 1]);
        if (service && email) {
            add_entry_row(service, email, lines[i], lines[i + 1], lines[i + 2]);
        } else {
            g_warning("Skipping an entry that cannot be decrypted.");
        }
        crypto_free(service);
        crypto_free(email);
    }
    g_strfreev(lines);
}

gboolean delete_entry_from_file(const char *service_encrypted, const char *email_encrypted, const char *password_encrypted) {
    char **lines;
    if (!read_vault(&lines) || !lines) {
        return FALSE;
    }

    guint count = g_strv_length(lines);
    GString *contents = g_string_new(NULL);
    gboolean entry_found = FALSE;

    for (guint i = 0; i < HEADER_LINES && i < count; i++) {
        g_string_append_printf(contents, "%s\n", lines[i]);
    }
    for (guint i = HEADER_LINES; i + RECORD_LINES <= count; i += RECORD_LINES) {
        if (strcmp(lines[i], service_encrypted) == 0 &&
            strcmp(lines[i + 1], email_encrypted) == 0 &&
            strcmp(lines[i + 2], password_encrypted) == 0) {
            entry_found = TRUE;
            continue;
        }
        g_string_append_printf(contents, "%s\n%s\n%s\n", lines[i], lines[i + 1], lines[i + 2]);
    }

    gboolean ok = FALSE;
    if (!entry_found) {
        g_warning("Entry not found in file.");
    } else if (write_vault(contents->str)) {
        g_message("Entry deleted from file.");
        ok = TRUE;
    }

    g_string_free(contents, TRUE);
    g_strfreev(lines);
    return ok;
}
