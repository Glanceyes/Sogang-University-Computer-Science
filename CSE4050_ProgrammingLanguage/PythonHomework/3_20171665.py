def same(L):
    counter1 = 0
    counter2 = len(L) - 1
    result = 1
    while counter1 <= counter2:
        if L[counter1] is L[counter2]:
            result *= 1
        else:
            result *= 0
        counter1 += 1
        counter2 -= 1
    if result is 1:
        return True
    else:
        return False

# main
A = ['a','b','c','c','b','a']
B = ['a','b','c','d','c','b','a']
C = ['a','b','c','c','a','b']
D = ['a','b','c','b','c','a']
E = ['a']
F = ['a','a']
print(same(A))
print(same(B))
print(same(C))
print(same(D))
print(same(E))
print(same(F))
