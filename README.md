# Alpha-Lang-Compiler // Project hy-340

Ένας απλός μεταφραστής γλώσσας A σε C/CXX, με την χρήση lex-flex/bison-yak. Περιλαμβάνει υλοποίηση λεξικογραφικού και συντακτικού αναλυτή

## Contributors

- Χριστοδούλου Μαριάννα     ΑΜ:5208
- Παπαδάκης Ιωάννης - Τίτος ΑΜ:5200
- Παπαματθαιάκης Γιώργος    ΑΜ:5328

## Compilation And Execution

Είναι υλοποιημένες όλες οι απαιτήσεις τις πρώτης φάσης σε αυτό το πρότζεκτ
Για να γίνει compile το πρόγραμμα θα πρέπει να κάνουμε

```make -f makefile.mk all```

Για να τρέξει με κάποιο Χ τέστ, έχουμε:

```bin/ac tests/phase1/testX.a```

Για να καθαρίσουμε τα *binaries* εκτελούμε:

```make -f makefile.mk clean```

Τα components του μεταφραστή, μπορούν να γίνουν compile by parts, ένα ένα, σε περίπτωση που χρειαστεί:

Parser:  ```make -f makefile.mk ALPHA_PARSER // produces Alpha_Parser.h/.c files required ```

Lexer:   ```make -f makefile.mk ALPHA_LEXER // via flex```

Scanner: ```make -f makefile.mk ALPHA_SCANNER // executable to run, via gcc```