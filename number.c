
// gcc  --std=c99 -fsanitize=address,undefined -Wall -Wextra -g number.c -onumb -lm


#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>

typedef unsigned int uint;

// float/double epsilon
// close enough is good enough
double ɛ = 1E-6;

char* ordinal_decorator(uint n){
/*	// unicode is surprisingly bad at usperscripts for ordinals
   // spread over different character blocks, etc
   // A: unicode provides glyphs. superscripting is text markup
   
	switch(n){
		case 1:  return "\u02e2\u1d57"; break;
		case 2:  return "\u207f\u1d48"; break;
		case 3:  return "\u02b3\u1d48"; break;
		default: return "\u1d57\u02b0"; 
	}
	*/
	switch(n){
		case 1:  return "st"; break;
		case 2:  return "nd"; break;
		case 3:  return "rd"; break;
		default: return "th"; 
	}
	
}

uint is_factor(uint n, uint f){

	uint r = n / f;
	if( n == r * f ) return r;
	return 0;
}

uint is_triangular(uint n){

	double x = (sqrt(8 * n + 1) - 1) / 2;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

uint is_pentagonal(uint n){

	double x = (sqrt(24 * n + 1) + 1) / 6;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

uint is_hexagonal(uint n){

	double x = (sqrt(8 * n + 1) + 1) / 4;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

uint is_centered_hexagonal(uint n){

	double x = (sqrt(12 * n - 3) + 3 ) / 6;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

uint is_tetrahedral(uint n){

	double x = exp(log(3 * n + sqrt( 9*n*n - (1 / 27.0))) / 3.0) + \
	           exp(log(3 * n - sqrt( 9*n*n - (1 / 27.0))) / 3.0) - 1;
	
	volatile double Teterror = fabs(x - round(x) ); // make Teterror visible to gdn
	           
	if( Teterror < ɛ )
		return round(x);
	else
		return 0;
}

/*
	Returns a list of all primes less than n in array p_list
   Caller is responsible for ensuring that p_list is an array large enough to store all primes less than n
*/
uint make_primes(uint n, uint* p_list){

	// sieve of eratosthenes
	uint max_prime = n;
	uint* p = malloc(max_prime * sizeof(uint));
	if(!p) {return -1; /* insufficient memory to calculate */ }
	
	/*explicit_*/bzero(p, max_prime*sizeof(uint));
	
	uint idx = 0;
	for(uint i = 2; i <= max_prime ; i++){
	
		if(p[i] == 0){
			// i is prime; 
			p_list[idx++] = i;
			// mark all multiples of i
			for(uint j = 2*i; j < max_prime; j += i){ 
				p[j] = 1; 
			}

		}
	}
	free(p);
	return idx - 1; 
}

char * prime_decomposition(uint n){

// may decide to return 'free'able string rather than printf as a side effect

	if(n < 2) return NULL;
	
	uint max_prime = round(n/2);
	uint* p = malloc(max_prime * sizeof(uint));
	if(!p) {return NULL; /* insufficient memory to calculate */ }
	
	/*explicit_*/bzero(p, max_prime*sizeof(uint));
	
	// sieve of eratosthenes
	printf("Prime decomposition: { ");
	uint nn = n;
	for(uint i = 2; i <= max_prime ; i++){
	
		if(p[i] == 0){
			// i is prime; mark all multiples of i
			for(uint j = 2*i; j < max_prime; j += i){ 
				p[j] = 1; 
			}
			// remove every instance of i as a factor
			
			while(is_factor(nn, i)){

					printf("%d", i); 
					nn = nn/i;
					if(nn != 1){ printf(" * "); }else{
						printf(" }\n");
						free(p);
						return NULL;
					}

			}
		}
	
	
	}
	free(p);
	return NULL;
}


uint is_nth_root(uint n, uint power){

	uint root = round(exp(log( (double)n ) / ( (double)power)));   
	uint candidate = root;
	
	for(uint i = 0; i < power-1; i++){
	
		candidate *= root;
	
	}
	if( candidate == n){ return root; }
	
	return 0;

}

uint factorize(uint n){

	uint factor_sum = 0;
	if( n == 1 ) return 0; //trivial
	
	printf("Proper Factors: ");
	for(uint i = 1; i<= n/2; i++){
	
		if(is_factor(n, i)) { 
			printf("%d ", i); 
			factor_sum += i;	
		}

	}
	printf("\n");
	return factor_sum;
}

void usage(char* progname){

	printf("Usage: %s <integer>\n", progname);
	printf("    Display some number-theoretical properties of the supplied integer\n");

}

////////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, [[maybe_unused]] char* argv[argc+1]){

	if(argc < 2){ usage(argv[0]); exit(0); }

	errno = 0;
	uint target = strtoul(argv[1], NULL, 10);
	if(errno){ perror("Could not interpret input as a number"); }

	uint root;	
	
	// perfection/primality
	uint factor_sum = factorize(target);
	if(target == factor_sum){
		
		printf("Perfect number\n");		
	} 
	
	if((target > 1) && 1 == factor_sum){
		
		printf("Prime\n");
			
	}else{
		
		prime_decomposition(target);
			
	}
	
	// figurative numbers
	if((root = is_triangular(target)))  printf("%d%s Triangular number\n",  root, ordinal_decorator(root));
	if((root = is_pentagonal(target)))  printf("%d%s Pentagonal number\n",  root, ordinal_decorator(root));
	if((root = is_hexagonal(target)))   printf("%d%s Hexagonal number\n",   root, ordinal_decorator(root));
	if((root = is_tetrahedral(target))) printf("%d%s Tetrahedral number\n", root, ordinal_decorator(root));
	if((root = is_centered_hexagonal(target)))   printf("%d%s Centered Hexagonal number\n",   root, ordinal_decorator(root));
	
	// powers
	if( (root = is_nth_root(target, 2))){ printf("%d\u00B2\n", root); }
	if( (root = is_nth_root(target, 3))){ printf("%d\u00B3\n", root); }
	if( (root = is_nth_root(target, 4))){ printf("%d\u2074\n", root); }
	if( (root = is_nth_root(target, 5))){ printf("%d\u2075\n", root); }
	if( (root = is_nth_root(target, 6))){ printf("%d\u2076\n", root); }
	if( (root = is_nth_root(target, 7))){ printf("%d\u2077\n", root); }
	if( (root = is_nth_root(target, 8))){ printf("%d\u2078\n", root); }
	if( (root = is_nth_root(target, 9))){ printf("%d\u2079\n", root); }
	
	//sum of squares / cubes
	
	uint n_squares = round(exp(log( (double)target ) / ( 2.0)));
	uint squares[n_squares+1];
	for(uint i = 0; i < n_squares +1; squares[i] = i*i, i++){ /* */}
	for(uint i = 1; i <= n_squares; i++){
		for(uint j = i; j <= n_squares; j++){
			if(target == squares[i] + squares[j]){
				printf("%d\u00b2 + %d\u00b2\n", i, j);
			}
			for(uint k = j; k <= n_squares; k++){
				if(target == squares[i] + squares[j] + squares[k]){
					printf("%d\u00b2 + %d\u00b2 + %d\u00b2\n", i, j,k);
				}
			}	
		}
	}

	
	uint n_cubes = round(exp(log( (double)target ) / ( 3.0)));
	uint cubes[n_cubes+1];
	for(uint i = 0; i < n_cubes+1; cubes[i] = i*i*i, i++){ /* */ }
	
	//printf("there are [%d] cubes less than/equal to target\n", n_cubes);
	for(uint i = 1; i <= n_cubes; i++){
		for(uint j = i; j <= n_cubes; j++){
			if(target == cubes[i] + cubes[j]){
				printf("%d\u00b3 + %d\u00b3\n", i, j);
			}
			for(uint k = j; k <= n_cubes; k++){
				if(target == cubes[i] + cubes[j] + cubes[k]){
					printf("%d\u00b3 + %d\u00b3 + %d\u00b3\n", i, j, k);
				}
			}	
		}
	}
	
/*	// goldbach code works, but is too noisy: there are multiple solutions

	// Goldbach: for n > 2 && n%2 == 0, n = p1 + p2

	// allocate enough ints to store primes less than target
	uint* p_list;
	// x/ln(x) sucks, especially for low x. 
	double pi_n = 1.1 * target / floor(log(target + 0.0));
	p_list = malloc(sizeof(uint) * pi_n);
	bzero(p_list, (int)pi_n * sizeof(uint));
	
	uint np = make_primes(target, p_list);	
	
	printf("<Goldbach>\n");
	if(target > 2){
	
		for(uint i = 0; i < np; i++){
		
			for(uint j = i; j < np; j++){
			
				if((target % 2 == 0) && target == p_list[i] + p_list[j]){
				 	printf("\t%d + %d\n", p_list[i], p_list[j]);
				}
				// weak Goldbach
				// Every integer greater than 5 can be written as the sum of three primes.
				for(uint k = j; k < np; k++){
				
					if(target > 5 && target == p_list[i] + p_list[j] + p_list[k]){
				 		printf("\t%d + %d + %d\n", p_list[i], p_list[j], p_list[k]);
				 	}
				}
			}
		}
	}

	printf("</Goldbach>\n");
	
	free(p_list); // free primes
*/	
	
	return 0; //nothing bad happened
}
