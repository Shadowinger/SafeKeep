//
// Created by Standa Křikava on 29.04.2025.
//

#include "storage.h"
#include "gui.h"
#include "crypto.h"
#include <gtk/gtk.h>
#include <string.h>

extern GtkWidget *grid;
extern int row_counter;

void add_new_entry(const char *service_text, const char *email_text, const char *password_text) {
    unsigned char encrypted_service[512];
    unsigned char encrypted_email[512];
    unsigned char encrypted_password[512];
    char encoded_service[1024];
    char encoded_email[1024];
    char encoded_password[1024];
    int ciphertext_len;

    // Encrypt and encode password first 
    encrypt_data(password_text, encrypted_password, encoded_password, &ciphertext_len);

    // Copy text from Entry fields
    char *service_copy = g_strdup(service_text);
    char *email_copy = g_strdup(email_text);
    char *password_copy = g_strdup(password_text);

    // Create new widgets for the grid
    GtkWidget *label_service = gtk_label_new(service_copy);
    gtk_widget_set_name(label_service, "label");
    gtk_widget_set_halign(label_service, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label_service, 10);
    gtk_widget_set_margin_end(label_service, 10);
    gtk_widget_set_margin_top(label_service, 5);
    gtk_widget_set_margin_bottom(label_service, 5);
    gtk_widget_set_hexpand(label_service, TRUE);
    gtk_widget_set_valign(label_service, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_service, 0, row_counter, 1, 1);

    // Apply the same margins to email and password labels
    GtkWidget *label_email = gtk_label_new(email_copy);
    gtk_widget_set_name(label_email, "email");
    gtk_widget_set_halign(label_email, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label_email, 10);
    gtk_widget_set_margin_end(label_email, 10);
    gtk_widget_set_margin_top(label_email, 5);
    gtk_widget_set_margin_bottom(label_email, 5);
    gtk_widget_set_hexpand(label_email, TRUE);
    gtk_widget_set_valign(label_email, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_email, 1, row_counter, 1, 1);

    GtkWidget *label_password = gtk_label_new("********");
    gtk_widget_set_halign(label_password, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label_password, 10);
    gtk_widget_set_margin_end(label_password, 10);
    gtk_widget_set_margin_top(label_password, 5);
    gtk_widget_set_margin_bottom(label_password, 5);
    gtk_widget_set_hexpand(label_password, TRUE);
    gtk_widget_set_valign(label_password, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_password, 2, row_counter, 1, 1);
    
    GtkWidget *show_button = gtk_button_new_with_label("Show");
    gtk_widget_set_name(show_button, "show-button");
    gtk_widget_set_margin_start(show_button, 5);
    gtk_widget_set_margin_end(show_button, 10);
    gtk_widget_set_margin_top(show_button, 2);
    gtk_widget_set_margin_bottom(show_button, 2);
    gtk_widget_set_halign(show_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(show_button, GTK_ALIGN_CENTER);
    g_object_set_data(G_OBJECT(show_button), "row", GINT_TO_POINTER(row_counter));
    g_object_set_data_full(G_OBJECT(show_button), "encrypted_password", g_strdup(encoded_password), g_free);
    g_signal_connect(show_button, "clicked", G_CALLBACK(on_show_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), show_button, 3, row_counter, 1, 1);

    GtkWidget *copy_button = gtk_button_new_with_label("Copy");
    gtk_widget_set_name(copy_button, "copy-button");
    gtk_widget_set_margin_start(copy_button, 5);
    gtk_widget_set_margin_end(copy_button, 10);
    gtk_widget_set_margin_top(copy_button, 2);
    gtk_widget_set_margin_bottom(copy_button, 2);
    gtk_widget_set_halign(copy_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(copy_button, GTK_ALIGN_CENTER);
    g_object_set_data(G_OBJECT(copy_button), "row", GINT_TO_POINTER(row_counter));
    g_object_set_data_full(G_OBJECT(copy_button), "encrypted_password", g_strdup(encoded_password), g_free);
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), copy_button, 4, row_counter, 1, 1);






    FILE *file = fopen("data/passwords.dat", "a");
    if (!file) {
        g_warning("Nelze otevřít soubor data/passwords.dat");
        return;
    }

    // Encrypt and encode service
    encrypt_data(service_text, encrypted_service, encoded_service, &ciphertext_len);

    // Encrypt and encode email
    encrypt_data(email_text, encrypted_email, encoded_email, &ciphertext_len);

    // Write to file
    fprintf(file, "%s\n%s\n%s\n", encoded_service, encoded_email, encoded_password);
    fclose(file);

    row_counter++;

    // Update layout
    gtk_widget_set_visible(label_service, true);
    gtk_widget_set_visible(label_email, true);
    gtk_widget_set_visible(show_button, true);

    // Free copies
    g_free(service_copy);
    g_free(email_copy);
    g_free(password_copy);
}

void load_entries_from_file() {
    FILE *file = fopen("data/passwords.dat", "r");
    if (file) {
        char service_line[1024];
        char email_line[1024];
        char password_line[1024];

        while (fgets(service_line, sizeof(service_line), file) &&
               fgets(email_line, sizeof(email_line), file) &&
               fgets(password_line, sizeof(password_line), file)) {

            service_line[strcspn(service_line, "\n")] = 0;
            email_line[strcspn(email_line, "\n")] = 0;
            password_line[strcspn(password_line, "\n")] = 0;

            GtkWidget *label_service = gtk_label_new("");
            gtk_widget_set_name(label_service, "label-service");
            gtk_widget_set_halign(label_service, GTK_ALIGN_START);
            gtk_widget_set_margin_start(label_service, 10);
            gtk_widget_set_margin_end(label_service, 10);
            gtk_widget_set_margin_top(label_service, 5);
            gtk_widget_set_margin_bottom(label_service, 5);
            gtk_widget_set_hexpand(label_service, TRUE);
            gtk_widget_set_valign(label_service, GTK_ALIGN_CENTER);
            gtk_grid_attach(GTK_GRID(grid), label_service, 0, row_counter, 1, 1);
            decrypt_and_show_entry(service_line, label_service);

            GtkWidget *label_email = gtk_label_new("");
            gtk_widget_set_name(label_email, "label-email");
            gtk_widget_set_halign(label_email, GTK_ALIGN_START);
            gtk_widget_set_margin_start(label_email, 10);
            gtk_widget_set_margin_end(label_email, 10);
            gtk_widget_set_margin_top(label_email, 5);
            gtk_widget_set_margin_bottom(label_email, 5);
            gtk_widget_set_hexpand(label_email, TRUE);
            gtk_widget_set_valign(label_email, GTK_ALIGN_CENTER);
            gtk_grid_attach(GTK_GRID(grid), label_email, 1, row_counter, 1, 1);
            decrypt_and_show_entry(email_line, label_email);

            GtkWidget *label_password = gtk_label_new("********");
            gtk_widget_set_halign(label_password, GTK_ALIGN_START);
            gtk_widget_set_name(label_password, "label-password");
            gtk_widget_set_margin_start(label_password, 10);
            gtk_widget_set_margin_end(label_password, 10);
            gtk_widget_set_margin_top(label_password, 5);
            gtk_widget_set_margin_bottom(label_password, 5);
            gtk_widget_set_hexpand(label_password, TRUE);
            gtk_widget_set_valign(label_password, GTK_ALIGN_CENTER);
            gtk_grid_attach(GTK_GRID(grid), label_password, 2, row_counter, 1, 1);

            GtkWidget *show_button = gtk_button_new_with_label("Show");
            gtk_widget_set_name(show_button, "show-button");
            gtk_widget_set_margin_start(show_button, 5);
            gtk_widget_set_margin_end(show_button, 10);
            gtk_widget_set_margin_top(show_button, 2);
            gtk_widget_set_margin_bottom(show_button, 2);
            gtk_widget_set_halign(show_button, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(show_button, GTK_ALIGN_CENTER);
            g_object_set_data(G_OBJECT(show_button), "row", GINT_TO_POINTER(row_counter));
            g_object_set_data_full(G_OBJECT(show_button), "encrypted_password", g_strdup(password_line), g_free);
            g_signal_connect(show_button, "clicked", G_CALLBACK(on_show_button_clicked), NULL);
            gtk_grid_attach(GTK_GRID(grid), show_button, 3, row_counter, 1, 1);

            GtkWidget *copy_button = gtk_button_new_with_label("Copy");
            gtk_widget_set_name(copy_button, "copy-button");
            gtk_widget_set_margin_start(copy_button, 5);
            gtk_widget_set_margin_end(copy_button, 10);
            gtk_widget_set_margin_top(copy_button, 2);
            gtk_widget_set_margin_bottom(copy_button, 2);
            gtk_widget_set_halign(copy_button, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(copy_button, GTK_ALIGN_CENTER);
            g_object_set_data(G_OBJECT(copy_button), "row", GINT_TO_POINTER(row_counter));
            g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), NULL);
            g_object_set_data_full(G_OBJECT(copy_button), "encrypted_password", g_strdup(password_line), g_free);
            gtk_grid_attach(GTK_GRID(grid), copy_button, 4, row_counter, 1, 1); 

            row_counter++;
        }
        fclose(file);
    }
}
