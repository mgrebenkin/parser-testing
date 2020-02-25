#include <iostream>
#include "ExpressionParser/ExpressionParser.h"

using namespace std;


int main()
{
    ExpressionParser parser;
    cout<<"Parser created\n";
    //добавление операторов в порядке убывания их приоритета
    parser.add_operator("-", [](double operand1, double operand2)->double{
                        return operand1-operand2;
                        });
    parser.add_operator("+", [](double operand1, double operand2)->double{
    return operand1+operand2;
    });
    parser.add_operator("/", [](double operand1, double operand2)->double{
    return operand1/operand2;
    }); //опасность поделить на 0!
    parser.add_operator("*", [](double operand1, double operand2)->double{
    return operand1*operand2;
    });
    
    std::map<std::string, double> args;
    std::string expression  = "   (1 +3 * 4  ) " ;
    cout << "Parsing expression: " << expression << endl;
    cout << expression << " = " << parser.parse_string(expression, args )<<endl;
    cout<<"Parsing done"<<endl;
    return 0;
}
