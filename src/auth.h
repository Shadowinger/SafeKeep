//
// Created by Standa Křikava on 29.04.2025.
//

#ifndef AUTH_H
#define AUTH_H

int authenticate(const char *input_password);
int set_master_password(const char *new_password);
int is_authenticated();
void logout();





#endif //AUTH_H
