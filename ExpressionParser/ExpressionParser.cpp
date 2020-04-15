#include "ExpressionParser.h"

double Expression::evaluate(double left_operand, double right_operand){
    if(pivot_operator->is_binary()){
        double res = 0;
        res = pivot_operator->get_binary_operation()(left_operand, right_operand);
        return res;
    }else{
        return pivot_operator->get_unary_operation()(right_operand);
    }
}

std::string Expression::get_string() const{
    if(_hasOperator){
        if(pivot_operator->is_binary()){
            return "("+left_operand_expr.get_string()+")"+
                    pivot_operator->get_notation()+
                    "("+right_operand_expr.get_string()+")";
        }
        else{
            return  pivot_operator->get_notation()+
                    "("+right_operand_expr.get_string()+")";
        }
    }else{
            return right_operand_expr.get_string();
    }
}



double ExpressionParser::parse_string(std::string string_expr, const std::map<std::string,  double>& args){
    base_string = string_expr;
    CleanUpStringExpression();
    build_brackets_level_map();
    build_expression_tree(args);
    double result = evaluate_expression_tree();
    return result;
}

void ExpressionParser::build_expression_tree(const ArgsSet& args){
    expression_tree_stack.push_back(make_expression(StringExpression(base_string)));
    unsigned int i = 0;

    while(i < expression_tree_stack.size()){
        if(expression_tree_stack[i].isTerminal()){
            if(expression_tree_stack[i].get_pivot_operator()->is_binary()){
                expression_tree_stack.push_back(make_expression(expression_tree_stack[i].get_left_operand()));
            }
            expression_tree_stack.push_back(make_expression(expression_tree_stack[i].get_right_operand()));
        }
        i++;
    }

}

Expression ExpressionParser::make_expression(const StringExpression& base_string_expr){
    bool operator_found = false;
    OperatorPtr current_operator = operators.begin();
    Position operator_pos = base_string_expr.find_operator_outside_brackets(current_operator);
    while(operator_pos == base_string_expr.end() && current_operator != operators.end() ){
      operator_pos = base_string_expr.find_outside_brackets(current_operator->get_notation(),  base_string_expr.begin());
      current_operator++;
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
    double expression_value, left_value, right_value;
    std::queue<double> expression_value_queue;

    while(!expression_tree_stack.empty()){

        Expression current_expression = expression_tree_stack.back();
        expression_tree_stack.pop_back();
        if(current_expression.hasOperator()){
            right_value = expression_value_queue.front();
            expression_value_queue.pop();
            if(current_expression.get_pivot_operator()->is_binary()){
                left_value = expression_value_queue.front();
                expression_value_queue.pop();
                expression_value = current_expression.evaluate(left_value, right_value);
            }else{
                expression_value = current_expression.evaluate(right_value, right_value);

            }
            expression_value_queue.push(expression_value);
        }else{
            expression_value_queue.push(std::stod(current_expression.get_right_operand().get_string()));
        }


    }
    return expression_value;
}

bool StringExpression::isInBrackets() {
    if (*m_begin == '(') {
        int brackets_balance = 1;
        for (auto c = m_begin+1; c != m_end; c++)
    }
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
    while(*m_begin == '(' && *(m_end-1) == ')'){
        m_begin++;
        m_end--;
    }
    //DBLOG(get_string());
}

StringExpression::StringExpression(Position _begin, Position _end){
    m_begin = _begin;
    m_end = _end;
    while(*m_begin == '(' && *(m_end-1) == ')'){
        m_begin++;
        m_end--;
    }
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
    size_t substr_len = op->get_notation().length();
    size_t chars_found;
    int brackets_balance = 0;
    if (m_end == m_begin) return m_end;
    for (Position c = m_begin+1; c != m_end; c++) {
        if (*c == '(') brackets_balance++;
        else if (*c == ')') brackets_balance--;
        else if (brackets_balance == 0) {
            chars_found = 0;
            while (*(c + chars_found) == *(s.begin() + chars_found) && brackets_balance == 0) {
                chars_found++;
            }
            if (chars_found == substr_len) return c;
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

