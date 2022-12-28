def check(clist):
    if (clist[0] == clist[len(clist) - 1]):
        return True
    else :
        return False

#main

A = [3,5,4,6,7]
B = [1,2,3,4,5]
C = [1,5,4,7,1,7,8,9,1]

print(check(A))
print(check(B))
print(check(C))
