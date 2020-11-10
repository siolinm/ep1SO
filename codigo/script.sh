#!/bin/bash

for i in {1..3}
do
    for j in {10,100,1000}
    do
        for k in {1..30}
        do
            ./ep1 $i testes/trace$j.txt saidas/saida$i-$j-$k.txt
            echo "$i-$j-$k"
        done
    done
done
