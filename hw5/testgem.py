
dofile = ""
def ADDS():
    indofile=""
    for i in range(10000,99999):
        cmd = " -s "+str(i)
        indofile+="adta"+cmd+'\n'
    indofile += 'usage\n'
    indofile += 'adta -s 99999\n'
    indofile += 'usage\n'
    return indofile
def ADDR():
    indofile=""
    for i in range(0,10):
        cmd = 'adta -r 2000\n'
        indofile += cmd
    indofile += 'usage\n'
    return indofile
def DeleteAll():
    indofile=""
    indofile+='adta -r 20000\n'
    indofile += 'usage\n'
    indofile+='adtd -all\n'
    indofile += 'usage\n'
    return indofile
def DeleteFrontorBack(back = 0): 
    indofile=""
    # indofile+='adta -r 20000\n'
    indofile+=ADDS()
    indofile += 'usage\n'
    for i in range(0,10):
        if(back==0):
            cmd = 'adtd -f 2000\n'
        elif(back==1):
            cmd = 'adtd -b 2000\n'
        elif(back==2):
            cmd = 'adtd -r 2000\n'
        indofile += cmd
    indofile += 'usage\n'
    return indofile
def Find():
    indofile=ADDS()
    indofile+='adtq 99999\n'
    indofile += 'usage\n'
    return indofile
dofile = DeleteFrontorBack(0)
with open("dotest", 'w') as file:
    file.write(dofile)

