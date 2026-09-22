#include "interpreter.h"
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

std::optional<Evaluation> parse(const std::string &line)
{
    std::optional<Evaluation> result;
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

struct Frame
{
    std::string before, after;
    std::string env = "{}";
};

void interpret(const std::string &expr)
{
    std::optional<Evaluation> initial = parse(expr);
    if (!initial)
        throw std::runtime_error("could not parse '" + expr + "': " + last_parse_error());

    std::string cursor = to_string(initial->rule);
    std::string env = to_string(*initial->env);
    std::vector<Frame> context(1);

    do
    {
        std::string line = "Eval(" + cursor + ", " + env + ")";
        std::optional<Evaluation> eval = parse(line);
        if (!eval)
            throw std::runtime_error("stuck at '" + line + "': " + last_parse_error());

        std::string s = line;
        for (std::size_t i = context.size(); i-- > 1;)
            s = "Eval(" + context[i].before + s + context[i].after + ", " + context[i].env + ")";
        std::cout << s << std::endl;

        auto rule = std::visit(Applier{std::move(*eval->env)}, eval->rule.rule);
        std::visit(
            [&](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Atom>)
                {
                    Frame frame = context.back();
                    context.pop_back();

                    cursor = frame.before + "Val " + to_string(*arg.val) + frame.after;
                    env = frame.env;
                }
                else if constexpr (std::is_same_v<T, Rewrite>)
                {
                    cursor = to_string(*arg.expr);
                    env = to_string(*rule.env);
                }
                else if constexpr (std::is_same_v<T, Hole>)
                {
                    context.push_back({arg.before(), arg.after(), env});
                    cursor = arg.cursor();
                    env = to_string(*rule.env);
                }
                else
                {
                    static_assert(false, "non-exhaustive visitor!");
                }
            },
            rule.result);
    } while (!context.empty());

    std::cout << cursor << std::endl;
};

int main(void)
{
    std::cout << "PicoML small-step evaluator. Enter an evaluation state (e.g. Eval((1, 2 + 3), {}))" << std::endl;

    std::string line;
    while (std::cout << ">>> " && std::getline(std::cin, line))
    {
        try
        {
            interpret(line);
        }
        catch (const std::exception &e)
        {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    std::cout << "\n";
    return 0;
}
