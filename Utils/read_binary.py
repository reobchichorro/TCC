# nrowsfile = open("/home/reobc/Documents/Disciplinas/TCC/site/adir/nrows", "r")
# nrows=int(nrowsfile.read())
# print(nrows)

# file = open("/home/reobc/Documents/Disciplinas/TCC/site/adir/elevs", "rb")

# byte = file.read(2)
# i = 0
# j = 0
# k = 0
# l = []
# while byte:

#     l.append(int.from_bytes(byte, "little"))
#     byte = file.read(2)
#     j+=1
#     if j == nrows:
#         j=0
#         i+=1
#         if i< 3:
#             print(l)
#         l=[]
#     k+=1

def byte_into_bits(intbyte):
    l = []
    for i in range(0,8):
        # print('haha',intbyte)
        l.append(intbyte%2)
        intbyte //= 2
    # print('hahaf',intbyte)
    return l

nrowsfile = open("/home/reobc/Documents/Disciplinas/TCC/wrf/c/site/2925-1-SO/nrows", "r")
nrows=int(nrowsfile.read())
# print(nrows)

file = open("/home/reobc/Documents/Disciplinas/TCC/wrf/c/site/2925-1-SO/r400/h0/shedbin/x0y213", "rb")

byte = file.read(1)
i = 0
j = 0
k = 0
l = []
while byte:
    ll = byte_into_bits(int.from_bytes(byte, "big"))
    for bit in ll:
        l.append(bit)
    byte = file.read(1)
    j+=8
    if j >= nrows:
        j=0
        i+=1
        # if i< 3 and j < 50:
        print('', *l, sep='')
        l=[]
    k+=1
