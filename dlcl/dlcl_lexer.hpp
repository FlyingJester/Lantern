#pragma once

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_utils.h"

#include <cstdlib>

namespace DarkLight {
namespace CL{

// Operators. Different from terminal token types in that eevry operator
// requires an lvalue and an rvalue
enum Operator {
    Plus,
    Minus,
    Multiply,
    Divide,
    LessThan,
    NotLessThan,
    GreaterThan,
    NotGreaterThan,
    Equal,
    NotEqual,
    And,
    Or,
    Xor
};

// Token definition. Note that comments are considered whitespace and not given a token
struct DLCL_EXPORT Token {
    enum Type { 
        Ident,
        String,
        Number,
        GetIdent, // Holds an ident
        SetIdent, // Holds an ident
        IntIdent, // Holds an ident
        StringIdent, // Holds an ident
        BoolIdent, // Holds an ident
        TrueLiteral, FalseLiteral,
        FromIdent, // Holds an ident
        CallIdent, // Holds an ident
        If,
        Loop,
        Next,
        End,
        Return,
        OpenParen,
        CloseParen,
        Colon,
        Dot,
        BeginArgs,
        EndArgs,
        Const,
        Oper
    } m_type;
    
    inline bool isDeclaration() const {
        return m_type == IntIdent ||
            m_type == StringIdent ||
            m_type == BoolIdent;
    }
    
    inline bool isAddOp() const {
        return m_type == Oper &&
            (m_value.oper == Plus || m_value.oper == Minus);
    }
    
    inline bool isMulOp() const {
        return m_type == Oper &&
            (m_value.oper == Multiply || m_value.oper == Divide);
    }
    
    inline bool isBOp() const {
        if(m_type != Oper)
            return false;
        switch(m_value.oper){
            case LessThan:
            case NotLessThan:
            case GreaterThan:
            case NotGreaterThan:
            case Equal:
            case NotEqual:
            case And:
            case Or:
            case Xor:
                return true;
            default:
                return false;
        }
    }
    
    void toString(char *to)const;
    
    union {
        char *ident, *string, *get_ident, *set_ident, 
            *int_ident, *string_ident, *bool_ident;
        float number;
        Operator oper;
    } m_value;
    unsigned short m_length;
    unsigned short m_line_num;
};

// Memory constraints
static const unsigned s_max_lexer_arena_size = 0x80 * 0x400;
static const unsigned s_max_lexer_string_size = s_max_lexer_arena_size;
static const unsigned s_lexer_error_len = 0x100;
static const unsigned s_max_num_tokens = s_max_lexer_arena_size / sizeof(Token);

class DLCL_EXPORT Lexer{
    struct Token m_tokens[s_max_num_tokens];
    unsigned m_num_tokens;
    char m_string_data[s_max_lexer_arena_size];
    unsigned m_string_len;
    char m_data[s_lexer_error_len];
    
    inline void addTokenMod(unsigned short line_num, Token::Type t, unsigned l = 0u){
        m_tokens[m_num_tokens].m_type = t;
        m_tokens[m_num_tokens].m_length = l;
        m_tokens[m_num_tokens].m_line_num = line_num;
        m_num_tokens++;
    }
    
    inline void addToken(unsigned short line_num, Token::Type t, char *s, unsigned l){
        m_tokens[m_num_tokens].m_value.ident = s;
        addTokenMod(line_num, t, l);
    }

    inline void addToken(unsigned short line_num, Token::Type t, float f){
        m_tokens[m_num_tokens].m_value.number = f;
        addTokenMod(line_num, t);
    }
    
    inline void addToken(unsigned short line_num, Operator oper){
        m_tokens[m_num_tokens].m_value.oper = oper;
        addTokenMod(line_num, Token::Oper);
    }
    
public:
    Lexer();
    
    void clear() {
        m_num_tokens = m_string_len = 0;
        m_data[0] = m_data[s_lexer_error_len-1] = '\0';
    }
    
    const char *getError() const { return m_data; }
    
    bool lex(const char *str, unsigned len);
    
    inline struct Token *begin() { return m_tokens; }
    inline const struct Token *begin() const { return m_tokens; }
    inline const struct Token *cbegin() const { return m_tokens; }

    inline struct Token *end() { return m_tokens + m_num_tokens; }
    inline const struct Token *end() const { return m_tokens + m_num_tokens; }
    inline const struct Token *cend() const { return m_tokens + m_num_tokens; }
    
    inline size_t size() const { return m_num_tokens; }
    inline bool empty() const { return m_num_tokens == 0; }
    
};

} // namespace CL
} // namespace DarkLight
