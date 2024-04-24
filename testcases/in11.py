def factorial(n):
    if n == 0:
        return 1
    else:
        return n * factorial(n - 1)

y = 5
x = factorial(y)
print("y =", y)
print("factorial(5) =", x)