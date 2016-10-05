/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_utils.h"

void DLCL_Utils_NumberToString(char *to, int n, int len, bool null_term){
    int x;
    int i = 0;
    char buffer[80];

    if(len < 0){
        len = -1;
        do{
            len++;
        }while(to[len]);
    }
        
    while(n){
        buffer[sizeof(buffer) - (++i)] = (n % 10) + '0';
        n /= 10;
    }
    
    char *const from = buffer + sizeof(buffer) - i;
    
    for(x = 0; x < i && x < len; x++){
        to[x] = from[x];
    }
    if(i < len && null_term)
        to[i] = '\0';
}
