# Number Explorer

**Usage**: numb [options] \<integer\>  
    -a   apply all. Same as supplying no options  
    -p   primality / factors / prime decomposition  
    -f   figurative numbers, e.g. triangular, hexagonal  
    -s   is sum of two squares  
    -S   is sum of three squares  
    -c   is sum of two cubes  
    -C   is sum of three cubes  
    -x   is of the form xⁿ  

**Behavior**: Lists some properties of the supplied integer, e.g. :  

- factors
- prime decomposition
- primality
- is a perfect number
- figuative numbers such as triangular, pentagonal, hexagonal
- sum of two squares or cubes
- sum of three squares or cubes

Of little practical use except perhaps to observe an interesting fact about one's age. While it will report that Ramanujan's taxicab number is the sum of two cubes, it cannot tell you that it is the smallest such integer.  

**Examples:**  

$ numb 1729  
Factors: 1 7 13 19 91 133 247 1729  
Prime decomposition: 7 13 19  
6² + 18² + 37²  
8² + 12² + 39²  
8² + 24² + 33²  
10² + 27² + 30²  
12² + 17² + 36²  
18² + 26² + 27²  
1³ + 12³  
9³ + 10³  

$ numb 28  
Factors: 1 2 4 7 14  
Perfect number  
Prime decomposition: 2 2 7  
7th Triangular number  
4th Hexagonal number  
1³ + 3³  
