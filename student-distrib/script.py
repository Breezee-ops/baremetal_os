# arr = [line.split() for line in open('functions.c').read().split('\n')]
# arr = [com for com in arr if com != [] and com[0] == 'void']
# for i in range(len(arr)):
#     arr[i].pop()
#     temp = list(arr[i][1])
#     temp.pop()
#     temp.pop()
#     arr[i][1] = ''.join(temp)
#     arr[i].append('_asm')
#     del arr[i][0]
#     arr[i] = ''.join(arr[i])
#     print(arr[i], end=" ")

print("uint8_t f0[] =", end=' ')
for i in range(33):
    print(f'\"aframe{i}.txt\"', end=', ')