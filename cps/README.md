# About

PicoML is a tiny subset of OCaml.
A simple way of writing code is "direct style", where functions return their value back to their caller.
Instead, "continuation passing style" (CPS) return their output to another continuation *in tail position*.
CPS is a very powerful way of expressing control flow, more explicitly shows the evaluation order, and is an important part of compilation (turning functional code into imperative).

Here, we build a REPL that turns a code block from direct style into CPS.
The transformation rules are:

## Building and running

```console
make main
```

Here is a simple case of CPS, for evaluating a conditional.
The predicate (true) is passed into a function that takes a boolean.
Both branches of the if statement must get translated to return their value to the continuation.

```{console}
$ echo "[[if true then 1 else 2]] K" | ./main
>>> [[if true then 1 else 2]] (K)
[[true]] (fun __a0 -> if __a0 then [[1]] (K) else [[2]] (K))
[[true]] (fun __a0 -> if __a0 then K 1 else [[2]] (K))
[[true]] (fun __a0 -> if __a0 then K 1 else K 2)
(fun __a0 -> if __a0 then K 1 else K 2) true
```

More interesting are function calls.

```{console}
$ echo "[[ (fun x -> x + 1) 2 ]] K" | ./main
>>> [[(fun x -> x + 1) 2]] (K)
[[2]] (fun __a0 -> [[fun x -> x + 1]] (fun __a1 -> (__a1 __a0) K))
[[2]] (fun __a0 -> (fun __a1 -> (__a1 __a0) K) (fun x -> fun __a2 -> [[x + 1]] (__a2)))
[[2]] (fun __a0 -> (fun __a1 -> (__a1 __a0) K) (fun x -> fun __a2 -> [[1]] (fun __a3 -> [[x]] (fun __a4 -> __a2 (__a4 + __a3)))))
[[2]] (fun __a0 -> (fun __a1 -> (__a1 __a0) K) (fun x -> fun __a2 -> [[1]] (fun __a3 -> (fun __a4 -> __a2 (__a4 + __a3)) x)))
[[2]] (fun __a0 -> (fun __a1 -> (__a1 __a0) K) (fun x -> fun __a2 -> (fun __a3 -> (fun __a4 -> __a2 (__a4 + __a3)) x) 1))
(fun __a0 -> (fun __a1 -> (__a1 __a0) K) (fun x -> fun __a2 -> (fun __a3 -> (fun __a4 -> __a2 (__a4 + __a3)) x) 1)) 2
```

First, we have to pass in the argument to a function.
This function takes in an argument `__a0` (which will be 2 in the example above), and passes our increment function to another function that applies `__a1` (which will be the transformed increment) to `__a0` and calls the continuation.
Note that the transformed increment function now must increase in arity, so that it can call the continuation.
We can see that indeed, `[[fun x -> x + 1]]` gets to a function that takes in two arguments.

## Transformation rules

- \[\[x\]\] K => K x
- \[\[c\]\] K => K c
- \[\[if e1 then e2 else e3\]\] K => \[\[e1\]\] (fun b -> if b then \[\[e2\]\] K else \[\[e3\]\] K)
- \[\[e1 e2\]\] K => \[\[e2\]\] (fun x -> \[\[e1\]\] (fun y -> y x K))
- \[\[e1 + e2\]\] K => \[\[e2\]\] (fun x -> \[\[e1\]\] (fun y -> K(y + x))
- \[\[~e1\]\] K => \[\[e1\]\] (fun x -> K(~x))
- \[\[fun x -> e\]\] K => K (fun x -> fun y -> \[\[e\]\] y)
- \[\[let x = e1 in e2\]\] K => \[\[e1\]\] (fun x -> \[\[e2\]\] K)
