def fibonacci(n):
    if n <= 0:
        return 0
    elif n == 1:
        return 1
    else:
        a, b = 0, 1
        for _ in range(2, n + 1):
            a, b = b, a + b
        return b

def main():
    n = int(input("Enter the position of Fibonacci number: "))
    result = fibonacci(n)
    print(f"The {n}th Fibonacci number is: {result}")



if __name__ == "__main__":
    for i in range(1, 11):
        print(i)


