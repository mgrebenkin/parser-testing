#include "ExpressionParser.h"

double const Expression::evaluate(double left_operand, double right_operand){
    if(pivot_operator->is_binary()){
        double res = 0;
        res = pivot_operator->get_binary_operation()(left_operand, right_operand);
        return res;
    }else{
        return pivot_operator->get_unary_operation()(right_operand);
    }
}

std::string Expression::get_string() const{
    if(m_hasOperator){
        if(pivot_operator->is_binary()){
            return "{"+left_operand_expr.get_string()+"}"+
                    pivot_operator->get_notation()+
                    "{"+right_operand_expr.get_string()+"}";
        }
        else{
            return  pivot_operator->get_notation()+
                    "{"+right_operand_expr.get_string()+"}";
        }
    }else{
            return right_operand_expr.get_string();
    }
}



double ExpressionParser::parse_string(std::string string_expr, const std::map<std::string,  double>& _args){
    base_string = string_expr;
    arguments = _args;
    CleanUpStringExpression();
    //build_brackets_level_map();
    build_expression_tree();
    double result = evaluate_expression_tree();
    return result;
}

void ExpressionParser::build_expression_tree(){
    expression_tree_stack.push_back(make_expression(StringExpression(base_string)));
    unsigned int i = 0;

    while(i < expression_tree_stack.size()){
        if (!expression_tree_stack[i].isTerminal()) {
            if (expression_tree_stack[i].hasOperator()) {
                //case of normal expression with operator and operand(s)
                if (expression_tree_stack[i].get_pivot_operator()->is_binary()) {
                    expression_tree_stack.push_back(make_expression(expression_tree_stack[i].get_left_operand()));
                }
                expression_tree_stack.push_back(make_expression(expression_tree_stack[i].get_right_operand()));
            }
            else {
                //case of expression that is brackets with other expression inside with optional '-' in the begining (not terminal, no operator)
                if (*(expression_tree_stack[i].get_right_operand().begin()) == '-') { //with '-' before brackets ("-(...)") then ignore 2 symbols -(" at the begining "-(" and 1 ")" at the end
                    expression_tree_stack.push_back(make_expression(
                        StringExpression(
                            expression_tree_stack[i].get_right_operand().begin() + 2, expression_tree_stack[i].get_right_operand().end() - 1)));
                }
                else {  //otherwise ignore 1 symbol from each side "( )"
                    expression_tree_stack.push_back(make_expression(
                        StringExpression(
                            expression_tree_stack[i].get_right_operand().begin() + 1, expression_tree_stack[i].get_right_operand().end() - 1)));
                }
                
            }
        }
        i++;
    }

}

Expression ExpressionParser::make_expression(const StringExpression& base_string_expr){
    bool operator_found = false;
    OperatorPtr current_operator = operators.begin();
    Position operator_pos;
    while(!operator_found && current_operator != operators.end() ){
        operator_pos = base_string_expr.find_operator_outside_brackets(current_operator);
        if (operator_pos != base_string_expr.end()) {
            operator_found = true;
        }
        else {
            current_operator++;
        }
    }

    if(current_operator == operators.end())
        return Expression(base_string_expr);
    else{
      StringExpression right_expression(operator_pos+current_operator->get_notation().length(),
                                       base_string_expr.end());
      if(current_operator->is_binary()){
          StringExpression left_expression(base_string_expr.begin(), operator_pos);
          return Expression(current_operator, left_expression, right_expression);
      }else{
          return Expression(current_operator, right_expression);
      }
    }
}


double ExpressionParser::evaluate_expression_tree(){
    double expression_value;
    

    while(!expression_tree_stack.empty()){

        Expression current_expression = expression_tree_stack.back();
        expression_tree_stack.pop_back();
        
        expression_value = evaluate_expression(current_expression);

        expression_value_queue.push(expression_value);

    }
    return expression_value;
}

double ExpressionParser::evaluate_expression(const Expression& expr) {
    if (expr.hasOperator()) {
        double right_value = expression_value_queue.front();
        expression_value_queue.pop();
        if (expr.get_pivot_operator()->is_binary()) {
            double left_value = expression_value_queue.front();
            expression_value_queue.pop();
            return expr.get_pivot_operator()->get_binary_operation()(left_value, right_value);
        }
        else {
            return expr.get_pivot_operator()->get_unary_operation()(right_value);
        }
    }
    else {
        std::string string_atomic_expr = expr.get_string();
        bool isNegative = (string_atomic_expr[0] == '-');
        if (expr.isTerminal()) { 
            if (isNegative) string_atomic_expr.erase(string_atomic_expr.begin());
            auto a = arguments.find(string_atomic_expr);
            if (a != arguments.end()) return isNegative ? -1 * (*a).second : (*a).second;
            else return isNegative ? -std::stod(string_atomic_expr) : std::stod(string_atomic_expr);
        }
        else {
            double result = expression_value_queue.front();
            expression_value_queue.pop();
            return isNegative?-result:result;
        }
    }

}

bool StringExpression::isInBrackets() {
    return true;
}

 //void ExpressionParser::build_brackets_level_map(){
 //       int brackets_level = 0;
 //       for(auto c:base_string){
 //           if(c == '(') brackets_level++;
 //           else if(c == ')') brackets_level--;

 //           _brackets_level_map.push_back(brackets_level);
 //       }
 //   }

StringExpression::StringExpression(const std::string& base_string){
    m_begin = base_string.begin();
    m_end = base_string.end();

    /*for (auto c = m_begin; c != m_end-1; c++) {
        if (*c == '(')) bracket_counter++;
        if (*c == ')')) bracket_counter--;
        if (bracket_counter == 0) hasRedunda
    }*/
    /*while(*m_begin == '(' && *(m_end-1) == ')'){
        m_begin++;
        m_end--;
    }*/
    //DBLOG(get_string());
}

StringExpression::StringExpression(Position _begin, Position _end){
    m_begin = _begin;
    m_end = _end;
   /* while(*m_begin == '(' && *(m_end-1) == ')'){
        m_begin++;
        m_end--;
    }*/
    //DBLOG(get_string());
}

//std::string::const_iterator StringExpression::find_substr(
//                                            const std::string& s,
//                                            std::string::const_iterator search_start) const{
//    size_t pos = get_string().find(s, search_start - m_begin);
//    if(pos == std::string::npos) return m_end;
//    else return m_begin+pos;
//}

Position StringExpression::find_operator_outside_brackets(OperatorPtr op) const {
    
    std::string operator_notation = op->get_notation();
    size_t substr_len = operator_notation.length();
    size_t chars_found;
    int brackets_balance = 0;
    if (m_end == m_begin) return m_end;
    for (Position c = (*m_begin) != '-'? m_begin:m_begin+1; c != m_end; c++) {  //'-' in the begining denotes negative and is not operator 
        if (*c == '(') brackets_balance++;
        else if (*c == ')') brackets_balance--;
        else if (brackets_balance == 0) {
            chars_found = 0;
            while (*(c + chars_found) == *(operator_notation.begin() + chars_found) && brackets_balance == 0) {
                chars_found++;
                if (chars_found == substr_len && c != m_begin) return c;
            }
        }
    }
    return m_end;
}

void ExpressionParser::CleanUpStringExpression(){
    size_t space_pos = base_string.find_first_of(' ');
    while(space_pos != std::string::npos){
        base_string.erase(space_pos, 1);
        space_pos = base_string.find_first_of(' ');
    }

    return;
}

