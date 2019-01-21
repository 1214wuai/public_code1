#!/bin/bash


echo "hello world"
myint=123
myfloat=3.14
mystr="hello shell"
mystr2=",linux"
mychar='c'
totalStr=$mystr$mystr2
echo $myint
echo $myfloat
echo $mystr
echo $mychar
echo $mystr$mystr2
echo $totalStr
echo ${#myint}
echo ${#mystr}
