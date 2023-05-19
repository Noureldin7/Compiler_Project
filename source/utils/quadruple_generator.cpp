#include "quadruple_generator.hpp"

quadruple_generator::quadruple_generator(string filename){
    writer = fstream(filename);
}
void quadruple_generator::write_quadruple(ops operation, symbol *op1, symbol *op2, symbol *dst) {
    string op1_str = op1 ? op1->get_name() : "";
    string op2_str = op2 ? op2->get_name() : "";
    string dst_str = dst ? dst->get_name() : "";
    write_quadruple(operation, op1_str, op2_str, dst_str);
}

void quadruple_generator::write_quadruple(ops operation, string op1_str, string op2_str, string dst_str)
{
    string quad = opNames[operation] + " " + op1_str + ", " + op2_str + ", " + dst_str;
    // writer << quad << "\n";
    cout << quad << "\n";
}

void quadruple_generator::Numeric(symbol *op1, symbol *op2)
{
    // If not arithmetic => yyerror
    if (op1->type == types::String)
    {
        yyerror(("Invalid type " + typeNames[op1->type]).c_str());
    }
    if (op2->type == types::String)
    {
        yyerror(("Invalid type " + typeNames[op2->type]).c_str());
    }
    if (op1->type == op2->type)
    {
        // No coercion needed
        return;
    }
    else
    {
        // Coerce into stronger type (min value)
        if (op1->type < op2->type)
        {
            op2->type = op1->type;
            if(op2->type==types::Int)
            {
                write_quadruple(ops::To_Int,op2,NULL,NULL);
            }
            else
            {
                write_quadruple(ops::To_Double,op2,NULL,NULL);
            }
        }
        else
        {
            op1->type = op2->type;
            if(op1->type==types::Int)
            {
                write_quadruple(ops::To_Int,op1,NULL,NULL);
            }
            else
            {
                write_quadruple(ops::To_Double,op1,NULL,NULL);
            }
        }
        // Print Coerce quad
    }
}

symbol* quadruple_generator::assign_op(symbol* dst, symbol* src){
    // String => ALL
    // Integer => ALL but String
    // Double => ALL but String
    // Bool => ALL
    if(src->type==dst->type)
    {
        // Direct Assignment
    }
    else if (dst->type == types::String)
    {
        src = String(src);
    }
    else if (dst->type == types::Bool)
    {
        src = Bool(src);
    }
    else if (dst->type == types::Double)
    {
        if(src->type==types::String)
        {
            yyerror("Cannot cast string to double");
        }
        src = Double(src);
    }
    else if (dst->type == types::Int)
    {
        if(src->type==types::String)
        {
            yyerror("Cannot cast string to int");
        }
        src = Int(src);
    }
    write_quadruple(ops::Assign, src, NULL, dst);
    // write it in quadruples file
    // create new boolean symbol temp
    // return pointer to that new symbol
}


symbol *quadruple_generator::not_op(symbol *op)
{
    // print eni b3ks el operation eli 3ndy
    // write it in quadruples file
    // create new boolean symbol temp
    // return pointer to that new symbol
}
// add , minus , mul , divide
symbol *quadruple_generator::arth_op(ops operation, symbol *op1, symbol *op2)
{
    Numeric(op1, op2);
    // create new symbol temp
    symbol *dst = new symbol(generate_temp(), op1->scope_depth, op1->type, false, true);
    string quad = opNames[operation] + " " + op1->get_name() + ", " + op2->get_name() + ", " + dst->get_name();
    // Print the quad
    write_quadruple(operation, op1, op2, dst);
    if(op1->is_literal)
    {
        delete op1;
    }
    if (op2->is_literal)
    {
        delete op2;
    }
    return dst;
    // return pointer to that new symbol
}

symbol *quadruple_generator::plus_op(symbol *op1, symbol *op2)
{
    if (op1->type == types::String || op2->type == types::String)
    {
        return concat_op(op1, op2);
    }
    else
    {
        return arth_op(ops::Add, op1, op2);
    }
}

symbol *quadruple_generator::concat_op(symbol *op1, symbol *op2)
{
    op1 = String(op1);
    op2 = String(op2);
    symbol *result = new symbol(generate_temp(), max(op1->scope_depth, op2->scope_depth), types::String, false, true);
    write_quadruple(ops::Concat, op1, op2, result);
    delete op1;
    delete op2;
    return result;
}

void quadruple_generator::jmp_on_condition(symbol *op, bool on_true, string label)
{
    op = Bool(op);
    if (on_true)
    {
        write_quadruple(ops::Jmp_True, op->get_name(), "", label);
    }
    else
    {
        write_quadruple(ops::Jmp_False, op->get_name(), "", label);
    }
    delete op;
    return;
}

symbol *quadruple_generator::relational_op(ops operation, symbol *op1, symbol *op2)
{
    // gte x , 3 , t1
    symbol *result = new symbol(generate_temp(), max(op1->scope_depth, op2->scope_depth), types::Bool, false, true);
    if (op1->type == types::Function || op2->type == types::Function)
    {
        yyerror("Error: Function can't be compared");
    }
    if (op1->type != op2->type)
    {
        if (op1->type == types::String || op2->type == types::String)
        {
            yyerror("Error: String can't be compared with other types");
        }
        if (op1->type == types::Double || op2->type == types::Double)
        {
            op1 = Double(op1);
            op2 = Double(op2);
        }
        else
        {
            op1 = Int(op1);
            op2 = Int(op2);
        }
    }
    write_quadruple(operation, op1, op2, result);
    delete op1;
    delete op2;
    return result;
}

void quadruple_generator::jmp_unconditional(string label)
{
    write_quadruple(ops::Jmp, label, "", "");
}

void quadruple_generator::push(symbol *op)
{
    if (op->type == types::Function)
        yyerror("Error: Function can't be pushed");
    write_quadruple(ops::Push, op->get_name(), "", "");
}

symbol *quadruple_generator::pop(symbol *op)
{
    if (op->type == types::Function)
        yyerror("Error: Function can't be popped");
    write_quadruple(ops::Pop, "", "", op->get_name());
    return op;
}

symbol *quadruple_generator::Int(symbol *op)
{
}

symbol *quadruple_generator::Double(symbol *op)
{
    // check that op is not string
    // if it is not string call arth_op
    // else throw error
}

symbol *quadruple_generator::String(symbol *op)
{
    // check that op is string
    // if it is string call concat_op
    // else throw error
}

symbol *quadruple_generator::Bool(symbol *op)
{
    // check that op is not string
    // if it is not string call not_op
    // else throw error
}

quadruple_generator::~quadruple_generator()
{
    writer.close();
}

// list of needed quadruples generating functions
//? 1- assign_op --> NOUR
// 2- jmp op(label) --> NASHAR
// 4- push_op --> NASHAR
// 5- pop_op --> NASHAR
// 6- logical_op(&&,||)-->JOHN
// DONE: 7- relational_op -->(special cases for == and != (plus_op_like)) --> ATAREK
// 8- bitwise_op(&,|,^) --> JOHN
// 9- not_op{logical_op} --> JOHN
//? 10- arth_op(+,-,*,/) --> NOUR
//? 11- plus_op(+) --> ATAREK
//? 12- concat_op(+) --> ATAREK

// for each op , validate the type match --> return error if not matched