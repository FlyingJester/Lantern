/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_std.hpp"
#include <cstring>

#ifndef DLCL_NO_STDIO
#include <cstdio>
#endif

namespace DarkLight {
namespace CL{
namespace Std{

#define UNUSED_PARAM(X_) if(X_){}

bool Max(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    UNUSED_PARAM(arg)
    if(num_args == 0){
        strcpy(err, "Expected 1 or more arguments to Max");
        return false;
    }

    int m = args->m_value.integer;
    for(unsigned i = 0; i < num_args; i++){
        if(args[i].m_type != Value::Integer){
            strcpy(err, "Non-integer argument to Max");
            return false;
        }
        if(args[i].m_value.integer > m)
            m = args[i].m_value.integer;
    }
    
    return_val.m_type = Value::Integer;
    return_val.m_value.integer = m;
    return true;
}

bool Min(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    UNUSED_PARAM(arg)
    if(num_args == 0){
        strcpy(err, "Expected 1 or more arguments to Min");
        return false;
    }

    int m = args->m_value.integer;
    for(unsigned i = 0; i < num_args; i++){
        if(args[i].m_type != Value::Integer){
            strcpy(err, "Non-integer argument to Min");
            return false;
        }
        if(args[i].m_value.integer < m)
            m = args[i].m_value.integer;
    }
    
    return_val.m_type = Value::Integer;
    return_val.m_value.integer = m;
    return true;
}

bool Sum(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    UNUSED_PARAM(arg)
    int m = 0;
    for(unsigned i = 0; i < num_args; i++){
        if(args[i].m_type != Value::Integer){
            strcpy(err, "Non-integer argument to Sum");
            return false;
        }
        m += args[i].m_value.integer;
    }
    
    return_val.m_type = Value::Integer;
    return_val.m_value.integer = m;
    return true;
}

bool Any(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    UNUSED_PARAM(arg)
    return_val.m_type = Value::Boolean;
    for(unsigned i = 0; i < num_args; i++){
        if(args[i].m_type != Value::Boolean){
            strcpy(err, "Non-boolean argument to Any");
            return false;
        }
        if(args[i].m_value.boolean){
            return return_val.m_value.boolean = true;
        }
    }
    
    return_val.m_value.boolean = false;
    return true;
}

bool All(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    UNUSED_PARAM(arg)
    return_val.m_type = Value::Boolean;
    for(unsigned i = 0; i < num_args; i++){
        if(args[i].m_type != Value::Boolean){
            strcpy(err, "Non-boolean argument to Any");
            return false;
        }
        if(!args[i].m_value.boolean){
            return_val.m_value.boolean = false;
            return true;
        }
    }
    
    return return_val.m_value.boolean = true;
}

bool Len(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    UNUSED_PARAM(arg)
    if(num_args != 1){
        strcpy(err, "Expected 1 argument to Len");
        return false;
    }
    
    if(args->m_type != Value::String){
        strcpy(err, "Expected string argument to Len");
        return false;
    }
    
    return_val.m_type = Value::Integer;
    return_val.m_value.integer = args->a.length;
    return true;
}

bool Empty(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    UNUSED_PARAM(arg)
    if(num_args != 1){
        strcpy(err, "Expected 1 argument to Empty");
        return false;
    }
    
    if(args->m_type != Value::String){
        strcpy(err, "Expected string argument to Empty");
        return false;
    }
    
    return_val.m_type = Value::Boolean;
    return_val.m_value.boolean = args->a.length == 0;
    return true;
}

bool Print(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    if(err || arg || args || num_args){}
#ifndef DLCL_NO_STDIO
    for(unsigned i = 0; i < num_args; i++){
        
        if(args[i].m_type == Value::String)
            fwrite(args[i].m_value.string, args[i].a.length, 1, stdout);
        else if(args[i].m_type == Value::Integer)
            printf("%i", args[i].m_value.integer);
        else if(args[i].m_type == Value::Boolean)
            fputs(args[i].m_value.boolean ? "true" : "false", stdout);
        else
            printf("%x ( %x )", args[i].m_value.function, args[i].a.length);
    }
    
#endif
    
    return_val.m_type = Value::Boolean;
    return_val.m_value.boolean = true;

    return true;
}

} // namespace Std
} // namespace CL
} // namespace DarkLight
