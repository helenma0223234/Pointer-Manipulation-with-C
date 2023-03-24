# Pointer Manipulation and Stealing Private Key Memory from local Address

## Background
To learn the hazards of doing cryptography on general purpose computers... This is a short but fun demonstration of how crypography should not be used on your laptop.

## Material
`list.txt` contains a series of bit sequences that satisfy the following:
- as ASCII strings of 0 and 1
- each sequence varyies in lengths (but no more than 5000)
- one sequence per line

Half of these are "random" sequences made up by students and submitted back earlier in class.  

## What the code does

- part I contains a subroutine `BIGNUM *good(BIGNUM *input)` that 
	1. asks the user for the name of key file
	2. opens that file
	3. extracts d and n
	4. closes the file
	5. and returns input^d mod n.

- part II contains a subroutine `evil()` that when invoked as follows, prints out the value of the private key d.

```
// no globals!
int main( ) {
	// no other locals in main
	BIGNUM *output, *input;
	// set up input to be some integer smaller than your modulus
	output = good(input);
	evil();
}
```

### Assumption
The program doesn't know the name of the private key file, and can't search around the directory to obtain it.

Also importantly, the previous private key pointer can't be freed.

## Explanation
With C and C compiler, the pointers get different addresses on the memory stack every time when the program runs, however, the difference between the private key pointer and some other pointers (that were assigend to point to some other memory spaces) remain the same, or remain constant between every run (assuming no change that allocates new memory was made in the code). Therefore with debugger like `gdb`, we can run both the subroutine once, examine the content of the private key pointers, and the content of a dummy pointer that allocate some other memory (its content is not important). Then we calculate the difference between the contents, which are two memory addresses of where the number resides in the computer's memory stack.

Then with pointer arithmatics, we move the second dummy pointer to point to the private key. Now we have the exact private key.
