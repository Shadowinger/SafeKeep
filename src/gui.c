//
// Created by Standa Křikava on 29.04.2025.
//
#include "gui.h"
#include <gtk/gtk.h>

typedef struct {
    GtkWidget *entry_service;
    GtkWidget *entry_email;
} EntryWidgets;

static GtkWidget *grid;
static int row_counter = 0;

static void on_add_entry_clicked(GtkButton *button, gpointer user_data) {
    EntryWidgets *entries = (EntryWidgets *)user_data;
    // Simulace dat – můžeš nahradit za reálný vstup
    const char *service_text = gtk_editable_get_text(GTK_EDITABLE(entries->entry_service));
    const char *email_text = gtk_editable_get_text(GTK_EDITABLE(entries->entry_email));
    
    // Uložení do bufferů
    char service_buffer[100];
    char email_buffer[100];
    
    snprintf(service_buffer, sizeof(service_buffer), "%s", service_text);
    snprintf(email_buffer, sizeof(email_buffer), "%s", email_text);
 
    GtkWidget *label_service = gtk_label_new(service_text);
    gtk_widget_set_name(label_service, "label");
    GtkWidget *label_email = gtk_label_new(email_text);
    gtk_widget_set_name(label_email, "email");
    GtkWidget *show_button = gtk_button_new_with_label("Zobrazit");
    gtk_widget_set_name(show_button, "show-button");


    // Přidání do gridu
    gtk_grid_attach(GTK_GRID(grid), label_service, 0, row_counter, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_email,   1, row_counter, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), show_button,   2, row_counter, 1, 1);

    row_counter++;

    // Aktualizace rozvržení
    gtk_widget_show(label_service);
    gtk_widget_show(label_email);
    gtk_widget_show(show_button);
}

static void activate(GtkApplication *app, gpointer user_data) {
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

    EntryWidgets *entries = g_malloc(sizeof(EntryWidgets));
    entries->entry_service = entry_service;
    entries->entry_email = entry_email;

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

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("cz.password.manager", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}