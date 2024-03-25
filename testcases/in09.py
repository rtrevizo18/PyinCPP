scale = 2

def multiply(x, y):
    return x * y

def conditionalAddAndMultiply(x, y):
    scale = 3
    if y > 5:
        if x > 2:
            return multiply(x, scale)
        else:
            return multiply(x, scale)
    else:
        return add(x, y)

def add(x, y):
    return x + y

#Function call within a function, nested conditional
result = conditionalAddAndMultiply(add(1, 2), 6)

print("Result =", result)
