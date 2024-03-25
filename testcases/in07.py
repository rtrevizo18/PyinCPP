#Function with nested if/else 

globalValue = 10

def evaluate(x):
    localValue = x
    if localValue > 5:
        # Nested condition
        if localValue < 10:
            result = localValue + globalValue 
        else:
            result = localValue - 2
    else:
        if localValue == 5:
            result = localValue * globalValue 
        else:
            result = globalValue / localValue 
    return result

testValue = 12
resultFromFunction = evaluate(testValue)

print("ResultFromFunction =", resultFromFunction)
