def differ_one_char(string1, string2):
    if (len(string1) != len(string2)):
        return False
    else:
        flag = 0
        for i in range(len(string1)):
            if(string1[i] != string2[i]):
                flag += 1
        if (flag == 1):
            return True
        else:
            return False

#main

print(differ_one_char('abc','afc'))
print(differ_one_char('def','fff'))
print(differ_one_char('abc','abcd'))
print(differ_one_char('cat','fat'))
print(differ_one_char('power','tower'))
