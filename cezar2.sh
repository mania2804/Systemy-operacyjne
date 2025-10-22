p=$1
napis="$2"
wybor=$3   # lub możesz też to zamienić miejscami, jeśli wolisz

tab="abcdefghijklmnopqrstuvwxyz"
d=${#tab}
d2=${#napis}

if [ "$wybor" -eq 1 ]; then
    p=$p
elif [ "$wybor" -eq 2 ]; then
    p=-$p
else
    echo "Nieprawidłowe polecenie"
    exit 1
fi

for ((i=0; i<d2; i++)); do
    litera="${napis:i:1}"
    if [[ "$litera" == " " ]]; then
        echo -n " "
        continue
    fi
    for ((j=0; j<d; j++)); do
        if [ "$litera" == "${tab:j:1}" ]; then
            index=$(( (j + p) % d ))
            if [ $index -lt 0 ]; then
                index=$((index + d))
            fi
            echo -n "${tab:index:1}"
        fi
    done
done
echo
