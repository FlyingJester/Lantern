/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_lexer.hpp"
#include "dlcl_parser.hpp"
#include <cstdio>
#include <cstring>
#include <string>

#ifdef _MSC_VER
#pragma warning (disable: 4477)
#endif

using namespace DarkLight::CL;

bool Print(char *err, Value &return_val, void *arg, Value *args, unsigned num_args){
    fputs("dlcl> ", stdout);
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
    
    putchar('\n');
    
    return_val.m_type = Value::Boolean;
    return_val.m_value.boolean = true;
    
    return true;
}
int main(int argc, char *argv[]){
	if(argc || argv){}
	
	printf("Size of Lexer is %i, size of Parser is %i\n", sizeof(Lexer), sizeof(Parser));
	printf("Maximum number of variables: %i\n", s_max_parser_arena_size / sizeof(struct Variable));
    printf("Sizeof Token: %i\n", sizeof(Token));

    Lexer lex;
    lex.clear();
    Parser parse;
    parse.clear();
    
    std::string src = "call Print: \"Hello, world!\". \n";
    
    lex.lex(src.c_str(), src.length() - 1);
    
    printf("Lexer error: %s\n", lex.getError());
    if(*lex.getError() == '\0'){
        char buffer[80];
        for(const struct Token *i = lex.cbegin(); i!= lex.cend(); i++){
            i->toString(buffer);
            buffer[79] = 0;
            printf("%i\t", i - lex.cbegin());
            puts(buffer);
        }
        
        parse.bindCallback("Print", Print, NULL);
        const Variable *const function = parse.findVariableConst("Print", 5);
        parse.parse(lex);
        printf("Parser error: %s\n", parse.getError());
    }
    
	return 0;
}
