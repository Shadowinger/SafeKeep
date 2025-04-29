//
// Created by Standa Křikava on 29.04.2025.
//

#include "auth.h"
#include <string.h>
#include <stdio.h>



static char *stored_password = NULL;
static int authenticated = 0;

int authenticate(const char *password) {
    if (stored_password && strcmp(stored_password, password) == 0) {
        authenticated = 1;
        return 1;
    }
    return 0;
}

int set_master_password(const char *new_password) {
    if (stored_password) free(stored_password);
    stored_password = strdup(new_password);
    return stored_password != NULL;
}

int is_authenticated() {
    return authenticated;
}

void logout() {
    authenticated = 0;
}