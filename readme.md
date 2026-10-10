# Simple Search Engine – C Console Application

---

## 1. Project Overview
The program mimics a simplified search engine. It reads a sequence of commands from an input file (such as file addition, deletion, and searching) and organizes words and files in memory using custom lists and a Trie structure for fast execution. All query results and system outputs are written to an `indexare.out` file.

## 2. Core Data Structures
The system relies on four interconnected data structures:
1. **Word List (File Word List):** A singly linked list storing the text of each unique word read for a specific file, facilitating precise deletions.
2. **File List (`CelFile`/`LFile`):** A doubly linked list maintaining all files added to the system, tracking their unique IDs, relevance scores, and associated word lists.
3. **Trie (`Tree`):** A prefix tree storing words letter by letter. Each node features an array of 26 children (for characters 'a' through 'z') and an end-of-word marker (`eow`).
4. **Reference List (`RefFile`):** Associated with every terminal word node (`eow == 1` in the Trie), this list contains pointers to the files containing the word, kept constantly sorted in alphabetical order by filename.

---

## 3. Operation Logic
### "ADD" (File Addition)
* Checks whether the file already exists in the main file list; if found, it outputs `EXISTS`.
* If new, it creates the file entry and appends it to the end of the file list.
* Reads the file's words sequentially, avoiding duplicates. New words are inserted letter by letter into the Trie and added to the file's word list. At the end of the word path in the Trie, the file is linked into the alphabetical reference list.

### "DEL" (Complete File Deletion)
* Searches for the file; if missing, outputs `NOT FOUND`.
* Iterates through the file's word list, navigating to the Trie to remove the file reference from each word's reference list.
* If a word is left without any associated files, a recursive function (`rmvKW`) prunes empty Trie nodes to optimize memory usage.
* Finally, removes the file from the main list and deallocates its memory.

### "ADDKW" & "DELKW" (Add / Delete Word from a File)
* **ADDKW:** Searches for the file (outputs `NOT FOUND` if absent). If the file already contains the word, it does nothing and returns `OK`. Otherwise, adds the word to both the file's list and the Trie.
* **DELKW:** If the file does not contain the word, it directly returns `OK`. If present, removes it from the Trie's reference list (cleaning up the Trie if necessary) and deletes it from the file's local word list.

### "FIND" (Word Lookup)
* Traverses the Trie along the word's characters. If any character or the terminal node is missing, outputs `EMPTY`.
* If found, counts how many files contain the word, outputs the count, followed by their IDs (which are pre-sorted alphabetically).

### "TOPK" (Top K Most Relevant Files)
* Locates the word in the Trie; outputs `EMPTY` if not found.
* Gathers all files containing the word into an array structured as a **Binary Heap** (prioritizing higher relevance scores, and falling back to alphabetical sorting for equal scores).
* Extracts up to $K$ top-performing files and displays their count and names.

### "PRINT" (System Dump)
* Recursively traverses the entire Trie (alphabetically from 'a' to 'z').
* Upon hitting a valid word node, prints its text, the total file count, and the associated file list.

### "PREFIX" (Prefix-Based Search)
* Traverses the Trie to the end of the specified prefix; outputs `EMPTY` if unreachable.
* Recursively traverses all sub-branches from that point down, gathering all matching files into a dynamic array.
* Sorts the array alphabetically, filters out duplicates (if a file matched multiple words under the same prefix), and displays the final results.

---

## 4. Memory Management
At program termination, the system invokes the `freeSystem` function to execute a complete memory cleanup:
* Recursively deletes all Trie nodes and their associated reference lists (`freeTree`).
* Traverses the main file list, deallocating word lists (including strings allocated via `strdup` and node structures), and finally frees the file cells.