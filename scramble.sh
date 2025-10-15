#!/bin/bash

# Plik słownika
DICT="/usr/share/dict/words"

# Filtrujemy słowa długości 5-8 liter, małe litery, bez znaków specjalnych
WORDS=($(grep -E '^[a-zA-Z]{5,8}$' "$DICT" | tr 'A-Z' 'a-z' | sort -u))

# Losujemy jedno słowo
WORD="${WORDS[RANDOM % ${#WORDS[@]}]}"

# Funkcja do mieszania liter słowa
shuffle_word() {
    local word=$1
    local arr=($(echo $word | grep -o .))
    local shuffled=()

    while (( ${#arr[@]} )); do
        local i=$(( RANDOM % ${#arr[@]} ))
        shuffled+=("${arr[i]}")
        unset 'arr[i]'
        arr=("${arr[@]}") # usuwamy dziury w indeksach
    done

    echo "${shuffled[*]}" | tr -d ' '
}

# Wymieszane słowo
SCRAMBLED=$(shuffle_word "$WORD")

echo "Ułóż słowo z liter: $SCRAMBLED"
echo "Masz 10 prób."

# Próby zgadywania
TRIES=10
SUCCESS=0

while (( TRIES > 0 )); do
    read -p "Twoja odpowiedź: " GUESS
    GUESS=$(echo "$GUESS" | tr 'A-Z' 'a-z')
    if [[ "$GUESS" == "$WORD" ]]; then
        echo "Gratulacje! Odgadłeś słowo."
        SUCCESS=1
        break
    else
        (( TRIES-- ))
        echo "Niepoprawne. Pozostało prób: $TRIES"
    fi
done

if (( SUCCESS == 0 )); then
    echo "Niestety, nie udało się odgadnąć słowa. Prawidłowe słowo to: $WORD"
fi

# Funkcja do sprawdzania, czy słowo można ułożyć z liter oryginalnego słowa
can_form() {
    local base=$1
    local test=$2
    for letter in $(echo "$test" | grep -o .); do
        if [[ $(echo "$base" | grep -o "$letter" | wc -l) -lt $(echo "$test" | grep -o "$letter" | wc -l) ]]; then
            return 1
        fi
    done
    return 0
}

# Szukamy ile innych słów można ułożyć z tych samych liter
COUNT=0
for w in "${WORDS[@]}"; do
    if [[ "$w" != "$WORD" ]] && [[ ${#w} -eq ${#WORD} ]]; then
        can_form "$WORD" "$w"
        if [[ $? -eq 0 ]]; then
            (( COUNT++ ))
        fi
    fi
done

echo "Z liter słowa '$WORD' można ułożyć jeszcze $COUNT innych poprawnych słów o tej samej długości."

exit 0
