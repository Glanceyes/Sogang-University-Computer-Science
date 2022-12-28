A = [[1,2,3], [4,5], [1,3,5,7], [1,1,1,1,1]]

print('original A :', A)

element = []
for i in range(len(A)):
    element = A[i]
    result = 0
    for j in range(len(element)):
        result = result + pow(element[j], 2)
    A[i] = result


print('changed A :', A)
