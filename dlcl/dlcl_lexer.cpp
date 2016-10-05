/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_lexer.hpp"
#include "dlcl_utils.h"
#include <cstring>
#include <climits>
#include <cstdio>

namespace DarkLight {
namespace CL{

void Token::toString(char *to) const{
    const char *name = NULL;
    switch(m_type){
        case Ident:
            if(!name) name = "Ident";
        case String:
            if(!name) name = "String";
        case GetIdent:
            if(!name) name = "Get";
        case SetIdent:
            if(!name) name = "Set";
        case IntIdent:
            if(!name) name = "IntDecl";
        case StringIdent:
            if(!name) name = "StringDecl";
        case BoolIdent:
            if(!name) name = "BoolDecl";
        case FromIdent:
            if(!name) name = "From";
        case CallIdent:
            if(!name) name = "Call";
            strcpy(to, name);
            strcat(to, ": ");
            strncat(to, m_value.ident, m_length);
            return;
        case Number:
            strcpy(to, "Number: ");
            DLCL_Utils_NumberToString(to, m_value.number);
            return;
#define CASE_(X_) case X_: strcpy(to, #X_); return
        CASE_(TrueLiteral);
        CASE_(FalseLiteral);
        CASE_(If);
        CASE_(Loop);
        CASE_(Next);
        CASE_(End);
        CASE_(Return);
        CASE_(OpenParen);
        CASE_(CloseParen);
        CASE_(Colon);
        CASE_(Dot);
        CASE_(BeginArgs);
        CASE_(EndArgs);
        CASE_(Const);
        case Oper:
        switch(m_value.oper){
            CASE_(Plus);
            CASE_(Minus);
            CASE_(Multiply);
            CASE_(Divide);
            CASE_(LessThan);
            CASE_(NotLessThan);
            CASE_(GreaterThan);
            CASE_(NotGreaterThan);
            CASE_(Equal);
            CASE_(NotEqual);
            CASE_(And);
            CASE_(Or);
            CASE_(Xor);
#undef CASE_
        }
    
    }
}

Lexer::Lexer(){

}

static bool is_whitespace(char c){
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '%';
}

static bool is_digit(char c){
    return c == '0' || (c >= '1' && c <= '9');
}

static bool is_ident(char c){
    return (c>='a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || is_digit(c);
}

static bool get_op(const char *&str, unsigned &len, Operator &oper){
    switch(*str){
#define CASE_(CHAR_, OPER_, INC_)\
        case CHAR_: oper = OPER_; len -= INC_; str += INC_; return true
#define CASE_1(A_, B_) CASE_(A_, B_, 1)
#define CASE_2(A_, B_) CASE_(A_, B_, 2)

        CASE_1('+', Plus);
        CASE_1('-', Minus);
        CASE_1('*', Multiply);
        CASE_1('/', Divide);
        CASE_1('<', LessThan);
        CASE_1('>', GreaterThan);
        CASE_1('=', Equal);
        CASE_1('&', And);
        CASE_1('|', Or);
        CASE_1('^', Xor);
        
        case '!':
            if(len == 1)
                return false;
            switch(str[1]){
                CASE_2('<', NotLessThan);
                CASE_2('>', NotGreaterThan);
                CASE_2('=', NotEqual);
            }
    }
    return false;

#undef CASE_
#undef CASE_1
#undef CASE_2
}

static unsigned parseIdentifier(const char *&str, unsigned &len, char *to, unsigned max = 79){
    unsigned str_len = 0;
    do{
        to[0] = *str;
        to++;
        str++;
        len--;
        str_len++;
    }while(len && is_ident(*str) && str_len < max);
    
    to[str_len] = '\0';
    
    return str_len;
}

static void skipWhitespace(const char *&str, unsigned &len, unsigned &line){
    while(len && is_whitespace(*str)){
        // Remove comments
        if(*str == '%'){
            do{
                str++;
                len--;
            }while(len && *str != '\n');
            continue;
        }
        
        if(*str == '\n')
            line++;
        str++;
        len--;
    }
}

bool Lexer::lex(const char *str, unsigned len){
    clear();
    
//    char *string_cache[0x100];
//    unsigned num_cached_strings = 0;
    const char *const start = str;
    unsigned line = 0u;

    while(len){
    
        if(m_num_tokens >= s_max_num_tokens){
            sprintf(m_data, "Out of token memory at line %i", line);
            return false;
        }
        
        if(is_whitespace(*str)){
            skipWhitespace(str, len, line);
        }
        else if(is_digit(*str)){
            
            const unsigned max = UINT_MAX / 11u;
            unsigned u = 0;
            do{
                u *= 10;
                u += *str - '0';
                
                if(u > max){
                    sprintf(m_data, 
                        "Integer literal %u too large (max is %u) on line %i",
                        u, max, line);
                    return false;
                }
                
                str++;
            }while(--len && is_digit(*str));
            const int n = u;
            addToken(line, Token::Number, static_cast<float>(n));
        }
        else if(is_ident(*str)){
            char id[80];
            unsigned str_len = 0u;
            str_len = parseIdentifier(str, len, id);
            
            bool another_ident = false, swallow_colon = false;
            Token::Type which = (Token::Type)99;
            
            // This organization is for optimization of sctring comparsions
            if(str_len == 3 && *id == 'g' && memcmp(id, "get", 3)==0){
                which = Token::GetIdent;
                another_ident = true;
            }
            else if(*id == 's'){ 
                if(str_len == 3 && memcmp(id, "set", 3)==0){
                    which = Token::SetIdent;
                    another_ident = true;
                }
                else if(str_len == 6 && memcmp(id, "string", 6)==0){
                    which = Token::StringIdent;
                    swallow_colon = true;
                    another_ident = true;
                }
            }
            else if(str_len == 4 && *id == 'c' && memcmp(id, "call", 4)==0){
                which = Token::CallIdent;
                another_ident = true;
            }
            else if(*id == 'i'){
                if(str_len == 3 && memcmp(id, "int", 3) == 0){
                    which = Token::IntIdent;
                    swallow_colon = another_ident = true;
                }
                else if(str_len == 2 && id[1] == 'f'){
                    addTokenMod(line, Token::If);
                    continue;
                }
            }
            else if(str_len == 4 && *id == 'b' && memcmp(id, "bool", 4) == 0){
                which = Token::BoolIdent;
                another_ident = true;
                swallow_colon = true;
            }
            else if(str_len == 4 && *id == 't' && memcmp(id, "true", 4) == 0){
                addTokenMod(line, Token::TrueLiteral);
                continue;
            }
            else if(str_len == 5 && *id == 'f' && memcmp(id, "false", 5) == 0){
                addTokenMod(line, Token::FalseLiteral);
                continue;
            }
            else if(str_len == 4 && *id == 'l' && memcmp(id, "loop", 4) == 0){
                addTokenMod(line, Token::Loop);
                continue;
            }
            else if(str_len == 4 && *id == 'n' && memcmp(id, "next", 4) == 0){
                addTokenMod(line, Token::Next);
                continue;
            }
            else if(str_len == 3 && *id == 'e' && memcmp(id, "end", 3) == 0){
                addTokenMod(line, Token::End);
                continue;
            }
            else if(str_len == 5 && *id == 'c' && memcmp(id, "const", 5) == 0){
                addTokenMod(line, Token::Const);
                continue;
            }
            
            if(another_ident){
                skipWhitespace(str, len, line);
                const unsigned max_size = s_max_lexer_arena_size - m_string_len;
                char *const str_data = m_string_data + m_string_len;
                str_len = parseIdentifier(str, len, str_data, max_size);
                m_string_len += str_len;
                
                if(m_string_len == s_max_lexer_arena_size){
                    sprintf(m_data, "Out of lexer memory at line %i", line);
                    return false;
                }
                else{
                    addToken(line, which, str_data, str_len);
                }
            }
            else{
                if(s_max_lexer_arena_size - m_string_len < str_len){
                    sprintf(m_data, "Out of lexer memory at line %i", line);
                    return false;
                }
                else{
                    char *const str_data = m_string_data + m_string_len;
                    memcpy(str_data, id, str_len);
                    addToken(line, Token::Ident, str_data, str_len);
                }
            }
            if(swallow_colon && len && *str == ':'){
                str++;
                len--;
                skipWhitespace(str, len, line);
            }
        }
        else if(*str == '"'){
            str++;
            len--;
            unsigned str_len = 0;
            char *const str_data = m_string_data + m_string_len;
            while(str[str_len] != '"' && len && m_string_len + str_len < s_max_lexer_arena_size){
                m_string_data[m_string_len + str_len] = str[str_len];
                if(*str == '\n')
                    line++;
                str_len++;
                len--;
            }
            str += str_len;
            m_string_len += str_len;
            addToken(line, Token::String, str_data, str_len);
            
            if(len && *str == '"'){
                str++;
                len--;
            }
        }
        
#define SINGLE_CHAR_TOKEN(ENUM_, CHAR_)\
        else if(*str == CHAR_){\
            addTokenMod(line, Token:: ENUM_);\
            str++;\
            len--;\
        }
        
        SINGLE_CHAR_TOKEN(Colon, ':')
        SINGLE_CHAR_TOKEN(Dot, '.')
        SINGLE_CHAR_TOKEN(OpenParen, '(')
        SINGLE_CHAR_TOKEN(CloseParen, ')')
        SINGLE_CHAR_TOKEN(BeginArgs, '[')
        SINGLE_CHAR_TOKEN(EndArgs, ']')
        
#undef SINGLE_CHAR_TOKEN
        
#define SINGLE_CHAR_TOKEN_OPER(ENUM_, CHAR_)\
        else if(*str == CHAR_){\
            addToken(line, ENUM_);\
            str++;\
            len--;\
        }
        
        SINGLE_CHAR_TOKEN_OPER(Or, '|')
        SINGLE_CHAR_TOKEN_OPER(And, '&')
        SINGLE_CHAR_TOKEN_OPER(Xor, '^')
        SINGLE_CHAR_TOKEN_OPER(GreaterThan, '>')
        SINGLE_CHAR_TOKEN_OPER(LessThan, '<')
        SINGLE_CHAR_TOKEN_OPER(Equal, '=')
        else if(*str == '!'){
            
            str++;
            len--;
            
            if(!len){
                sprintf(m_data, "Unterminated negation following pling (!) at line %i: '%c'", line, *str);
                return false;
            }
            SINGLE_CHAR_TOKEN_OPER(NotGreaterThan, '>')
            SINGLE_CHAR_TOKEN_OPER(NotLessThan, '<')
            SINGLE_CHAR_TOKEN_OPER(NotEqual, '=')
            else{
                sprintf(m_data, "Invalid character following pling (!) at line %i: '%c'", line, *str);
                return false;
            }
        }
        
#undef SINGLE_CHAR_TOKEN_OPER
        
        else{
            Operator oper;
            if(get_op(str, len, oper)){
                addToken(line, oper);
            }
            else{
                if(str - start > 2 && len > 2)
                    sprintf(m_data, "Invalid character at line %i: (%i) %c%c[%c]%c%c'", line, *str, str[-2], str[-1], str[0], str[1], str[2]);
                else
                    sprintf(m_data, "Invalid character at line %i: (%i) %c'", line, *str, *str);
                return false;
            }
        }
    }
    return true;
}

} // namespace CL
} // namespace DarkLight
