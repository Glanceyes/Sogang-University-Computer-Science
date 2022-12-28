f = open("p2_data.txt", 'r')

while True:
    line = f.readline()
    if not line:
        break
    split_line = line.split(' ')
    f1 = float(split_line[0])
    f2 = float(split_line[1])
    result = f1 + f2
    print("%.2f + %.2f = %.2f" %(f1, f2, result))
    
    
