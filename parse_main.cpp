#include "picoml.h"
#include "picoml.tab.h"

#include <iostream>

int main(void)
{
    Evaluation result;
    yy::parser parser(result);
    int rc = parser.parse();
    if (rc == 0)
        std::cout << result << "\n";
    return rc;
};
