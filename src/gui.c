//
// Created by Standa Křikava on 29.04.2025.
//
#include "gui.h"
#include "crypto.h"
#include "storage.h"
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

// Global variables - must match the extern declarations in the header
GtkWidget *grid;
int row_counter = 0;

// Changed from static to public 
void login(GtkApplication *app, gpointer user_data) {
    EntryWidgets *entries = (EntryWidgets *)user_data;

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "ui/styles.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    GtkWidget *login_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(login_window), "Password Manager");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 300, 150);
    gtk_widget_set_name(login_window, "login-window");
    gtk_window_set_resizable(GTK_WINDOW(login_window), FALSE);

    GtkWidget *login_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(login_window), login_vbox);
    
    GtkWidget *main_pass = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(main_pass), "Enter-masterpass");
    gtk_box_append(GTK_BOX(login_vbox), main_pass);
    entries->main_pass = main_pass;
    
    GtkWidget *main_pass_button = gtk_button_new_with_label("Enter");
    gtk_box_append(GTK_BOX(login_vbox), main_pass_button);
    g_signal_connect(main_pass_button, "clicked", G_CALLBACK(main_login), entries);
    
    gtk_window_present(GTK_WINDOW(login_window)); 
}

void main_login(GtkButton *main_pass_button, gpointer user_data) {
    EntryWidgets *entries = (EntryWidgets *)user_data;
    GtkApplication *app = GTK_APPLICATION(g_application_get_default());
    const char *main_text = gtk_editable_get_text(GTK_EDITABLE(entries->main_pass));

    const char *folder = ".safekeep";
    const char *file = "main-pass.dat";
    char dir_path[512];
    char file_path[512];

    // Get home directory
    const char *home = getenv("HOME");
    if (!home) home = getenv("USERPROFILE");
    if (!home) {
        g_warning("Cannot determine home directory.");
        return;
    }

    // Build directory and file paths
    snprintf(dir_path, sizeof(dir_path), "%s/%s", home, folder);
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, file);

    // Create hidden directory if it doesn't exist
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        #ifdef _WIN32
        mkdir(dir_path);
        #else
        mkdir(dir_path, 0700);
        #endif
    }

    // Try to open the password file
    FILE *filep = fopen(file_path, "r");
    if (!filep) {
        // File does not exist, create it and save the entered password
        filep = fopen(file_path, "w");
        if (!filep) {
            g_warning("Cannot create password file.");
            return;
        }
        fprintf(filep, "%s\n", main_text);
        fclose(filep);
        #ifndef _WIN32
        chmod(file_path, 0600);
        #endif
        g_message("Master password set.");
        // Proceed to main window
        GtkWidget *button = GTK_WIDGET(main_pass_button);
        GtkWidget *login_window = gtk_widget_get_ancestor(button, GTK_TYPE_WINDOW);
        if (login_window) {
            activate(app, entries);
            gtk_window_close(GTK_WINDOW(login_window));
        }
        return;
    }

    // File exists, check password
    char stored_pass[256];
    if (!fgets(stored_pass, sizeof(stored_pass), filep)) {
        g_warning("Failed to read password from file.");
        fclose(filep);
        return;
    }
    fclose(filep);

    stored_pass[strcspn(stored_pass, "\n")] = '\0';

    if (strcmp(main_text, stored_pass) == 0) {
        GtkWidget *button = GTK_WIDGET(main_pass_button);
        GtkWidget *login_window = gtk_widget_get_ancestor(button, GTK_TYPE_WINDOW);
        if (login_window) {
            activate(app, entries);
            gtk_window_close(GTK_WINDOW(login_window));
        }
    } else {
        g_warning("Špatné heslo");
    }
}

void activate(GtkApplication *app, gpointer user_data) {
    EntryWidgets *entries = (EntryWidgets *)user_data;

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "ui/styles.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Password Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    gtk_widget_set_name(window, "main-window");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    GtkWidget *entry_service = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_service), "Enter-Service");
    gtk_box_append(GTK_BOX(vbox), entry_service);

    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email),"Enter-Email");
    gtk_box_append(GTK_BOX(vbox), entry_email);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password),"Enter-Password");
    gtk_box_append(GTK_BOX(vbox), entry_password);

    entries->entry_service = entry_service;
    entries->entry_email = entry_email;
    entries->entry_password = entry_password;

    // Button for adding new entry
    GtkWidget *add_button = gtk_button_new_with_label("New Entry");
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_entry_clicked), entries); 
    gtk_box_append(GTK_BOX(vbox), add_button);

    // Grid for entries
    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);  
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);     
    gtk_widget_set_margin_start(grid, 20);            
    gtk_widget_set_margin_end(grid, 20);              
    gtk_widget_set_margin_top(grid, 20);              
    gtk_widget_set_margin_bottom(grid, 20);           
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    // ScrolledWindow wrapper
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), grid);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);

    // Add scrolled window to the main vbox
    gtk_box_append(GTK_BOX(vbox), scrolled_window);

    // Load existing entries
    load_entries_from_file();

    gtk_window_present(GTK_WINDOW(window)); 
}

void on_show_button_clicked(GtkButton *button, gpointer user_data) {
    (void)user_data; // Silence unused parameter warning
    
    const char *encrypted_password = g_object_get_data(G_OBJECT(button), "encrypted_password");
    int row = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "row"));
    
    if (encrypted_password) {
        GtkWidget *label = gtk_grid_get_child_at(GTK_GRID(grid), 2, row);
        if (label) {
            decrypt_and_show_entry(encrypted_password, label);
        }
    }
}

void on_add_entry_clicked(GtkButton *button, gpointer user_data) {
    EntryWidgets *entries = (EntryWidgets *)user_data;
    (void)button; // To avoid unused parameter warning
    
    if (!entries->entry_service || !entries->entry_email || !entries->entry_password) {
        g_warning("One or more entry fields are not initialized!");
        return;
    }

    // Check if all items are GtkEditable type
    if (!GTK_IS_EDITABLE(entries->entry_service) || 
        !GTK_IS_EDITABLE(entries->entry_email) || 
        !GTK_IS_EDITABLE(entries->entry_password)) {
        g_warning("Some Entry items are not GtkEditable type!");
        return;
    }

    // Get text from entry fields
    const char *service_text = gtk_editable_get_text(GTK_EDITABLE(entries->entry_service));
    const char *email_text = gtk_editable_get_text(GTK_EDITABLE(entries->entry_email));
    const char *password_text = gtk_editable_get_text(GTK_EDITABLE(entries->entry_password));

    // Skip adding if service or email is empty
    if(strlen(service_text) == 0 && strlen(email_text) == 0) {
        return;
    }

    // Encrypt data and add new entry
    add_new_entry(service_text, email_text, password_text);

    // Clear the entry fields
    gtk_editable_set_text(GTK_EDITABLE(entries->entry_service), "");
    gtk_editable_set_text(GTK_EDITABLE(entries->entry_email), "");
    gtk_editable_set_text(GTK_EDITABLE(entries->entry_password), "");
}

void on_copy_button_clicked(GtkButton *button, gpointer user_data) {
    (void)user_data; // Silence unused parameter warning
    
    const char *encrypted_password = g_object_get_data(G_OBJECT(button), "encrypted_password");
    
    if (encrypted_password) {
        // Decrypt the password without showing it
        unsigned char decoded[512];
        unsigned char decrypted[512] = {0};
        int len, plaintext_len;
        
        // Base64 decode
        EVP_ENCODE_CTX *decode_ctx = EVP_ENCODE_CTX_new();
        EVP_DecodeInit(decode_ctx);
        int decoded_len = 0;
        EVP_DecodeUpdate(decode_ctx, decoded, &decoded_len, (unsigned char *)encrypted_password, strlen(encrypted_password));
        int final_len;
        EVP_DecodeFinal(decode_ctx, decoded + decoded_len, &final_len);
        decoded_len += final_len;
        EVP_ENCODE_CTX_free(decode_ctx);
        
        // Decrypt using the same key/iv as in crypto.c
        unsigned char key[32] = "01234567890123456789012345678901"; // 32 bytes
        unsigned char iv[16] = "0123456789012345"; // 16 bytes
        
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
        EVP_DecryptUpdate(ctx, decrypted, &len, decoded, decoded_len);
        plaintext_len = len;
        EVP_DecryptFinal_ex(ctx, decrypted + len, &len);
        plaintext_len += len;
        EVP_CIPHER_CTX_free(ctx);
        
        decrypted[plaintext_len] = '\0';
        
        // Copy to clipboard using GTK4 clipboard API
        GdkClipboard *clipboard = gdk_display_get_clipboard(gdk_display_get_default());
        gdk_clipboard_set_text(clipboard, (char *)decrypted);
        
        g_print("Password copied to clipboard\n");
    }
}

typedef struct {
    int row;
    char *service_encrypted;
    char *email_encrypted;
    char *password_encrypted;
} DeleteData;

void on_delete_button_clicked(GtkButton *button, gpointer user_data) {
    (void)user_data; // Silence unused parameter warning
    
    int row = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "row"));
    const char *service_encrypted = g_object_get_data(G_OBJECT(button), "service_encrypted");
    const char *email_encrypted = g_object_get_data(G_OBJECT(button), "email_encrypted");
    const char *password_encrypted = g_object_get_data(G_OBJECT(button), "password_encrypted");
    
    if (!service_encrypted || !email_encrypted || !password_encrypted) {
        g_warning("Missing encrypted data for deletion.");
        return;
    }
    
    // Create delete data structure
    DeleteData *delete_data = g_new(DeleteData, 1);
    delete_data->row = row;
    delete_data->service_encrypted = g_strdup(service_encrypted);
    delete_data->email_encrypted = g_strdup(email_encrypted);
    delete_data->password_encrypted = g_strdup(password_encrypted);
    
    // Create confirmation dialog
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    if (GTK_IS_WINDOW(root)) {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Confirm Deletion",
            GTK_WINDOW(root),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            "Cancel", GTK_RESPONSE_CANCEL,
            "Delete", GTK_RESPONSE_YES,
            NULL
        );
        
        // Add content to dialog
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *label = gtk_label_new("Are you sure you want to delete this entry?\nThis action cannot be undone.");
        gtk_box_append(GTK_BOX(content_area), label);
        gtk_widget_set_margin_top(label, 20);
        gtk_widget_set_margin_bottom(label, 20);
        gtk_widget_set_margin_start(label, 20);
        gtk_widget_set_margin_end(label, 20);
        
        // Connect response signal
        g_signal_connect(dialog, "response", G_CALLBACK(on_delete_dialog_response), delete_data);
        
        // Show dialog
        gtk_window_present(GTK_WINDOW(dialog));
    } else {
        // Free memory if dialog can't be shown
        g_free(delete_data->service_encrypted);
        g_free(delete_data->email_encrypted);
        g_free(delete_data->password_encrypted);
        g_free(delete_data);
    }
}

void on_delete_dialog_response(GtkDialog *dialog, int response, gpointer user_data) {
    DeleteData *delete_data = (DeleteData *)user_data;
    
    if (response == GTK_RESPONSE_YES) {
        // User confirmed deletion
        delete_entry_from_file(delete_data->service_encrypted, 
                               delete_data->email_encrypted, 
                               delete_data->password_encrypted);
        
        // Remove widgets from grid (all 6 columns: service, email, password, show, copy, delete)
        for (int col = 0; col < 6; col++) {
            GtkWidget *widget = gtk_grid_get_child_at(GTK_GRID(grid), col, delete_data->row);
            if (widget) {
                gtk_grid_remove(GTK_GRID(grid), widget);
            }
        }
        
        g_print("Entry deleted successfully\n");
    }
    
    // Clean up
    g_free(delete_data->service_encrypted);
    g_free(delete_data->email_encrypted);
    g_free(delete_data->password_encrypted);
    g_free(delete_data);
    
    // Destroy the dialog
    gtk_window_destroy(GTK_WINDOW(dialog));
}