#pragma once
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <functional>

#include<iostream>
#define DBLOG(s) std::cout<<"[DEBUG]: ["<<(s)<<"]"<<std::endl;



typedef std::string::const_iterator Position;
typedef double (UnaryFunctionPointer)(double);
typedef double (BinaryFunctionPointer)(double, double);

class ParsedOperator{
    public:
    ParsedOperator(std::string& notation, UnaryFunctionPointer operation_ptr){
        _notation = notation;
        unary_operation = operation_ptr;
        isBinary = false;
        }
    ParsedOperator(std::string& notation, BinaryFunctionPointer operation_ptr){
        _notation = notation;
        binary_operation = operation_ptr;
        isBinary = true;
        }
    bool is_binary(){return isBinary;}
    std::function<UnaryFunctionPointer>& get_unary_operation(){return unary_operation;}
    std::function<BinaryFunctionPointer>& get_binary_operation(){return binary_operation;}
    std::string& get_notation(){return _notation;}
    private:
    ParsedOperator();
    std::string _notation;
    bool isBinary;
    std::function<UnaryFunctionPointer> unary_operation;
    std::function<BinaryFunctionPointer> binary_operation;

};

typedef std::vector<ParsedOperator> OperatorSet;
typedef OperatorSet::iterator OperatorPtr;
typedef std::map<std::string, double> ArgsSet;


class StringExpression{
public:
    StringExpression(const std::string& base_string);
    StringExpression(Position _begin,
        Position _end);
    Position begin() const { return m_begin;}
    Position end() const { return m_end;}
    Position find_operator_outside_brackets(OperatorPtr op) const;
    std::string get_string() const {return std::string(m_begin, m_end);}
    bool isInBrackets();
private:
    StringExpression();
    Position m_begin;
    Position m_end;
};



class Expression{
public:
    Expression(OperatorPtr op, StringExpression left, StringExpression right):
        pivot_operator(op), left_operand_expr(left), right_operand_expr(right), m_hasOperator(true), m_isTerminal(false) {
        DBLOG("New expression: " + get_string());}
    Expression(OperatorPtr op, StringExpression right):
        pivot_operator(op), left_operand_expr(right), right_operand_expr(right), m_hasOperator(true), m_isTerminal(false) {
        DBLOG("New expression: " + get_string());}
    Expression(StringExpression right):
        left_operand_expr(right), right_operand_expr(right), m_hasOperator(false), 
        m_isTerminal(
            !((*(right.begin()) == '-' || *(right.begin()) == '(') && *(right.end() - 1) == ')')
        ){
        DBLOG("New expression: " + get_string());}
    double const evaluate(double left_operand, double right_operand);
    bool hasOperator() const  {return m_hasOperator;}
    bool isTerminal() const  { return m_isTerminal; }
    OperatorPtr get_pivot_operator() const {return pivot_operator;}
    StringExpression& get_left_operand()  {return left_operand_expr;}
    StringExpression& get_right_operand()  {return right_operand_expr;}
    std::string get_string() const;
private:
    Expression();
    OperatorPtr pivot_operator;
    StringExpression left_operand_expr;
    StringExpression right_operand_expr;
    bool m_hasOperator;
    bool m_isTerminal;

};
typedef std::vector<Expression> ExpressionsSeq;
typedef ExpressionsSeq::iterator ExpressionPtr;


class ExpressionParser{
public:
    ExpressionParser(){};
    double parse_string(std::string expr, const std::map<std::string, double>& args);
    void add_operator(std::string notation, UnaryFunctionPointer operator_func){
        ParsedOperator new_operator(notation, operator_func);
        operators.push_back(new_operator);
    }
    void add_operator(std::string notation, BinaryFunctionPointer operator_func){
        ParsedOperator new_operator(notation, operator_func);
        operators.push_back(new_operator);
    }
private:
    OperatorSet operators;
    ExpressionsSeq expression_tree_stack;
    std::queue<double> expression_value_queue;
    ArgsSet arguments;
    std::vector<int> _brackets_level_map;
    std::string base_string;
    void CleanUpStringExpression();
    void build_expression_tree();
    Expression make_expression(const StringExpression& base_expr);
    double evaluate_expression_tree();
    //void build_brackets_level_map();
    bool isWithinBrackets(size_t pos){
        if(pos<_brackets_level_map.size())
            return _brackets_level_map.at(pos) > 0;
        else
            return false;
    }
    bool isWithinBrackets(std::string::const_iterator pos){
        if(pos<base_string.end())
            return _brackets_level_map.at(pos-base_string.begin()) > 0;
        else
            return false;

    }
    double evaluate_expression(const Expression& expr);



};



