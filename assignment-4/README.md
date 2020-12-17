Assignment 4: Multi-threaded Programming
Name: Hao Jia


How to compile my code:
1. run: gcc -pthread -o line_processor line_processor.c
2. run: line_processor
3. enter characters(see ruler below)
4. When the program receive "DONE", the program will not receive input continually.
5. Waiting about 15 seconds, the program will display the content


When you enter characters, please follow the below rulers:
1. Other than the line separator, the input will only consist of ASCII characters from space (decimal 32) to tilde (decimal 126).
2. The input will not contain any empty lines, i.e., lines that only have space characters or no characters except the line separator.
3. An input line will never be longer than 1000 characters (including the line separator).


My program has 4 threads as follows:
1. Input Thread: This thread performs input on a line-by-line basis from standard input.
2. Line Separator Thread: This thread replaces line separators with blanks.
3. Plus Sign Thread: This thread performs the required replacement of pair of plus signs.
4. Output Thread: This thread writes output lines to the standard output.
