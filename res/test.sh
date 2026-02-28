#!/bin/bash

EXECUTABLE="./start"
TEST_DIR="pdf"
EXTRACT_DIR="extract"
ARCHIVE_NAME="arh.tar"
REPORT_FILE="stats.txt"

MAX_PROCESSES=25

mkdir -p "$EXTRACT_DIR"

for p in $(seq 1 $MAX_PROCESSES); do
    echo "Сейчас тестирую $p процессов"
    echo "Процессов: $p" >> "$REPORT_FILE"

    echo "Упаковка:" >> "$REPORT_FILE"
    $EXECUTABLE make-archive -P $p $ARCHIVE_NAME "$TEST_DIR" >> "$REPORT_FILE" 2>&1

    echo "Распаковка:" >> "$REPORT_FILE"
    $EXECUTABLE extract-archive -P $p $ARCHIVE_NAME "$EXTRACT_DIR" >> "$REPORT_FILE" 2>&1

    rm -f $ARCHIVE_NAME
    rm -rf "$EXTRACT_DIR"/*

    echo "------------------------------------" >> "$REPORT_FILE"
done

# табличка - https://docs.google.com/spreadsheets/d/15itEr4PgCN2gCQO-LuK0SJSjYUPsKRn40gb1HeosHcE/edit?gid=0#gid=0
