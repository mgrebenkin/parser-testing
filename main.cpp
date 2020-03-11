#include <iostream>
#include "ExpressionParser/ExpressionParser.h"

using namespace std;


int main()
{
    ExpressionParser parser;
    cout<<"Parser created\n";
    parser.add_operator("+", [](double operand1, double operand2)->double{
    return operand1+operand2;
    });
    //добавление операторов в порядке роста их приоритета
    parser.add_operator("-", [](double operand1, double operand2)->double{
                        return operand1-operand2;
                        });
    parser.add_operator("/", [](double operand1, double operand2)->double{
    return operand1/operand2;
    }); //опасность поделить на 0!
    parser.add_operator("*", [](double operand1, double operand2)->double{
    return operand1*operand2;
    });
    
    std::map<std::string, double> args;
    std::string expression  = "   ((1 -4) +3 * 4/2  ) " ;
    cout << "Parsing expression: " << expression << endl;
    double parsing_result = parser.parse_string(expression, args );
    cout << expression << " = " << parsing_result<<endl;
    cout<<"Parsing done"<<endl;
    return 0;
}
