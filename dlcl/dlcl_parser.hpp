#pragma once

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_utils.h"

#include <cstdlib>

namespace DarkLight {
namespace CL{

class Lexer;
struct Token;

struct Value;

typedef bool (*native_callback)(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);

// Value structure
struct Value {
    enum Type {
        Integer,
        Boolean,
        String,
        Function,
        NativeFunction,
        NumTypes
    } m_type;
    
    union {
        char *string;
        int integer;
        bool boolean;
        unsigned function;
        native_callback native_function;
    } m_value;
    
    union {
        unsigned num_args;
        unsigned length;
        void *arg;
    } a;
    
    static Type typeFromToken(const Token *t);
    const char *readableTypeName() const;
    
    // Determines equality. This is different than a bytewise equality in that strings
    // will be compared
    bool equal(const Value &other) const;
};

struct Variable {
    char m_name[80];
    bool m_const;
    Value m_value;
};

// Constraints for memory usage
static const unsigned s_max_parser_arena_size = 0x80 * 0x400;
static const unsigned s_parser_error_len = 0x100;
static const unsigned s_max_parser_variables = 
    (s_max_parser_arena_size / sizeof(struct Variable)) - 8;

class DLCL_EXPORT Parser{
    
    // VM Stack. Used to pass results to/from expressions and expression parts
    unsigned m_sp;
    struct Value m_stack[8];
    // Argument stack. Not used currently
    struct Value m_arg_stack[16];

    // Variable arena
    unsigned m_num_variables;
    struct Variable m_variables[s_max_parser_variables];

    // Callstack. Not used currently
    unsigned m_callp;
    unsigned m_callstack[8];
    
    unsigned m_scope_stack_len;
    unsigned m_scopestack[8];
    
    // Error message. NULL terminated
    char m_error[s_parser_error_len];
    
    // String data arena
    char m_string_data[s_max_parser_arena_size];
    unsigned m_string_length;
    
    int findVariableIndex(const char *name, unsigned len = 0) const;
    
    // <statement> ::= <set_statement> | <call_statement> | <smalltalk_call> | <if> | <loop>
    bool parseStatement(const Token *const start, const Token *&i, const Token *const end);
    // <bin_op> ::= [<not>] (<greater_than> | <less_than> | <equal> | <and> | <or> | <xor>)
    // <expression> ::= <bterm> [<bin_op> <bterm>]*
    bool parseExpression(const Token *const start, const Token *&i, const Token *const end);
    // <add_op> ::= '+' | '-'
    // <bterm> ::= <term> [<add_op> <term>]*
    bool parseBTerm(const Token *const start, const Token *&i, const Token *const end);
    // <mul_op> ::= '*' | '/'
    // <term> ::= <factor> [<mul_op> <factor>]*
    bool parseTerm(const Token *const start, const Token *&i, const Token *const end);
    // <factor> ::= <literal> | <call_statement> | <smalltalk_call> | <get_statement>
    bool parseFactor(const Token *const start, const Token *&i, const Token *const end);
    // <set_statement> ::= 'set' <identifier> <expression>
    bool parseAssignment(const Token *const start, const Token *&i, const Token *const end);
    // <smalltalk_call> ::= '[' <identifier> [':'] [<expression>]* ']'
    // <call_statement> ::= 'call' <identifier> [':'] [<expression>]* '.'
    bool parseCall(const Token *const start, const Token *&i, const Token *const end);
    Variable *validateVariableDeclaration(const Token &i);

    bool parseControl(
        const Token *const start, const Token *&i, const Token *const end, bool loop);
    // <if> ::= 'if' <expression> ':' <statement>* '.'
    inline bool parseIf(const Token *const start, const Token *&i, const Token *const end){
        return parseControl(start, i, end, false);
    }
    // <loop> ::= 'loop' <expression> ':' <statement>* '.'
    inline bool parseLoop(const Token *const start, const Token *&i, const Token *const end){
        return parseControl(start, i, end, true);
    }
    
    bool doCall(const char *name, unsigned name_len,
        const Token *const start, const Token *&i, const Token *const end, int /*Operator*/ ender);
    
public:
    
    // Binds a native function to a variable in script
    void bindCallback(const char *name, native_callback, void *arg);
    
    inline Variable *findVariable(const char *name, unsigned len = 0) {
        const int r = findVariableIndex(name, len);
        return (r >= 0 && static_cast<unsigned>(r) < s_max_parser_variables) ?
            (m_variables + r) : NULL;
    }
    inline const Variable *findVariableConst(const char *name, unsigned len = 0) const {
        const int r = findVariableIndex(name, len);
        return (r >= 0 && static_cast<unsigned>(r) < s_max_parser_variables) ? 
            (m_variables + r) : NULL;
    }
    inline const Variable *findVariable(const char *name, unsigned len = 0) const {
        return findVariableConst(name, len);
    }
    
    Parser();
    
    void clear(){
        m_sp = 0;
        m_num_variables = 0;
        m_string_length = 0;
        m_scope_stack_len = 0;
        m_error[0] = m_error[s_parser_error_len - 1] = '\0';
    }
    
    bool parse(const Lexer &lexer);
    
    const char *getError() const { return m_error; }
};

} // namespace CL
} // namespace DarkLight
