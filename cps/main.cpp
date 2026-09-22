#include "cps.h"
#include "picoml.h"
#include "picoml.tab.h"

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>

struct yy_buffer_state;
using YY_BUFFER_STATE = yy_buffer_state *;

YY_BUFFER_STATE yy_scan_string(const char *yy_str);
void yy_delete_buffer(YY_BUFFER_STATE b);

namespace
{

std::optional<Rule> parse(const std::string &line)
{
    std::optional<Rule> result;
    YY_BUFFER_STATE buffer = yy_scan_string(line.c_str());

    yy::parser parser(result);
    if (parser.parse())
    {
        yy_delete_buffer(buffer);
        return {};
    }

    yy_delete_buffer(buffer);
    return result;
}

} // namespace

void transform(const std::string &expr)
{
    std::optional<Rule> initial = parse(expr);
    if (!initial)
        throw std::runtime_error("could not parse '" + expr + "': " + last_parse_error());

    std::string cursor = to_string(initial->rule);
    std::unique_ptr<Expression> continuation = std::move(initial->continuation);
    std::vector<AppliedRule> context;

    do
    {
        std::string line = "[[" + cursor + "]] (" + to_string(*continuation) + ")";
        std::optional<Rule> rule = parse(line);
        if (!rule)
            throw std::runtime_error("stuck at '" + line + "': " + last_parse_error());

        std::cout << line << std::endl; // TODO

        auto applied = std::visit(Applier{std::move(continuation)}, rule->rule);
        continuation = std::move(applied.continuation);
        std::visit(
            [&](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Value>)
                {
                    cursor = to_string(Expression::makeApp(std::move(*continuation), Expression(arg)));
                }
                else if constexpr (std::is_same_v<T, Variable>)
                {
                    cursor = to_string(Expression::makeApp(std::move(*continuation), Expression(arg)));
                }
                else if constexpr (std::is_same_v<T, Rewrite>)
                {
                }
                else if constexpr (std::is_same_v<T, Hole>)
                {
                }
                else
                {
                    static_assert(false, "non-exhaustive visitor!");
                }
            },
            applied.result);

    } while (!context.empty());

    std::cout << cursor << "\n";
}

int main(void)
{
    std::cout << "PicoML transformation to continuation passing style. ";
    std::cout << "Enter an expression to transform (e.g. [[ if true then 1 else 3 - 2 ]] _k" << std::endl;

    std::string line;
    while (std::cout << ">>> " && std::getline(std::cin, line))
    {
        try
        {
            transform(line);
        }
        catch (const std::exception &e)
        {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    std::cout << "\n";
    return 0;
}
