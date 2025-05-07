
// gcc  --std=c99 -fsanitize=address,undefined -Wall -Wextra -g number.c -onumb -lm


#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <limits.h>

#define FLAGS
#define OPT(short,long) {  #long,  #short }

typedef uint32_t u32;

// float/double epsilon
// close enough is good enough
double ɛ = 1E-6;

char* ordinal_decorator(u32 n){

	switch(n){
		case 1:  return "st"; break;
		case 2:  return "nd"; break;
		case 3:  return "rd"; break;
		default: return "th"; 
	}
	
}


u32 is_factor(u32 n, u32 f){

	u32 r = n / f;
	if( n == r * f ) return r;
	return 0;
}

u32 is_triangular(u32 n){

	double x = (sqrt(8 * n + 1) - 1) / 2;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

u32 is_pentagonal(u32 n){

	double x = (sqrt(24 * n + 1) + 1) / 6;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

u32 is_hexagonal(u32 n){

	double x = (sqrt(8 * n + 1) + 1) / 4;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

u32 is_centered_hexagonal(u32 n){

	double x = (sqrt(12 * n - 3) + 3 ) / 6;
	if( fabs(x - round(x) ) < ɛ )
		return round(x);
	else
		return 0;
}

u32 is_tetrahedral(u32 n){

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
u32 make_primes(u32 n, u32* p_list){

	// sieve of eratosthenes
	u32 max_prime = n;
	u32* p = malloc(max_prime * sizeof(u32));
	if(!p) {return -1; /* insufficient memory to calculate */ }
	
	/*explicit_*/bzero(p, max_prime*sizeof(u32));
	
	u32 idx = 0;
	for(u32 i = 2; i <= max_prime ; i++){
	
		if(p[i] == 0){
			// i is prime; 
			p_list[idx++] = i;
			// mark all multiples of i
			for(u32 j = 2*i; j < max_prime; j += i){ 
				p[j] = 1; 
			}

		}
	}
	free(p);
	return idx - 1; 
}

char * prime_decomposition(u32 n){

// may decide to return 'free'able string rather than printf as a side effect

	if(n < 2) return NULL;
	
	u32 max_prime = round(n/2);
	u32* p = malloc(max_prime * sizeof(u32));
	if(!p) {return NULL; /* insufficient memory to calculate */ }
	
	/*explicit_*/bzero(p, max_prime*sizeof(u32));
	
	// sieve of eratosthenes
	printf("Prime decomposition: ");
	u32 nn = n;
	for(u32 i = 2; i <= max_prime ; i++){
	
		if(p[i] == 0){
			// i is prime; mark all multiples of i
			for(u32 j = 2*i; j < max_prime; j += i){ 
				p[j] = 1; 
			}
			// remove every instance of i as a factor
			
			while(is_factor(nn, i)){

					printf("%d ", i); 
					nn = nn/i;
					// if(nn != 1){ printf(" * "); }else{
					// 	printf("\n");
					// 	free(p);
					// 	return NULL;
					// }

			}
			
		}
	
	
	}
	printf("\n");
	free(p);
	return NULL;
}


u32 is_nth_root(u32 n, u32 power){

	u32 root = round(exp(log( (double)n ) / ( (double)power)));   
	u32 candidate = root;
	
	for(u32 i = 0; i < power-1; i++){
	
		candidate *= root;
	
	}
	return( candidate == n );

}
typedef struct int_list{ size_t size; u32* factor; }int_list;
u32 factorize(u32 n){ //}, int_list* factors){

	u32 factor_sum = 0;
	if( n == 1 ) return 0; //trivial
	
	printf("Proper Factors: ");
	for(u32 i = 1; i<= n/2; i++){
	
		if(is_factor(n, i)) { 
			printf("%d ", i); 
			factor_sum += i;	
		}

	}
	printf("\n");
	return factor_sum;
}

union options{
	u32 all_bits;
	struct optbits{
		int prime       :1;
		int figurative  :1;
		int sum2squares :1;
		int sum3squares :1;
		int sum2cubes   :1;
		int sum3cubes   :1;
		int powers      :1;
	
	}bits;
}options;

void usage(char* progname){

	printf("Usage: %s [options] <integer>\n", progname);
	printf("    -a   apply all. Same as supplying no options\n");
	printf("    -p   primality / factors / prime decomposition  \n");
	printf("    -f   figurative numbers, e.g. triangular, hexagonal \n");
	printf("    -s   is sum of two squares  \n");
	printf("    -S   is sum of three squares  \n");
	printf("    -c   is sum of two cubes  \n");
	printf("    -C   is sum of three cubes  \n");
	printf("    -x   is of the form xⁿ  \n");

	//  N = xⁿ

	exit(EXIT_FAILURE);
}

void parse_opts( int argc, char* argv[]){

	if(argc < 2){ usage(argv[0]); exit(0); }

	if(argc == 2){ options.all_bits = -1U;} // no options (or no number)

	else{

		int opt;
		while ((opt = getopt(argc, argv, "fpscSC")) != -1) {

			switch (opt) {
				case 'a':
					options.bits.figurative = -1U;
					break;
				case 'f':
					options.bits.figurative = 1;
					break;
				case 'p':
					options.bits.prime = 1;
					break;
				case 's':
					options.bits.sum2squares = 1;
					break;
				case 'c':
					options.bits.sum2cubes = 1;
					break;
				case 'S':
					options.bits.sum3squares = 1;
					break;
				case 'C':
					options.bits.sum3cubes = 1;
					break;
				case 'x':
					options.bits.powers = 1;
					break;
				default: /* '?' */
				// ignore unrecognized option chars
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, [[maybe_unused]] char* argv[argc+1]){

	errno = 0;
	u32 target = strtoul(argv[argc - 1], NULL, 10);
	if (target == UINT_MAX ){ perror("Too big!"); }
	if(errno){ perror("Could not interpret input as a number"); }

	parse_opts(argc, argv);

	// perfection/primality
	if(options.bits.prime){

		u32 factor_sum = factorize(target);
		if(target == factor_sum){
			
			printf("Perfect number\n");		
		} 	
		if((target > 1) && 1 == factor_sum){
			
			printf("Prime\n");
				
		}else{
			
			prime_decomposition(target);
				
		}
	}
	
	// figurative numbers
	if(options.bits.figurative){
		u32 root;
		if((root = is_triangular(target)))  printf("%d%s Triangular number\n",  root, ordinal_decorator(root));
		if((root = is_pentagonal(target)))  printf("%d%s Pentagonal number\n",  root, ordinal_decorator(root));
		if((root = is_hexagonal(target)))   printf("%d%s Hexagonal number\n",   root, ordinal_decorator(root));
		if((root = is_tetrahedral(target))) printf("%d%s Tetrahedral number\n", root, ordinal_decorator(root));
		if((root = is_centered_hexagonal(target)))   printf("%d%s Centered Hexagonal number\n",   root, ordinal_decorator(root));
	}
	// powers
	if(options.bits.powers){		
		u32 root;
		if( (root = is_nth_root(target, 2))){ printf("%d\u00B2\n", root); } //unicode is superscript of power
		if( (root = is_nth_root(target, 3))){ printf("%d\u00B3\n", root); }
		if( (root = is_nth_root(target, 4))){ printf("%d\u2074\n", root); }
		if( (root = is_nth_root(target, 5))){ printf("%d\u2075\n", root); }
		if( (root = is_nth_root(target, 6))){ printf("%d\u2076\n", root); }
		if( (root = is_nth_root(target, 7))){ printf("%d\u2077\n", root); }
		if( (root = is_nth_root(target, 8))){ printf("%d\u2078\n", root); }
		if( (root = is_nth_root(target, 9))){ printf("%d\u2079\n", root); }
	}
	
	//sum of squares / cubes
	if(options.bits.sum2squares || options.bits.sum3squares){
		u32 n_squares = round(exp(log( (double)target ) / ( 2.0)));
		u32 squares[n_squares+1];
		for(u32 i = 0; i < n_squares +1; squares[i] = i*i, i++){ /* */}
		for(u32 i = 1; i <= n_squares; i++){
			for(u32 j = i; j <= n_squares; j++){
				if(options.bits.sum2squares && target == squares[i] + squares[j]){
					printf("%d\u00b2 + %d\u00b2\n", i, j);
				}
				for(u32 k = j; k <= n_squares; k++){
					if(options.bits.sum3squares && target == squares[i] + squares[j] + squares[k]){
						printf("%d\u00b2 + %d\u00b2 + %d\u00b2\n", i, j,k);
					}
				}	
			}
		}
	}

	if(options.bits.sum2cubes || options.bits.sum3cubes){
		u32 n_cubes = round(exp(log( (double)target ) / ( 3.0)));
		u32 cubes[n_cubes+1];
		for(u32 i = 0; i < n_cubes+1; cubes[i] = i*i*i, i++){ /* */ }		
		//printf("there are [%d] cubes less than/equal to target\n", n_cubes);
		for(u32 i = 1; i <= n_cubes; i++){
			for(u32 j = i; j <= n_cubes; j++){
				if(options.bits.sum2cubes && target == cubes[i] + cubes[j]){
					printf("%d\u00b3 + %d\u00b3\n", i, j);
				}
				for(u32 k = j; k <= n_cubes; k++){
					if(options.bits.sum3cubes && target == cubes[i] + cubes[j] + cubes[k]){
						printf("%d\u00b3 + %d\u00b3 + %d\u00b3\n", i, j, k);
					}
				}	
			}
		}
	}	
	return 0; //nothing bad happened
}
