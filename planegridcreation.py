# -*- coding: utf-8 -*-
"""planegridcreation.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1SJ-y77Utc3FDA8zAs9cMZHM4YDXExShq
"""

print("Value is" + str(43) + '"')

resT = 11;
for i in range( resT ):
  for j in range(resT):
    print("v",j,0,i)

for i in range(resT):
  for j in range(resT):
    print("vt","{:.5f}".format(j/(resT-1)),"{:.5f}".format(i/(resT-1)))

print("vn 1.0000 -0.0000 0.0000")
print("s off")

for i in range(1,resT):
  for j in range(1,resT):
    pos = str(j+(i-1)*resT)
    posRight = str(j+1 + (i-1)*resT)
    posUPRIGHT = str(j+1 + i*resT)
    posUP = str(resT -1 + j+1 + (i-1)*resT)
    print("f",pos +"/"+ pos+"/"+"1",posRight+"/"+posRight+"/"+"1",posUPRIGHT+"/"+posUPRIGHT+"/"+"1")
    print("f",pos + "/"+ pos+"/"+"1",posUP+"/"+posUP+"/"+"1",posUPRIGHT+"/"+posUPRIGHT+"/"+"1")