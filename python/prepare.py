import sys

reader = open(sys.argv[1])

lines = reader.readlines()

count = 0
left = 0
right = 0
for line in lines:
    if count == 0:
        right = int(line.split("\t")[0])
    else:
        fr = line.split("\t")[0].replace("u","")
        to = int(line.strip().split("\t")[1].replace("v",""))+right
        print fr+" "+str(to)
    count+= 1
