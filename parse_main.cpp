#include "picoml.h"
#include "picoml.tab.h"

int main(void)
{
    yy::parser parser;
    return parser.parse();
};
