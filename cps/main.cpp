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
    std::vector<Hole> context;
    bool done = false;
    int epoch = 0;

    do
    {
        std::string line = "[[" + cursor + "]] (" + to_string(*continuation) + ")";
        std::optional<Rule> rule = parse(line);
        if (!rule)
            throw std::runtime_error("stuck at '" + line + "': " + last_parse_error());

        std::string state = line;
        for (auto frame = context.rbegin(); frame != context.rend(); ++frame)
            state = frame->render(state);

        std::cout << state << std::endl;

        auto applied = std::visit(Applier{std::move(continuation), epoch}, rule->rule);
        std::visit(
            [&](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Expression>)
                {
                    Expression cur = std::move(arg);
                    bool unwinding = true;
                    while (unwinding && !context.empty())
                    {
                        Hole top = std::move(context.back());
                        context.pop_back();

                        unwinding = std::visit(
                            [&](auto &&next) -> bool {
                                using T2 = std::decay_t<decltype(next)>;
                                if constexpr (std::is_same_v<T2, Expression>)
                                {
                                    cur = std::move(next);
                                    return true;
                                }
                                else if constexpr (std::is_same_v<T2, Rewrite>)
                                {
                                    cursor = to_string(*next.expr);
                                    continuation = std::move(next.continuation);
                                    return false;
                                }
                                else if constexpr (std::is_same_v<T2, Hole>)
                                {
                                    cursor = next.getNextCursor();
                                    continuation = next.getNextContinuation();
                                    context.push_back(std::move(next));
                                    return false;
                                }
                                else
                                    static_assert(false, "non-exhaustive visitor!");
                            },
                            top.plug(std::move(cur)));
                    }

                    if (unwinding)
                    {
                        cursor = to_string(cur);
                        done = true;
                    }
                }
                else if constexpr (std::is_same_v<T, Rewrite>)
                {
                    cursor = to_string(*arg.expr);
                    continuation = std::move(arg.continuation);
                }
                else if constexpr (std::is_same_v<T, Hole>)
                {
                    cursor = arg.getNextCursor();
                    continuation = arg.getNextContinuation();
                    context.push_back(std::move(arg));
                }
                else
                {
                    static_assert(false, "non-exhaustive visitor!");
                }
            },
            applied);

    } while (!done);

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
