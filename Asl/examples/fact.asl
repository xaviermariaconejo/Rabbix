/**
 * A function that computes the factorial of a number
 * using a recursive definition of factorial.
 */
func factorial(n)
  if n <= 1 then return 1 endif;
  return n*factorial(n-1)
endfunc

// The main program
func main()
  write "Enter a number: "; read i;
  write "The factorial of "; write i;
  write " is: "; write factorial(i);
  write "%n"
endfunc
