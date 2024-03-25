scale = 2 

def computeFactor(value):
    result = 1
    if value > 10:
        result = result + scale
        if value > 15:
            result = result + 1  # Adds 1 if over 15
    else:
        if value > 5:
            result = result * 3  # Multiply by 3 if over 5
    return result

def addValue(num):
    bias = 1 + num
    bias = bias + scale
    if num > 0:
        bias = bias + 1  # add 1 if positive
        if num > 5:
            bias = bias + 1  # add 1 if over 5
    return bias

def calculateResult(base, factor):
    scale = 5  
    factorAdjustment = base * factor + scale
    finalResult = base + factorAdjustment 
    return finalResult

inputValue = 16
factor = computeFactor(inputValue)
epsilon = addValue(inputValue) 
result = calculateResult(factor, epsilon)

print("Final Result =", result)
