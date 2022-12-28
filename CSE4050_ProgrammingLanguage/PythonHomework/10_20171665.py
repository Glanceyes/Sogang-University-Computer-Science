city = {}
status = {}

f = open("p10_data.txt","r")

while(1):
    string = f.readline()
    
    if not string:
        break
    
    element = string.split(" ")
    
    if element[1] in city:
        city[element[1]] = city[element[1]] + int(element[3])
    else:
        city[element[1]] = int(element[3])
        
    if element[2] in status:
        status[element[2]] = status[element[2]] + int(element[3])
    else:
        status[element[2]] = int(element[3])
        
print("*** Category : city ***\n\n")

keys = list(city.keys())
values = list(city.values())
for i in range(len(keys)):
    print("%s : %s" %(keys[i], values[i]))

print("\n*** Category : status ***\n")
keys = list(status.keys())
values = list(status.values())
for i in range(len(keys)):
    print("%s : %s" %(keys[i], values[i]))
