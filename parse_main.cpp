#include "picoml.h"
#include "picoml.tab.h"

#include <iostream>
#include <optional>

int main(void)
{
    std::optional<Evaluation> result;
    yy::parser parser(result);
    parser.parse();

    if (!result)
        return 1;

    std::cout << result.value() << "\n";
    return 0;
};
