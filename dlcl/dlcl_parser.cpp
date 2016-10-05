/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#include "dlcl_parser.hpp"
#include "dlcl_lexer.hpp"
#include <cstring>
#include <cassert>

namespace DarkLight {
namespace CL{

//=============================================================================
Value::Type Value::typeFromToken(const Token *t){
    switch(t->m_type){
        case Token::IntIdent: return Integer;
        case Token::BoolIdent: return Boolean;
        case Token::StringIdent: return String;
        default: return NumTypes;
    }
}

//=============================================================================
const char *Value::readableTypeName() const{
    switch(m_type){
        case Integer: return "Integer";
        case Boolean: return "Boolean";
        case String:  return "String";
        case NativeFunction: return "Native Code";
        case Function:return "Function";
        case NumTypes:return "NULL";
    }
    return "ERROR";
}

//=============================================================================
bool Value::equal(const Value &other) const{
    
    if(other.m_type != m_type)
        return false;

    switch(m_type){
        case Value::String:
            return a.length == other.a.length &&
                memcmp(other.m_value.string, m_value.string, a.length) == 0;
        case Value::Boolean:
            return m_value.boolean == other.m_value.boolean;
        case Value::Integer:
            return m_value.integer == other.m_value.integer;
        case Value::Function:
            return m_value.function == other.m_value.function;
        case Value::NativeFunction:
            return m_value.native_function == other.m_value.native_function &&
                a.arg == other.a.arg;
        default:
            return false; // Invalid or uncomparable type
    }
}

//=============================================================================
int Parser::findVariableIndex(const char *name, unsigned len) const {
    if(!len)
        len = strnlen(name, 80);
    if(len == 0 || len >= 80)
        return -1;

    for(unsigned i = 0; i<m_num_variables; i++){
        if(strncmp(name, m_variables[i].m_name, len) == 0)
            return i;
    }
    return -2;
}

//=============================================================================
// Helpers
//=============================================================================
static bool handleVariable(char *errdest, Variable *that, const Token *i){
    if(!that){
        if(i->m_length >= 80)
            strcpy(errdest, "Overlength variable name.");
        else{
            char tmp[80];
            memcpy(tmp, i->m_value.ident, i->m_length);
            tmp[i->m_length] = '\0';
            strcpy(errdest, "Undeclared variable: ");
            strcat(errdest, tmp);
        }
        return false;
    }
    else
        return true;
}

static bool handleTypeComparison(char *errdest, const Value &v1, const Value &v2){
    if(v1.m_type != v2.m_type){
        strcpy(errdest, "TypeError: cannot convert ");
        strcat(errdest, v1.readableTypeName());
        strcat(errdest, " to ");
        strcat(errdest, v2.readableTypeName());
        return false;
    }
    else
        return true;
}

static bool canPopValue(char *errdest, const Value *stack, unsigned &sp){
    if(stack){}
    if(sp == 0){
        strcpy(errdest, "Stack underflow");
        return false;
    }
    else if(sp >= 8){
        strcpy(errdest, "Stack overflow");
        return false;
    }
    else
        return true;
}

static bool requireInteger(char *errdest, const char *opname){
    strcpy(errdest, "Integer required for operation ");
    strcat(errdest, opname);
    return false;
}

//=============================================================================
// End Helpers
//=============================================================================

//=============================================================================
Variable *Parser::validateVariableDeclaration(const Token &i){
    // Validate the name, and that the variable has not been declared yet
    if(i.m_length >= 80){
        strcpy(m_error, "Overlength variable name.");
        return NULL;
    }
    if(findVariableIndex(i.m_value.ident, i.m_length) >= 0){
        strcpy(m_error, "Redeclaration of variable ");
        strncat(m_error, i.m_value.ident, i.m_length);
        return NULL;
    }
    if(m_num_variables + 1 >= s_max_parser_variables){
        strcpy(m_error, "Too many variables at declaration of variable ");
        strncat(m_error, i.m_value.ident, i.m_length);
        return NULL;
    }
    
    // Grab the type before we move off of the current token
    strncpy(m_variables[m_num_variables].m_name, i.m_value.string, i.m_length);
    m_variables[m_num_variables].m_value.m_type = Value::typeFromToken(&i);
    
    m_num_variables++;
    
    return m_variables + m_num_variables - 1;
}

//=============================================================================
bool Parser::doCall(const char *func_name, unsigned func_name_len,
    const Token *const start, const Token *&i, const Token *const end, int i_ender){
    const Token::Type ender = static_cast<Token::Type>(i_ender);
    const Variable *const function_variable = findVariableConst(func_name, func_name_len);
    if(!function_variable){
        strcpy(m_error, "Call to undeclared function: ");
        strncat(m_error, func_name, func_name_len);
        return false;
    }
    const Value &function = function_variable->m_value;
    
    if(function.m_type == Value::Function){
        const unsigned backtrack_variable_size = m_num_variables,
            backtrack_string_length = m_string_length;
        
        // TODO: Agument parsing, anyone?
#define DLCL_MAX_ARGS 16
        unsigned arg_num = 0;
        while(i != end && i->m_type != ender){
            if(!parseExpression(start, i, end))
                return false;
            if(!canPopValue(m_error, m_stack, m_sp))
                return false;
            m_arg_stack[arg_num++] = m_stack[--m_sp];
            if(arg_num >= DLCL_MAX_ARGS){
                strcpy(m_error, "Too many arguments to function call (limit is 15) for function: ");
                strncat(m_error, i->m_value.ident, i->m_length);
                return false;
            }
        }
        
        if(arg_num != function.a.num_args){
            strcpy(m_error, "Incorrect number of arguments for function: ");
            strncat(m_error, i->m_value.ident, i->m_length);
            return false;
        }
        
        const Token *func_i = start + function.m_value.function;
        // Set up the arguments
        const unsigned first_num_variables = m_num_variables;
        // TODO: Cleanup
        if(first_num_variables){}
        
        for(unsigned n = 0; n < arg_num && func_i != end; n++){
            if(!func_i->isDeclaration()){
                strcpy(m_error, "Expected argument type for argument in function: ");
                strncat(m_error, i->m_value.ident, i->m_length);
                return false;
            }
            else{
                Variable *var = validateVariableDeclaration(*i);
                if(!var)
                    return false;
                
                if(var->m_value.m_type != m_arg_stack[n].m_type){
                    strcpy(m_error, "Invalid argument type in function: ");
                    strncat(m_error, i->m_value.ident, i->m_length);
                    strcat(m_error, " for argument ");
                    strncat(m_error, var->m_name, 80);
                    return false;
                }
                var->m_value = m_arg_stack[n];
                
            }
        }
        
        if(func_i == end){
            strcpy(m_error, "Unexpected end of input at start of function: ");
            strncat(m_error, i->m_value.ident, i->m_length);
            return false;
        }
        
        do{
            if(func_i->m_type == Token::Return){
                if(!parseExpression(start, ++func_i, end))
                    return false;
                func_i = end;
            }
            else if(!parseStatement(start, func_i, end))
                return false;
        }while(func_i != end);
        
        m_num_variables = backtrack_variable_size;
        if(m_stack[m_sp - 1].m_type != Value::String)
            m_string_length = backtrack_string_length;
        
        // TODO: Go through the stack, and if no stack variables point
        //   to or contain a string location that has been allocated
        //   after the function call, rollback the string data length.
        
    }
    else if(function.m_type == Value::NativeFunction){
        unsigned arg_num = 0;
        if(i != end)
            i++;
        
        if(i != end && i->m_type == Token::Colon)
            i++;
        
        while(i != end && i->m_type != ender){
            if(!parseExpression(start, i, end))
                return false;
            if(!canPopValue(m_error, m_stack, m_sp))
                return false;
            m_arg_stack[arg_num++] = m_stack[--m_sp];
            if(arg_num >= DLCL_MAX_ARGS){
                strcpy(m_error, "Too many arguments to function call (limit is 15)");
                return false;
            }
        }
        if(i != end && i->m_type == ender)
            i++;
#undef DLCL_MAX_ARGS
        if(!function.m_value.native_function(
            m_error, m_stack[m_sp++], function.a.arg, m_arg_stack, arg_num)){
            strcpy(m_error, "Internal error in native function call");
            return false;
        }
        else
            return true;
    }
    else{
        strcpy(m_error, "Variable is not callable: ");
        strncat(m_error, func_name, func_name_len);
        strcat(m_error, " (type was ");
        char n[] = {'0', '0', ')', 0 };
        
        n[0] += function.m_type / 10;
        n[1] += function.m_type % 10;
        strcat(m_error, n);
        return false;
    }
    
    strcpy(m_error, "Internal error in generic function call");
    return false;
}

//=============================================================================
bool Parser::parseFactor(const Token *const start, const Token *&i, const Token *const end){
    Value &value = m_stack[m_sp++];

    value.a.length = 0u;
    bool b = false;
    const char *t = NULL;
    switch(i->m_type){
        case Token::String:
            value.m_type = Value::String;
            value.m_value.string = i->m_value.string;
            value.a.length = i->m_length;
            i++;
            return true;
        case Token::Number:
            value.m_type = Value::Integer;
            value.m_value.integer = i->m_value.number;
            i++;
            return true;
        case Token::TrueLiteral:
            b = true; // FALLTHROUGH
        case Token::FalseLiteral:
            value.m_type = Value::Boolean;
            value.m_value.boolean = b;
            i++;
            return true;
        case Token::Oper:
            if(!t) t = "Operator";
        case Token::If:
            if(!t) t = "if";
        case Token::Loop:
            if(!t) t = "loop";
        case Token::End:
            if(!t) t = "end";
        case Token::Next:
            if(!t) t = "next";
        case Token::Colon:
            if(!t) t = "Colon (:)";
        case Token::Dot:
            if(!t) t = "Dot (.)";
        case Token::Const:
            if(!t) t = "const";
        case Token::EndArgs:
            if(!t) t = "EndArgs (])";
            strcpy(m_error, "Unexpected ");
            strcat(m_error, t);
            return false;
        case Token::BeginArgs:
            strcpy(m_error, "Smalltalk Call Syntax not yet implemented!");
            return false;
        case Token::Ident:
            if(!t) t = "Ident";
        case Token::SetIdent:
            if(!t) t = "SetIdent";
        case Token::IntIdent:
            if(!t) t = "IntIdent";
        case Token::BoolIdent:
            if(!t) t = "BoolIdent";
        case Token::StringIdent:
            if(!t) t = "StringIdent";
        case Token::FromIdent:
            if(!t) t = "FromIdent";
            strcpy(m_error, "Unexpected ");
            strcat(m_error, t);
            strcat(m_error, " identifier: ");
            strncat(m_error, i->m_value.ident, i->m_length);
            return false;
        case Token::GetIdent:
        {
            const Variable *const value = findVariableConst(i->m_value.ident, i->m_length);
            if(!value){
                strcpy(m_error, "Get of undeclared variable ");
                strncat(m_error, i->m_value.ident, i->m_length);
                return false;
            }
            
            if(m_sp >= 7){
                strcpy(m_error, "Stack Overflow");
                return false;
            }
            m_stack[m_sp++] = value->m_value;
            
            i++;
            
            return true;
            
        }
        case Token::CallIdent:
        {
            const char *func_name = i->m_value.ident;
            const unsigned func_name_len = i->m_length;
            return doCall(func_name, func_name_len, start, ++i, end, Token::Dot);
        }
        case Token::Return:
            return parseExpression(start, i, end);
        case Token::OpenParen:
            if(!parseExpression(start, ++i, end))
                return false;
            if(i->m_type != Token::CloseParen){
                strcpy(m_error, "Expected close paren at the end of parenthetical statement");
                return false;
            }
            i++;
            return true;
        case Token::CloseParen:
            strcpy(m_error, "Expected factor at close paren");
            return false;
    }

    strcpy(m_error, "Internal error interpreting Factor (type was ");
    char n[] = {'0', '0', ')', 0 };
    
    n[0] += i->m_type / 10;
    n[1] += i->m_type % 10;
    strcat(m_error, n);
    
    return false;
}

//=============================================================================
bool Parser::parseTerm(const Token *const start, const Token *&i, const Token *const end){
    if(!parseFactor(start, i, end))
        return false;
    while(i->isMulOp()){
        
        // Pop the factor value
        if(!canPopValue(m_error, m_stack, m_sp))
            return false;
        
        // Store the operation to be performed
        const Operator op = i->m_value.oper;
        
        const Value lvalue = m_stack[--m_sp];
        
        // We can check the values as they come in, since only numbers are allowed.
        if(lvalue.m_type != Value::Integer)
            return requireInteger(m_error, (op==Multiply)?"multiplication" : "division");
        
        
        // Parse the next factor
        if(!parseFactor(start, ++i, end))
            return false;
        
        // Pop the second value
        if(!canPopValue(m_error, m_stack, m_sp))
            return false;
        const Value &rvalue = m_stack[--m_sp];
        
        // finish typecheck
        if(rvalue.m_type != Value::Integer)
            return requireInteger(m_error, (op==Multiply)?"multiplication" : "division");
        
        Value &dest = m_stack[m_sp];
        dest.m_type = Value::Integer;
        
        switch(op){
            case Multiply:
                dest.m_value.integer = lvalue.m_value.integer * rvalue.m_value.integer;
                break;
            case Divide:
                dest.m_value.integer = lvalue.m_value.integer / rvalue.m_value.integer;
                break;            
            default:
                strcpy(m_error, "Internal error interpreting Multiply-Op.");
                return false;
        }
    }
    return true;
}

//=============================================================================
bool Parser::parseBTerm(const Token *const start, const Token *&i, const Token *const end){
    if(!parseTerm(start, i, end))
        return false;
    while(i->isAddOp()){
        
        // Pop the term value
        if(!canPopValue(m_error, m_stack, m_sp))
            return false;
        const Value lvalue = m_stack[--m_sp];
        
        // Store the operation to be performed
        const Operator op = i->m_value.oper;
        
        // Parse the next term
        if(!parseTerm(start, ++i, end))
            return false;
        
        // Pop the second value
        if(!canPopValue(m_error, m_stack, m_sp))
            return false;
        const Value &rvalue = m_stack[--m_sp];

        // Typecheck
        if(!handleTypeComparison(m_error, lvalue, rvalue))
            return false;
        
        Value &dest = m_stack[m_sp];
        
        switch(op){
            case Plus:
                switch(lvalue.m_type){
                    case Value::String:
                    {
                        const unsigned new_length = lvalue.a.length + rvalue.a.length;
                        if(m_string_length + new_length >= s_max_parser_arena_size){
                            strcpy(m_error, "Out of string memory");
                            return false;
                        }
                        const unsigned start = m_string_length;
                        
                        // TODO: In the case that rvalue is on the end of the string arena, we could
                        //   just reuse its location and memove it beyond where lvalue will be inserted.
                        //   This would save notably on string memory.
                        
                        memcpy(m_string_data + m_string_length, lvalue.m_value.string, lvalue.a.length);
                        m_string_length += lvalue.a.length;
                        memcpy(m_string_data + m_string_length, rvalue.m_value.string, rvalue.a.length);
                        m_string_length += rvalue.a.length;
                        
                        dest.m_type = Value::String;
                        dest.m_value.string = m_string_data + start;
                        dest.a.length = new_length;
                        break;
                    }
                    case Value::Integer:
                        dest.m_value.integer = lvalue.m_value.integer + rvalue.m_value.integer;
                        dest.m_type = Value::Integer;
                        break;
                    default:
                        strcpy(m_error, "Integer or string required for addition");
                        return false;
                }
                break;
            case Minus:
                if(lvalue.m_type != Value::Integer){
                    return requireInteger(m_error, "subtraction");
                }
                dest.m_value.integer = lvalue.m_value.integer - rvalue.m_value.integer;
                dest.m_type = Value::Integer;
                break;
            default:
                strcpy(m_error, "Internal error interpreting Add-Op.");
                return false;
        }
        m_sp++;
    }
    return true;
}

//=============================================================================
bool Parser::parseExpression(const Token *const start, const Token *&i, const Token *const end){
    if(!parseBTerm(start, i, end))
        return false;
    while(i->isBOp()){
        bool negate = false;
        
        if(!canPopValue(m_error, m_stack, m_sp))
            return false;
        const Value lvalue = m_stack[--m_sp];
        
        const Operator op = i->m_value.oper;
        
        if(!parseBTerm(start, ++i, end))
            return false;
        
        if(!canPopValue(m_error, m_stack, m_sp))
            return false;
        const Value &rvalue = m_stack[--m_sp];
       
        // Typecheck
        if(!handleTypeComparison(m_error, lvalue, rvalue))
            return false;
        
        m_stack[m_sp].m_type = Value::Boolean;
        bool &dest = m_stack[m_sp].m_value.boolean;
        
#define NEGATED_CASE(WHAT_)\
        case Not ## WHAT_: negate = true; case WHAT_
        
#define BOOL_CHECK(WHAT_X_)\
            if(lvalue.m_type != Value::Boolean){\
                strcpy(m_error, "Boolean values required for '" #WHAT_X_ "' operator");\
                return false;\
            } do {} while(0)
        
#define BOOL_CASE(WHAT_, OPER_)\
        case WHAT_:\
            BOOL_CHECK(WHAT_);\
            dest = lvalue.m_value.boolean OPER_ rvalue.m_value.boolean;\
            break
        
        switch(op){
            BOOL_CASE(And, &&);
            BOOL_CASE(Or, ||);
            case Xor:
                BOOL_CHECK(Xor);
                dest = (lvalue.m_value.boolean || rvalue.m_value.boolean) && 
                    !(lvalue.m_value.boolean && rvalue.m_value.boolean);
                break;
            NEGATED_CASE(LessThan):
                if(lvalue.m_type != Value::Integer)
                    return requireInteger(m_error,
                        negate?"Not Less Than":"Less Than");
                dest = lvalue.m_value.integer < rvalue.m_value.integer;
                break;
            NEGATED_CASE(GreaterThan):
                if(lvalue.m_type != Value::Integer)
                    return requireInteger(m_error,
                        negate?"Not Greater Than":"Greater Than");
                dest = lvalue.m_value.integer > rvalue.m_value.integer;
                break;
            NEGATED_CASE(Equal):
                if(lvalue.m_type == Value::Integer)
                    dest = lvalue.m_value.integer == rvalue.m_value.integer;
                else if(lvalue.m_type == Value::Boolean)
                    dest = lvalue.m_value.boolean == rvalue.m_value.boolean;
                else if(lvalue.m_type == Value::String)
                    dest = lvalue.a.length == rvalue.a.length &&
                        memcmp(lvalue.m_value.string, rvalue.m_value.string, rvalue.a.length) == 0;
                break;
            default:
                strcpy(m_error, "Internal error interpreting Binary-Op.");
                return false;
        }
        
#undef NEGATED_CASE
        
#undef BOOL_CHECK
        
#undef BOOL_CASE
        
        if(negate)
            dest = !dest;
        m_sp++;
    }

    return true;
}

//=============================================================================
bool Parser::parseControl(const Token *const start, const Token *&i,
    const Token *const end, bool loop){

    const Token *const begin = i;

conditional_start:
    if(!parseExpression(start, i, end))
        return false;
    
    if(i == end || i->m_type != Token::Colon){
        strcpy(m_error, "Expected colon after conditional");
        return false;
    }
    i++;

    if(!canPopValue(m_error, m_stack, m_sp))
        return false;
    
    const Value &do_work = m_stack[--m_sp];
    
    if(do_work.m_type != Value::Boolean){
        strcpy(m_error, "Expected boolean in conditional");
        return false;
    }
    else if(do_work.m_value.boolean){
        while(i != end && i->m_type != Token::Dot)
            if(!parseStatement(start, i, end))
                return false;
        
        if(i != end && i->m_type == Token::Dot)
            i++;
        if(loop){
            i = begin;
            goto conditional_start;
        }
        return true;
    }
    else{
        if(i == end){
            strcpy(m_error, "Missing body after conditional.");
            return false;
        }
        
        unsigned level = 1;
        do{
            
            if(i->m_type == Token::CallIdent ||
                i->m_type == Token::SetIdent ||
                i->isDeclaration()){
                do{
                    i++;
                }while( i != end && i->m_type != Token::Dot );
                // We _may_ be sitting on the end here, so we need to back off
                // so the later check won't go past the end
                if(i == end)
                    i--;
            }
            else if(i->m_type == Token::Dot){
                level--;
            }
            else if(i->m_type == Token::Colon){
                level++;
            }
            
            if(++i == end && level){
                strcpy(m_error, "Expected end of scope after conditional.");
                return false;
            }
        }while(level);
        
        return true;
    }
}

//=============================================================================
bool Parser::parseStatement(const Token *const start, const Token *&i, const Token *const end){
    if(i->isDeclaration() || i->m_type == Token::Const){
        
        bool is_const = i->m_type == Token::Const;
        
        if(is_const){
            if(++i == end){
                strcpy(m_error, "Unexpected end of input after const");
                return false;
            }
            if(!i->isDeclaration()){
                strcpy(m_error, "Expected declaration after const");
                return false;
            }
        }
        
        Variable *const variable = validateVariableDeclaration(*i);
        if(!variable)
            return false;
        
        // Copy name
        memcpy(variable->m_name, i->m_value.ident, i->m_length);
        variable->m_name[i->m_length] = '\0';
        
        // Set const flag
        variable->m_const = is_const;
        
        Value &that = variable->m_value;
        
        if(!parseExpression(start, ++i, end))
            return false;
        
        // Pop value
        if(!canPopValue(m_error, m_stack, m_sp))
            return false;
        Value &value = m_stack[--m_sp];
        
        // Typecheck
        if(!handleTypeComparison(m_error, value, that))
            return false;
        
        // Assign to the new variable value
        that.m_value = value.m_value;
        that.a = value.a;
        
        return true;
    }
    else if(i->m_type == Token::CallIdent){
        const char *const func_name = i->m_value.ident;
        const unsigned func_name_len = i->m_length;
        if(!doCall(func_name, func_name_len, start, ++i, end, Token::Dot))
            return false;
        else{
            m_sp--;
            return true;
        }
    }
    else if(i->m_type == Token::BeginArgs){
        i++;
        if(i == end || i->m_type != Token::Ident){
            strcpy(m_error, "Expected function name in smalltalk-style call");
            return false;
        }
        const char *const func_name = i->m_value.ident;
        const unsigned func_name_len = i->m_length;
        if(!doCall(func_name, func_name_len, start, ++i, end, Token::EndArgs))
            return false;
        else{
            m_sp--;
            return true;
        }
    }
    else if(i->m_type == Token::SetIdent){
        Variable *const that = findVariable(i->m_value.ident, i->m_length);
        
        if(!handleVariable(m_error, that, i))
            return false;
        if(that->m_const){
            strcpy(m_error, "Invalid set of const variable ");
            strncat(m_error, i->m_value.ident, i->m_length);
            return false;
        }
        
        if(!parseExpression(start, ++i, end))
            return false;
        
        Value &value = m_stack[--m_sp];
        
        // Typecheck
        if(!handleTypeComparison(m_error, value, that->m_value))
            return false;
        
        that->m_value.m_value = value.m_value;
        that->m_value.a = value.a;
        return true;
    }
    else if(i->m_type == Token::Loop){
        return parseLoop(start, ++i, end);
    }
    else if(i->m_type == Token::If){
        return parseIf(start, ++i, end);
    }
    else{
        strcpy(m_error, "Invalid token ");
        char n[] = {'(', 0, 0, ')', 0, 0, 0};
        n[1] = '0' + (i->m_type / 10);
        n[2] = '0' + (i->m_type % 10);
        n[4] = '0' + (((i - start) / 10)%10);
        n[5] = '0' + ((i - start) % 10);
        strcat(m_error, n);
        strncat(m_error, i->m_value.ident, i->m_length);
        
        return false;
    }
}

Parser::Parser(){
    clear();
}

//=============================================================================
void Parser::bindCallback(const char *name, native_callback cb, void *arg){
    strncpy(m_variables[m_num_variables].m_name, name, 79);
    m_variables[m_num_variables].m_value.m_type = Value::NativeFunction;
    m_variables[m_num_variables].m_value.m_value.native_function = cb;
    m_variables[m_num_variables].m_value.a.arg = arg;
    
    m_num_variables++;
}

//=============================================================================
bool Parser::parse(const Lexer &lexer){
    const Token *const end = lexer.cend();
    const Token *const start = lexer.cbegin(), *i = start;

    do{
        if(!parseStatement(start, i, end))
            return false;
    }while(i != end);

    return true;
}

} // namespace CL
} // namespace DarkLight
