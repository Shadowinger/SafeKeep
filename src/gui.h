//
// Created by Standa Křikava on 29.04.2025.
//

#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *entry_service;
    GtkWidget *entry_email;
    GtkWidget *entry_password;
    GtkWidget *main_pass;
} EntryWidgets;

// GUI functions - changing from static to public visibility
void login(GtkApplication *app, gpointer user_data);
void main_login(GtkButton *main_pass_button, gpointer user_data);
void activate(GtkApplication *app, gpointer user_data);
void on_show_button_clicked(GtkButton *button, gpointer user_data);
void on_add_entry_clicked(GtkButton *button, gpointer user_data);
void on_copy_button_clicked(GtkButton *button, gpointer user_data);
void on_delete_button_clicked(GtkButton *button, gpointer user_data);

// Global variables needed across files
extern GtkWidget *grid;
extern int row_counter;

#endif //GUI_H





