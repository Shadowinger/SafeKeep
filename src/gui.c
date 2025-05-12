//
// Created by Standa Křikava on 29.04.2025.
//
#include "gui.h"
#include <gtk/gtk.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>


typedef struct {
    GtkWidget *entry_service;
    GtkWidget *entry_email;
    GtkWidget *entry_password;
    GtkWidget *main_pass;
} EntryWidgets;



static GtkWidget *grid;
static int row_counter = 0;

int main(int argc, char **argv);
static void login(GtkApplication    *app,  gpointer user_data);
static void main_login(GtkButton *main_pass_button, gpointer user_data);
static void activate(GtkApplication *app, gpointer user_data);
static void on_show_button_clicked(GtkApplication *app,  gpointer user_data);







static void login(GtkApplication *app,  gpointer user_data){
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
    gtk_window_set_default_size(GTK_WINDOW(login_window), 600, 400);
    gtk_widget_set_name(login_window, "login-window");

    GtkWidget *login_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(login_window), login_vbox);
    
    GtkWidget *main_pass = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(main_pass), "Enter-masterpass");
    gtk_box_append(GTK_BOX(login_vbox), main_pass);
    entries->main_pass = main_pass;
    
    GtkWidget *main_pass_button = gtk_button_new_with_label("Enter");
    gtk_box_append(GTK_BOX(login_vbox), main_pass_button);
    g_signal_connect(main_pass_button, "clicked", G_CALLBACK(main_login), entries);
    
    gtk_widget_show(login_window);

}
static void main_login(GtkButton *main_pass_button, gpointer user_data) {
    EntryWidgets *entries = (EntryWidgets *)user_data;
    GtkApplication *app = GTK_APPLICATION(g_application_get_default());
    const char *main_text = gtk_editable_get_text(GTK_EDITABLE(entries->main_pass));

    // Načtení hesla ze souboru
    FILE *file = fopen("data/main-pass.dat", "r");
    if (!file) {
        g_warning("Nelze otevřít soubor password.dat");
        return;
    }

    char stored_pass[256];
    if (!fgets(stored_pass, sizeof(stored_pass), file)) {
        g_warning("Nepodařilo se přečíst heslo ze souboru");
        fclose(file);
        return;
    }
    fclose(file);

    // Odstranění případného '\n' na konci
    stored_pass[strcspn(stored_pass, "\n")] = '\0';

    if (strcmp(main_text, stored_pass) == 0) {
        GtkWidget *button = GTK_WIDGET(main_pass_button);
        GtkWidget *login_window = gtk_widget_get_ancestor(button, GTK_TYPE_WINDOW);
        gtk_widget_set_name(login_window, "login");
        if (login_window) {
            activate(app, entries);
            gtk_window_close(GTK_WINDOW(login_window));
            g_signal_connect(app, "activate", G_CALLBACK(activate), entries);
        }
    } else {
        g_warning("Špatné heslo");
    }



}

static void on_add_entry_clicked(GtkButton *button, gpointer user_data) {
    EntryWidgets *entries = (EntryWidgets *)user_data;
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

    // Copy text from Entry fields
    char *service_copy = g_strdup(service_text);
    char *email_copy = g_strdup(email_text);
    char *password_copy = g_strdup(password_text);

    // Create new widgets for the grid
    GtkWidget *label_service = gtk_label_new(service_copy);
    gtk_widget_set_name(label_service, "label");
    GtkWidget *label_email = gtk_label_new(email_copy);
    gtk_widget_set_name(label_email, "email");
    GtkWidget *label_password = gtk_label_new(password_copy);
    GtkWidget *show_button = gtk_button_new_with_label("Zobrazit");
    gtk_widget_set_name(show_button, "show-button");

    // Connect button to callback with password data
    g_object_set_data_full(G_OBJECT(show_button), "password", password_copy, g_free);
    // g_signal_connect(show_button, "clicked", G_CALLBACK(on_show_password_clicked), NULL);

    // Add to grid
    gtk_grid_attach(GTK_GRID(grid), label_service, 0, row_counter, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_email, 1, row_counter, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_password, 2, row_counter, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), show_button, 3, row_counter, 1, 1);

    FILE *file = fopen("data/passwords.dat", "a");
    if (!file) {
        g_warning("Nelze otevřít soubor data/passwords.dat");
        return;
    }

    unsigned char key[32] = "01234567890123456789012345678901"; // 32 bajtů
    unsigned char iv[16] = "0123456789012345"; // 16 bajtů

    unsigned char encrypted_service[512];
    unsigned char encrypted_email[512];
    unsigned char encrypted_password[512];
    char encoded_service[1024];
    char encoded_email[1024];
    char encoded_password[1024];
    int len, ciphertext_len;

    // Encrypt and encode service
    EVP_CIPHER_CTX *ctx_service = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx_service, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx_service, encrypted_service, &len, (unsigned char *)service_text, strlen(service_text));
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx_service, encrypted_service + len, &len);
    ciphertext_len += len;
    EVP_EncodeBlock((unsigned char *)encoded_service, encrypted_service, ciphertext_len);
    EVP_CIPHER_CTX_free(ctx_service);

    // Encrypt and encode email
    EVP_CIPHER_CTX *ctx_email = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx_email, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx_email, encrypted_email, &len, (unsigned char *)email_text, strlen(email_text));
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx_email, encrypted_email + len, &len);
    ciphertext_len += len;
    EVP_EncodeBlock((unsigned char *)encoded_email, encrypted_email, ciphertext_len);
    EVP_CIPHER_CTX_free(ctx_email);

    // Encrypt and encode password
    EVP_CIPHER_CTX *ctx_password = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx_password, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx_password, encrypted_password, &len, (unsigned char *)password_text, strlen(password_text));
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx_password, encrypted_password + len, &len);
    ciphertext_len += len;
    EVP_EncodeBlock((unsigned char *)encoded_password, encrypted_password, ciphertext_len);
    EVP_CIPHER_CTX_free(ctx_password);

    fprintf(file, "%s\n%s\n%s\n", encoded_service, encoded_email, encoded_password);
    fclose(file);

    row_counter++;

    // Update layout
    gtk_widget_set_visible(label_service, true);
    gtk_widget_set_visible(label_email, true);
    gtk_widget_set_visible(show_button, true);

    // Free copies used for labels (except password_copy which is attached to the button)
    g_free(service_copy);
    g_free(email_copy);
    // password_copy will be freed when the button is destroyed

    // Clear the entry fields
    gtk_editable_set_text(GTK_EDITABLE(entries->entry_service), "");
    gtk_editable_set_text(GTK_EDITABLE(entries->entry_email), "");
    gtk_editable_set_text(GTK_EDITABLE(entries->entry_password), "");
}

static void activate(GtkApplication *app, gpointer user_data) {
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
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
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

    // Tlačítko pro přidání položky
    GtkWidget *add_button = gtk_button_new_with_label("New Entry");
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_entry_clicked), entries); 
    gtk_box_append(GTK_BOX(vbox), add_button);

    // Grid na položky
    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    // ScrolledWindow obal
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), grid);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);

    // Přidání scrolled_window místo přímo gridu
    gtk_box_append(GTK_BOX(vbox), scrolled_window);

    gtk_widget_show(window);
}
static void on_show_button_clicked(GtkApplication *app,  gpointer user_data);


int main(int argc, char **argv) {
    gtk_init();

    EntryWidgets *entries = g_malloc0(sizeof(EntryWidgets));  // Allocate memory for EntryWidgets

    GtkApplication *app = gtk_application_new("cz.password.manager", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(login), entries);  // Pass it to the activate function

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_free(entries);

    g_object_unref(app);

    return status;
            
}