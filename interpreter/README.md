# About

PicoML is a tiny subset of OCaml.
This project produces a REPL that steps through how expressions are evaluated.

Expressions are first scanned (Flex) and then parsed (Bison) into a "reduction rule".
This rule indicates the next step of evaluation; rules are applied until the expression is fully evaluated.

## Building and running

```console
make main
```

This generates a REPL; enter an expression and see its evaluation.
For example, arithmetic expressions in OCaml are left associative, so `1 - 2 - 3` gets grouped into `(1 - 2) - 3 = -4`.
```{console}
$ echo "Eval(1 - 2 - 3, {})" | ./main
>>> Eval((1 - 2) - 3, {})
Eval((1 - 2) - Eval(3, {}), {})
Eval((1 - 2) - Val 3, {})
Eval(Eval(1 - 2, {}) - Val 3, {})
Eval(Eval(1 - Eval(2, {}), {}) - Val 3, {})
Eval(Eval(1 - Val 2, {}) - Val 3, {})
Eval(Eval(Eval(1, {}) - Val 2, {}) - Val 3, {})
Eval(Eval(Val 1 - Val 2, {}) - Val 3, {})
Eval(Val -1 - Val 3, {})
Val -4
```

More interesting are other language constructs, such as if conditions,
```{console}
$ echo "Eval(if true then 1 else 0, {})" | ./main
>>> Eval(if true then 1 else 0, {})
Eval(if Eval(true, {}) then 1 else 0, {})
Eval(if Val true then 1 else 0, {})
Eval(1, {})
Val 1
```

or function applications,
```{console}
$ echo "Eval(Val <x -> x + 1, {}> Val 4, {})" | ./main
>>> Eval(Val <x -> x + 1, {}> Val 4, {})
Eval(x + 1, {x -> 4})
Eval(x + Eval(1, {x -> 4}), {x -> 4})
Eval(x + Val 1, {x -> 4})
Eval(Eval(x, {x -> 4}) + Val 1, {x -> 4})
Eval(Val 4 + Val 1, {x -> 4})
Val 5
```

The parser and lexer can also be run independently, if desired.

### Examples

The main language constructs are shown in the `examples/` directory.
Though `let rec` is not implemented in this grammar, recursion can still be achieved; the simplest is by passing the function as its own argument (see `examples/25-eval_fact.ml`).

## Evaluation rules

- EvalConst:	    Eval(const, env) => Val const
- EvalVar:	      Eval(var, env) => Val(env var) --- if var is in the domain of env
- EvalPair:	      Eval((Val v1, Val v2), env) => Val (v1,v2)
- EvalPairFst:	  Eval((exp1, Val v2), env) => Eval((Eval(exp1, env), Val v2), env)
- EvalPairSnd:	  Eval((exp1,exp2), env) => Eval((exp1, Eval(exp2, env)), env)
- EvalMonOp:      Eval(op (Val v), env) => Val (op v)
- EvalMonOpR:     Eval(op exp1, env) => Eval(op Eval(exp1, env))
- EvalPrimOp:	    Eval((Val v1) op (Val v2), env) => Val (v1 op v2)
- EvalPrimOpL:	  Eval(exp1 op (Val v2), env) => Eval((Eval(exp1, env) op (Val v2)), env)
- EvalPrimOpR:	  Eval(exp1 op exp2, env) => Eval(exp1 op (Eval(exp2, env)), env)
- EvalIfTrue:	    Eval(if Val true then exp_t else exp_f, env) => Eval(exp_t, env)
- EvalIfFalse:	  Eval(if Val false then exp_t else exp_f, env) => Eval(exp_f, env)
- EvalIf:         Eval(if exp_b then exp_t else exp_f, env) => Eval(if Eval(exp_b, env) then exp_t else exp_f, env)
- EvalApp:	      Eval ((Val < x -> exp_b, env_f>) (Val v), env) => Eval (exp_b, {x -> v} + env_f)
- EvalAppFun:	    Eval (exp_f (Val va), env) => Eval (Eval(exp_f, env) (Val va), env)
- EvalAppArg:	    Eval(exp_f exp_a, env) => Eval(exp_f (Eval(exp_a, env)), env)
- EvalFun:	      Eval(fun x -> exp, env) => Val <x -> exp, env>
- EvalLet:	      Eval(let x = Val v in exp2, env) => Eval(exp2, {x -> v} + env)
- EvalLetBinding:	Eval(let x = exp1 in exp2, env) => Eval(let x = Eval(exp1, env) in exp2, env)

## Grammar

The full grammar for PicoML is found in `picoml.y`.
Most features are intuitive and match OCaml.

## Implementation

Bison is used to generate a parser that when given an expression (string) would determine what is the next evaluation rule to apply.
There are 19 rules, as specified above.

In particular, the interpreter is largely string based.
An expression (string) is parsed into an evaluation rule; applying the rule generates a new expression which gets serialized back into a string, after which the process repeats until a Value is obtained.

After the parser generates an abstract syntax tree (AST), with the root indicating the evaluation rule, the evaluation step then has to happen.
Sometimes, one has to recurse with partially evaluated values (such as `(1, Val 2)` a pair where the first element has not yet been evaluted yet).
To do this, a stack of "contexts" is stored, containing the current environment (for unwinding) as well as the expressions to the left/right of the current expression being evaluated.

## Acknowledgements

The evaluation rules for PicoML are presented in CS 421: Programming Languages and Compilers, Fall 2026 at the University of Illiois Urbana-Champaign.
