g++ -pthread -o test cas.cc

rm result.txt 

i=1
while [[ i -lt 48 ]]; do 
    echo $i | tr "\n" " " >> result.txt
    ./test $i 1000000 >>result.txt
    i=$((i+i))
done
cat result.txt
