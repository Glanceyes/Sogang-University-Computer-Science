def density(population, area):
    dens = float (population) / float(area)
    if (dens >= 1000):
        return "very densely populated"
    elif (dens >= 500):
        return "densely populated"
    elif (dens >= 300):
        return "normally populated"
    elif (dens >= 100):
        return "sparsely populated"
    else:
        return "very sparsely populated"

#main

file = open("p6_data.txt", "r")

while(1):
    string = file.readline()
    if not string:
        break
    element = []
    element = string.split(" ")
    result = density(element[1], element[2])
    print("%s is %s" %(element[0], result))
    
    
