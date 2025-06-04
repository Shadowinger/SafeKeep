//
// Created by Standa Křikava on 29.04.2025.
//

#include "gui.h"
#include "storage.h"
#include <gtk/gtk.h>

int main(int argc, char **argv) {
    gtk_init();

    // Create password file if it doesn't exist
    create_password_file_if_not_exists();

    EntryWidgets *entries = g_malloc0(sizeof(EntryWidgets));  // Allocate memory for EntryWidgets

    GtkApplication *app = gtk_application_new("cz.password.manager", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(login), entries);  // Pass it to the activate function

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_free(entries);

    g_object_unref(app);

    return status;
}