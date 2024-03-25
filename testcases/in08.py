thresholdValue = 20

def calculateDiscount(price):
    if price > 100:
        if price > 200:
            if price > 300:
                discountedPrice = thresholdValue - 10
            else:
                discountedPrice = price / 2  # Target condition
        else:
            discountedPrice = price * 2
    else:
        if price < 50:
            discountedPrice = thresholdValue / 2
        else:
            discountedPrice = thresholdValue + price
    return discountedPrice

originalPrice = 250
finalPrice = calculateDiscount(originalPrice)
print("OriginalPrice=", originalPrice)
print("FinalPrice =", finalPrice)
