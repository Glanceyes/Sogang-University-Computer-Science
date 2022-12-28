def encrypt(msg, code):
    string = ''
    for index in msg:
        string = string + code[index]
    return string
    
# main

crypt_code = {'a':'g', 'b':'r', 'c':'q', 'd':'i', 'e':'u', 'f':'e', 'g':'w',
              'h':'n', 'i':'d', 'j':'l', 'k':'v', 'l':'t', 'm':'f', 'n':'s',
              'o':'o', 'p':'a', 'q':'k', 'r':'x', 's':'m', 't':'p', 'u':'y',
              'v':'b', 'w':'j', 'x':'z', 'y':'c', 'z':'h'}

A = encrypt('python', crypt_code)
print(A)

B = encrypt('student', crypt_code)
print(B)

C = encrypt('programming', crypt_code)
print(C)

D = encrypt('television', crypt_code)
print(D)

E = encrypt('vacation', crypt_code)
print(E)
