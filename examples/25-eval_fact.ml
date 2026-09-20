Eval(let fact = fun self -> fun n -> if n <= 1 then 1 else n * self self (n - 1) in fact fact 3, {})
