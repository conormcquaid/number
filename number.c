
// gcc  --std=c99 -fsanitize=address,undefined -Wall -Wextra -g number.c -onumb -lm


#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#define FLAGS
#define OPT(short,long) {  #long,  #short }

typedef uint32_t u32;

// float/double epsilon
// close enough is good enough
double ɛ = 1E-6;

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
	return( candidate == n ) ? root : 0;
}

// dynamic array for factors
#define INT_CAP 16
typedef struct int_list{ 
	size_t capacity; 
	size_t size; 
	u32* factors;
}int_list;

// append will grow dynamic array if needed
void append(int_list* l, u32 i){

	if(l->size == l->capacity){ 
				
		void* n = realloc(l->factors, l->capacity * 2 * sizeof(u32));
		if(NULL != n){
			l->factors = (u32*)n;
			l->capacity = l->capacity * 2;
		}else{
			perror("realloc");
		}
	}
	l->factors[l->size++] = i;
}
// factorize (n) into factor list (l) and return sum of factors
u32 factorize(u32 n, int_list* l){ 

	u32 max_factor = sqrt(n) + 1; // ceil is more 

	u32 factor_sum = 1;
	append(l, 1); // 1 is a given
	
	for(u32 i = 2; i<= max_factor; i++){
	
		if(is_factor(n, i)) { 
			
			append(l, i);
			factor_sum += i;	

			u32 cofactor = n / i;
			if(cofactor != i){
				append(l, cofactor);
				factor_sum += cofactor;
			}
		}
	}
	return factor_sum;
}

static int cmpi(const void* a, const void* b){
	u32 x = *(u32*)a;
	u32 y = *(u32*)b;
	if(x < y) return -1;
	if(x > y) return  1;
	return 0;
}

void do_primes_and_factors(u32 target){

	int_list factors;
	factors.size = 0;
	factors.capacity = INT_CAP;
	factors.factors = malloc(INT_CAP * sizeof(u32));
	if(factors.factors == NULL) perror("malloc");

	u32 factor_sum = factorize(target, &factors);

	qsort(factors.factors, factors.size, sizeof(u32), cmpi); 

	printf("Factors: ");
	for(size_t i =0; i < factors.size; i++){

		printf("%d ", factors.factors[i]);
	}
	if(factors.size) printf("\n");

	if(target == factor_sum){
		
		printf("Perfect number\n");		
	} 	
	if((target > 1) && 1 == factor_sum){
		
		printf("Prime\n");
			
	}else{
		
		prime_decomposition(target);
			
	}
	free(factors.factors);
}
void do_figurative(u32 target){
	u32 root;
	if((root = is_triangular(target)))  printf("%d%s Triangular number\n",  root, ordinal_decorator(root));
	if((root = is_pentagonal(target)))  printf("%d%s Pentagonal number\n",  root, ordinal_decorator(root));
	if((root = is_hexagonal(target)))   printf("%d%s Hexagonal number\n",   root, ordinal_decorator(root));
	if((root = is_tetrahedral(target))) printf("%d%s Tetrahedral number\n", root, ordinal_decorator(root));
	if((root = is_centered_hexagonal(target)))   printf("%d%s Centered Hexagonal number\n",   root, ordinal_decorator(root));
}
void do_powers(u32 target){
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
void do_squares(u32 target){
	u32 n_squares = round(exp(log( (double)target ) / ( 2.0)));
	u32 squares[n_squares+1];
	for(u32 i = 0; i < n_squares +1; squares[i] = i*i, i++){ /* generate squares < N */}
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
void do_cubes(u32 target){
	u32 n_cubes = round(exp(log( (double)target ) / ( 3.0)));
	u32 cubes[n_cubes+1];
	for(u32 i = 0; i < n_cubes+1; cubes[i] = i*i*i, i++){ /* generate cubes < N */ }		
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



void usage(char* progname){

	printf("%s provides some properties of a supplied integer. \n\r" \
		    "It is intended for amusement purposes, e.g. discovering\n\r" \
			"interesting attributes of someone's age. It is neither intended,\n\r" \
			"nor optimized for mathematical research.\n\n", progname);

	printf("Usage: %s [options] <integer>\n", progname);
	printf("    -a   apply all. Same as supplying no options\n");
	printf("    -p   primality / factors / prime decomposition  \n");
	printf("    -f   figurative numbers, e.g. triangular, hexagonal \n");
	printf("    -s   is sum of two squares  \n");
	printf("    -S   is sum of three squares  \n");
	printf("    -c   is sum of two cubes  \n");
	printf("    -C   is sum of three cubes  \n");
	printf("    -x   is of the form xⁿ  \n");

	exit(EXIT_FAILURE);
}

void parse_opts( int argc, char* argv[]){

	if(argc < 2){ usage(argv[0]); exit(0); }

	if(argc == 2){ options.all_bits = -1U;} // no options (or no number)

	extern int opterr;
	opterr = 0;
	int opt;
	while ((opt = getopt(argc, argv, "hafpscSC")) != -1) {

		switch (opt) {
			case 'h':
				usage(argv[0]); 
				break;
			case 'a':
				options.all_bits = -1U;
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

////////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, [[maybe_unused]] char* argv[argc+1]){

	parse_opts(argc, argv);
	
	if(strcmp(argv[1],"--help") == 0) { usage(argv[0]); exit(0); }

	errno = 0;
	u32 target = strtoul(argv[argc - 1], NULL, 10);
	if (target == UINT_MAX ){ perror("Too big!"); }
	if (target == 0 ){ printf("Boring!\n"); }
	if(errno){ perror("Could not interpret input as a number"); }

	// perfection/primality
	if(options.bits.prime){
		do_primes_and_factors(target);
	}	
	// figurative numbers
	if(options.bits.figurative){
		do_figurative(target);
	}
	// powers
	if(options.bits.powers){	
		do_powers(target);	
	}	
	//sum of squares / cubes
	if(options.bits.sum2squares || options.bits.sum3squares){
		do_squares(target);
	}
	if(options.bits.sum2cubes || options.bits.sum3cubes){
		do_cubes(target);
	}	
	return 0; //nothing bad happened
}
