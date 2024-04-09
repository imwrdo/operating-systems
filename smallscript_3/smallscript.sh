#!/bin/bash

name="Small_script_3"

zenity --info --title "$name" --text "\nAutor: Mykola Pozniak\nIndex: 201267\nWelcome to my smallscript project, which implements find command in terminal\nIn this project, I am using Bash and Zenity\nEnjoy!"

EXIT=0

while [ $EXIT -eq 0 ]; do
    OPTION=$(zenity --list --height 310 --width 300 \
                    --title="Wybierz opcje" \
                    --column="Numer" --column="Opcja" \
                    1 "Nazwa" \
                    2 "Katalog" \
                    3 "Czas ostatniej modyfikacji" \
                    4 "Typ danych (plik/folder)" \
                    5 "Rozmiar" \
                    6 "Zawartosc" \
                    7 "Szukaj" \
                    8 "Koniec")

    case $OPTION in
        1)  NAME=$(zenity --entry --title "Podaj nazwę" --text="Podaj nazwę:")
            ;;
        2)  DIRECTORY=$(zenity --file-selection --directory --title="Wybierz katalog")
            ;;
        3)  DAYS=$(zenity --entry --title="Podaj liczbę dni" --text="Podaj liczbę dni:")
            ;;
        4)  TYPE=$(zenity --entry --title="Podaj typ" --text="Podaj typ (plik/folder):")
            ;;
        5)  SIZE=$(zenity --entry --title="Podaj rozmiar" --text="Podaj rozmiar w bajtach:")
            ;;
        6)  if [ -z "$NAME" ]; then
        NAME="*"
            fi
            if [ -z "$DIRECTORY" ]; then
                DIRECTORY="$HOME/PG/"
            fi
            command="find \"$DIRECTORY\" -name \"$NAME\""
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
            found_files=$(eval "$command")
            if [ -n "$found_files" ]; then
                temp_file=$(mktemp /tmp/files_content.XXXXXX)
                for file in $found_files; do
                    echo "$(basename "$file"):" >> "$temp_file"
                    cat "$file" >> "$temp_file"
                    echo -e "\n\n" >> "$temp_file"
                done
                zenity --text-info --title="Zawartość znalezionych plików" --width=600 --height=400 --filename="$temp_file"
                rm "$temp_file"
            else
                zenity --info --title="Wyniki wyszukiwania" --text="Nie znaleziono plików."
            fi
            ;;


        7)  if [ -z "$NAME" ]; then
                NAME="*"
            fi
            if [ -z "$DIRECTORY" ]; then
                DIRECTORY="$HOME/PG/"
            fi
            command="find \"$DIRECTORY\" -name \"$NAME\""
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
            zenity --info --title="Wyniki wyszukiwania" --text="$(eval "$command")"
            ;;
        8)  EXIT=1
            zenity --info --title="Koniec" --text="\nKoniec."
            ;;
        *)  zenity --info --title="Błąd" --text="Niepoprawna opcja."
            ;;
    esac
done
