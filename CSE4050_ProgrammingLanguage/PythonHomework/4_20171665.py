def AverageTopThree(L):
    L.sort()
    L.reverse()
    threeSum = 0
    for i in range(1, 4):
        threeSum += L[i]
    Average = threeSum / 3
    print(L[0], '-', '%0.2f' %Average)
    return

f = open("p4_data.txt", 'r')

split_line = []
index = 0

print('-' * 26)
print('<학생 성적>')
print('-' * 26)

while True:
    line = f.readline()
    if not line:
        break
    tmpline = [ int(x) for x in line.split(' ')]
    split_line.append(tmpline)
    print(split_line[index][0], '[%s,' %split_line[index][1], '%s,' %split_line[index][2], '%s,' %split_line[index][3], '%s]' %split_line[index][4])
    index += 1

index = 0

print('-' * 26)
print('<평균>')
print('-' * 26)

while index < len(split_line):
    AverageTopThree(split_line[index])
    index += 1
