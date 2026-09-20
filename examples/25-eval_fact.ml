Eval(let z = fun f -> (fun x -> f (fun v -> x x v)) (fun x -> f (fun v -> x x v)) in let fact = z (fun self -> fun n -> if n <= 1 then 1 else n * self (n - 1)) in fact 3, {})
