#!/bin/bash
EXIT=0

while [ $EXIT -eq 0 ]
do
    echo "Wybierz opcje:"
    echo "1. Nazwa"
    echo "2. Katalog"
    echo "3. Czas ostatniej modyfikacji"
    echo "4. Typ danych (plik/folder)"
    echo "5. Rozmiar"
    echo "6. Zawartosc"
    echo "7. Szukaj"
    echo "8. Koniec"
    
    read -p "Wybierz numer opcji: " OPTION

    case $OPTION in
        1)  read -p "Podaj nazwę: " NAME
            ;;
        2)  read -p "Podaj ścieżkę do katalogu: " DIRECTORY
            ;;
        3)  read -p "Podaj liczbę dni: " DAYS
            ;;
        4)  read -p "Podaj typ (plik/folder): " TYPE
            ;;
        5)  read -p "Podaj rozmiar w bajtach: " SIZE
            ;;
        6)  if [ -z "$NAME" ]; then
                NAME="*"
            fi
            if [ -z "$DIRECTORY" ]; then
                DIRECTORY='~/PG/'
            fi
            command="find $DIRECTORY -name \"$NAME\""
            if [ -n "$DAYS" ]; then
                command+=" -mtime -$DAYS"
            fi
            if [ -n "$SIZE" ]; then
                command+=" -size +$SIZE"c
            fi
            if [ "$TYPE" == "plik" ]; then
                command+=" -type f"
            elif [ "$TYPE" == "folder" ]; then
                command+=" -type d"
            fi
            found_files=$(eval $command)
            if [ -n "$found_files" ]; then
                for file in $found_files; do
                    echo "Zawartość pliku: $file"
                    echo "====================================="
                    cat "$file"
                    echo "====================================="
                done
            else
                echo "Nie znaleziono plików."
            fi
            ;;
        7)  if [ -z "$NAME" ]; then
                NAME="*"
            fi
            if [ -z "$DIRECTORY" ]; then
                DIRECTORY='~/PG/'
            fi
            command="find $DIRECTORY -name \"$NAME\""
            if [ -n "$DAYS" ]; then
                command+=" -mtime -$DAYS"
            fi
            if [ -n "$SIZE" ]; then
                command+=" -size +$SIZE"c
            fi
            if [ "$TYPE" == "plik" ]; then
                command+=" -type f"
            elif [ "$TYPE" == "folder" ]; then
                command+=" -type d"
            fi
            eval $command
            ;;
        8)  EXIT=1
            echo -e "\nKoniec."
            ;;
        *)  echo "Niepoprawna opcja."
            ;;
    esac
done
