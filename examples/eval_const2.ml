Eval(2, {
  k1 -> <fun y -> z + 1, {z -> 3}>,
  k2 -> (2, 3),
  k3 -> <fun x -> if true then x else 2, {}>,
  k4 -> <fun x -> f x y, {y -> 1, f -> <fun x -> <fun y -> 1, {}>, {}> }>,
  k5 -> <fun x -> let y = 1 in x + y, {}>
})
