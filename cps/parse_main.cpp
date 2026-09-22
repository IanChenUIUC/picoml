#include "picoml.h"
#include "picoml.tab.h"

#include <iostream>
#include <optional>

int main(void)
{
    std::optional<Rule> result;
    yy::parser parser(result);
    parser.parse();

    if (!result)
    {
        std::cerr << last_parse_error() << "\n";
        return 1;
    }

    std::cout << result.value() << "\n";
    return 0;
};
